#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>
#include "res_output.h"
#include "af_primitive_object.h"
using namespace std;
struct af_mesh
{
	vector<string> _text_diffuse_list;
	vector<string> _text_specular_list;
	vector<string> _text_height_list;
	vector<string> _text_ambient_list;
	string _prm_id;
	vector<ps_af_texture> _ps_text_diffuse_list;
	vector<ps_af_texture> _ps_text_specular_list;
	vector<ps_af_texture> _ps_text_height_list;
	vector<ps_af_texture> _ps_text_ambient_list;
	ps_primrive_object _ps_prm_id;
};
extern const char* assimp_support_format;
using af_model = vector<af_mesh>;
using af_mesh_list = af_model;
using mp_model=map<string, shared_ptr<af_model>>;
extern mp_model g_mmodel_list;
//extern const char* model_group_name;
bool import_models(const char* md_file_name);