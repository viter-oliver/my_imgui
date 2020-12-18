#include "af_shader.h"
#include <sstream>
#include <regex>
#include <algorithm>
struct vtype_size 
{
	int _utsize,_cnt; 
	vtype_size(int us,int ct) :_utsize(us),_cnt(ct){}
	vtype_size() :_utsize(0), _cnt(0){}
};
map < GLenum, vtype_size> shader_variable_type_size = 
{
	{ GL_FLOAT, { sizeof(float), 1 } }, { GL_FLOAT_VEC2, { sizeof(float), 2 } }, { GL_FLOAT_VEC3, { sizeof(float), 3 } }, { GL_FLOAT_VEC3, { sizeof(float), 3 } }, { GL_FLOAT_VEC4, { sizeof(float), 4 } }, \
	{ GL_DOUBLE,{ sizeof(double), 1} }, { GL_DOUBLE_VEC2, { sizeof(double), 2 } }, { GL_DOUBLE_VEC3, { sizeof(double), 3 } }, { GL_DOUBLE_VEC3, { sizeof(double), 3 } }, { GL_DOUBLE_VEC4, { sizeof(double), 4 } }, \
	{ GL_INT, { sizeof(int), 1 }}, { GL_INT_VEC2, { sizeof(int), 2 } }, { GL_INT_VEC3, { sizeof(int), 3 } }, { GL_INT_VEC3, { sizeof(int), 3 } }, { GL_INT_VEC4, { sizeof(int), 4 } }, \
	{ GL_UNSIGNED_INT, { sizeof(unsigned int), 1 } }, { GL_UNSIGNED_INT_VEC2, { sizeof(unsigned int), 2 } }, { GL_UNSIGNED_INT_VEC3, { sizeof(unsigned int), 3 } }, { GL_UNSIGNED_INT_VEC3, { sizeof(unsigned int), 3 } }, { GL_UNSIGNED_INT_VEC4, { sizeof(unsigned int), 4 } }, \
	{ GL_BOOL, { sizeof(bool), 1 }}, { GL_BOOL_VEC2, { sizeof(bool), 2 } }, { GL_BOOL_VEC3, { sizeof(bool), 3 } }, { GL_BOOL_VEC4, { sizeof(bool), 4 } }, \
	{ GL_FLOAT_MAT2, {sizeof(float), 2 * 2 } }, { GL_FLOAT_MAT3, { sizeof(float), 3 * 3 } }, { GL_FLOAT_MAT4, { sizeof(float), 4 * 4 } }, \
	{ GL_FLOAT_MAT2x3, { sizeof(float), 2 * 3 } }, { GL_FLOAT_MAT2x4, { sizeof(float), 2 * 4 } }, { GL_FLOAT_MAT3x2, { sizeof(float), 3 * 2 } }, \
	{ GL_FLOAT_MAT3x4, { sizeof(float), 3 * 4 } }, { GL_FLOAT_MAT4x2, { sizeof(float), 4 * 2 } }, { GL_FLOAT_MAT4x3, { sizeof(float), 4 * 3 } }, \
	{ GL_DOUBLE_MAT2, { sizeof(double), 2 * 2 }}, { GL_DOUBLE_MAT3, { sizeof(double), 3 * 3 } }, { GL_DOUBLE_MAT4, { sizeof(double), 4 * 4 } }, \
	{ GL_DOUBLE_MAT2x3,{ sizeof(double), 2 * 3 }}, { GL_DOUBLE_MAT2x4, { sizeof(double), 2 * 4 } }, { GL_DOUBLE_MAT3x2, { sizeof(double), 3 * 2 } }, \
	{ GL_DOUBLE_MAT3x4, { sizeof(double), 3 * 4 } }, { GL_DOUBLE_MAT4x2, { sizeof(double), 4 * 2 } }, { GL_DOUBLE_MAT4x3, { sizeof(double), 4 * 3 } },
};
/*
GL_SAMPLER_1D	sampler1D
GL_SAMPLER_2D	sampler2D
GL_SAMPLER_3D	sampler3D
GL_SAMPLER_CUBE	samplerCube
GL_SAMPLER_1D_SHADOW	sampler1DShadow
GL_SAMPLER_2D_SHADOW	sampler2DShadow
GL_SAMPLER_1D_ARRAY	sampler1DArray
GL_SAMPLER_2D_ARRAY	sampler2DArray
GL_SAMPLER_1D_ARRAY_SHADOW	sampler1DArrayShadow
GL_SAMPLER_2D_ARRAY_SHADOW	sampler2DArrayShadow
GL_SAMPLER_2D_MULTISAMPLE	sampler2DMS
GL_SAMPLER_2D_MULTISAMPLE_ARRAY	sampler2DMSArray
GL_SAMPLER_CUBE_SHADOW	samplerCubeShadow
GL_SAMPLER_BUFFER	samplerBuffer
GL_SAMPLER_2D_RECT	sampler2DRect
GL_SAMPLER_2D_RECT_SHADOW	sampler2DRectShadow
GL_INT_SAMPLER_1D	isampler1D
GL_INT_SAMPLER_2D	isampler2D
GL_INT_SAMPLER_3D	isampler3D
GL_INT_SAMPLER_CUBE	isamplerCube
GL_INT_SAMPLER_1D_ARRAY	isampler1DArray
GL_INT_SAMPLER_2D_ARRAY	isampler2DArray
GL_INT_SAMPLER_2D_MULTISAMPLE	isampler2DMS
GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	isampler2DMSArray
GL_INT_SAMPLER_BUFFER	isamplerBuffer
GL_INT_SAMPLER_2D_RECT	isampler2DRect
GL_UNSIGNED_INT_SAMPLER_1D	usampler1D
GL_UNSIGNED_INT_SAMPLER_2D	usampler2D
GL_UNSIGNED_INT_SAMPLER_3D	usampler3D
GL_UNSIGNED_INT_SAMPLER_CUBE	usamplerCube
GL_UNSIGNED_INT_SAMPLER_1D_ARRAY	usampler2DArray
GL_UNSIGNED_INT_SAMPLER_2D_ARRAY	usampler2DArray
GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE	usampler2DMS
GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	usampler2DMSArray
GL_UNSIGNED_INT_SAMPLER_BUFFER	usamplerBuffer
GL_UNSIGNED_INT_SAMPLER_2D_RECT	usampler2DRect
GL_IMAGE_1D	image1D
GL_IMAGE_2D	image2D
GL_IMAGE_3D	image3D
GL_IMAGE_2D_RECT	image2DRect
GL_IMAGE_CUBE	imageCube
GL_IMAGE_BUFFER	imageBuffer
GL_IMAGE_1D_ARRAY	image1DArray
GL_IMAGE_2D_ARRAY	image2DArray
GL_IMAGE_2D_MULTISAMPLE	image2DMS
GL_IMAGE_2D_MULTISAMPLE_ARRAY	image2DMSArray
GL_INT_IMAGE_1D	iimage1D
GL_INT_IMAGE_2D	iimage2D
GL_INT_IMAGE_3D	iimage3D
GL_INT_IMAGE_2D_RECT	iimage2DRect
GL_INT_IMAGE_CUBE	iimageCube
GL_INT_IMAGE_BUFFER	iimageBuffer
GL_INT_IMAGE_1D_ARRAY	iimage1DArray
GL_INT_IMAGE_2D_ARRAY	iimage2DArray
GL_INT_IMAGE_2D_MULTISAMPLE	iimage2DMS
GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY	iimage2DMSArray
GL_UNSIGNED_INT_IMAGE_1D	uimage1D
GL_UNSIGNED_INT_IMAGE_2D	uimage2D
GL_UNSIGNED_INT_IMAGE_3D	uimage3D
GL_UNSIGNED_INT_IMAGE_2D_RECT	uimage2DRect
GL_UNSIGNED_INT_IMAGE_CUBE	uimageCube
GL_UNSIGNED_INT_IMAGE_BUFFER	uimageBuffer
GL_UNSIGNED_INT_IMAGE_1D_ARRAY	uimage1DArray
GL_UNSIGNED_INT_IMAGE_2D_ARRAY	uimage2DArray
GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE	uimage2DMS
GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY	uimage2DMSArray
GL_UNSIGNED_INT_ATOMIC_COUNTER	atomic_uint
*/

af_shader::af_shader(GLenum binFormat, void* bin, GLsizei binLength)
{
	_shader_program_id = glCreateProgram();
	glProgramBinary(_shader_program_id, binFormat, bin, binLength);
	GLint success;
	glGetProgramiv(_shader_program_id, GL_LINK_STATUS, &success);
	if (success)
	{
		_valid = true;
	}
	if (_valid)
	{
		refresh_viarable_list();
	}
}

af_shader::~af_shader()
{
	if (is_valid())
	{
		glDeleteProgram(_shader_program_id);
		if (_vertex_shader)
		{
			glDeleteShader(_vertex_shader);
		}
		if (_fragment_shader)
		{
			glDeleteShader(_fragment_shader);
		}
	}
}

af_shader::af_shader(const GLchar* vertex_shader_source, const GLchar* fragment_shader_source)
{
	_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	_shader_program_id = glCreateProgram();
	string vs_code(vertex_shader_source);
	string fs_code(fragment_shader_source);
	build_vs_code(vs_code);
	build_fs_code(fs_code);
	if (_vs_code_valid&&_fs_code_valid)
	{
		link();
	}
	if (_valid)
	{
		refresh_viarable_list();
	}
	
}
static char buffer[512];
bool af_shader::build_vs_code(string& vs_code)
{
	_vs_code = vs_code;
	char* pvs = &vs_code[0];
	glShaderSource(_vertex_shader, 1, &pvs, NULL);
	glCompileShader(_vertex_shader);
	GLint status;
	glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &status);
	buffer[0] = '\0';
	bool be_success = true;
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(_vertex_shader, 512, NULL, buffer);
		printf("vertex shader error:%s\n", buffer);
		_valid = false;
		be_success = false;
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	compile_error_info = buffer;
#endif
	_vs_code_valid = be_success;
	return be_success;
}
bool af_shader::build_fs_code(string& fs_code)
{
	_fs_code = fs_code;
	char* pfs = &fs_code[0];
	glShaderSource(_fragment_shader, 1, &pfs, NULL);
	glCompileShader(_fragment_shader);
	GLint status;
	glGetShaderiv(_fragment_shader, GL_COMPILE_STATUS, &status);
	bool be_success = true;
	buffer[0] = '\0';
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(_fragment_shader, 512, NULL, buffer);
		printf("fragment shader error:%s\n", buffer);
		be_success= false;
		_valid = false;
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	compile_error_info = buffer;
#endif
	_fs_code_valid = be_success;
	return be_success;
}
using vattr = vector<string>;
void collect_attr(string& vscode, vattr& vat)
{
	stringstream sexp(vscode);
	string line;
	std::regex re_layout("layout");
	std::regex re_attribute("attribute");
	std::regex re(".*attribute.*vec\\d\\s*|\\s*;\\s*");
    std::regex re_main("main");
	while (getline(sexp,line))
	{
		if (std::regex_search(line, re_layout))
		{
			printf("layout exists!\n");
			vat.clear();
			return;
		}
		else
		{
			if (std::regex_search(line, re_main))
			{
				return;
			}
			if (std::regex_search(line, re_attribute))
			{
				vat.push_back(std::regex_replace(line, re, ""));
			}
		}
	}
}
string remove_semicolon(const string& star)
{
	string str_rt;
	for (auto& ch:star)
	{
		if (ch != ' '&&ch != ';'&&ch!='\n')
		{
			str_rt += ch;
		}
	}
	return str_rt;
}
void collect_out( string& vscode, attr_vect& vat )
{
     stringstream sexp( vscode );
     string line;
     std::regex re( ".*out.*vec\\d\\s*" );
     std::regex re_main( "main" );
     std::regex re_float( "float" );
     std::regex re_vec2( "vec2" );
     std::regex re_vec3( "vec3" );
     std::regex re_vec4( "vec4" );

     smatch sout;
     while( getline( sexp, line ) )
     {
          if( std::regex_search( line, re_main ) )
          {
               return;
          }
          if( std::regex_search( line, sout,re ) )
          {
               const auto& stype = sout.str( 0 );
               const auto& sname = sout.suffix().str();
               vat.emplace_back();
               auto& attr = vat.back();
			   attr._name = remove_semicolon(sname);
               attr._location = 0;
               if( std::regex_match( stype, re_vec2) )
               {
                    attr._variable_type = GL_FLOAT_VEC2;
                    attr._size = 2 * sizeof(GLfloat);
               }
               else if( std::regex_match( stype, re_vec3 ) )
               {
                    attr._variable_type = GL_FLOAT_VEC3;
                    attr._size = 3 * sizeof(GLfloat);
               }
               else if( std::regex_match( stype, re_vec4 ) )
               {
                    attr._variable_type = GL_FLOAT_VEC4;
                    attr._size = 4 * sizeof(GLfloat);
               }
               else if( std::regex_match( stype, re_float ) )
               {
                    attr._variable_type = GL_FLOAT;
                    attr._size = sizeof(GLfloat);
               }
          }
     }
}
void af_shader::link()
{
	//link
	if (_shader_program_id!=0)
	{
		glDeleteProgram(_shader_program_id);
		_shader_program_id = glCreateProgram();
	}
	_att_list.clear();
	_unf_list.clear();
	_out_list.clear();
	glAttachShader(_shader_program_id, _vertex_shader);
	glAttachShader(_shader_program_id, _fragment_shader);
	vattr vat;
	collect_attr(_vs_code, vat);
	if (vat.size()>0)
	{
		sort(vat.begin(), vat.end());
		for (int ix = 0; ix < vat.size();ix++)
		{
			_att_list.emplace_back();
			auto& attr=_att_list.back();
			attr._name = vat[ix];
			attr._location = ix;
			//attr._variable_type;
			glBindAttribLocation(_shader_program_id, ix, vat[ix].c_str());
		}
	}
	//glBindFragDataLocation(_shader_program_id, 0, "outColor");
	collect_out( _fs_code, _out_list );
	if (_out_list.size()==0)
	{
		collect_out( _vs_code, _out_list );
	}
	specify_transfeedback();
	glLinkProgram(_shader_program_id);
	glReleaseShaderCompiler();
	glUseProgram(_shader_program_id);
	_valid = true;

}

void af_shader::specify_transfeedback( bool relink )
{
	char** feedbackVaryings = new char*[_out_list.size()];
	auto isz = _out_list.size();
	for (int ix = 0; ix < isz;ix++)
	{
	  //GLchar *varyings[] = { "outValue" };
	 	feedbackVaryings[ix] = (char*)_out_list[ix]._name.c_str();
	}
	glTransformFeedbackVaryings(_shader_program_id, isz, (const GLchar* const*)feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	delete[] feedbackVaryings;
	if( relink )
	{
	 	glLinkProgram( _shader_program_id );
	}
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void af_shader::refresh_sourcecode(string& vertex_shader_source, string& fragment_shader_source)
{
	if (is_valid())
	{
		glDeleteProgram(_shader_program_id);
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
	}
	build_vs_code(vertex_shader_source);
	build_fs_code(fragment_shader_source);

	if (_vs_code_valid&&_fs_code_valid)
	{
		link();
	}
	if (_valid)
	{
		refresh_viarable_list();
	}
}
#endif

void af_shader::refresh_viarable_list()
{
	GLint idx;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 256; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	if (_att_list.size()==0)
	{
		glGetProgramiv(_shader_program_id, GL_ACTIVE_ATTRIBUTES, &count);
		printf("Active Attributes: %d\n", count);
		for (idx = 0; idx < count; idx++)
		{
			glGetActiveAttrib(_shader_program_id, (GLuint)idx, bufSize, &length, &size, &type, name);

			printf("Attribute #%d Type: 0x%x Name: %s\n", idx, type, name);
			GLuint location = glGetAttribLocation(_shader_program_id, name);
			//_att_list[name] = shader_variable(type, location, size);
			_att_list.emplace_back(); 
			_att_list.back()={name, type, location, size};
		}
	}
	else
	{
		int count = _att_list.size();
		for (idx = 0; idx < count; idx++)
		{
			glGetActiveAttrib(_shader_program_id, (GLuint)idx, bufSize, &length, &size, &type, name);
			_att_list[idx]._variable_type = type;
			_att_list[idx]._size = size;

		}
	}


	glGetProgramiv(_shader_program_id, GL_ACTIVE_UNIFORMS, &count);
	//printf("Active Uniforms: %d\n", count);

	for (idx = 0; idx < count; idx++)
	{
		glGetActiveUniform(_shader_program_id, (GLuint)idx, bufSize, &length, &size, &type, name);

		//printf("Uniform #%d Type: 0x%x Name: %s\n", idx, type, name);
		GLint location = glGetUniformLocation(_shader_program_id, name);
		_unf_list[name] = shader_variable(type, location, size);
	}
}
bool af_shader::match_format(vector<GLubyte>& fmt)
{
	if (fmt.size()!=_att_list.size())
	{
		return false;
	}
	for (int ix = 0; ix < fmt.size();ix++)
	{
		GLubyte fmt_stride = fmt[ix];
		auto attr_type = _att_list[ix]._variable_type;
		auto cnt = shader_variable_type_size[attr_type]._cnt;
		//auto sz=shader_variable_type_size[attr_type]._utsize;
		GLubyte attr_stride = cnt;
		if (fmt_stride!=attr_stride)
		{
			return false;
		}
	}
	return true;
}
bool af_shader::vertex_att_pointer()
{
	int stride=0;
	for (auto& iattr:_att_list)
	{
		stride += shader_variable_type_size[iattr._variable_type]._cnt* shader_variable_type_size[iattr._variable_type]._utsize;
	}
	
	int pointer = 0;
	for (auto& iattr : _att_list)
	{

		auto& attr = iattr;
		glEnableVertexAttribArray(attr._location);
		auto& shd_tp_sz = shader_variable_type_size[attr._variable_type];
		glVertexAttribPointer(attr._location, shd_tp_sz._cnt, attr._variable_type, GL_FALSE, stride, (void*)(pointer));
		pointer += shd_tp_sz._cnt;
	}
	return true;
}
/*
template<typename T> bool af_shader::uniform(string unf_name, GLsizei icnt, T* pvalue)
{
	auto tt = _unf_list.find(unf_name);
	if (tt == _unf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;
	switch (unif._variable_type)
	{
	case GL_FLOAT:
		glUniform1fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(unif._location, icnt, pvalue);
		break;	
	case GL_FLOAT_VEC4:
		glUniform4fv(unif._location, icnt, pvalue);
		break;	
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT2x3:
		glUniformMatrix2x3fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_MAT3x2:
		glUniformMatrix3x2fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_MAT2x4:
		glUniformMatrix2x4fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_MAT4x2:
		glUniformMatrix4x2fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_MAT3x4:
		glUniformMatrix3x4fv(unif._location, icnt, pvalue);
		break;
	case GL_FLOAT_MAT4x3:
		glUniformMatrix4x3fv(unif._location, icnt, pvalue);
		break;
	case GL_INT:
		glUniform1iv(unif._location, icnt, pvalue);
		break;
	case GL_INT_VEC2:
		glUniform2iv(unif._location, icnt, pvalue);
		break;
	case GL_INT_VEC3:
		glUniform3iv(unif._location, icnt, pvalue);
		break;
	case GL_INT_VEC4:
		glUniform4iv(unif._location, icnt, pvalue);
		break;
	case GL_DOUBLE:
		glUniform1dv(unif._location, icnt, pvalue);
		break;
	case GL_DOUBLE_VEC2:
		glUniform2dv(unif._location, icnt, pvalue);
		break;
	case GL_DOUBLE_VEC3:
		glUniform3dv(unif._location, icnt, pvalue);
		break;
	case GL_DOUBLE_VEC4:
		glUniform4dv(unif._location, icnt, pvalue);
		break;
	case GL_DOUBLE_MAT2:
		glUniformMatrix2dv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT3:
		glUniformMatrix3dv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT4:
		glUniformMatrix4dv(unif._location, icnt, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT2x3:
		glUniformMatrix2x3dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	case GL_DOUBLE_MAT3x2:
		glUniformMatrix3x2dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	case GL_DOUBLE_MAT2x4:
		glUniformMatrix2x4dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	case GL_DOUBLE_MAT4x2:
		glUniformMatrix4x2dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	case GL_DOUBLE_MAT3x4:
		glUniformMatrix3x4dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	case GL_DOUBLE_MAT4x3:
		glUniformMatrix4x3dv(unif._location, icnt, GL_FALSE,pvalue);
		break;
	}
	return true;
}*/
bool af_shader::uniform(string unf_name, float* pvalue)
{
	auto tt = _unf_list.find(unf_name);
	if (tt == _unf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;
	switch (unif._variable_type)
	{
	case GL_FLOAT:
		glUniform1fv(unif._location, unif._size, pvalue);
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(unif._location, unif._size, pvalue);
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(unif._location, unif._size, pvalue);
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(unif._location, unif._size, pvalue);
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT2x3:
		glUniformMatrix2x3fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT3x2:
		glUniformMatrix3x2fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT2x4:
		glUniformMatrix2x4fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT4x2:
		glUniformMatrix4x2fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT3x4:
		glUniformMatrix3x4fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_FLOAT_MAT4x3:
		glUniformMatrix4x3fv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	default:
		printf("unmatched type");
		return false;
	}
	return true;
}
bool af_shader::uniform(string unf_name, int* pvalue)
{
	auto tt = _unf_list.find(unf_name);
	if (tt == _unf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;
	switch (unif._variable_type)
	{

	case GL_INT:
		glUniform1iv(unif._location, unif._size, pvalue);
		break;
	case GL_INT_VEC2:
		glUniform2iv(unif._location, unif._size, pvalue);
		break;
	case GL_INT_VEC3:
		glUniform3iv(unif._location, unif._size, pvalue);
		break;
	case GL_INT_VEC4:
		glUniform4iv(unif._location, unif._size, pvalue);
		break;
	default:
		printf("unmatched type");
		return false;
	}
	return true;
}
 #if defined(GLFW_INCLUDE_GLCOREARB)
bool af_shader::uniform(string unf_name, double* pvalue)
{
	auto tt = _unf_list.find(unf_name);
	if (tt == _unf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;
	switch (unif._variable_type)
	{
	case GL_DOUBLE:
		glUniform1dv(unif._location, unif._size, pvalue);
		break;
	case GL_DOUBLE_VEC2:
		glUniform2dv(unif._location, unif._size, pvalue);
		break;
	case GL_DOUBLE_VEC3:
		glUniform3dv(unif._location, unif._size, pvalue);
		break;
	case GL_DOUBLE_VEC4:
		glUniform4dv(unif._location, unif._size, pvalue);
		break;
	case GL_DOUBLE_MAT2:
		glUniformMatrix2dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT3:
		glUniformMatrix3dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT4:
		glUniformMatrix4dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT2x3:
		glUniformMatrix2x3dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT3x2:
		glUniformMatrix3x2dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT2x4:
		glUniformMatrix2x4dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT4x2:
		glUniformMatrix4x2dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT3x4:
		glUniformMatrix3x4dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	case GL_DOUBLE_MAT4x3:
		glUniformMatrix4x3dv(unif._location, unif._size, GL_FALSE, pvalue);
		break;
	default:
		printf("unmatched type");
		return false;
	}
	return true;
}
 #endif
bool af_shader::uniform(string unf_name, int ivalue)
{
	auto tt = _unf_list.find(unf_name);
	if (tt == _unf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;
	switch (unif._variable_type)
	{
	case GL_SAMPLER_2D:
		glUniform1i(unif._location, ivalue);
		break;
	default:
		printf("unmatched type");
		return false;
	}
	return true;
}

maf_shader g_af_shader_list;