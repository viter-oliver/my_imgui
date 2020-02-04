#include "primitve_edit.h"
#include <string>
#include "user_control_imgui.h"
#include "res_internal.h"
#ifdef IMGUI_WAYLAND
#include "../../deps/glad/glad.h"
#else
#include <GL/gl3w.h> 
#endif
#include <sstream>
#include "af_type.h"
#include "common_functions.h"
enum en_mem_usage
{
	en_GL_STREAM_DRAW,
	en_GL_STREAM_READ,
	en_GL_STREAM_COPY,
	en_GL_STATIC_DRAW,
	en_GL_STATIC_READ,
	en_GL_STATIC_COPY,
	en_GL_DYNAMIC_DRAW,
	en_GL_DYNAMIC_READ,
	en_GL_DYNAMIC_COPY,
	en_mem_usage_cnt
};
static struct{
	const char* str_show;
	GLuint mem_usage;
} mem_usage_item[en_mem_usage_cnt] =
{
	"GL_STREAM_DRAW", GL_STREAM_DRAW,
	"GL_STREAM_READ", GL_STREAM_READ,
	"GL_STREAM_COPY", GL_STREAM_COPY,
	"GL_STATIC_DRAW", GL_STATIC_DRAW,
	"GL_STATIC_READ", GL_STATIC_READ,
	"GL_STATIC_COPY", GL_STATIC_COPY,
	"GL_DYNAMIC_DRAW", GL_DYNAMIC_DRAW,
	"GL_DYNAMIC_READ", GL_DYNAMIC_READ,
	"GL_DYNAMIC_COPY", GL_DYNAMIC_COPY
};
static int mem_usage_idx = en_GL_STATIC_DRAW;
int init_mem_usage_idx(GLuint mem_usage)
{
	for (int idx = en_GL_STREAM_DRAW; idx < en_mem_usage_cnt;idx++)
	{
		if (mem_usage_item[idx].mem_usage==mem_usage)
		{
			return idx;
		}
	}
	return 0;
}
static bool get_mem_usage_item(void* data, int idx, const char** out_str)
{
	*out_str = mem_usage_item[idx].str_show;
	return true;
}
void primitve_edit::draw_primitive_list()
{
	if (ImGui::Button("New primitive..."))
	{
		ImGui::OpenPopup("NewPrimitive");
	}
	if (ImGui::BeginPopupModal("NewPrimitive"))
	{
		static char prm_name_str[FILE_NAME_LEN] = "";
		static vector<GLubyte> prm_frm = { 3 };
		static GLint vetex_cnt = 1;
		ImGui::InputText("enter a name for the new primitive object", prm_name_str, FILE_NAME_LEN);
		ImGui::InputInt("enter a number for the count of vertex of new primitive object", &vetex_cnt);
		ImGui::Combo("memory usage", &mem_usage_idx, &get_mem_usage_item, mem_usage_item, en_mem_usage_cnt);
		ImGui::Text("Element format:");
		string str_minus("-##");
		auto& fmts = prm_frm;
		auto fsz = fmts.size();
		string str_plus("+##");
		GLuint stride = 0;
		for (int ix = 0; ix < fsz; ix++)
		{
			stride += fmts[ix];
			ImGui::Text("%d", fmts[ix]);
			if (fmts[ix] > 1)
			{
				ImGui::SameLine();
				str_minus += "1";
				if (ImGui::Button(str_minus.c_str()))
				{
					fmts[ix]--;
				}
			}
			if (fmts[ix] < 10)
			{
				ImGui::SameLine();
				str_plus += "1";
				if (ImGui::Button(str_plus.c_str()))
				{
					fmts[ix]++;
				}
			}
		}

		if (fsz < 10)
		{
			if (ImGui::Button("Add..."))
			{
				fmts.emplace_back();
				fmts.back() = 1;
			}
		}
		if (fsz > 1)
		{
			if (fsz < 10)
			{
				ImGui::SameLine();
			}
			if (ImGui::Button("Del..."))
			{
				fmts.resize(fsz - 1);
			}
		}
		if (ImGui::Button("New"))
		{
			string prm_name(prm_name_str);
			string prm_kname = find_a_key_from_mp(g_primitive_list, prm_name);
			prm_kname = find_a_key_from_mp(g_mfiles_list, prm_kname);
			auto pmtv = make_shared<primitive_object>();
			pmtv->set_ele_format(prm_frm);
			auto stride = pmtv->get_stride();
			auto vlen = stride* vetex_cnt;
			GLfloat* pvertex = new GLfloat[vlen];
			memset(pvertex, 0, vlen*sizeof GLfloat);
			auto mem_usage = mem_usage_item[mem_usage_idx].mem_usage;
			pmtv->load_vertex_data(pvertex, vlen,0,0,mem_usage);
			auto buff_len = 4 + vlen*sizeof(GLfloat);
			ps_af_file ps_file = make_shared<af_file>(buff_len);
			char* phead = (char*)ps_file->_pbin;
			GLuint* phead_len = (GLuint*)phead;
			*phead_len = buff_len - 4;
			phead += 4;
			memcpy(phead, pvertex, *phead_len);
			g_mfiles_list[prm_kname] = ps_file;
			pmtv->_ps_file = ps_file;
			save_ojfile_to_file(prm_kname);
			delete[] pvertex;
			g_primitive_list[prm_kname] = pmtv;
			mem_usage_idx= en_GL_STATIC_DRAW;
			prm_name_str[0]='\0';
			prm_frm = { 3 };
			vetex_cnt = 1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_DefaultOpen;
	string icon_str = icn_primitive;
	if (IconTreeNode(icon_str,"primitive objects",node_flag))
	{
		for (auto& iprm:g_primitive_list)
		{
			auto& key_name = iprm.first;
			auto& prm = iprm.second;
			node_flag = ImGuiTreeNodeFlags_Leaf;
			if (prm->_sel)
			{
				node_flag |= ImGuiTreeNodeFlags_Selected;
			}
			IconTreeNode(icon_str, key_name.c_str(), node_flag);
			if (ImGui::IsItemClicked() && _pmobj!=prm)
			{
				if (_pmobj)
				{
					_pmobj->_sel = false;
				}
				_pmobj = prm;
				_key_name = key_name;
				_pmobj->_sel = true;
				auto& ps_file = _pmobj->_ps_file;
				char* phead = (char*)ps_file->_pbin;
				//GLuint* phead_buff_len = (GLuint*)phead;
				phead += 4;
				_pvertex = (float*)phead;
				//_vertex_buff.resize(_pmobj->_vertex_buf_len);
				//memcpy(&_vertex_buff[0], phead, *phead_buff_len);

			}
			
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextWindow())
	{
		
			
		if (_pmobj&&ImGui::MenuItem("delete", NULL, false, _pmobj.use_count() == 2))
		{
			auto& item_del = g_primitive_list.find(_key_name);
			g_primitive_list.erase(item_del);
			_pmobj = nullptr;
			_key_name = "";
		}
		ImGui::EndPopup();
	}
	
}
void primitve_edit::clear_states()
{
     _pmobj = nullptr;
     _key_name = "";
}
void primitve_edit::draw_primitive_item_property()
{
	if (_pmobj)
	{
		ImGui::Text("Vertex buffer length:%d ,element buffer length:%d", _pmobj->_vertex_buf_len, _pmobj->_ele_buf_len);
		auto& fmts = _pmobj->_ele_format;
		static char str_numb[0xa] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		string str_fmt;
		for (auto& fmt_u : fmts)
		{
			str_fmt += str_numb[fmt_u];
			str_fmt += ':';
		}
		str_fmt.back() = '\0';
		ImGui::Text("Element format:%s", str_fmt.c_str());
		if (ImGui::Button("Vertex Edit..."))
		{
			ImGui::OpenPopup("vertex_edit");
			mem_usage_idx = init_mem_usage_idx( _pmobj->_mem_usage);
		}

		if (ImGui::BeginPopupModal("vertex_edit"))
		{
			auto& vlen=_pmobj->_vertex_buf_len;
			GLubyte stride = _pmobj->get_stride();
			GLuint vcnt = vlen / stride;
			GLuint mem_usage = _pmobj->_mem_usage;
			float* pvt = _pvertex;
            if( ImGui::Button( "Save" ) )
            {
                glBindBuffer( GL_ARRAY_BUFFER, _pmobj->_vbo );
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*_pmobj->_vertex_buf_len, _pvertex, mem_usage);
                ImGui::CloseCurrentPopup();
				_pmobj->_mem_usage = mem_usage_item[mem_usage_idx].mem_usage;
            }
            ImGui::SameLine();
            if( ImGui::Button( ( "Cancel" ) ) )
            {
                ImGui::CloseCurrentPopup();
            }
			ImGui::Combo("memory usage", &mem_usage_idx, &get_mem_usage_item, mem_usage_item, en_mem_usage_cnt);
			stringstream stm_it;
			ImGui::Text("vertex buffer:");
			for (int ix = 0; ix < vcnt;ix++)
			{
				stm_it.str(string());
				stm_it.clear();
				stm_it << "Vertex" << ix;
				string citstr = stm_it.str();
				ImGui::InputFloatN(citstr.c_str(), pvt, stride,-1,0);
				pvt += stride;
			}
			/*
			ImGui::InputFloat("##0", pvt);
			pvt++;
			ImGui::SameLine();
			for (GLuint ix = 1; ix < vlen;ix++)
			{
				stm_it.str(string());
				stm_it.clear();
				stm_it << "##" << ix;
				string citstr = stm_it.str();
				ImGui::InputFloat(citstr.c_str(), pvt);
				pvt++;
				auto mvalue = ix%stride;
				if (mvalue)
					ImGui::SameLine();
			}*/

			ImGui::EndPopup();
		}
	}
}
