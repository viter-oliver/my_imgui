
#include "mobileye_adas.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "enum_txt_name0.h"
using namespace auto_future_utilities;
extern msg_host_n g_msg_host;
const char* road_primitice = "road.FBX0";
const char* mtl_name = "mt_sp";
const char* ldw_left_txt = "ldw_left.png";
const char* ldw_right_txt = "ldw_righr.png";
const char* road_txt = "road.png";
MSC_PACK_HEAD
//bool set_mp_text_uf(const char* mtl_nm, const char* txt_uf_nm, const char* txt_nm)
//{
//	const auto& imtl = g_material_list.find(mtl_nm);
//	if (imtl != g_material_list.end())
//	{
//		auto& mtl = *imtl->second;
//		auto& mp_sduf = mtl.get_mp_sd_uf();
//		const auto& isduf = mp_sduf.find(txt_uf_nm);
//		if (isduf != mp_sduf.end())
//		{
//			auto& txt_sduf = *isduf->second;
//			char* data_hd = (char*)txt_sduf.get_data_head();
//			memcpy(data_hd, txt_nm, strlen(txt_nm));
//			data_hd[strlen(txt_nm)] = '\0';
//			txt_sduf.link();
//			return true;
//		}
//	}
//	return false;
//}


//ps_primrive_object ps_prm_road;
/*
const int numb_of_vertex = 126;
const int len_of_verterx = 8;
static float base_road_vertexs[numb_of_vertex][len_of_verterx];
static float road_vertexs[numb_of_vertex][len_of_verterx];
static bool road_veters_left[numb_of_vertex];
static float base_y=0.;

void init_raod_vertex()
{
	for (int ix = 0; ix < numb_of_vertex;ix++)
	{
		auto& vertex_u = base_road_vertexs[ix];
		road_veters_left[ix] = vertex_u[3] < 0;
		if (vertex_u[4]>base_y)
		{
			base_y = vertex_u[4];
		}
	}
}
void calcu_offset_x(float curvature, float lane_heading,int ix)
{
	float yvalue = base_y-base_road_vertexs[ix][4];
	road_vertexs[ix][3] = curvature*yvalue*yvalue + lane_heading*yvalue + base_road_vertexs[ix][3];
}
void calcu_vertex(float curvature_left, float lane_heading_left, float curvature_right, float lane_heading_right)
{
	for (int ix = 0; ix < numb_of_vertex;ix++)
	{
		if (road_veters_left[ix])
		{
			calcu_offset_x(curvature_left, lane_heading_left, ix);
		}
		else
		{
			calcu_offset_x(curvature_right, lane_heading_right, ix);
		}
	}
}
*/
//static const char* mtl_name = "mt_sp";
void print_buff(u8* pbuff, int len)
{
	static char num_tb[0x10] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	struct buff_data
	{
		u8 data_l : 4;
		u8 data_h : 4;
	};
	string pt_buff;
	while (len > 0)
	{
		buff_data* pdata = (buff_data*)pbuff++;
		len--;
		pt_buff += " x";
		pt_buff += num_tb[pdata->data_h];
		pt_buff += num_tb[pdata->data_l];
	}
	printf("%s\n", pt_buff.c_str());
}

ps_mtl mtl_lane;
enum lane_type
{
	en_lane_dashed,
	en_lane_solid,
	en_lane_none,
	en_lane_road_edge,
	en_lane_double_lane_mark,
	en_lane_bott_dots,
	en_lane_invalid,
};
bool prohibit_lane_swith(u8 lane_type)
{
	bool ret= lane_type==en_lane_solid||lane_type==en_lane_road_edge||lane_type==en_lane_double_lane_mark||lane_type==en_lane_invalid;
	return ret;
}
u8 left_lane_type=0,right_lane_type=0;
static u16 left_curvature_stm = 0, left_lane_heading_stm = 0;
static u16 right_curvature_stm = 0, right_lane_heading_stm = 0;
static s16 left_position=0,right_position=0;
float uf_value = 0.f;
void calcu_left_right_lanes()
{
	//return;
	if (left_curvature_stm != 0 && left_lane_heading_stm != 0 && right_curvature_stm != 0 && right_lane_heading_stm!=0)
	{
		float cuv_lt = (left_curvature_stm - 0x7fff) / 1024000.0;
		float lh_lt = (left_lane_heading_stm - 0x7fff) / 1024.0;
		float cuv_rt = (right_curvature_stm - 0x7fff) / 1024000.0;
		float lh_rt = (right_lane_heading_stm - 0x7fff) / 1024.f;
		//calcu_vertex(cuv_lt, lh_lt, cuv_rt, lh_rt);
		//printf("left_curvature_stm=0x%x,left_lane_heading_stm=0x%x,right_curvature_stm=0x%x,right_lane_heading_stm=0x%x\n", left_curvature_stm, left_lane_heading_stm, right_curvature_stm, right_lane_heading_stm);
		//printf("cuv_lt=%f,lh_lt=%f,cuv_rt=%f,lh_rt=%f\n", cuv_lt, lh_lt, cuv_rt, lh_rt);
		//glBindBuffer(GL_ARRAY_BUFFER, ps_prm_road->_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*ps_prm_road->_vertex_buf_len, road_vertexs, GL_DYNAMIC_DRAW);
		//update_prm_vbo(road_primitice, road_vertexs[0], numb_of_vertex * len_of_verterx);
		mtl_lane->set_value("curv_l", &cuv_lt, 1);
		mtl_lane->set_value("head_l", &lh_lt, 1);
		mtl_lane->set_value("curv_r", &cuv_rt, 1);
		mtl_lane->set_value("head_r", &lh_rt, 1);

		left_curvature_stm = left_lane_heading_stm = right_curvature_stm = right_lane_heading_stm = 0;
	}
}
void register_adas_cmd_handl()
{
	//const auto& iprm = g_primitive_list.find(road_primitice);
	//if (iprm!=g_primitive_list.end())
	//{
	//	ps_prm_road = iprm->second;
	//	auto& ps_file = ps_prm_road->_ps_file;
	//	char* phead = (char*)ps_file->_pbin;
	//	GLuint* phead_buff_len = (GLuint*)phead;
	//	phead += 4;
	//	assert(*phead_buff_len == numb_of_vertex * len_of_verterx * sizeof(float));
	//	memcpy(&road_vertexs[0][0], phead, *phead_buff_len);
	//	memcpy(&base_road_vertexs[0][0], phead, *phead_buff_len);
	//	init_raod_vertex();
	//}
	/*if (get_prm_data(road_primitice, &road_vertexs[0][0], numb_of_vertex * len_of_verterx))
	{
	memcpy(&base_road_vertexs[0][0], road_vertexs, numb_of_vertex * len_of_verterx * sizeof(float));
	init_raod_vertex();
	}*/
	const auto& imtl = g_material_list.find("mt_sp");
	assert(imtl != g_material_list.end());
	mtl_lane=imtl->second;
	uf_value = 350.f;
	mtl_lane->set_value("base_y", &uf_value, 1);
	g_msg_host.attach_monitor("can message", [&](u8* pbuff, int len){
		printf("can message:\n");
		print_buff(pbuff, len);
		pbuff += 2;
		struct GNU_DEF can_msg
		{
			u8 b0_undocumented : 3;
			u8 b0_time_indicator : 2;
			u8 b0_sound_type : 3;

			u8 b1_reserved : 2;
			u8 b1_zero_spped : 1;
			u8 b1_reserved_n : 4;
			u8 b1_0 : 1;

			u8 b2_headway_measurement : 7;
			u8 b2_headway_valid : 1;

			u8 b3_error_code : 7;
			u8 b3_error : 1;

			u8 b4_failsafe : 1;
			u8 b4_maintenance : 1;
			u8 b4_undocumented : 2;
			u8 b4_fcw_on : 1;
			u8 b4_right_ldw_on : 1;
			u8 b4_left_ldw_on : 1;
			u8 b4_ldw_off : 1;

			u8 b5_tsr_enabled : 1;
			u8 b5_reseved : 1;
			u8 b5_tamper_alert : 1;
			u8 b5_reserved2 : 2;
			u8 b5_peds_in_dz : 1;
			u8 b5_peds_fcw : 1;
			u8 b5_zero : 1;

			u8 b6_reserved : 5;
			u8 b6_tsr_warning_level : 3;

			u8 b7_reserved : 5;
			u8 b7_hw_repeatable_enabled : 1;
			u8 b7_headway_warning_level : 2;
		};
		can_msg* pcan_msg = (can_msg*)pbuff;
		bool be_show;
		if (pcan_msg->b2_headway_valid)
		{
			int dist_id = 0;
			static u8 segs[] = { 99 - 16, 99 - 2 * 16, 99 - 3 * 16, 99 - 3 * 16, 99 - 4 * 16, 99 - 5 * 16 };
			u8 head_way_mm = pcan_msg->b2_headway_measurement;
			for (; dist_id < sizeof(segs); dist_id++)
			{
				if (head_way_mm > segs[dist_id])
				{
					break;
				}
			}
			if (dist_id>5)
			{
				dist_id = 5;
			}
			be_show = true;
			char seg_name[50] = "";
			sprintf(seg_name, "show_distance%d", dist_id);
			set_property_aliase_value(seg_name, &be_show);
		}
		if (pcan_msg->b4_ldw_off)
		{
			set_mp_text_uf("mt_sp", "text", "road.png");
		}
		else
		{
			if ((pcan_msg->b0_sound_type==1||pcan_msg->b4_left_ldw_on)&&pcan_msg->b4_right_ldw_on==0)
			{
				set_mp_text_uf("mt_sp", "text", "ldw_left.png");
			}
			else
			if ((pcan_msg->b0_sound_type==2||pcan_msg->b4_right_ldw_on)&&pcan_msg->b4_left_ldw_on==0)
			{
				set_mp_text_uf("mt_sp", "text", "ldw_righr.png");
			}
		}
		
		be_show = pcan_msg->b5_peds_in_dz;
		if (pcan_msg->b5_peds_in_dz)
		{
			be_show = true;
			if (pcan_msg->b5_peds_fcw)
			{
				set_property_aliase_value("peds_war2", &be_show);
			}
			else
			{
				set_property_aliase_value("peds_war1", &be_show);
			}
		}
		else
		{
			be_show = false;
			set_property_aliase_value("peds_war1", &be_show);
			set_property_aliase_value("peds_war2", &be_show);
		}
		//set_property_aliase_value("")
	});
       g_msg_host.attach_monitor("car info",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF car_info
		{
			u8 b0_reserved:1;
			u8 b0_resvered1:1;
			u8 b0_high_beam:1;
			u8 b0_low_beam:1;
			u8 b0_wipers:1;
			u8 b0_right_signal:1;
			u8 b0_left_signal:1;
			u8 b0_brakes:1;
			
			u8 b1_speed_available:1;
			u8 b1_reserved:1;
			u8 b1_high_beam_available:1;
			u8 b1_low_beam_available:1;
			u8 b1_wipers_available:1;
			u8 b1_reserved2:3;

			u8 b2_speed;
			u8 b3_reserved;
			u8 b4_reserved;
			u8 b5_reserved;
			u8 b6_reserved;
			u8 b7_reserved;
		};
		car_info* pcar_info=(car_info*)pbuff;

		bool be_show;
		int txt_idx = 0;
		if (pcar_info->b0_high_beam||pcar_info->b0_low_beam)
		{
			be_show = true;
			txt_idx = pcar_info->b0_high_beam ? en_vinfo_fullbeam_headlight_png : en_vinfo_dipped_headlight_png;
			set_property_aliase_value("show_beam_low_high", &be_show);
			set_property_aliase_value("control_beam_low_high", &txt_idx);
		}
		else{
			be_show = false;
			set_property_aliase_value("show_beam_low_high", &be_show);
		}

		//if(pcar_info)
	});	
    g_msg_host.attach_monitor("tsr message",[&](u8* pbuff,int len){
	   	u16* ptsr_id=(u16*)pbuff;
		pbuff+=2;
		struct GNU_DEF tsr_message
		{
			u8 b0_vision_only_sign_type;
			u8 b1_vision_only_supplementary_sign_type;
			u8 b2_sign_position_x;
			u8 b3_na:1;
			u8 b3_sign_position_y:7;
			u8 b4_na:2;
			u8 b4_position_z:6;
			u8 b5_filter_type;
			u8 b6_na;
			u8 b7_na;
		};
		tsr_message* ptsr_message=(tsr_message*)pbuff;
	});	
    g_msg_host.attach_monitor("tsr display",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF tsr_display
		{
			u8 b0_vision_only_sign_type_display1;
			u8 b1_vision_only_supplementary_sign_type_display1;
			u8 b2_vision_only_sign_type_display2;
			u8 b3_vision_only_supplementary_sign_type_display2;
			u8 b4_vision_only_sign_type_display3;
			u8 b5_vision_only_supplementary_sign_type_display3;
			u8 b6_vision_only_sign_type_display4;
			u8 b7_vision_only_supplementary_sign_type_display4;
			
		};
		tsr_display* ptsr_display=(tsr_display*)pbuff;
	});
    g_msg_host.attach_monitor("details of lane",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_of_lane
		{
			u8 b0_lane_type_left:4;
			u8 b0_undocumented:1;
			u8 b0_ldw_available_left:1;
			u8 b0_lane_confidence_left:2;
			
			u8 b1_distance_to_left_lane_lsb:4;
			u8 b1_reserved:4;
			
			u8 b2_distance_to_left_lane_msb;
			
			u8 b3_resreved;
			u8 b4_reserved;
			
			u8 b5_lane_type_right:4;
			u8 b5_undocumented:1;
			u8 b5_ldw_available_right:1;
			u8 b5_lane_confidence_right:2;
			
			u8 b6_distance_to_right_lane_lsb:4;
			u8 b6_reserved:4;
			
			u8 b7_distance_to_right_lane_msb;
			
		};
		details_of_lane* pdetails_of_lane=(details_of_lane*)pbuff;
		left_lane_type=pdetails_of_lane->b0_lane_type_left;
		right_lane_type=pdetails_of_lane->b5_lane_type_right;
	});
    g_msg_host.attach_monitor("lka left lane a",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF lka_left_lane_a
		{
			u8 b0_model_degree:2;u8 b0_quality:2;u8 b0_lane_type : 4;

			u8 b1_position_c0_byte0;

			u8 b2_position_c0_byte1;
			
			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;			
		};
		lka_left_lane_a* pdetails_lane = (lka_left_lane_a*)pbuff;
		s16* plpos=(s16*)(pbuff+1);
		left_position= *plpos;
		left_curvature_stm = pdetails_lane->b3_curvature_c2_byte0  + pdetails_lane->b4_curvature_c2_byte1* 0x100;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka left lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_left_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_availability:1;
			u8 b3_view_range_msb:7;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;			
		};
		lka_left_lane_b* pdetails_lane = (lka_left_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		left_lane_heading_stm = *pheading;
		calcu_left_right_lanes();
	});
	g_msg_host.attach_monitor("lka right lane a", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_a
		{
			u8 b0_model_degree : 2;	u8 b0_quality : 2;u8 b0_lane_type : 4;

			u8 b1_position_c0_byte0;

			u8 b2_position_c0_byte1;

			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;
		};
		lka_right_lane_a* pdetails_lane = (lka_right_lane_a*)pbuff;
		s16* plpos=(s16*)(pbuff+1);
		right_position= *plpos;
		right_curvature_stm = pdetails_lane->b3_curvature_c2_byte0  + pdetails_lane->b4_curvature_c2_byte1* 0x100;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka right lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_availability : 1;
			u8 b3_view_range_msb : 7;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;
		};
		lka_right_lane_b* pdetails_lane = (lka_right_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		right_lane_heading_stm = *pheading;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("details - lane", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_lane
		{
			u8 b0_lane_cuevature_lsb;
			u8 b1_lane_cuevature_msb;

			u8 b2_lane_heading_lsb;

			u8 b3_na : 1;
			u8 b3_left_ldw_availability : 1;
			u8 b3_right_ldw_availability : 1;
			u8 b3_ca : 1;
			u8 b3_lane_heading_msb : 4;

			u8 b4_yaw_agle_lsb;
			u8 b5_yaw_agle_msb;

			u8 b6_pitch_angel_lsb;
			u8 b7_pitch_angel_msb;


		};
		details_lane* pdetails_lane = (details_lane*)pbuff;
	});
    g_msg_host.attach_monitor("details - obstacle status",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_status
		{
			u8 b0_num_obstacles;
			
			u8 b1_timestamp;
			
			u8 b2_appliaction_vesion;
			
			u8 b3_go:4;
			u8 b3_right_close_range_cut_in:1;
			u8 b3_left_close_range_cut_in:1;
			u8 b3_avtive_version_number_section:2;
			
			u8 b4_protocol_version;
			
			u8 b5_reserved:3;
			u8 b5_failsafe:4;
			u8 b5_close_car:1;			
		};
		details_obstacle_status* pdetails_obstacle_status=(details_obstacle_status*)pbuff;
		
	});
       g_msg_host.attach_monitor("details - obstacle data a",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_data_a
		{
			u8 b0_obstacle_id;
			
			u8 b1_obstacle_pos_x_lsb;
			
			u8 b2_null:3;
			u8 b2_obstacle_pos_x_msb:5;
			
			u8 b3_obstacle_pos_y_lsb;
			
			u8 b4_cut_in_and_out:3;
			u8 b4_blinker_info:3;
			u8 b4_obstacle_pos_y_msb:2;
			
			u8 b5_obstacle_rel_vel_x_lsb;
			
			u8 b6_reserved:1;
			u8 b6_obstacle_type:3;
			u8 b6_obstacle_rel_vel_x_msb;
			
			u8 b7_obstacle_valid:2;
			u8 b7_reserved:2;
			u8 b7_obstacle_brake_lights:1;
			u8 b7_obstacle_status:3;
		
		};
		details_obstacle_data_a* pdetails_obstacle_data=(details_obstacle_data_a*)pbuff;
	});
       g_msg_host.attach_monitor("details - obstacle data b",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_data_b
		{
			u8 b0_obstacle_length;
			
			u8 b1_obstacle_width;
			
			u8 b2_obstacle_age;
			
			u8 b3_rader_pos_x_lsb:4;
			u8 b3_reserved:1;
			u8 b3_cipv_flag:1;
			u8 b3_obstacle_lane:2;
			
			u8 b4_rader_pos_x_msb;
			
			u8 b5_radar_vel_x_lsb;
			
			u8 b6_reserved:1;
			u8 b6_radar_match_confidence:3;
			u8 b6_radar_vel_x_msb:4;
			
			u8 b7_reserved:1;
			u8 b7_matched_radar_id;
		
		};
		details_obstacle_data_b* pdetails_obstacle_data=(details_obstacle_data_b*)pbuff;
	});
       g_msg_host.attach_monitor("details - obstacle data c",[&](u8* pbuff,int len){
		pbuff+=2;
		struct GNU_DEF details_obstacle_data_c
		{
			u8 b0_obstacle_angle_rate_lsb;
			
			u8 b1_obstacle_angle_rate_msb;
						
			u8 b2_obstacle_scale_change_lsb;
			
			u8 b3_obstacle_scale_change_msb;
						
			u8 b4_object_accel_x;
			
			u8 b5_reserved:3;
			u8 b5_obstacle_replaced:1;
			u8 b5_null:2;
			u8 b5_object_accel_x:2;
						
			u8 b6_obstacle_angle_lsb;
			
			u8 b7_obstacle_angle_msb;
		
		};
		details_obstacle_data_c* pdetails_obstacle_data=(details_obstacle_data_c*)pbuff;
	});
	   
	
	   
}
