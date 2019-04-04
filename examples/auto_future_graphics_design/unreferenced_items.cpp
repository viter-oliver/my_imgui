#include "unreferenced_items.h"
#include "imgui.h"
//#include "res_output.h"
#include "af_material.h"
#include "af_model.h"
template<class T> void search_unref_items(T& map_container, unref_items_list& unref_list)
{
	unref_list.clear();
	for (auto& itm : map_container)
	{
		auto& ps_itm = itm.second;
		if (ps_itm.use_count()==1)
		{
			unref_list.emplace_back();
			auto& uref_item = unref_list.back();
			uref_item._item_name = itm.first;
		}
	}
}
void unreferenced_items::search_unreferenced_items()
{
	search_unref_items(g_mmodel_list, _model_list);
	search_unref_items(g_material_list, _material_list);
	search_unref_items(g_af_shader_list, _shader_list);
	search_unref_items(g_primitive_list, _primitive_list);
	search_unref_items(g_mfiles_list, _file_list);
	search_unref_items(g_mtexture_list, _txt_list);
}
void show_unref_items(string res_name, unref_items_list& unref_list)
{
	ImGui::Text(res_name.c_str());
	int idx = 1;
	int usz = unref_list.size();
	for (auto& unref_item:unref_list)
	{
		ImGui::Checkbox(unref_item._item_name.c_str(), &unref_item._be_sel);
		auto md = idx % 4;
		/*if (md!=0&&idx!=usz)
		{
		ImGui::SameLine(10);
		}*/
		idx++;
	}
}
template<class T>
void delete_unref_items(T& mp_container,unref_items_list& unref_list)
{
	for (auto& uitem:unref_list)
	{
		if (uitem._be_sel)
		{
			auto& iuref = mp_container.find(uitem._item_name);
			mp_container.erase(iuref);
		}
	}
};
void unreferenced_items::show_delete_unreferenced_items()
{
	show_unref_items("unreferenced models:", _model_list);
	show_unref_items("unreferenced materials:", _material_list);
	show_unref_items("unreferenced shaders:", _shader_list);
	show_unref_items("unreferenced primitives:", _primitive_list);
	show_unref_items("unreferenced files:", _file_list);
	show_unref_items("unreferenced textures:", _txt_list);
	if (ImGui::Button("Ok"))
	{
		delete_unref_items(g_mmodel_list, _model_list);
		delete_unref_items(g_material_list, _material_list);
		delete_unref_items(g_af_shader_list, _shader_list);
		delete_unref_items(g_primitive_list, _primitive_list);
		delete_unref_items(g_mfiles_list, _file_list);
		delete_unref_items(g_mtexture_list, _txt_list);
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		ImGui::CloseCurrentPopup();
	}
}
