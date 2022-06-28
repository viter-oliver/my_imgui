#include "screen_image_distortion.h"
#include "common_functions.h"
#include "res_output.h"
//#include <ft_source.h>

//***************************
#include "af_type.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CALIBRATION_DATA_NODE "/dev/mmcblk3boot1"
#define CALIBRATION_DATA_OFFSET 8 * 1024

static uint32_t CRC32[ 256 ];
static char init = 0;

static void init_table()
{
     int i, j;
     uint32_t crc;
     for( i = 0; i < 256; i++ )
     {
          crc = i;
          for( j = 0; j < 8; j++ )
          {
               if( crc & 1 )
               {
                    crc = ( crc >> 1 ) ^ 0xEDB88320;
               }
               else
               {
                    crc = crc >> 1;
               }
          }
          CRC32[ i ] = crc;
     }
}

static uint32_t Getcrc32( char *buf, int len )
{
     uint32_t g_ret = 0xFFFFFFFF;
     int i;
     if( !init )
     {
          init_table();
          init = 1;
     }
     for( i = 0; i < len; i++ )
     {
          g_ret = CRC32[ ( ( g_ret & 0xFF ) ^ buf[ i ] ) ] ^ ( g_ret >> 8 );
     }
     g_ret = ~g_ret;
     printf( "Return CRC=%x  len=%d\r\n", g_ret, len );
     return g_ret;
}

uint32_t GetCalibrationData( char *&pBuffer )
{
     int fd = 0, iReadRet = 0;
     uint32_t Length = 0, CRCFlag = 0, flag = 0;
     static char pBuf[ 1024 ] = { 0 };

     if( ( fd = open( CALIBRATION_DATA_NODE, O_RDONLY ) ) < 0 )
     {
          LOGE( "open error: %s\n", strerror( errno ) );
     }
     else
     {
          //SEEK_SET（文件指针开始），SEEK_CUR（文件指针当前位置） ，SEEK_END为文件指针尾
          if( -1 == lseek( fd, CALIBRATION_DATA_OFFSET, SEEK_SET ) )
          {
               LOGE( "lseek error: %s", strerror( errno ) );
          }

          if( 0 >= ( iReadRet = read( fd, &Length, sizeof( int ) ) ) )
          {
               LOGE( "read length error: %s", strerror( errno ) );
          }

          printf( "calibration file : length = %d", Length );
          if( 0 >= ( iReadRet = read( fd, pBuf, Length ) ) )
          {
               LOGE( "read file error: %s", strerror( errno ) );
          }

          if( 0 >= ( iReadRet = read( fd, &CRCFlag, sizeof( int ) ) ) )
          {
               LOGE( "read CRC error: %s", strerror( errno ) );
          }
     }
     close( fd );

     if( ( flag = Getcrc32( pBuf, Length ) ) != CRCFlag )
     {
          printf( "read calibration data crc error: %d : %d", CRCFlag, flag );
          Length = 0;
     }

     Length > 0 ? pBuffer = pBuf : (char *)0;
     return Length;
}
//****************************
namespace zl_future
{

	const char* vs_code = R"glsl(
	   attribute vec2 position;
        //attribute vec2 txtcoord;
        uniform mat2 customMtx;
        uniform vec2 customDelta;
        varying vec2 outTxtCd;
        void main()
        {
             outTxtCd=(position+vec2(1,1))*0.5;
             vec2 tmpv=customMtx*position.xy+customDelta;
             gl_Position=vec4(tmpv,0.0,1.0);
        }
		)glsl";
	const char* fs_code = R"glsl(
        varying vec2 outTxtCd;
        uniform sampler2D text;
        //uniform sampler2D txtDistortion;
        vec2 TextureCoord2GLCoord(vec2 textureCoord) 
        {
			vec2 tmpCoord;
			tmpCoord.x=(textureCoord.x-0.5)*2.0;
			tmpCoord.y=(textureCoord.y-0.5)*2.0;
			return tmpCoord;
        }
        vec2 GLCoord2TextureCoord(vec2 glCoord)
        {
        	vec2 tmpCoord;
        	tmpCoord.x=glCoord.x*0.5+0.5;
			tmpCoord.y=glCoord.y*0.5+0.5;;
			return tmpCoord;
        }
        uniform float px[15];
        uniform float py[15];
		uniform int distortion_file_valid; 
        void main()
        {
			if(distortion_file_valid < 1)
			{
				if(outTxtCd.x>0.9999||outTxtCd.x<0.0001|| outTxtCd.y > 0.9999 || outTxtCd.y < 0.0001)
				{
					gl_FragColor=vec4(0.0,1.0,0.0,0.0);
				}
				else
				{
					gl_FragColor = texture2D(text, outTxtCd);
				}
			}
			else{
            //gl_FragColor = texture2D(text, outTxtCd);
            vec2 Texcoord=TextureCoord2GLCoord(outTxtCd);
            vec2 tmpPos=Texcoord;
            Texcoord.x = px[0]
						+px[1] * tmpPos.x
						+px[2] * tmpPos.x  * tmpPos.x 
						+px[3] * tmpPos.x  * tmpPos.x  * tmpPos.x 
						+px[4] * tmpPos.x  * tmpPos.x  * tmpPos.x  * tmpPos.x 
						+px[5] * tmpPos.y 
						+px[6] * tmpPos.x  * tmpPos.y 
						+px[7] * tmpPos.x  * tmpPos.x  * tmpPos.y
						+px[8] * tmpPos.x  * tmpPos.x  * tmpPos.x  * tmpPos.y 
						+px[9] * tmpPos.y  * tmpPos.y 
						+px[10] * tmpPos.x  * tmpPos.y  * tmpPos.y 
						+px[11]  * tmpPos.x  * tmpPos.x  * tmpPos.y  * tmpPos.y 
						+px[12] * tmpPos.y  * tmpPos.y  * tmpPos.y 
						+px[13] * tmpPos.x  * tmpPos.y  * tmpPos.y  * tmpPos.y 
						+px[14] * tmpPos.y  * tmpPos.y  * tmpPos.y  * tmpPos.y ;
		   Texcoord.y = py[0]
						+py[1] * tmpPos.x
						+py[2] * tmpPos.x  * tmpPos.x 
						+py[3] * tmpPos.x  * tmpPos.x  * tmpPos.x 
						+py[4] * tmpPos.x  * tmpPos.x  * tmpPos.x  * tmpPos.x 
						+py[5] * tmpPos.y 
						+py[6] * tmpPos.x  * tmpPos.y 
						+py[7] * tmpPos.x  * tmpPos.x  * tmpPos.y
						+py[8] * tmpPos.x  * tmpPos.x  * tmpPos.x  * tmpPos.y 
						+py[9] * tmpPos.y  * tmpPos.y 
						+py[10] * tmpPos.x  * tmpPos.y  * tmpPos.y 
						+py[11] * tmpPos.x  * tmpPos.x  * tmpPos.y  * tmpPos.y 
						+py[12] * tmpPos.y  * tmpPos.y  * tmpPos.y 
						+py[13] * tmpPos.x  * tmpPos.y  * tmpPos.y  * tmpPos.y 
						+py[14] * tmpPos.y  * tmpPos.y  * tmpPos.y  * tmpPos.y ;
		    Texcoord=GLCoord2TextureCoord(Texcoord);
		    if(Texcoord.x>0.9999||Texcoord.x<0.0001|| Texcoord.y > 0.9999 || Texcoord.y < 0.0001)
		    {
		        gl_FragColor=vec4(0.0,1.0,0.0,0.0);
		    }
		    else
		    {
 			    gl_FragColor = texture2D(text, Texcoord);
		    }
			}
        }
		)glsl";
    

//static char* distortion_bin_file="/tmp/calibration.bin";
#define A_SINGLE_CALIBRATION_SIZE_MAX 160
#define HEIGHT_CNT 7


struct distortion_unit
{
	int height={0};
	int length={0};
	float px[15];
	float py[15];
	int crc_flag;
	char  reserved[A_SINGLE_CALIBRATION_SIZE_MAX-sizeof(float)*30-sizeof(int)*3];
};
distortion_unit distortion_data[HEIGHT_CNT];


     GLfloat _plain_vertices[] = {
          -1.f, -1.f, 
          1.f, -1.f, 
          -1.f, 1.f,
          1.f, 1.0f, 
     };
	screen_image_distortion::screen_image_distortion(float width, float height)
		:_width(width), _height(height)
	{
	    #if 0
		string fs_code_from_bin;
        char* file_buf = NULL;
		float ferrormax = 0.0;
		int buf_size = GetCalibrationData(file_buf);
		//int rtn=paramToFtstrCode(distortion_bin_file,fs_code_from_bin);
		int rtn = 0;
		if(file_buf == NULL)
		{
			rtn = -1;
		}
		else
		{
			rtn = bufDxdyToFtstrCode(file_buf,buf_size,fs_code_from_bin,ferrormax);
		}
		
		if(rtn==0)
		{
		 //printf("get fssource:%s\n",fs_code_from_bin.c_str());
			_pshader = make_shared<af_shader>( vs_code, fs_code_from_bin.c_str() );
		}
		#endif
		int fd=open( CALIBRATION_DATA_NODE, O_RDONLY );
		if(fd<0)
		{
			LOGE("%s is invalid\n",CALIBRATION_DATA_NODE);
		}
		else
		{
			if(-1==lseek(fd,CALIBRATION_DATA_OFFSET,SEEK_SET))
			{
				LOGE("offset:%d is invalid\n",CALIBRATION_DATA_OFFSET);
			}
			else
			{
				int rs=read(fd,distortion_data,sizeof(distortion_data));
				for(auto i=0;i<HEIGHT_CNT;i++)
				{
					if(distortion_data[i].height != i)
					{
						distortion_data[i].length = 0;
						//break;
					}
				}
				
				
				if(rs<=0)
				{
					LOGE("fail to read distortion data\n");
				}
			}
		}
		_pshader = make_shared<af_shader>( vs_code, fs_code );
		_pshader->use();
		auto pmid = _pshader->program_id();
		glGenBuffers( 1, &g_VboHandle );
		glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
		glBufferData( GL_ARRAY_BUFFER, sizeof( _plain_vertices ), _plain_vertices, GL_STATIC_DRAW );
		GLint posAttrib = glGetAttribLocation( pmid, "position" );
		glEnableVertexAttribArray( posAttrib );
		_Ul_customMtx = glGetUniformLocation( pmid, "customMtx" );
        _Ul_customDelta = glGetUniformLocation( pmid, "customDelta" );
		glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE,0, (GLvoid*)0 );
		prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, _width, _height);

	}

	screen_image_distortion::~screen_image_distortion()
	{
        glDeleteBuffers( 1, &g_VboHandle );
		glDeleteTextures(1, &_colorTextId);
		glDeleteTextures(1, &_depthStencilTextId);
		glDeleteFramebuffers(1, &_fboId);
	}
	void screen_image_distortion::set_rotate_angle( float angle )
	{
		 g_cs_a = cos( angle );
		 g_sn_a = sin( angle );
		 //printf("$$$$$$$$$$set angle%f\n",angle);
	}
	void screen_image_distortion::set_rotate_axis_pos( float px, float py )
	{
		 g_ox = px;
		 g_oy = py;
	}
    void screen_image_distortion::set_height(int height)
	{
		_virImgHeight=height;
	}
	void screen_image_distortion::bind_framebuffer()
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING,&_prev_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
		//glViewport(0, 0, _width, _height);
		//glClearColor(1, 1,1, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
	}
	void screen_image_distortion::draw()
	{
		glViewport(0, 0, _width, _height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		_pshader->use();
		glActiveTexture(GL_TEXTURE0);

		auto pmid = _pshader->program_id();
		float custom_matrix[ 2 ][ 2 ] =
		{
			 { g_cs_a, g_sn_a },
			 { -g_sn_a, g_cs_a },
		};
		glUniformMatrix2fv( _Ul_customMtx, 1, GL_FALSE, &custom_matrix[ 0 ][ 0 ] );
		float vdelta[ 2 ] =
		{
			 g_ox*( 1 - g_cs_a ) - g_oy*g_sn_a,
			 g_ox*g_sn_a + g_oy*( 1 - g_cs_a ),
		};
		glUniform2fv( _Ul_customDelta, 1, vdelta );
		distortion_unit& cur_dis=distortion_data[_virImgHeight];
		_pshader->uniform("px[0]",cur_dis.px);
		_pshader->uniform("py[0]",cur_dis.py);
		_pshader->uniform("distortion_file_valid",&cur_dis.length);
		
		glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
		glBufferData( GL_ARRAY_BUFFER, sizeof( _plain_vertices ), _plain_vertices, GL_STATIC_DRAW );
		GLint posAttrib = glGetAttribLocation( pmid, "position" );

		glEnableVertexAttribArray( posAttrib );

		glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0 );
		//auto txtid = g_mtexture_list["road.png"]->_atxt_id;
		glBindTexture(GL_TEXTURE_2D,  _colorTextId);
		//_pshader->uniform("text", 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void screen_image_distortion::disbind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _prev_fbo);
	}

}
