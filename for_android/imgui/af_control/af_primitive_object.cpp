#include "af_primitive_object.h"
#include "common_functions.h"
const char* draw_mode[ en_gl_count ] =
{
     "GL_POINTS",
     "GL_LINES",
     "GL_LINE_LOOP",
     "GL_LINE_STRIP",
     "GL_TRIANGLES",
     "GL_TRIANGLE_STRIP",
     "GL_TRIANGLE_FAN"
};
const char* str_trans_order[ en_trans_order_cnt ] =
{
     "translate_scale_rotate",
     "translate_rotate_scale",
     "scale_translate_rotate",
     "scale_rotate_translate",
     "rotate_translate_scale",
     "rotate_scale_translate",
};
const char* str_rotate_oder[ en_rotate_order_cnt ] =
{
     "x_y_z",
     "x_z_y",
     "y_x_z",
     "y_z_x",
     "z_x_y",
     "z_y_x",
};
void primitive_object::load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLuint* pele_buff, GLuint ele_cnt,GLuint mem_usage)
{
	_vertex_buf_len = vetexlen;
	_ele_buf_len = ele_cnt;
     _mem_usage = mem_usage;

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_vertex_buf_len, pvertex_data, mem_usage);

	if (pele_buff)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*_ele_buf_len, pele_buff, mem_usage);
	}	

	GLuint idx = 0;
	GLubyte stride = get_stride();
	int pointer = 0;

	for (auto& el : _ele_format)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, el, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(pointer*sizeof(GLfloat)));//
		pointer += el;
		idx++;
	}

	glBindVertexArray(0);
}
void primitive_object::enableVertex()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	if (_ele_buf_len > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	}

	GLuint idx = 0;
	GLubyte stride = get_stride();
	int pointer = 0;

	for (auto& el : _ele_format)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, el, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(pointer*sizeof(GLfloat)));//
		pointer += el;
		idx++;
	}
}
mp_primitive g_primitive_list;
#if !defined(DISABLE_DEMO)
const float half_side_len = 0.5f;
GLfloat _vertices[] = {
	-half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	half_side_len, -half_side_len, -half_side_len, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	half_side_len, -half_side_len, half_side_len, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-half_side_len, -half_side_len, half_side_len, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, -half_side_len, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	half_side_len, half_side_len, -half_side_len, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	half_side_len, half_side_len, half_side_len, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-half_side_len, half_side_len, half_side_len, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
};

GLuint _indices[] = {
	0, 4, 5,
	5, 1, 0,

	3, 2, 6,
	6, 7, 3,

	7, 4, 0,
	0, 3, 7,

	6, 2, 1,
	1, 5, 6,

	0, 1, 2,
	2, 3, 0,

	4, 7, 6,
	6, 5, 4
};
GLfloat _plain_vertices[] = {
	-half_side_len, -half_side_len, 0.f, 0.0f, 1.0f,
	half_side_len, -half_side_len, 0.f, 1.0f, 1.0f,
	-half_side_len, half_side_len, 0.f, 0.0f, 0.0f,
	half_side_len, half_side_len, 0.f, 1.0f, 0.0f,
};
GLfloat _direction[] =
{
	0, 0, 0,
	1.f, 0, 0,
};
GLfloat _arrow[] =
{
	0.75f, -0.25f, 0.f,
	1.f, 0.f, 0.f,
	0.75f, 0.25f, 0.f,
};
struct internal_prm  
{
	string _name;
	vector<GLubyte> _fmt;
	GLfloat* _pvert;
	GLuint _vert_cnt;
	GLuint* _ele;
	GLuint _ele_cnt;
};
static internal_prm internal_primitive_name_list[] =
{
	{ "cube", { 3, 3, 2 }, _vertices, sizeof(_vertices) / sizeof(GLfloat), _indices, sizeof(_indices) / sizeof(GLuint) },
	{ "plain", {3,2},_plain_vertices,sizeof(_plain_vertices)/sizeof(GLfloat),0,0 },
};
bool ref_a_intenal_primitive(string& prm_name)
{
	auto& itn_prm_lst = internal_primitive_name_list;
	GLuint isz = sizeof(itn_prm_lst) / sizeof(internal_prm);

	for (int ix = 0; ix < isz;ix++)
	{
		auto& prm_un = itn_prm_lst[ix];
		if (prm_name == prm_un._name)
		{
			auto ps_prm = make_shared<primitive_object>();
			ps_prm->set_ele_format(prm_un._fmt);
			ps_prm->load_vertex_data(prm_un._pvert, prm_un._vert_cnt, prm_un._ele, prm_un._ele_cnt);
			auto buff_len = 4 + prm_un._vert_cnt*sizeof(float) + prm_un._ele_cnt*sizeof(GLuint);
			ps_af_file ps_file = make_shared<af_file>(buff_len);
			char* phead = (char*)ps_file->_pbin;
			GLuint* phead_len = (GLuint*)phead;
			*phead_len = prm_un._vert_cnt*sizeof(float);
			phead += 4;
			memcpy(phead, prm_un._pvert, *phead_len);
			if (prm_un._ele_cnt)
			{
				phead += *phead_len;
				memcpy(phead, prm_un._ele, prm_un._ele_cnt*sizeof(GLuint));
			}
			prm_name = find_a_key_from_mp(g_primitive_list, prm_name);
			prm_name = find_a_key_from_mp(g_mfiles_list, prm_name);
			ps_prm->_file_name = prm_name;
			ps_prm->_ps_file = ps_file;
			g_mfiles_list[prm_name]= ps_file;
			g_primitive_list[prm_name] = ps_prm;
		    
			return true;
		}
	}
	return false;
}

#endif
ps_primrive_object get_prm_object(const char* prm_name)
{
	auto iprm = g_primitive_list.find(prm_name);
	if (iprm != g_primitive_list.end())
	{
		return iprm->second;	
	}
	return nullptr;
}

bool get_prm_data(const char* prm_name, float* pbuff, uint32_t buff_len)
{
	const auto& iprm = g_primitive_list.find(prm_name);
	if (iprm==g_primitive_list.end())
	{
		return false;
	}
	auto& ps_prm_obj = iprm->second;
	auto& ps_file = ps_prm_obj->_ps_file;
	char* phead = (char*)ps_file->_pbin;
	GLuint* phead_buff_len = (GLuint*)phead;
	phead += 4;
	assert(*phead_buff_len == buff_len * sizeof(float)&&"cannot get primitive data for invalid buff_len");
	memcpy(pbuff, phead, *phead_buff_len);
	return true;
}
bool update_prm_vbo(const char* prm_name, float* pvertex, uint32_t buff_len)
{
	const auto& iprm = g_primitive_list.find(prm_name);
	if (iprm == g_primitive_list.end())
	{
		return false;
	}
	auto& prm_obj = *iprm->second;
	assert(buff_len == prm_obj._vertex_buf_len&&"cannot update vbo for invalid buff_len");
	glBindBuffer(GL_ARRAY_BUFFER, prm_obj._vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*prm_obj._vertex_buf_len, pvertex, GL_DYNAMIC_DRAW);
	return true;
}