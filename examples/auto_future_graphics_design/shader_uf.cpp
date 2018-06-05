#include "shader_uf.h"
#include<string>
using namespace std;
#include "user_control_imgui.h"
void shader_uf_float::edit()
{
	for (int ix = _usize; ix >0;)
	{
		//ImGui::InputFloatN()
		if (ix > 4)
		{
			ImGui::InputFloat4("input float", _pfvalue);
			ix -= 4;
			_pfvalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputFloat4("input float", _pfvalue);
				return;
			case 1:
				ImGui::InputFloat("input float", _pfvalue);
				return;
			case 2:
				ImGui::InputFloat2("input float", _pfvalue);
				return;
			case 3:
				ImGui::InputFloat3("input float", _pfvalue);
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

void shader_uf_int::edit()
{
	for (int ix = _usize; ix >0;)
	{
		//ImGui::InputFloatN()
		if (ix > 4)
		{
			ImGui::InputInt("input float", _pivalue);
			ix -= 4;
			_pivalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputInt4("input float", _pivalue);
				return;
			case 1:
				ImGui::InputInt("input float", _pivalue);
				return;
			case 2:
				ImGui::InputInt2("input float", _pivalue);
				return;
			case 3:
				ImGui::InputInt3("input float", _pivalue);
				return;
			default:
				return;
			}
		}
	}

}

void shader_uf_uint::edit()
{
	for (int ix = _usize; ix >0;)
	{
		//ImGui::InputFloatN()
		if (ix > 4)
		{
			ImGui::InputInt("input float", (int*)_puivalue);
			ix -= 4;
			_puivalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputInt4("input float", (int*)_puivalue);
				return;
			case 1:
				ImGui::InputInt("input float", (int*)_puivalue);
				return;
			case 2:
				ImGui::InputInt2("input float", (int*)_puivalue);
				return;
			case 3:
				ImGui::InputInt3("input float", (int*)_puivalue);
				return;
			default:
				return;
			}
		}
	}
}

void shader_uf_double::edit()
{
	for (int ix = _usize; ix >0;)
	{
		//ImGui::InputFloatN()
		if (ix > 4)
		{
			ImGui::InputFloat4("input float", (float*)_pdvalue);
			ix -= 4;
			_pdvalue += 4;
		}
		else
		{
			int itrim = ix % 4;
			switch (itrim)
			{
			case 0:
				ImGui::InputFloat4("input float", (float*)_pdvalue);
				return;
			case 1:
				ImGui::InputFloat("input float", (float*)_pdvalue);
				return;
			case 2:
				ImGui::InputFloat2("input float", (float*)_pdvalue);
				return;
			case 3:
				ImGui::InputFloat3("input float", (float*)_pdvalue);
				return;
			default:
				return;
			}
		}
	}
}