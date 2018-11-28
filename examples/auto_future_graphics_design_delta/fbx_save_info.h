#pragma once
#include <string>
#include <map>
#include <vector>
#include "FbxImporter.h"
#include "json.h"
using namespace std;
using namespace Json;
struct mesh_info{
	string _mesh_name; //mesh名称
	string _material_name;
	string _shader_mode;
	string _texture_name;

	void clear()
	{
		_mesh_name.clear();
		_material_name.clear();
		_texture_name.clear();
	}
};

struct fbx_info{
	int _mesh_count;
	vector<mesh_info> _mesh_info;

	void clear()
	{
		_mesh_count = 0;
		_mesh_info.clear();
	}
};

typedef map<string, fbx_info> AllFbxInfoDef;
extern AllFbxInfoDef fbx_info_map;

typedef vector<string> fbx_name_list;
extern fbx_name_list g_fbx_name_vector;

void display_model(FBXImporterUnmanaged::FBXModelPtr mode);
void import_fbx_info(const char *fbx_path);

void load_fbx_file();
void save_fbx_file();