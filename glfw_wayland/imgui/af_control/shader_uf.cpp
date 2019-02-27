#include <imgui.h>
#include <stdio.h>
#include "shader_uf.h"
#include<string>
#include<sstream>
using namespace std;
//#include "user_control_imgui.h"


#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void shader_uf::output_2_json(Value& jvalue)
{
	jvalue["usize"] = _usize;
	jvalue["el_size"] = _el_size;
	jvalue["utype"] = _utype;
}
void shader_uf::init_from_json(Value& jvalue)
{
	//_usize = jvalue["usize"].asUInt();
	//_el_size=jvalue["el_size"].asUInt();
	_utype = jvalue["utype"].asUInt();

}


void shader_uf_float::edit()
{
	float* pfvalue = _pfvalue;
	int wsize = _usize*_el_size;
	for (int ix = wsize; ix >0;)
	{
		string str_show ="###"+ _unf_name;
		ostringstream ostream;
		ostream << ix;
		str_show+= ostream.str();
		if (ix > 4)
		{
			ImGui::InputFloat4(str_show.c_str(), pfvalue);
			ix -= 4;
			pfvalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputFloat4(str_show.c_str(), pfvalue);
				return;
			case 1:
				ImGui::InputFloat(str_show.c_str(), pfvalue);
				return;
			case 2:
				ImGui::InputFloat2(str_show.c_str(), pfvalue);
				return;
			case 3:
				ImGui::InputFloat3(str_show.c_str(), pfvalue);
				return;
			default:
				return;
			}
		}
	}
	/*int n = (_usize + 15) / 16;
	switch (_usize % 16) {
		           case 0:    do {		*to++ = *from++;
			           case 15:          *to++ = *from++;
			           case 14:          *to++ = *from++;
			           case 13:          *to++ = *from++;
			           case 12:          *to++ = *from++;
			           case 11:          *to++ = *from++;
			           case 10:          *to++ = *from++;
			           case 9:          *to++ = *from++;
			           case 8:          *to++ = *from++;
			           case 7:          *to++ = *from++;
			           case 6:          *to++ = *from++;
			           case 5:          *to++ = *from++;
			           case 4:          *to++ = *from++;
			           case 3:          *to++ = *from++;
			           case 2:          *to++ = *from++;
			           case 1:          *to++ = *from++;
		} while (--n >    0);

	}*/
}
void shader_uf_float::output_2_json(Value& jvalue)
{
	shader_uf::output_2_json(jvalue);
	jvalue["type"] = "shader_uf_float";

	Value jlist(arrayValue);
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize ; ix++)
	{
		jlist.append(Value(_pfvalue[ix]));
	}
	jvalue["value_list"] = jlist;
}
void shader_uf_float::init_from_json(Value& jvalue)
{
	shader_uf::init_from_json(jvalue);
	Value& value_list = jvalue["value_list"];
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		_pfvalue[ix] = value_list[ix].asDouble();
	}
}
void shader_uf_int::edit()
{
	int *pivalue = _pivalue;
	int wsize = _usize*_el_size;
	for (int ix = wsize; ix >0;)
	{
		string str_show = "###" + _unf_name;
		ostringstream ostream;
		ostream << ix;
		str_show += ostream.str();
		if (ix > 4)
		{
			ImGui::InputInt(str_show.c_str(), pivalue);
			ix -= 4;
			pivalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputInt4(str_show.c_str(), pivalue);
				return;
			case 1:
				ImGui::InputInt(str_show.c_str(), pivalue);
				return;
			case 2:
				ImGui::InputInt2(str_show.c_str(), pivalue);
				return;
			case 3:
				ImGui::InputInt3(str_show.c_str(), pivalue);
				return;
			default:
				return;
			}
		}
	}

}
void shader_uf_int::output_2_json(Value& jvalue)
{
	shader_uf::output_2_json(jvalue);
	jvalue["type"] = "shader_uf_int";
	Value jlist(arrayValue);
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		jlist.append(Value(_pivalue[ix]));
	}
	jvalue["value_list"] = jlist;
}
void shader_uf_int::init_from_json(Value& jvalue)
{
	shader_uf::init_from_json(jvalue);
	Value& value_list = jvalue["value_list"];
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		_pivalue[ix] = value_list[ix].asInt();
	}
}
void shader_uf_uint::edit()
{
	unsigned int* puivalue = _puivalue;
	int wsize = _usize*_el_size;
	for (int ix = wsize; ix >0;)
	{
		string str_show = "###" + _unf_name;
		ostringstream ostream;
		ostream << ix;
		str_show += ostream.str();
		if (ix > 4)
		{
			ImGui::InputInt(str_show.c_str(), (int*)puivalue);
			ix -= 4;
			puivalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputInt4(str_show.c_str(), (int*)puivalue);
				return;
			case 1:
				ImGui::InputInt(str_show.c_str(), (int*)puivalue);
				return;
			case 2:
				ImGui::InputInt2(str_show.c_str(), (int*)puivalue);
				return;
			case 3:
				ImGui::InputInt3(str_show.c_str(), (int*)puivalue);
				return;
			default:
				return;
			}
		}
	}
}
void shader_uf_uint::output_2_json(Value& jvalue)
{
	shader_uf::output_2_json(jvalue);
	jvalue["type"] = "shader_uf_uint";
	Value jlist(arrayValue);
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		jlist.append(Value(_puivalue[ix]));
	}
	jvalue["value_list"] = jlist;
}
void shader_uf_uint::init_from_json(Value& jvalue)
{
	shader_uf::init_from_json(jvalue);
	Value& value_list = jvalue["value_list"];
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		_puivalue[ix] = value_list[ix].asUInt();
	}
}
void shader_uf_double::edit()
{
	double* pdvalue = _pdvalue;
	int wsize = _usize*_el_size;
	for (int ix = wsize; ix >0;)
	{
		string str_show = "###" + _unf_name;
		ostringstream ostream;
		ostream << ix;
		str_show += ostream.str();
		if (ix > 4)
		{
			ImGui::InputFloat4(str_show.c_str(), (float*)pdvalue);
			ix -= 4;
			pdvalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputFloat4(str_show.c_str(), (float*)pdvalue);
				return;
			case 1:
				ImGui::InputFloat(str_show.c_str(), (float*)pdvalue);
				return;
			case 2:
				ImGui::InputFloat2(str_show.c_str(), (float*)pdvalue);
				return;
			case 3:
				ImGui::InputFloat3(str_show.c_str(), (float*)pdvalue);
				return;
			default:
				return;
			}
		}
	}
}
void shader_uf_double::output_2_json(Value& jvalue)
{
	shader_uf::output_2_json(jvalue);
	jvalue["type"] = "shader_uf_double";
	Value jlist(arrayValue);
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		jlist.append(Value(_pdvalue[ix]));
	}
	jvalue["value_list"] = jlist;
}
void shader_uf_double::init_from_json(Value& jvalue)
{
	shader_uf::init_from_json(jvalue);
	Value& value_list = jvalue["value_list"];
	GLuint wsize = _usize*_el_size;
	for (int ix = 0; ix <wsize; ix++)
	{
		_pdvalue[ix] = value_list[ix].asDouble();
	}
}
#endif

