#include <cmath>

#include "ft_raw_trans.h"

namespace auto_future
{
     ft_raw_trans::ft_raw_trans()
     {
		 float transM[16] = {1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0, 
							 1, 0, 0, 1, };
		 memcpy(_pt._trans_mat4x4, transM, 16 * sizeof (float));

		 
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		 
		 reg_property_handle(&_pt, 0, [this](void* member_address)
		 {
			 ImGui::Text("transformation:");
			 float* pf = _pt._trans_mat4x4;
			 ImGui::InputFloat4("1st vec4:", pf);
			 ImGui::InputFloat4("2nd vec4:", pf + 4);
			 ImGui::InputFloat4("3rd vec4:", pf + 8);
			 ImGui::InputFloat4("4th vec4:", pf + 12);

		 });
#endif
     }
     void ft_raw_trans::transform( glm::mat4& model )
     {
		
#if 1
		 glm::mat4 transM = glm::make_mat4(_pt._trans_mat4x4);
		 model = model* transM;
#else
		 glm::vec3 gtranslate(_pt._trans_pos.x, _pt._trans_pos.y, _pt._trans_pos.z);
		 glm::vec3 gscale(_pt._trans_scale.x, _pt._trans_scale.y, _pt._trans_scale.z);
		 glm::quat gquat(_pt._trans_qua.w, _pt._trans_qua.x, _pt._trans_qua.y, _pt._trans_qua.z);

		 model = glm::mat4_cast(gquat) * model;
		 model = glm::scale(model, gscale);
		 model = glm::translate(model, gtranslate);
#endif
     }
}