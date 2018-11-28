#include "basic3d.h"

/*
        3              2


    0             1


	    7              6


	4              5
*/
#ifdef _TEST_MESH
struct base_vertex
{
	ImVec3 position;
	ImVec3 vnormal;
	base_vertex(float x, float y, float z) :position(x, y, z){}
	//base_vertex(){ vnormal = glm::vec3(0.f); }
};

struct tri_face
{
	unsigned short vidx[3];
	tri_face(){ vidx[0] = vidx[1] = vidx[2] = 0; }
	tri_face(int i0, int i1, int i2){ vidx[0] = i0; vidx[1] = i1; vidx[2] = i2; }
};
struct tri_mesh 
{
	ImVector<base_vertex> vertices;
	ImVector<tri_face> faces;
	GLuint _vbo, _vao, _ebo;
	tri_mesh();
	~tri_mesh();
};
tri_mesh::tri_mesh()
{
	glGenVertexArrays(1, &_vao);
	//vbo
	glGenBuffers(1, &_vbo);
	//ebo;
	glGenBuffers(1, &_ebo);
}
tri_mesh::~tri_mesh()
{
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}
void init_cube_trimesh_faces(tri_mesh& cube_trmesh)
{
	if (cube_trmesh.vertices.size()!=8)
	{
		printf("fail to pick up cube faces because the number of vertices is %d!\n", cube_trmesh.vertices.size());
		return;
	}
#define FACE_INIT(x,y,z) cube_trmesh.faces.push_back(tri_face(x, y, z))
	FACE_INIT(0, 4, 5);
	FACE_INIT(5, 1, 0);

	FACE_INIT(3, 2, 6);
	FACE_INIT(6, 7, 3);

	FACE_INIT(7, 4, 0);
	FACE_INIT(0, 3, 7);

	FACE_INIT(6, 2, 1);
	FACE_INIT(1, 5, 6);

	FACE_INIT(0, 1, 2);
	FACE_INIT(2, 3, 0);

	FACE_INIT(4, 7, 6);
	FACE_INIT(6, 5, 4);

}
void tri_mesh_normalize(tri_mesh& trmesh)
{
	for (auto& ia:trmesh.faces)
	{
		ImVec3 ev1=trmesh.vertices[ia.vidx[0]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 e1 = { ev1.x, ev1.y, ev1.z };
		ImVec3 ev2=trmesh.vertices[ia.vidx[2]].position - trmesh.vertices[ia.vidx[1]].position;
		glm::vec3 e2 = { ev2.x, ev2.y, ev2.z };
		glm::vec3 no = glm::cross(e1, e2);
		ImVec3 eno = { no.x, no.y, no.z };
		trmesh.vertices[ia.vidx[0]].vnormal += eno;
		trmesh.vertices[ia.vidx[1]].vnormal += eno;
		trmesh.vertices[ia.vidx[2]].vnormal += eno;
	}
	for (auto& ib:trmesh.vertices)
	{
		glm::vec3 enm= { ib.vnormal.x, ib.vnormal.y, ib.vnormal.z };
		glm::vec3 enmn = glm::normalize(enm);
		ib.vnormal = {enmn.x,enmn.y,enmn.z};
	}
}
#endif
int attr_size[en_attr_type_count]=
{
	//en_attr_float,
	1,
	//en_attr_vec2,
	2,
	//en_attr_vec3,
	3,
	//en_attr_vec4,
	4,
};

bool basic_shader::loading_shader_attributes_from_avbo(GLuint vao, GLuint vbo, const GLvoid* pvertices, int cnt_vertices, vector<string>& attr_name_list, GLuint ebo, GLushort* pindices, int cnt_indics)
{
	//glUseProgram(_shader_program_id);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cnt_vertices, pvertices, GL_STATIC_DRAW);
	if (ebo!=0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cnt_indics, pindices, GL_STATIC_DRAW);

	}
	int stride = 0;
	for (auto& attr_name : attr_name_list)
	{
		auto& it_attr = _mattr_list.find(attr_name);
		if (it_attr==_mattr_list.end())
		{
			printf("invalid attribute name:%s\n", attr_name);
			return false;
		}		
		shader_attribute& attr = it_attr->second;
		stride += attr_size[attr._type];
	}

	int pointer = 0;
	for (auto& attr_name : attr_name_list)
	{

		shader_attribute& attr = _mattr_list[attr_name];
		glEnableVertexAttribArray(attr._location);
		glVertexAttribPointer(attr._location, attr_size[attr._type], GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(pointer*sizeof(GLfloat)));
		pointer += attr_size[attr._type];
	}

	return true;
}

bool basic_shader::set_uniform(string& unf_name, GLsizei icnt, float* falue)
{
	auto tt = _munf_list.find(unf_name);
	if (tt == _munf_list.end())
	{
		printf("fail to find attr:%s\n", unf_name.c_str());
		return false;
	}
	auto& unif = tt->second;

	switch (unif._type)
	{
	case en_unf_float:
		glUniform1fv(unif._location, icnt, falue);
		break;
	case en_unf_vec2:
		glUniform2fv(unif._location, icnt, falue);
		break;
	case en_unf_vec3:
		glUniform3fv(unif._location, icnt, falue);
		break;
	case en_unf_vec4:
		glUniform4fv(unif._location, icnt, falue);
		break;
	case en_unf_mat4:
		glUniformMatrix4fv(unif._location, icnt, GL_FALSE, falue);
		break;
	case en_unf_tex:
		glUniform1i(unif._location, icnt);
		break;
	default:
		printf("invalilde type\n");
		return false;
		break;
	}
	return true;
}
map<string, vector<unique_ptr<basic_shader>>> g_shader_list;
Factory<basic_shader>& get_shader_fc()
{
   static Factory<basic_shader> shader_factory;
   return shader_factory;
}
void instantiating_internal_shader()
{
	get_shader_fc().iterate_types([](string key, std::unique_ptr<basic_shader>& shd_insntance){
		//auto sss = shd_insntance;
		g_shader_list[key].push_back(std::move(shd_insntance));
	});
}