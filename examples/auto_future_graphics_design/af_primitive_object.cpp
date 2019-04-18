#include "af_primitive_object.h"
#include "common_functions.h"
void primitive_object::load_vertex_data(GLfloat* pvertex_data, GLuint vetexlen, GLuint* pele_buff, GLuint ele_cnt)
{
	_vertex_buf_len = vetexlen;
	_ele_buf_len = ele_cnt;
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_vertex_buf_len, pvertex_data, GL_STATIC_DRAW);
	if (pele_buff)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*_ele_buf_len, pele_buff, GL_STATIC_DRAW);
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
mp_primitive g_primitive_list;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
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
			g_mfiles_list[prm_name]= ps_file;
			g_primitive_list[prm_name] = ps_prm;
			return true;
		}
	}
	return false;
}

#endif