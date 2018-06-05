#pragma once
class material_shader_edit
{
public:
	material_shader_edit(){}
	~material_shader_edit(){}
	void load_shader();
	void load_shader_info();
	void draw_shader();
	void draw_material();
	void draw_shader_item_property();
	void draw_material_item_property();
};

