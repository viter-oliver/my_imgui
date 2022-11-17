#include "model_edit.h"
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "common_functions.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "dir_output.h"

model_edit::model_edit()
{
	
}

model_edit::~model_edit()
{
}

void model_edit::draw_model_list()
{

	bool model_list_open = ImGui::TreeNode("model", "count:%d", g_mmodel_list.size());
	if (model_list_open)
	{
		int ix = 0;
		for (auto& mdid:g_mmodel_list)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
			if (_pmodel==mdid.second)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			ImGui::TreeNodeEx((void*)(intptr_t)ix, node_flags, mdid.first.c_str());
			if (ImGui::IsItemClicked())
			{
				_pmodel = mdid.second;
				_key_name = mdid.first;
			}
			ImGui::TreePop();
			ix++;
		}
		ImGui::TreePop();
	}
	if (_pmodel&&ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("delete", NULL, false, _pmodel.use_count() ==2))
		{
			auto& item_del = g_mmodel_list.find(_key_name);
			g_mmodel_list.erase(item_del);
			_pmodel = nullptr;
			_key_name = "";
		}
		ImGui::EndPopup();
	}
}
void model_edit::clear_states()
{
     _pmodel = nullptr;
     _key_name = "";
}
void model_edit::draw_model_item_property()
{
	if (_pmodel)
	{
		auto& mld = *_pmodel;
		ImGui::Text("mesh count:%d", mld.size());
          for( int ix = 0; ix < mld.size(); ix++ )
          {
               ImGui::Text( "Mesh%d:", ix );
               for( const auto& idiff : mld[ ix ]._text_diffuse_list )
               {
                    ImGui::Text( "text diffuse:%s", idiff.c_str() );
               }
               for( const auto& ispec : mld[ ix ]._text_specular_list )
               {
                    ImGui::Text( "text specular:%s", ispec.c_str() );
               }
               for( const auto& ihg : mld[ ix ]._text_height_list )
               {
                    ImGui::Text( "text height:%s", ihg.c_str() );
               }
               for( const auto& iam : mld[ ix ]._text_ambient_list )
               {
                    ImGui::Text( "text ambient:%s", iam.c_str() );
               }
          }
	}
}

const char* model_group_name = "model_group.json";
const char* assimp_support_format = "ALL Files(*.*)\0*.*\0"
"fbx\0*.fbx\0"
"dae\0*.dae\0"
"blend\0*.blend\0"
"3ds\0*.3ds\0"
"gltf\0*.gltf\0"
"glb\0*.glb\0"
"ase\0*.ase\0"
"obj\0*.obj\0"
"ifc\0*.ifc\0"
"xgl\0*.xgl\0"
"zgl\0*.zgl\0"
"ply\0*.ply\0"
"dxf\0*.dxf\0"
"lwo\0*.lwo\0"
"lws\0*.lws\0"
"lxo\0*.lxo\0"
"stl\0*.stl\0"
"x\0*.x\0"
"ac\0*.ac\0"
"ms3d\0*.ms3d\0"
"cob\0*.cob\0"
"scn\0*.scn\0"
"bvh\0*.bvh\0"
"csm\0*.csm\0\0";
const char* assimp_support_other_format = "\0*.xml\0*.irrmesh\0*.irr\0*.md1\0*.md2\0*.md3\0*.pk3\0*.mdc\0*.md5*\0*.smd\0*.vta\0*.ogex\0*.3d\0*.b3d\0*.q3d\0*.q3s\0*.nff\0*.off\0*.raw\0*.ter\0*.mdl\0*.hmp\0*.ndo\0\0";
//extern string g_cureent_directory;

static string model_path;
void loadMaterialTextures(aiMaterial *mat, aiTextureType type, vector<string>& txt_list)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		string str_txt_name = str.C_Str();
		str_txt_name = str_txt_name.substr(str_txt_name.find_last_of('\\') + 1);
		txt_list.push_back(str_txt_name);
		//string str_txt_path = model_path + str_txt_name;
		//add_image_to_mtexure_list(str_txt_path);
	}
}
void processMesh(aiMesh *mesh, const aiScene *scene, primitive_object& obj_pm, af_mesh& mesh_unit)
{
	// data to fill

	struct  af_vertex
	{
		float _nms[3];
		float _pos[3];
		float _txt[2];
	};
	af_vertex* pvertexs = new af_vertex[mesh->mNumVertices];
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//position
		pvertexs[i]._pos[0] = mesh->mVertices[i].x;
		pvertexs[i]._pos[1] = mesh->mVertices[i].y;
		pvertexs[i]._pos[2] = mesh->mVertices[i].z;
		if (mesh->mVertices[i].x<mesh_unit._box._xmin)
		{
			mesh_unit._box._xmin = mesh->mVertices[i].x;
		}
		else if (mesh->mVertices[i].x>mesh_unit._box._xmax)
		{
			mesh_unit._box._xmax = mesh->mVertices[i].x;
		}

		if (mesh->mVertices[i].y<mesh_unit._box._ymin)
		{
			mesh_unit._box._ymin = mesh->mVertices[i].y;
		}
		else if (mesh->mVertices[i].y>mesh_unit._box._ymax)
		{
			mesh_unit._box._ymax = mesh->mVertices[i].y;
		}

		if (mesh->mVertices[i].z<mesh_unit._box._zmin)
		{
			mesh_unit._box._zmin = mesh->mVertices[i].z;
		}
		else if (mesh->mVertices[i].z>mesh_unit._box._zmax)
		{
			mesh_unit._box._zmax = mesh->mVertices[i].z;
		}


		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			pvertexs[i]._txt[0] = mesh->mTextureCoords[0][i].x;
			pvertexs[i]._txt[1] = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			pvertexs[i]._txt[0] = 0;
			pvertexs[i]._txt[1] = 0;

		}
		//normals
		pvertexs[i]._nms[0] = mesh->mNormals[i].x;
		pvertexs[i]._nms[1] = mesh->mNormals[i].y;
		pvertexs[i]._nms[2] = mesh->mNormals[i].z;

		/*
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		*/
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	GLuint* pface_idx = NULL;
	GLuint num_indices = 0;
	GLuint face_len = 0;
	if (mesh->mNumFaces == 0)
	{
		return;
	}
	aiFace face = mesh->mFaces[0];
	num_indices = face.mNumIndices;
	face_len = mesh->mNumFaces * num_indices;
	pface_idx = new GLuint[face_len];
	for (unsigned int ix = 0; ix < mesh->mNumFaces; ix++)
	{
		aiFace face = mesh->mFaces[ix];
		// retrieve all indices of the face and store them in the indices vector

		auto id = ix * 3;
		for (int idx = 0; idx < num_indices; idx++)
		{
			pface_idx[id + idx] = face.mIndices[idx];

		}
	}

	obj_pm.set_ele_format({ 3, 3, 2 });
	auto float_size = sizeof(af_vertex) / sizeof(float);
	auto float_cnt = float_size*mesh->mNumVertices;
	obj_pm.load_vertex_data((GLfloat*)pvertexs, float_cnt, pface_idx, face_len);
	auto buff_len = 4 + float_cnt*sizeof(float) + face_len* sizeof(GLuint);
	ps_af_file ps_file = make_shared<af_file>(buff_len);
	char* phead = (char*)ps_file->_pbin;
	GLuint* phead_len = (GLuint*)phead;
	*phead_len = float_cnt*sizeof(float);
	phead += 4;
	memcpy(phead, pvertexs, *phead_len);
	phead += *phead_len;
	memcpy(phead, pface_idx, face_len*sizeof(GLuint));
	delete[] pvertexs;
	delete[] pface_idx;
	g_mfiles_list[mesh_unit._prm_id] = ps_file;
	save_ojfile_to_file(mesh_unit._prm_id);
	obj_pm._file_name = mesh_unit._prm_id;
	obj_pm._ps_file = ps_file;
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN
	/*aiString vertex_sd;
	material->Get(AI_MATKEY_SHADER_VERTEX, vertex_sd);*/

	aiColor3D a_color_diff(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, a_color_diff);
	aiColor3D a_color_ambient(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, a_color_ambient);
	aiColor3D a_color_specular(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_SPECULAR, a_color_specular);
	aiColor3D a_color_emissive(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_EMISSIVE, a_color_emissive);
	aiColor3D a_color_tranparent(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_TRANSPARENT, a_color_tranparent);
	aiColor3D a_color_reflective(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_REFLECTIVE, a_color_reflective);
	float b_reflectcity=0.f;
	material->Get(AI_MATKEY_REFLECTIVITY, b_reflectcity);
	int b_twosided = 0;
	material->Get(AI_MATKEY_TWOSIDED, b_twosided);
	int b_shading_model = 0;//gouraud ;aiShadingMode 
	material->Get(AI_MATKEY_SHADING_MODEL, b_shading_model);
	int b_blend_func=0;//aiBlendMode
	material->Get(AI_MATKEY_BLEND_FUNC, b_blend_func);
	float b_opacity = 0.f;
	material->Get(AI_MATKEY_OPACITY, b_opacity);
	float b_shininess=0.f;
	material->Get(AI_MATKEY_SHININESS, b_shininess);
	float b_shininess_strength = 0.f;
	material->Get(AI_MATKEY_SHININESS_STRENGTH, b_shininess_strength);
	float b_refracti = 0.f;//ÕÛÉä
	material->Get(AI_MATKEY_REFRACTI, b_refracti);

	// 1. diffuse maps
	loadMaterialTextures(material, aiTextureType_DIFFUSE, mesh_unit._text_diffuse_list);
	loadMaterialTextures(material, aiTextureType_SPECULAR, mesh_unit._text_specular_list);
	loadMaterialTextures(material, aiTextureType_HEIGHT, mesh_unit._text_height_list);
	loadMaterialTextures(material, aiTextureType_AMBIENT, mesh_unit._text_ambient_list);



	// return a mesh object created from the extracted mesh data
}

void processNode(aiNode *node, const aiScene *scene, af_model& md, string& mesh_base_name)
{
	/*if (!node->mTransformation.IsIdentity()){

		auto quaternion_2_vec3_angle = [](aiQuaternion& rq, af_vec3& ax, float& angle){
			auto half_a = acosf(rq.w);
			angle = 2 * half_a;
			auto sinHalf_a = sinf(half_a);
			ax.x = rq.x / sinHalf_a;
			ax.y = rq.y / sinHalf_a;
			ax.z = rq.z / sinHalf_a;
		};
		aiVector3D scaling, position;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scaling, rotation, position);
		af_vec3 rotate_ax;
		float angle;
		quaternion_2_vec3_angle(rotation, rotate_ax, angle);
		printf("angle=%f\n", angle);
	}*/
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto pmtv = make_shared<primitive_object>();

		//meshes.push_back(processMesh(mesh, scene));
		auto id = md.size();
		char cc[50];
		itoa(i, cc, 10);
		
		string mesh_name = node->mName.C_Str();//mesh_base_name + cc;
		mesh_name += cc;
		string mesh_kname = find_a_key_from_mp(g_primitive_list, mesh_name);
		mesh_kname = find_a_key_from_mp(g_mfiles_list, mesh_kname);
		g_primitive_list[mesh_kname] = pmtv;
		md.emplace_back();
		auto& cmesh = md[id];
		cmesh._prm_id = mesh_kname;
		cmesh._ps_prm_id = pmtv;
		pmtv->_model_name = mesh_base_name;
		pmtv->_mesh_id = i;
		processMesh(mesh, scene, *pmtv, cmesh);


	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, md, mesh_base_name);
	}

}
bool import_models(const char* md_file_name)
{
	Assimp::Importer importer;
	string mdnm = md_file_name;
	const aiScene* scene = importer.ReadFile(mdnm, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
		return false;
	}
	bool embeded_txt=scene->HasTextures();
	string md_gp_nm = mdnm.substr(mdnm.find_last_of('\\') + 1);
	model_path = mdnm.substr(0, mdnm.find_last_of('\\') + 1);
	auto pmd = make_shared<af_model>();
	processNode(scene->mRootNode, scene, *pmd, md_gp_nm);
	g_mmodel_list[md_gp_nm] = pmd;
	return true;
}
#include <cmath>
#include <windows.h>
#include <locale.h>  
#include <ShlObj.h>
#include <Commdlg.h>
#include "ft_light_scene.h"
#include "ft_color_node.h"
#include "ft_1_texture_node.h"
#include "ft_trans.h"
#include "ft_raw_trans.h"
#include "ft_color_mesh.h"
#include "ft_1_texture_mesh.h"

#include "resource.h"
/*
void GModel::rotate(float angle, glm::vec3 axis) {

angle = angle* 3.141592 / 180; //rad

float sinHalfAngle = sin(angle / 2);
float cosHalfAngle = cos(angle / 2);

float rX = axis.x*sinHalfAngle;
float rY = axis.y*sinHalfAngle;
float rZ = axis.z*sinHalfAngle;
float rW = cosHalfAngle;

glm::quat rota(rX, rY, rZ, rW);

rotation = glm::mat4_cast(rota)*rotation;
modelMatrix();
}
*/
using namespace auto_future;
static BOOL isChked;
static UINT_PTR CALLBACK ofnHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OFNOTIFY* pofNotify;
	switch (uMsg)
	{
	case WM_NOTIFY:
		pofNotify = (OFNOTIFY*)lParam;
		switch (pofNotify->hdr.code)
		{
		case CDN_FILEOK:
			isChked = IsDlgButtonChecked(hDlg, IDC_CHK_MIPMAP);

		}
	default:
		break;
	}
	return FALSE;
}
bool model_ui_generator::import_models(const char* md_file_name){
	Assimp::Importer importer;
	string mdnm = md_file_name;
	string md_gp_nm = mdnm.substr(mdnm.find_last_of('\\') + 1);
	const aiScene* scene = importer.ReadFile(mdnm, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);// | aiProcess_SortByPType | aiProcess_FindDegenerates);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
		return false;
	}
	bool embeded_txt = scene->HasTextures();
	x_r[0] = x_r[1] = y_r[0] = y_r[1] = z_r[0] = z_r[1] = 0;
	base_ui_component* pscene = nullptr;
#if 0
	if (!embeded_txt){
		if (typeid(*_proot) != typeid(ft_light_scene)){
			pscene = new ft_light_scene();
			pscene->set_name("scene");
			_proot->add_child(pscene);
		}
		else {
			pscene = _proot;
		}
		auto procec_texture_mesh = [&](aiMesh *mesh, const aiScene *scene, primitive_object& obj_pm, ft_1_texture_node& nodeTar){
			struct  af_vertex
			{
				float _pos[3];
				float _nms[3];
				float _txt[2];
			};
			af_vertex* pvertexs = new af_vertex[mesh->mNumVertices];
			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				//position
				pvertexs[i]._pos[0] = mesh->mVertices[i].x;
				pvertexs[i]._pos[1] = mesh->mVertices[i].y;
				pvertexs[i]._pos[2] = mesh->mVertices[i].z;
				//normals
				pvertexs[i]._nms[0] = mesh->mNormals[i].x;
				pvertexs[i]._nms[1] = mesh->mNormals[i].y;
				pvertexs[i]._nms[2] = mesh->mNormals[i].z;
				//texture
				pvertexs[i]._txt[0] = mesh->mTextureCoords[0][i].x;
				pvertexs[i]._txt[1] = mesh->mTextureCoords[0][i].y;	
			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			GLuint* pface_idx = NULL;
			GLuint num_indices = 0;
			GLuint face_len = 0;
			if (mesh->mNumFaces == 0)
			{
				return;
			}
			aiFace face = mesh->mFaces[0];
			num_indices = face.mNumIndices;
			printf("num_indices=%u\n", num_indices);
			face_len = mesh->mNumFaces * num_indices;
			pface_idx = new GLuint[face_len];
			for (unsigned int ix = 0; ix < mesh->mNumFaces; ix++)
			{
				aiFace face = mesh->mFaces[ix];
				auto id = ix * 3;
				for (int idx = 0; idx < num_indices; idx++)
				{
					pface_idx[id + idx] = face.mIndices[idx];
				}
			}

			obj_pm.set_ele_format({ 3, 3, 2 });
			auto float_size = sizeof(af_vertex) / sizeof(float);
			auto float_cnt = float_size*mesh->mNumVertices;
			obj_pm.load_vertex_data((GLfloat*)pvertexs, float_cnt, pface_idx, face_len);
			auto buff_len = 4 + float_cnt*sizeof(float) + face_len* sizeof(GLuint);
			ps_af_file ps_file = make_shared<af_file>(buff_len);
			char* phead = (char*)ps_file->_pbin;
			GLuint* phead_len = (GLuint*)phead;
			*phead_len = float_cnt*sizeof(float);
			phead += 4;
			memcpy(phead, pvertexs, *phead_len);
			phead += *phead_len;
			memcpy(phead, pface_idx, face_len*sizeof(GLuint));
			delete[] pvertexs;
			delete[] pface_idx;
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			auto load_texture = [&](aiMaterial* mtl, aiTextureType txtType,ft_1_texture_node& nd){
				auto txtCnt = mtl->GetTextureCount(txtType);
				if (txtCnt>0){
					aiString str;
					mtl->GetTexture(txtType, 0, &str);
					// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
					string str_txt_name = str.C_Str();
					str_txt_name = str_txt_name.substr(str_txt_name.find_last_of('\\') + 1);
					string dlg_title = "pick up " + str_txt_name + "please!";
					OPENFILENAME ofn = { sizeof(OPENFILENAME) };
					ofn.hwndOwner = GetForegroundWindow();
					ofn.hInstance = GetModuleHandle(NULL);
					ofn.lpfnHook = ofnHookProc;
					ofn.lpstrFilter = "image file:\0*.png;*.bmp;*.jpg;*.jpeg;*.gif;*.dds;*.tga;*.psd;*.hdr\0\0";;
					char strFileName[MAX_PATH] = { 0 };
					ofn.nFilterIndex = 1;
					ofn.lpstrFile = strFileName;
					ofn.nMaxFile = sizeof(strFileName);
					ofn.lpstrTitle = dlg_title.c_str();
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					if (GetOpenFileName(&ofn))
					{
						printf("open file:%s\n", strFileName);

						//import_fbx_info(strFileName);
						auto txtKey = add_image_to_mtexure_list(string(strFileName), isChked == BST_CHECKED);
						nd.set_txt_obj(txtKey);

					}
				}
				return txtCnt;
			};
#define LOAD_SP_TXT(txtType) \
			load_texture(material, txtType, nodeTar)
			auto txt_cnt=LOAD_SP_TXT(aiTextureType_DIFFUSE);
			/*if (txt_cnt == 0)txt_cnt=LOAD_SP_TXT(aiTextureType_SPECULAR);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_AMBIENT);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_EMISSIVE);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_HEIGHT);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_NORMALS);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_SHININESS);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_OPACITY);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_LIGHTMAP);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_REFLECTION);
			if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_UNKNOWN);
			*/
			obj_pm._ps_file = ps_file;
			// process materials
			//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
						// 1. diffuse maps
			//loadMaterialTextures(material, aiTextureType_DIFFUSE, mesh_unit._text_diffuse_list);
		};
		auto process_mesh = [&](aiMesh *mesh, const aiScene *scene, primitive_object& obj_pm, ft_color_node& nodeTar){
			struct  af_vertex
			{
				float _pos[3];				
				float _nms[3];
			};
			af_vertex* pvertexs = new af_vertex[mesh->mNumVertices];
			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				//position
				pvertexs[i]._pos[0] = mesh->mVertices[i].x; 
				if (pvertexs[i]._pos[0] < x_r[0]) x_r[0] = pvertexs[i]._pos[0];
				if (pvertexs[i]._pos[0] > x_r[1]) x_r[1] = pvertexs[i]._pos[0];

				pvertexs[i]._pos[1] = mesh->mVertices[i].y;
				if (pvertexs[i]._pos[1] < y_r[0]) y_r[0] = pvertexs[i]._pos[1];
				if (pvertexs[i]._pos[1] > y_r[1]) y_r[1] = pvertexs[i]._pos[1];

				pvertexs[i]._pos[2] = mesh->mVertices[i].z;
				if (pvertexs[i]._pos[2] < z_r[0]) z_r[0] = pvertexs[i]._pos[2];
				if (pvertexs[i]._pos[2] > z_r[1]) z_r[1] = pvertexs[i]._pos[2];


				//normals
				pvertexs[i]._nms[0] = mesh->mNormals[i].x;
				pvertexs[i]._nms[1] = mesh->mNormals[i].y;
				pvertexs[i]._nms[2] = mesh->mNormals[i].z;
			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			GLuint* pface_idx = NULL;
			GLuint num_indices = 0;
			GLuint face_len = 0;
			if (mesh->mNumFaces == 0)
			{
				return;
			}
			aiFace face = mesh->mFaces[0];
			num_indices = face.mNumIndices;
			face_len = mesh->mNumFaces * num_indices;
			pface_idx = new GLuint[face_len];
			for (unsigned int ix = 0; ix < mesh->mNumFaces; ix++)
			{
				aiFace face = mesh->mFaces[ix];
				auto id = ix * 3;
				for (int idx = 0; idx < num_indices; idx++)
				{
					pface_idx[id + idx] = face.mIndices[idx];

				}
			}

			obj_pm.set_ele_format({ 3, 3 });
			auto float_size = sizeof(af_vertex) / sizeof(float);
			auto float_cnt = float_size*mesh->mNumVertices;
			obj_pm.load_vertex_data((GLfloat*)pvertexs, float_cnt, pface_idx, face_len);
			auto buff_len = 4 + float_cnt*sizeof(float) + face_len* sizeof(GLuint);
			ps_af_file ps_file = make_shared<af_file>(buff_len);
			char* phead = (char*)ps_file->_pbin;
			GLuint* phead_len = (GLuint*)phead;
			*phead_len = float_cnt*sizeof(float);
			phead += 4;
			memcpy(phead, pvertexs, *phead_len);
			phead += *phead_len;
			memcpy(phead, pface_idx, face_len*sizeof(GLuint));
			delete[] pvertexs;
			delete[] pface_idx;
			obj_pm._ps_file = ps_file;
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
			auto a2v3 = [](aiColor3D& clr)->af_vec3{
				return{ clr.r, clr.g, clr.b };
			};		

			aiColor3D a_color_diff(0.f, 0.f, 0.f);
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, a_color_diff) == AI_SUCCESS){
				nodeTar.set_diffuse(a2v3(a_color_diff));
			}
			/*aiColor3D a_color_ambient(0.f, 0.f, 0.f);
			if (material->Get(AI_MATKEY_COLOR_AMBIENT, a_color_ambient) == AI_SUCCESS){
			  nodeTar.set_ambient(a2v3(a_color_ambient));
			}
			*/

			aiColor3D a_color_specular(0.f, 0.f, 0.f);
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, a_color_specular) == AI_SUCCESS){
				nodeTar.set_specular(a2v3(a_color_specular));
			}

			
		};
		function<void(aiNode*, const aiScene *, base_ui_component* parent)> process_node = [&](aiNode *node, const aiScene *scene, base_ui_component* parent){
			base_ui_component* cur_node = NULL;
			if (node->mParent == NULL)//RootNode
			{
				cur_node = new ft_trans();
				cur_node->set_name(md_gp_nm);
				parent->add_child(cur_node);
				
			} else {
				if (node->mNumMeshes > 0 && scene->mMeshes[node->mMeshes[0]]->HasTextureCoords(0)){
					auto cur_t_node = new ft_1_texture_node();
					cur_node = cur_t_node;
					cur_t_node->set_name(node->mName.C_Str());
					parent->add_child(cur_t_node);
					auto& mTranspos = node->mTransformation.Transpose();
					auto fv = mTranspos[0];
					cur_t_node->set_trans_mat(fv);
					for (unsigned int i = 0; i < node->mNumMeshes; i++)
					{
						aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
						auto pmtv = make_shared<primitive_object>();
						procec_texture_mesh(mesh, scene, *pmtv, *cur_t_node);
						string mesh_name = node->mName.C_Str();//mesh_base_name + cc;

						string mesh_kname = find_a_key_from_mp(g_primitive_list, mesh_name);
						mesh_kname = find_a_key_from_mp(g_mfiles_list, mesh_kname);
						g_primitive_list[mesh_kname] = pmtv;
						g_mfiles_list[mesh_kname] = pmtv->_ps_file;
						pmtv->_file_name = mesh_kname;
						save_ojfile_to_file(mesh_kname);
						cur_t_node->set_prm_obj(mesh_kname);
						cur_t_node->link();
						//processMesh(mesh, scene, *pmtv, cmesh);
					}
				} else {
					auto cur_c_node = new ft_color_node();
					cur_node = cur_c_node;
					cur_c_node->set_name(node->mName.C_Str());
					parent->add_child(cur_c_node);
					auto& mTranspos = node->mTransformation.Transpose();
					auto fv = mTranspos[0];
					cur_c_node->set_trans_mat(fv);
					//mTranspos.Inverse();
	#if 0
					if (!mTranspos.IsIdentity()){
						aiVector3D scaling, position;
						aiVector3D rotation;
						mTranspos.Decompose(scaling, rotation, position);
						cur_c_node->set_pos({ position.x,position.z, position.z });
						cur_c_node->set_scale({scaling.x,scaling.y, scaling.z});
						cur_c_node->set_qua({rotation.x, rotation.y, rotation.z});
					}
	#endif
					//rotation.
					for (unsigned int i = 0; i < node->mNumMeshes; i++)
					{
						aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
						auto pmtv = make_shared<primitive_object>();

						process_mesh(mesh, scene, *pmtv, *cur_c_node);
						string mesh_name = node->mName.C_Str();//mesh_base_name + cc;

						string mesh_kname = find_a_key_from_mp(g_primitive_list, mesh_name);
						mesh_kname = find_a_key_from_mp(g_mfiles_list, mesh_kname);
						g_primitive_list[mesh_kname] = pmtv;
						g_mfiles_list[mesh_kname] = pmtv->_ps_file;
						pmtv->_file_name = mesh_kname;
						save_ojfile_to_file(mesh_kname);
						cur_c_node->set_prm_obj(mesh_kname);
						cur_c_node->link();
						//processMesh(mesh, scene, *pmtv, cmesh);
					}
				}
				
				
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				process_node(node->mChildren[i], scene, cur_node);
			}
			
		};
		process_node(scene->mRootNode, scene, pscene);
		pscene->link();
		printf("xmin:%f  xmax:%f\n", x_r[0], x_r[1]);
		printf("ymin:%f  ymax:%f\n", y_r[0], y_r[1]);
		printf("zmin:%f  zmax:%f\n", z_r[0], z_r[1]);
	}
#else
if (!embeded_txt) {
	if (typeid(*_proot) != typeid(ft_light_scene)) {
		pscene = new ft_light_scene();
		pscene->set_name("scene");
		_proot->add_child(pscene);
	}
	else {
		pscene = _proot;
	}
	auto procec_texture_mesh = [&](aiMesh* mesh, const aiScene* scene, primitive_object& obj_pm, ft_1_texture_mesh& nodeTar) {
		struct  af_vertex
		{
			float _pos[3];
			float _nms[3];
			float _txt[2];
		};
		af_vertex* pvertexs = new af_vertex[mesh->mNumVertices];
		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			//position
			pvertexs[i]._pos[0] = mesh->mVertices[i].x;
			pvertexs[i]._pos[1] = mesh->mVertices[i].y;
			pvertexs[i]._pos[2] = mesh->mVertices[i].z;
			//normals
			pvertexs[i]._nms[0] = mesh->mNormals[i].x;
			pvertexs[i]._nms[1] = mesh->mNormals[i].y;
			pvertexs[i]._nms[2] = mesh->mNormals[i].z;
			//texture
			pvertexs[i]._txt[0] = mesh->mTextureCoords[0][i].x;
			pvertexs[i]._txt[1] = mesh->mTextureCoords[0][i].y;
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		GLuint* pface_idx = NULL;
		GLuint num_indices = 0;
		GLuint face_len = 0;
		if (mesh->mNumFaces == 0)
		{
			return;
		}
		aiFace face = mesh->mFaces[0];
		num_indices = face.mNumIndices;
		printf("num_indices=%u\n", num_indices);
		face_len = mesh->mNumFaces * num_indices;
		pface_idx = new GLuint[face_len];
		for (unsigned int ix = 0; ix < mesh->mNumFaces; ix++)
		{
			aiFace face = mesh->mFaces[ix];
			auto id = ix * 3;
			for (int idx = 0; idx < num_indices; idx++)
			{
				pface_idx[id + idx] = face.mIndices[idx];
			}
		}

		obj_pm.set_ele_format({ 3, 3, 2 });
		auto float_size = sizeof(af_vertex) / sizeof(float);
		auto float_cnt = float_size * mesh->mNumVertices;
		obj_pm.load_vertex_data((GLfloat*)pvertexs, float_cnt, pface_idx, face_len);
		auto buff_len = 4 + float_cnt * sizeof(float) + face_len * sizeof(GLuint);
		ps_af_file ps_file = make_shared<af_file>(buff_len);
		char* phead = (char*)ps_file->_pbin;
		GLuint* phead_len = (GLuint*)phead;
		*phead_len = float_cnt * sizeof(float);
		phead += 4;
		memcpy(phead, pvertexs, *phead_len);
		phead += *phead_len;
		memcpy(phead, pface_idx, face_len * sizeof(GLuint));
		delete[] pvertexs;
		delete[] pface_idx;
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		auto load_texture = [&](aiMaterial* mtl, aiTextureType txtType, ft_1_texture_mesh& nd) {
			auto txtCnt = mtl->GetTextureCount(txtType);
			if (txtCnt > 0) {
				aiString str;
				mtl->GetTexture(txtType, 0, &str);
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				string str_txt_name = str.C_Str();
				str_txt_name = str_txt_name.substr(str_txt_name.find_last_of('\\') + 1);
				string dlg_title = "pick up " + str_txt_name + "please!";
				OPENFILENAME ofn = { sizeof(OPENFILENAME) };
				ofn.hwndOwner = GetForegroundWindow();
				ofn.hInstance = GetModuleHandle(NULL);
				ofn.lpfnHook = ofnHookProc;
				ofn.lpstrFilter = "image file:\0*.png;*.bmp;*.jpg;*.jpeg;*.gif;*.dds;*.tga;*.psd;*.hdr\0\0";;
				char strFileName[MAX_PATH] = { 0 };
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = strFileName;
				ofn.nMaxFile = sizeof(strFileName);
				ofn.lpstrTitle = dlg_title.c_str();
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
				if (GetOpenFileName(&ofn))
				{
					printf("open file:%s\n", strFileName);

					//import_fbx_info(strFileName);
					auto txtKey = add_image_to_mtexure_list(string(strFileName), isChked == BST_CHECKED);
					nd.set_txt_obj(txtKey);

				}
			}
			return txtCnt;
		};
#define LOAD_SP_TXT(txtType) \
			load_texture(material, txtType, nodeTar)
		auto txt_cnt = LOAD_SP_TXT(aiTextureType_DIFFUSE);
		/*if (txt_cnt == 0)txt_cnt=LOAD_SP_TXT(aiTextureType_SPECULAR);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_AMBIENT);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_EMISSIVE);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_HEIGHT);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_NORMALS);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_SHININESS);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_OPACITY);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_LIGHTMAP);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_REFLECTION);
		if (txt_cnt == 0)txt_cnt = LOAD_SP_TXT(aiTextureType_UNKNOWN);
		*/
		obj_pm._ps_file = ps_file;
		// process materials
		//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
					// 1. diffuse maps
		//loadMaterialTextures(material, aiTextureType_DIFFUSE, mesh_unit._text_diffuse_list);
	};
	auto process_mesh = [&](aiMesh* mesh, const aiScene* scene, primitive_object& obj_pm, ft_color_mesh& nodeTar) {
		struct  af_vertex
		{
			float _pos[3];
			float _nms[3];
		};
		af_vertex* pvertexs = new af_vertex[mesh->mNumVertices];
		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			//position
			pvertexs[i]._pos[0] = mesh->mVertices[i].x;
			if (pvertexs[i]._pos[0] < x_r[0]) x_r[0] = pvertexs[i]._pos[0];
			if (pvertexs[i]._pos[0] > x_r[1]) x_r[1] = pvertexs[i]._pos[0];

			pvertexs[i]._pos[1] = mesh->mVertices[i].y;
			if (pvertexs[i]._pos[1] < y_r[0]) y_r[0] = pvertexs[i]._pos[1];
			if (pvertexs[i]._pos[1] > y_r[1]) y_r[1] = pvertexs[i]._pos[1];

			pvertexs[i]._pos[2] = mesh->mVertices[i].z;
			if (pvertexs[i]._pos[2] < z_r[0]) z_r[0] = pvertexs[i]._pos[2];
			if (pvertexs[i]._pos[2] > z_r[1]) z_r[1] = pvertexs[i]._pos[2];


			//normals
			pvertexs[i]._nms[0] = mesh->mNormals[i].x;
			pvertexs[i]._nms[1] = mesh->mNormals[i].y;
			pvertexs[i]._nms[2] = mesh->mNormals[i].z;
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		GLuint* pface_idx = NULL;
		GLuint num_indices = 0;
		GLuint face_len = 0;
		if (mesh->mNumFaces == 0)
		{
			return;
		}
		aiFace face = mesh->mFaces[0];
		num_indices = face.mNumIndices;
		face_len = mesh->mNumFaces * num_indices;
		pface_idx = new GLuint[face_len];
		for (unsigned int ix = 0; ix < mesh->mNumFaces; ix++)
		{
			aiFace face = mesh->mFaces[ix];
			auto id = ix * 3;
			for (int idx = 0; idx < num_indices; idx++)
			{
				pface_idx[id + idx] = face.mIndices[idx];

			}
		}

		obj_pm.set_ele_format({ 3, 3 });
		auto float_size = sizeof(af_vertex) / sizeof(float);
		auto float_cnt = float_size * mesh->mNumVertices;
		obj_pm.load_vertex_data((GLfloat*)pvertexs, float_cnt, pface_idx, face_len);
		auto buff_len = 4 + float_cnt * sizeof(float) + face_len * sizeof(GLuint);
		ps_af_file ps_file = make_shared<af_file>(buff_len);
		char* phead = (char*)ps_file->_pbin;
		GLuint* phead_len = (GLuint*)phead;
		*phead_len = float_cnt * sizeof(float);
		phead += 4;
		memcpy(phead, pvertexs, *phead_len);
		phead += *phead_len;
		memcpy(phead, pface_idx, face_len * sizeof(GLuint));
		delete[] pvertexs;
		delete[] pface_idx;
		obj_pm._ps_file = ps_file;
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		auto a2v3 = [](aiColor3D& clr)->af_vec3 {
			return{ clr.r, clr.g, clr.b };
		};

		aiColor3D a_color_diff(0.f, 0.f, 0.f);
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, a_color_diff) == AI_SUCCESS) {
			nodeTar.set_diffuse(a2v3(a_color_diff));
		}
		/*aiColor3D a_color_ambient(0.f, 0.f, 0.f);
		if (material->Get(AI_MATKEY_COLOR_AMBIENT, a_color_ambient) == AI_SUCCESS){
		  nodeTar.set_ambient(a2v3(a_color_ambient));
		}
		*/

		aiColor3D a_color_specular(0.f, 0.f, 0.f);
		if (material->Get(AI_MATKEY_COLOR_SPECULAR, a_color_specular) == AI_SUCCESS) {
			nodeTar.set_specular(a2v3(a_color_specular));
		}


	};
	function<void(aiNode*, const aiScene*, base_ui_component* parent)> process_node = [&](aiNode* node, const aiScene* scene, base_ui_component* parent) {
		base_ui_component* cur_node = NULL;
		if (node->mParent == NULL)//RootNode
		{
			cur_node = new ft_trans();
			cur_node->set_name(md_gp_nm);
			parent->add_child(cur_node);
		}
		else {
			if (node->mTransformation.IsIdentity()) {
				cur_node = new ft_trans();
			}
			else {
				auto cur_t_node = new ft_raw_trans();
				auto& mTranspos = node->mTransformation.Transpose();
				auto fv = mTranspos[0];
				cur_t_node->set_trans_mat(fv);
				cur_node = cur_t_node;
			}
			cur_node->set_name(node->mName.C_Str());
			parent->add_child(cur_node);
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				auto pmtv = make_shared<primitive_object>();
				if (mesh->HasTextureCoords(0)) {
					auto pmesh = new ft_1_texture_mesh();
					procec_texture_mesh(mesh, scene, *pmtv, *pmesh);
					pmesh->set_name(mesh->mName.C_Str());
					string mesh_name = mesh->mName.C_Str();//mesh_base_name + cc;

					string mesh_kname = find_a_key_from_mp(g_primitive_list, mesh_name);
					mesh_kname = find_a_key_from_mp(g_mfiles_list, mesh_kname);
					g_primitive_list[mesh_kname] = pmtv;
					g_mfiles_list[mesh_kname] = pmtv->_ps_file;
					pmtv->_file_name = mesh_kname;
					save_ojfile_to_file(mesh_kname);
					pmesh->set_prm_obj(mesh_kname);
					pmesh->link();
					cur_node->add_child(pmesh);

				}
				else {
					auto pmesh = new ft_color_mesh();
					process_mesh(mesh, scene, *pmtv, *pmesh);
					pmesh->set_name(mesh->mName.C_Str());
					string mesh_name = mesh->mName.C_Str();//mesh_base_name + cc;

					string mesh_kname = find_a_key_from_mp(g_primitive_list, mesh_name);
					mesh_kname = find_a_key_from_mp(g_mfiles_list, mesh_kname);
					g_primitive_list[mesh_kname] = pmtv;
					g_mfiles_list[mesh_kname] = pmtv->_ps_file;
					pmtv->_file_name = mesh_kname;
					save_ojfile_to_file(mesh_kname);
					pmesh->set_prm_obj(mesh_kname);
					pmesh->link();
					cur_node->add_child(pmesh);
				}
			}
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			process_node(node->mChildren[i], scene, cur_node);
		}

	};
	process_node(scene->mRootNode, scene, pscene);
	pscene->link();
	printf("xmin:%f  xmax:%f\n", x_r[0], x_r[1]);
	printf("ymin:%f  ymax:%f\n", y_r[0], y_r[1]);
	printf("zmin:%f  zmax:%f\n", z_r[0], z_r[1]);
}
#endif
	//aiReleaseImport(scene);
	//aiDetachAllLogStreams();

	return true;
}