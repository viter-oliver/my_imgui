#include "fbx_save_info.h"
#include "material.h"
#include "res_output.h"
#include "primitive_object.h"

#if !defined(IMGUI_WAYLAND)
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "imguidock.h"
#include "FbxImporter.h"
#endif

//保存FBX结构
AllFbxInfoDef fbx_info_map;
fbx_name_list g_fbx_name_vector;

fbx_info _info;
mesh_info _mesh_detail_info;

void display_model(FBXImporterUnmanaged::FBXModelPtr mode)
{
	for (int i = 0; i < mode->GetChildCount(); ++i)
	{
		FBXImporterUnmanaged::FBXModelPtr _mode = mode->GetChild(i);
		if (_mode->GetIsMesh())
		{

			_mesh_detail_info.clear();
			string _tmp = _mode->GetName();
			_mesh_detail_info._mesh_name = _tmp;
			GLfloat *mesh_vertext_data = nullptr;
			mesh_vertext_data = new GLfloat[_mode->GetVertexCount() * 8];
			memset(mesh_vertext_data, 0, _mode->GetVertexCount() * 8 * sizeof(GLfloat));

			//vertext uv and normal count same, else is 0
			for (auto j = 0; j < _mode->GetVertexCount(); ++j)
			{
				mesh_vertext_data[8 * j + 0] = _mode->GetVertex(j).x;
				mesh_vertext_data[8 * j + 1] = _mode->GetVertex(j).y;
				mesh_vertext_data[8 * j + 2] = _mode->GetVertex(j).z;

				if (_mode->GetNormalCount() >= _mode->GetVertexCount()) 
				{
					mesh_vertext_data[8 * j + 3] = _mode->GetNormal(j).x;
					mesh_vertext_data[8 * j + 4] = _mode->GetNormal(j).y;
					mesh_vertext_data[8 * j + 5] = _mode->GetNormal(j).z;
				}
				if (_mode->GetCustomUVCount() >= _mode->GetVertexCount())
				{
					mesh_vertext_data[8 * j + 6] = _mode->GetCustomUV(j).x;
					mesh_vertext_data[8 * j + 7] = 1 - _mode->GetCustomUV(j).y;
				}
			}

			g_primitive_list[_tmp] = make_shared<primitive_object>();
			g_primitive_list[_tmp]->set_ele_format({ 3, 3, 2 });
			g_primitive_list[_tmp]->load_vertex_data(mesh_vertext_data, _mode->GetVertexCount() * 8);
			delete[] mesh_vertext_data;

			for (auto i = 0; i < _mode->GetMaterialCount(); ++i)
			{
				std::string shaderName = _mode->GetMaterial(i)->GetString("ReflectionModel");
				std::string materialname = _mode->GetMaterial(i)->GetName();
				std::string materialrealname = materialname;
				if ("Phong" != shaderName)
				{
					continue;
				}
				auto it = g_material_list.find(materialrealname);
				if (it == g_material_list.end())
				{
					string str = _mode->GetMaterial(i)->GetString("FbxTexture", _mode->GetMaterial(i)->GetName());
					if ("" != str)
					{
					TO_LOAD_IMAGE:
						if (!add_image_to_mtexure_list(str))
						{
							int result = MessageBox(GetForegroundWindow(), str.c_str(), "Load image failed, To load again?", MB_YESNOCANCEL);
							if (result == IDYES)
							{
								OPENFILENAME ofn = { sizeof(OPENFILENAME) };
								ofn.hwndOwner = GetForegroundWindow();
								ofn.lpstrFilter = "valid file(.png):\0*.png\0\0";
								char strFileName[MAX_PATH] = { 0 };
								ofn.nFilterIndex = 1;
								ofn.lpstrFile = strFileName;
								ofn.nMaxFile = sizeof(strFileName);
								ofn.lpstrTitle = "select a picture please!";
								ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
								if (GetOpenFileName(&ofn))
								{
									str = strFileName;
									goto TO_LOAD_IMAGE;
								}
							}
						}
						_mesh_detail_info._texture_name = str.substr(str.find_last_of('\\') + 1);
					}
					_mesh_detail_info._material_name = materialname;

					if ("" != str)
					{
						shaderName += "Textured";
					}
					_mesh_detail_info._shader_mode = shaderName;
					if (!create_material(shaderName, materialname, materialrealname))
					{
						MessageBox(GetForegroundWindow(), materialrealname.c_str(), "Load material failed", MB_OK);
						continue;
					}
				}

				{
					float opacity[1];
					opacity[0] = _mode->GetMaterial(i)->GetFloat("Opacity");
					g_material_list[materialrealname]->set_value("Opacity", opacity, 1);

					float ambient[4];
					ambient[0] = _mode->GetMaterial(i)->GetVector3("Ambient").x;
					ambient[1] = _mode->GetMaterial(i)->GetVector3("Ambient").y;
					ambient[2] = _mode->GetMaterial(i)->GetVector3("Ambient").z;
					ambient[3] = 1.f;
					g_material_list[materialrealname]->set_value("Ambient", ambient, 4);

					float diffuse[4];
					diffuse[0] = _mode->GetMaterial(i)->GetVector3("Diffuse").x;
					diffuse[1] = _mode->GetMaterial(i)->GetVector3("Diffuse").y;
					diffuse[2] = _mode->GetMaterial(i)->GetVector3("Diffuse").z;
					diffuse[3] = 1.f;
					g_material_list[materialrealname]->set_value("Diffuse", diffuse, 4);
					float emissive[4];
					emissive[0] = _mode->GetMaterial(i)->GetVector3("Emissive").x;
					emissive[1] = _mode->GetMaterial(i)->GetVector3("Emissive").y;
					emissive[2] = _mode->GetMaterial(i)->GetVector3("Emissive").z;
					emissive[3] = 1.f;
					g_material_list[materialrealname]->set_value("Emissive", emissive, 4);

					float specular[4];
					specular[0] = _mode->GetMaterial(i)->GetVector3("Specular").x;
					specular[1] = _mode->GetMaterial(i)->GetVector3("Specular").y;
					specular[2] = _mode->GetMaterial(i)->GetVector3("Specular").z;
					specular[3] = 1.f;
					g_material_list[materialrealname]->set_value("Specular", specular, 4);

					float Shiniess[1];
					Shiniess[0] = _mode->GetMaterial(i)->GetFloat("Shiniess");
					g_material_list[materialrealname]->set_value("Shiniess", Shiniess, 1);
				}
				
			}
			_info._mesh_info.push_back(_mesh_detail_info);
		}
	}

	if (mode->GetChildCount())
	{
		for (auto i = 0; i < mode->GetChildCount(); i++)
		{
			display_model(mode->GetChild(i));
		}
	}
}

#include <fstream>
extern string g_cureent_directory;
void import_fbx_info(const char *fbx_path)
{
	FBXImporterUnmanaged::FBXImporter m_oImporter;
	FBXImporterUnmanaged::FBXScenePtr _scence = m_oImporter.ImportFile(fbx_path);
	if (NULL == _scence) {
		MessageBox(GetForegroundWindow(), fbx_path, "Load fbx failed", MB_OK);
		return;
	}

	_info.clear();
	_info._mesh_count = m_oImporter.getMeshCount();
	display_model(_scence->GetModel());
	fbx_info_map[fbx_path] = _info;
	g_fbx_name_vector.push_back(fbx_path);
}

void load_fbx_file()
{
	fbx_info_map.clear();

	ifstream ifin(g_cureent_directory + "files\\" + "fbx.json", ios::in);
	if (ifin.is_open())
	{
		Reader FReader;
		Value root;
		Value FBX_File(arrayValue);

		if (FReader.parse(ifin, root, false))
		{
			FBX_File = root["fbx_root"];

			for (auto i = 0; i < FBX_File.size(); ++i)
			{
				g_fbx_name_vector.push_back(FBX_File[i]["fbx_name"].asCString());

				Value MeshInfo(arrayValue); //mesh信息数组
				MeshInfo = FBX_File[i]["mesh_info"];

				fbx_info _fbx_info;
				_fbx_info._mesh_count = FBX_File[i]["mesh_count"].asInt();

				for (auto j = 0; j < MeshInfo.size(); ++j)
				{
					mesh_info _mesh_info;
					_mesh_info._mesh_name = MeshInfo[j]["mesh_name"].isString()? MeshInfo[j]["mesh_name"].asCString():"";
					_mesh_info._material_name = MeshInfo[j]["material_name"].isString() ? MeshInfo[j]["material_name"].asCString() : "";
					_mesh_info._shader_mode = MeshInfo[j]["shader_mode"].isString() ? MeshInfo[j]["shader_mode"].asCString() : "";
					_mesh_info._texture_name = MeshInfo[j]["texture_name"].isString() ? MeshInfo[j]["texture_name"].asCString() : "";

					_fbx_info._mesh_info.push_back(_mesh_info);
				}

				fbx_info_map[FBX_File[i]["fbx_name"].asCString()] = _fbx_info;
			}
		}
		ifin.close();
	}
}

void save_fbx_file()
{
	ofstream ofin(g_cureent_directory + "files\\" + "fbx.json", ios::out | ios::trunc);
	if (ofin.is_open())
	{
		Value root;
		Value FBX_File(arrayValue); //fbx所有文件已数组存在
		for (auto it = fbx_info_map.begin(); it != fbx_info_map.end(); it++)
		{
			Value One_FBX_Info(objectValue); //每个fbx中包含的信息
			
			One_FBX_Info["fbx_name"] = it->first;
			One_FBX_Info["mesh_count"] = it->second._mesh_count;

			Value MeshInfo(arrayValue); //mesh信息数组

			for (auto i = 0; i < it->second._mesh_count; ++i)
			{
				Value mesh_file(objectValue);
				mesh_file["mesh_name"] = it->second._mesh_info[i]._mesh_name;
				if ("" != it->second._mesh_info[i]._material_name)
				{
					mesh_file["material_name"] = it->second._mesh_info[i]._material_name;
					mesh_file["shader_mode"] = it->second._mesh_info[i]._shader_mode;
					if ("" != it->second._mesh_info[i]._texture_name)
						mesh_file["texture_name"] = it->second._mesh_info[i]._texture_name;
				}

				MeshInfo.append(mesh_file);
			}
			
			One_FBX_Info["mesh_info"] = MeshInfo;
			FBX_File.append(One_FBX_Info);
		}
		root["fbx_root"] = FBX_File;
		ofin.clear(); //必须clear，否则写不到文件中
		ofin << root << endl;
		ofin.close();
	}

	auto it = g_mfiles_list.find(g_cureent_directory + "files\\" + "fbx.json");
	if (it == g_mfiles_list.end())
	{
		add_file_to_mfiles_list(g_cureent_directory + "files\\" + "fbx.json");
	}
}