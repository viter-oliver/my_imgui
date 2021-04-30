
#ifndef __WarringLightId_h
#define __WarringLightId_h

enum TTBaseID
{
  TT_Begin,

  TT_FrontFog_On,   //前雾灯开启
  TT_RearFog_On,    //后雾灯开启
  TT_HighBeam_On,   //远光灯开启
  TT_TurnLeft_On,   //左转向灯开启
  TT_TurnRight_On,  //右转向灯开启
  TT_Position_On,   //位置（小灯）指示灯 & 示宽指示灯& 尾灯指示灯
  TT_Engine_Fault,  //发动机故障指示灯
  TT_AirBag_Fault,  //安全气囊指示灯 & 安全气囊故障指示灯
  TT_ABS_Fault,     //ABS故障指示灯
  TT_EPS_Fault,     //EPS系统警告指示灯 & 电动助力转向系统警告 & EPS故障指示灯  //常亮表示故障，闪烁EPS工作

  TT_CoolantTmpHigh_On,		//水温过高报警指示灯 & 温度报警 & 冷却液温度报警
  TT_FuelOilLow_On,             //燃油液位过低指示灯 & 低燃油油位警告
  TT_EngineOilLow_On,           //机油压力过低指示灯 & 发动机油压不足警告灯
  TT_DoorOpen_On,               //门开指示灯（门未关）
  TT_SafetyBeltMain_On,		//主驾驶员安全带未系指示灯 & 座椅安全带提示灯
  TT_TirePressure_Fault,        //胎压故障指示灯 & 轮胎气压警告灯
  TT_EngineGuard_On,            //发动机防盗指示灯
  TT_GearBox_Fault,             //变速箱故障指示灯
  TT_ACC_On,                    //定速巡航工作状态指示灯 & 巡航控制指示灯
  TT_ESP_On,                    //ESC& 打滑& ESP& VSC工作指示灯

  TT_ESP_Off,                 //ESC& ESC& VSC& ESP OFF指示灯
  TT_Main_On,                 //主报警指示灯 & 主警告灯
  TT_Brake_Fault,             //制动系统故障指示灯 & 制动液位低&EBD指示灯
  TT_EBD_Fault_On,            //EBD故障指示灯   /*fs11与制动液分开的*/
  TT_Charge_On,               //充电指示灯
  TT_EPB_On,                  //EPB电子驻车工作指示灯 & 驻车状态指示灯
  TT_EPB_Fault,               //EPB故障指示灯
  TT_AVH_On,                  //自动车辆固定功能（AVH）工作指示灯/*有ESP系统实现此功能*/
  TT_AVH_Active,              //AVH激活指示灯
  TT_AVH_Fault,               //自动车辆固定功能（AVH）故障指示灯
  TT_EP_On,                   //EP工作指示灯
  TT_SafetyBeltAssist_On,     //副驾驶安全带未系

  TT_SafetyBeltSecondLeft_On,   //第二排左安全带未系
  TT_SafetyBeltSecondMid_On,    //第二排中安全带未系
  TT_SafetyBeltSecondRight_On,  //第二排右安全带未系
  TT_SafetyBeltThridLeft_On,    //第三排左安全带未系
  TT_SafetyBeltThridMid_On,     //第三排中安全带未系
  TT_SafetyBeltThridRight_On,   //第三排右安全带未系
  TT_ThinIce_On,                //薄冰警告
  TT_48vSystem_On,              //48v系统指示灯
  TT_StartStop_On,              //启停工作指示灯 & 启动指示灯
  TT_StartStop_Fault,           //启停故障指示灯 & 怠速系统故障指示灯

  TT_DoubleFlash_On,          //双闪指示灯
  TT_LowBeam_On,              //近光灯开启
  TT_KeyState_On,             //钥匙状态
  TT_Charge_Fault,            //充电系统故障警告灯
  TT_InitiativeSpeed_On,      //主动限速系统状态
  TT_ACCSystem_On,            //ACC系统状态
  TT_ACCSystem_Fault,         //ACC系统故障报警
  TT_FCW_On,                  //FCW/AEB系统状态提示/故障报警
  TT_FCW_Off,                 //FCW/AEB系统关闭提示
  TT_LDW_On,                  //LDW系统状态提示/故障报警

  TT_LCDA_On,                 //LCDA系统故障报警
  TT_LCDA_Off,                //LCDA系统关闭提示
  TT_IHC_On,                  //IHC系统状态提示/故障报警
  TT_HDC_On,                  //HDC指示灯
  TT_FrontRadar_On,           //前雷达指示灯
  TT_AFS_On,                  //AFS工作指示灯
  TT_SlowDown_On,             //下坡指示灯
  TT_SVS_On,                  //车辆服务指示灯
  TT_External_Fault,          //外部灯光故障
  TT_ExcessiveWeakBrake_On,   //制动衬快过度磨损报警


  TT_BackLeftSeatGreen_On,    //左后排安全带未系-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackMidSeatGreen_On,     //中后排安全带未系-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackRightSeatGreen_On,   //右后排安全带未系-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackLeftSeatGray_On,     //左后排安全带未系-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackMidSeatGray_On,      //中后排安全带未系-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackRightSeatGray_On,    //右后排安全带未系-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackLeftSeatRed_On,      //左后排安全带未系-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackMidSeatRed_On,       //中后排安全带未系-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_BackRightSeatRed_On,     //右后排安全带未系-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_EPBWorkGreen_On,         //EPB工作指示灯 & 自动驻车指示灯-绿 (除SRD中特别注明颜色外，不使用该ID)

  TT_EPBWorkRed_On,             //EPB工作指示灯 & 自动驻车指示灯-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_CoolantWarnBlue_On,      	//水温过高报警指示灯 & 温度报警-蓝 (除SRD中特别注明颜色外，不使用该ID)
  TT_CoolantWarnRed_On,         //水温过高报警指示灯 & 温度报警-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_CruiseControlGreen_On,     //定速巡航工作状态指示灯 & 巡航控制指示灯-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_CruiseControlGray_On,     	//定速巡航工作状态指示灯 & 巡航控制指示灯-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_InitiativeSpeedGreen_On,     //主动限速系统状态-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_InitiativeSpeedGrayLimit_On, //主动限速系统状态-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_AccGreenSystem_On,           //ACC系统状态-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_AccGraySystem_On,            //ACC系统状态-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_FcwSystemWarnYellow_On,      //FCW/AEB系统状态提示/故障报警-黄 (除SRD中特别注明颜色外，不使用该ID)

  TT_FcwSystemWarnRed_On,    	//FCW/AEB系统状态提示/故障报警-红 (除SRD中特别注明颜色外，不使用该ID)
  TT_FcwSystemWarnGray_On,    //FCW/AEB系统状态提示/故障报警-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_LdwSystemWarnGreen_On,   //LDW系统状态提示/故障报警-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_LdwSystemWarnYellow_On,  //LDW系统状态提示/故障报警-黄 (除SRD中特别注明颜色外，不使用该ID)
  TT_LdwSystemWarnGray_On,  	//LDW系统状态提示/故障报警-灰 (除SRD中特别注明颜色外，不使用该ID)
  TT_IhcSystemWarnGreen_On,   //IHC系统状态提示/故障报警-绿 (除SRD中特别注明颜色外，不使用该ID)
  TT_IhcSystemWarnYellow_On,  //IHC系统状态提示/故障报警-黄 (除SRD中特别注明颜色外，不使用该ID)
  TT_LeftFrontFog_On,           //左前雾灯开启
  TT_RightFrontFog_On,		//右前雾灯开启


  TT_BrakeLight_On,	//刹车灯

  TT_End
};
enum TT_self_check
{
	TT_light_self_check,
	TT_ligth_no_self_check,
	TT_self_check_end
};

enum TT_light_sts
{
	light_off,
	light_on,
	blink_1hz,
	blink_2hz,
	blink_3hz,
	blink_4hz,
	blink_max
};

enum TT_power_sts
{
	TT_POWER_ING_ON,
	TT_POWER_ING_OFF,
	TT_POWER_ING_OFF_AND_ING_ON,
	TT_POWER_END
};

#endif

