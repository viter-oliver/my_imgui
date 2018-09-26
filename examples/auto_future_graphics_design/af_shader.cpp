#include "af_shader.h"

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
af_shader::af_shader(const GLchar* vertex_shader_source, const GLchar* fragment_shader_source)
{
	_vs_code = vertex_shader_source;
	_fs_code = fragment_shader_source;
	build();
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
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
	}
}

void af_shader::refresh_sourcecode(string& vertex_shader_source, string& fragment_shader_source)
{
	if (is_valid())
	{
		glDeleteProgram(_shader_program_id);
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
	}
	_vs_code = vertex_shader_source;
	_fs_code = fragment_shader_source;
	build();
	if (_valid)
	{
		refresh_viarable_list();
	}
}

void af_shader::build()
{
	_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	char* pvs = &_vs_code[0];
	glShaderSource(_vertex_shader, 1, &pvs, NULL);
	glCompileShader(_vertex_shader);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	GLint status;
	char buffer[512];
	glGetShaderiv(_vertex_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(_vertex_shader, 512, NULL, buffer);
		compile_error_info = buffer;
		_valid = false;
		glDeleteShader(_vertex_shader);
		return;
	}
#endif
	//fragment shader
	_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	char* pfs = &_fs_code[0];
	glShaderSource(_fragment_shader, 1, &pfs, NULL);
	glCompileShader(_fragment_shader);
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	glGetShaderiv(_fragment_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(_fragment_shader, 512, NULL, buffer);
		compile_error_info = buffer;
		printf("er:%s\n", compile_error_info.c_str());
		_valid = false;
		glDeleteShader(_vertex_shader);
		glDeleteShader(_fragment_shader);
		return;
	}
#endif
	//link
	_shader_program_id = glCreateProgram();
	glAttachShader(_shader_program_id, _vertex_shader);
	glAttachShader(_shader_program_id, _fragment_shader);
	glBindFragDataLocation(_shader_program_id, 0, "outColor");
	glLinkProgram(_shader_program_id);
	glUseProgram(_shader_program_id);
	_valid = true;
}

void af_shader::refresh_viarable_list()
{
	GLint idx;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 256; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	_att_list.clear();
	_unf_list.clear();
	glGetProgramiv(_shader_program_id, GL_ACTIVE_ATTRIBUTES, &count);
	printf("Active Attributes: %d\n", count);

	for (idx = 0; idx < count; idx++)
	{
		glGetActiveAttrib(_shader_program_id, (GLuint)idx, bufSize, &length, &size, &type, name);

		printf("Attribute #%d Type: %u Name: %s\n", idx, type, name);
		GLuint location = glGetAttribLocation(_shader_program_id, name);
		_att_list[name] = shader_variable(type, location, size);
	}

	glGetProgramiv(_shader_program_id, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (idx = 0; idx < count; idx++)
	{
		glGetActiveUniform(_shader_program_id, (GLuint)idx, bufSize, &length, &size, &type, name);

		printf("Uniform #%d Type: %u Name: %s\n", idx, type, name);
		GLint location = glGetUniformLocation(_shader_program_id, name);
		_unf_list[name] = shader_variable(type, location, size);
	}
}

bool af_shader::vertex_att_pointer(initializer_list<string> att_name_list)
{
	int stride=0;
	for (auto& iname : att_name_list)
	{
		auto& iatt = _att_list.find(iname);
		if (iatt==_att_list.end())
		{
			printf("fail to find attribute name:%s\n", iname);
			return false;
		}
		auto& shd_atr = iatt->second;
		stride += shader_variable_type_size[shd_atr._variable_type]._cnt* shader_variable_type_size[shd_atr._variable_type]._utsize;
	}
	int pointer = 0;
	for (auto& iname : att_name_list)
	{

		auto& attr = _att_list[iname];
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