#include "shader_uf.h"
#include<string>
#include<sstream>
using namespace std;
GLuint shader_uf_txt::_sample_index = 0;

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "user_control_imgui.h"

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

void shader_uf_txt::edit()
{
	if (_pdtxt)
	{
		ImGui::Text("Texture name:%s", _txt_name);
		ImGui::SameLine();
		if (ImGui::Button("Delink##txtname"))
		{
			_pdtxt = nullptr;
			_txt_name[0] = '\0';
		}
		else
		{
			ImGui::Text("Size:%u,%u", _pdtxt->_width, _pdtxt->_height);
			float imw = _pdtxt->_width, imh = _pdtxt->_height;
			if (imw > 400.f)
			{
				imh = 400 * imh / imw;
				imw = 400;
			}
			ImGui::Image((ImTextureID)_pdtxt->_txt_id(), ImVec2(imw, imh), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}
		

	}
	else
	{
		ImGui::InputText("Texture name", _txt_name, FILE_NAME_LEN);
		if (ImGui::Button("Link##txtobj"))
		{
			auto& itxt = g_mtexture_list.find(_txt_name);
			if (itxt!=g_mtexture_list.end())
			{
				_pdtxt = itxt->second;
			}
			else
			{
				_txt_name[0] = '\0';
			}
		}
	}
}
void shader_uf_txt::output_2_json(Value& jvalue)
{
	shader_uf::output_2_json(jvalue);
	jvalue["type"] = "shader_uf_txt";
	jvalue["txt_name"] = _txt_name;
}
void shader_uf_txt::init_from_json(Value& jvalue)
{
	shader_uf::init_from_json(jvalue);
	strcpy(_txt_name,jvalue["txt_name"].asCString());
	auto& itxt = g_mtexture_list.find(_txt_name);
	if (itxt!=g_mtexture_list.end())
	{
		_pdtxt = itxt->second;
	}
}
#endif
void shader_uf_txt::link(){
      //auto isz=g_mtexture_list.size();
	//printf("mtext size=% d\n",isz); 
	/*char* ptn=_txt_name;
	while(*ptn)
	{
		ptn++;
	}
	*ptn='\0';*/
	printf("txt=%s",_txt_name);
	const auto& itxt = g_mtexture_list.find(_txt_name);
	if (itxt != g_mtexture_list.end())
	{
		_pdtxt = itxt->second;
		//printf("matched a texture\n");
	}
	else
	{
		//printf("failto find %s  \n",_txt_name);
		for(auto& imt:g_mtexture_list)
		{
			printf("txtname:%s\n",imt.first.c_str());	
		}
	}
}
