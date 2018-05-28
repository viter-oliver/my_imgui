#pragma once
#include "ft_base.h"
#include "basic3d.h"
#include "Mesh.h"
class ft_model :
	public ft_base
{
	vector<Vertex> _vertData;
	//string _texture_file;
	//string _mesh_data_file;
	struct intl_pt
	{
		ImVec3 _scale;
		ImVec3 _rotation;
		ImVec2 _translation;
		char _texture_file[FILE_NAME_LEN];
		char _mesh_data_file[FILE_NAME_LEN];
		GLuint _textureId;
		int _txt_width, _txt_height;
	//int _shader_instance_index;
		intl_pt()
			//:_texture_file({ 'V', 'W' })//_Golf_VII_2013.png")
			//, _mesh_data_file("golf.FBX")
		{
			strcpy(_texture_file, "VW_Golf_VII_2013.png");
			strcpy(_mesh_data_file, "golf.FBX");
		}
	};
	intl_pt _pt;
	basic_shader* _pshader;
	static GLuint _vao, _vbo;
	static unsigned short _model_instance_cnt;
public:
	ft_model();
	~ft_model();
	int collect_property_range(vproperty_list& vplist)
	{
		int plen = ft_base::collect_property_range(vplist);
		int len = sizeof(intl_pt);
		vplist.emplace_back(&_pt, len);
		len += plen;
		return len;

	}
	void load_mesh_data_2_vertices();
	void load_vertics_2_vbo();
	void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void draw_peroperty_page();
	bool init_from_json(Value& jvalue);
	bool init_json_unit(Value& junit);
#endif
};

REGISTER_CONTROL(ft_model)