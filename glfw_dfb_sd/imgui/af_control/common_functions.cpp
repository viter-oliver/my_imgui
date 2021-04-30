#include "common_functions.h"
#include <math.h>
#include "SOIL.h"
#include "texture.h"
#include "json/json.h"
std::wstring utf8ToWstring(const std::string& s)
{
    std::wstring ws;
    wchar_t wc;
    for( int i = 0;i < s.length(); )
    {
        char c = s[i];
        if ( (c & 0x80) == 0 )
        {
            wc = c;
            ++i;
        }
        else if ( (c & 0xE0) == 0xC0 )
        {
            wc = (s[i] & 0x1F) << 6;
            wc |= (s[i+1] & 0x3F);
            i += 2;
        }
        else if ( (c & 0xF0) == 0xE0 )
        {
            wc = (s[i] & 0xF) << 12;
            wc |= (s[i+1] & 0x3F) << 6;
            wc |= (s[i+2] & 0x3F);
            i += 3;
        }
        else if ( (c & 0xF8) == 0xF0 )
        {
            wc = (s[i] & 0x7) << 18;
            wc |= (s[i+1] & 0x3F) << 12;
            wc |= (s[i+2] & 0x3F) << 6;
            wc |= (s[i+3] & 0x3F);
            i += 4;
        }
        else if ( (c & 0xFC) == 0xF8 )
        {
            wc = (s[i] & 0x3) << 24;
            wc |= (s[i] & 0x3F) << 18;
            wc |= (s[i] & 0x3F) << 12;
            wc |= (s[i] & 0x3F) << 6;
            wc |= (s[i] & 0x3F);
            i += 5;
        }
        else if ( (c & 0xFE) == 0xFC )
        {
            wc = (s[i] & 0x1) << 30;
            wc |= (s[i] & 0x3F) << 24;
            wc |= (s[i] & 0x3F) << 18;
            wc |= (s[i] & 0x3F) << 12;
            wc |= (s[i] & 0x3F) << 6;
            wc |= (s[i] & 0x3F);
            i += 6;
        }
        ws += wc;
    }
    return ws;
}
/*
x'=(x-a)cosα+(y-b)sinα+a
y'=-(x-a)sinα+(y-b)cosα+b
*/
using namespace Json;

GLfloat* get_txt_uvs(const char* data_file, int& retn_len)
{
	GLfloat* puvs = NULL;
	Reader reader;
	Value jvalue;
	if (reader.parse(data_file, jvalue, false))
	{
		Value& meta = jvalue["meta"];
		Value& jsize = meta["size"];
		float w = jsize["w"].asInt();
		float h = jsize["h"].asInt();
		Value& frames = jvalue["frames"];
		if (frames.isArray())
		{
			int isize = frames.size();
			retn_len = isize * 8;
			puvs = new GLfloat[retn_len];
			for (int ix = 0; ix < isize; ix++)
			{
				Value& frame_unit = frames[ix];
				Value& frame = frame_unit["frame"];
				int sbidx = ix * 8;
				float x0 = frame["x"].asInt();
				float y0 = frame["y"].asInt();
				float x1 = x0 + frame["w"].asInt();
				float y1 = y0 + frame["h"].asInt();

				puvs[sbidx + 0] = x0 / w;
				puvs[sbidx + 1] = y0 / h;

				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y0 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y1 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y1 / h;
			}
		}
		if (frames.isObject())
		{
			Value::Members memb(frames.getMemberNames());
			retn_len = memb.size() * 8;
			puvs = new GLfloat[retn_len];
			int idx = 0;
			for (auto itmem = memb.begin(); itmem != memb.end(); ++itmem, ++idx)
			{
				auto& mname = *itmem;
				Value& junif = frames[mname];
				Value& frame = junif["frame"];
				int sbidx = idx * 8;
				float x0 = frame["x"].asInt();
				float y0 = frame["y"].asInt();
				float x1 = x0 + frame["w"].asInt();
				float y1 = y0 + frame["h"].asInt();

				puvs[sbidx + 0] = x0 / w;
				puvs[sbidx + 1] = y0 / h;

				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y0 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y1 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y1 / h;
				/*puvs[sbidx + 0] = x1 / w;
				puvs[sbidx + 1] = y0 / h;
				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y1 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y0 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y0 / h;*/

			}
		}
		
	}
	return puvs;
}
void get_txt_uv_vector(const char* data_file, vres_txt_cd& vtxt_cd)
{
	Reader reader;
	Value jvalue;
	if (reader.parse(data_file, jvalue, false))
	{
		Value& meta = jvalue["meta"];
		Value& jsize = meta["size"];
		float w = jsize["w"].asInt();
		float h = jsize["h"].asInt();
		Value& frames = jvalue["frames"];
		if (frames.isArray())
		{
			int isize = frames.size();
			for (int ix = 0; ix < isize; ix++)
			{
				vtxt_cd.emplace_back();
				res_texture_coordinate& res_txt_cd = vtxt_cd[ix];
				Value& frame_unit = frames[ix];
				Value& frame = frame_unit["frame"];
				Value& filename = frame_unit["filename"];
				res_txt_cd._file_name = filename.asString();

				bool rotated = frame["rotated"].asBool();
				res_txt_cd._x0 = frame["x"].asInt();
				res_txt_cd._y0 = frame["y"].asInt();
				res_txt_cd._x1 = frame["x"].asInt() + frame["w"].asInt();
				res_txt_cd._y1 = frame["y"].asInt() + frame["h"].asInt();

				if (rotated)
				{
					res_txt_cd._x1 = frame["x"].asInt() + frame["h"].asInt();;
					res_txt_cd._y1 = frame["y"].asInt() + frame["w"].asInt();
				}
			}
		}
		if (frames.isObject())
		{
			Value::Members memb(frames.getMemberNames());
			int idx = 0;
			for (auto itmem = memb.begin(); itmem != memb.end(); ++itmem, ++idx)
			{
				vtxt_cd.emplace_back();
				res_texture_coordinate& res_txt_cd = vtxt_cd[idx];
				auto& mname = *itmem;
				Value& junif = frames[mname];
				Value& frame = junif["frame"];
				Value& filename = junif["filename"];
				res_txt_cd._file_name = filename.asString();

				bool rotated = frame["rotated"].asBool();
				res_txt_cd._x0 = frame["x"].asInt();
				res_txt_cd._y0 = frame["y"].asInt();
				res_txt_cd._x1 = frame["x"].asInt() + frame["w"].asInt();
				res_txt_cd._y1 = frame["y"].asInt() + frame["h"].asInt();

				if (rotated)
				{
					res_txt_cd._x1 = frame["x"].asInt() + frame["h"].asInt();;
					res_txt_cd._y1 = frame["y"].asInt() + frame["w"].asInt();
				}
			}
		}

	}
}
ImVec2 rotate_point_by_zaxis(ImVec2& tar, float angle, ImVec2& basePoint)
{
	ImVec2 des;
	des.x = (tar.x - basePoint.x)*cos(angle) + (tar.y - basePoint.y)*sin(angle) + basePoint.x;
	des.y = -(tar.x - basePoint.x)*sin(angle) + (tar.y - basePoint.y)*cos(angle) + basePoint.y;
	return des;
}
bool prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	GLint last_fmid;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &last_fmid );
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加 纹理 color attachment
	colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGBA, frame_width, frame_height, GL_RGBA, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
	// 附加 depth stencil texture attachment
	depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8, frame_width,
		frame_height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
	// 检测完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer( GL_FRAMEBUFFER, last_fmid );
		return false;
	}
	glBindFramebuffer( GL_FRAMEBUFFER, last_fmid );
	return true;
}
bool af_prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	GLint last_fmid;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &last_fmid );
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加 纹理 color attachment
	glGenTextures(1, &colorTextId);
	glBindTexture(GL_TEXTURE_2D, colorTextId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, frame_width, frame_height, 0, GL_DEPTH_COMPONENT,GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,colorTextId, 0);

	// 附加 depth stencil texture attachment
	glGenTextures(1, &depthStencilTextId);
	glBindTexture(GL_TEXTURE_2D, depthStencilTextId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, frame_width, frame_height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);

	// 检测完整性
	auto chk = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (chk != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer( GL_FRAMEBUFFER, last_fmid );
		printf("chk=0x%x\n", chk);
		return false;
	}
	glBindFramebuffer( GL_FRAMEBUFFER, last_fmid );
	return true;
}
bool frame_buffer()
{
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	GLuint rboDepthStencil;
	glGenRenderbuffers(1, &rboDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	return true;
}
/*
* 附加纹理到Color Attachment
* 同时附加RBO到depth stencil Attachment
*/
bool prepareFBO2(GLuint& textId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加纹理 color attachment
	textId = TextureHelper::makeAttachmentTexture(0, GL_RGB, frame_width, frame_height, GL_RGB, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textId, 0);
	// 附加 depth stencil RBO attachment
	GLuint rboId;
	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		frame_width, frame_height); // 预分配内存
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
unsigned int conver_track_buff_to_pair(char* pbuff,unsigned int buff_len, vector<ImVec2>& vtrack0, vector<ImVec2>& vtrack1)
{
	int* parray_len = (int*)pbuff;
	int array_len = *parray_len;
	int wtrack_sz = sizeof(ImVec2)*array_len ;
	int bf_track_sz = buff_len - sizeof(int);
	if (wtrack_sz* 2!=bf_track_sz)
	{
		printf("invalid slider track buff\n");
		return 0;
	}
	char* phead0 = pbuff + sizeof(int);
	vtrack0.resize(array_len);
	memcpy(&vtrack0[0], phead0, wtrack_sz);
	char* phead1 = phead0 + wtrack_sz;
	vtrack1.resize(array_len);
	memcpy(&vtrack1[0], phead1, wtrack_sz);
	return array_len;
}