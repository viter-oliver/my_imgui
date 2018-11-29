#pragma once
#include "ft_base.h"
#include "material.h"
#include "Mesh.h"
#include "res_output.h"
#define FILE_NAME_LEN 50
namespace auto_future
{
	class AFG_EXPORT ft_model_3d :
		public ft_base
	{
		
		//string _texture_file;
		//string _mesh_data_file;

		DEF_STRUCT(intl_pt,_pt,
			(ImVec3,_scale),
			(ImVec3, _rotation),
			(ImVec3, _translation), 
			(char,_mesh_data_file[FILE_NAME_LEN]))
		shared_ptr<material> _pmaterial;
		shared_ptr<af_texture> _texture;
		GLuint _vao;
		GLuint _vbo;
		GLuint _tri_cnt;
	public:
		ft_model_3d();
		~ft_model_3d();

		void draw();
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		void draw_peroperty_page(int property_part = -1);
		void load_mesh_data_2_vertices();
		void load_vertics_2_vbo();
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif
	};

	REGISTER_CONTROL(ft_model_3d)
}