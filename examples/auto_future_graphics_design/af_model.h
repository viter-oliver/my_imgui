#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>
using namespace std;
struct af_mesh
{
	vector<string> _text_diffuse_list;
	vector<string> _text_specular_list;
	vector<string> _text_height_list;
	vector<string> _text_ambient_list;
	string _prm_id;
};
extern const char* assimp_support_format;
using af_model = vector<af_mesh>;
using mp_model=map<string, shared_ptr<af_model>>;
extern mp_model g_mmodel_list;
//extern const char* model_group_name;
void init_model_group_from_json(const char* pjson_buff);
void save_model_grop_to_json();
bool import_models(const char* md_file_name);