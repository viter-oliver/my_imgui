
#ifndef __WarringLightId_h
#define __WarringLightId_h

enum TTBaseID
{
  TT_Begin,

  TT_FrontFog_On,   //ǰ��ƿ���
  TT_RearFog_On,    //����ƿ���
  TT_HighBeam_On,   //Զ��ƿ���
  TT_TurnLeft_On,   //��ת��ƿ���
  TT_TurnRight_On,  //��ת��ƿ���
  TT_Position_On,   //λ�ã�С�ƣ�ָʾ�� & ʾ��ָʾ��& β��ָʾ��
  TT_Engine_Fault,  //����������ָʾ��
  TT_AirBag_Fault,  //��ȫ����ָʾ�� & ��ȫ���ҹ���ָʾ��
  TT_ABS_Fault,     //ABS����ָʾ��
  TT_EPS_Fault,     //EPSϵͳ����ָʾ�� & �綯����ת��ϵͳ���� & EPS����ָʾ��  //������ʾ���ϣ���˸EPS����

  TT_CoolantTmpHigh_On,		//ˮ�¹��߱���ָʾ�� & �¶ȱ��� & ��ȴҺ�¶ȱ���
  TT_FuelOilLow_On,             //ȼ��Һλ����ָʾ�� & ��ȼ����λ����
  TT_EngineOilLow_On,           //����ѹ������ָʾ�� & ��������ѹ���㾯���
  TT_DoorOpen_On,               //�ſ�ָʾ�ƣ���δ�أ�
  TT_SafetyBeltMain_On,		//����ʻԱ��ȫ��δϵָʾ�� & ���ΰ�ȫ����ʾ��
  TT_TirePressure_Fault,        //̥ѹ����ָʾ�� & ��̥��ѹ�����
  TT_EngineGuard_On,            //����������ָʾ��
  TT_GearBox_Fault,             //���������ָʾ��
  TT_ACC_On,                    //����Ѳ������״ָ̬ʾ�� & Ѳ������ָʾ��
  TT_ESP_On,                    //ESC& ��& ESP& VSC����ָʾ��

  TT_ESP_Off,                 //ESC& ESC& VSC& ESP OFFָʾ��
  TT_Main_On,                 //������ָʾ�� & �������
  TT_Brake_Fault,             //�ƶ�ϵͳ����ָʾ�� & �ƶ�Һλ��&EBDָʾ��
  TT_EBD_Fault_On,            //EBD����ָʾ��   /*fs11���ƶ�Һ�ֿ���*/
  TT_Charge_On,               //���ָʾ��
  TT_EPB_On,                  //EPB����פ������ָʾ�� & פ��״ָ̬ʾ��
  TT_EPB_Fault,               //EPB����ָʾ��
  TT_AVH_On,                  //�Զ������̶����ܣ�AVH������ָʾ��/*��ESPϵͳʵ�ִ˹���*/
  TT_AVH_Active,              //AVH����ָʾ��
  TT_AVH_Fault,               //�Զ������̶����ܣ�AVH������ָʾ��
  TT_EP_On,                   //EP����ָʾ��
  TT_SafetyBeltAssist_On,     //����ʻ��ȫ��δϵ

  TT_SafetyBeltSecondLeft_On,   //�ڶ�����ȫ��δϵ
  TT_SafetyBeltSecondMid_On,    //�ڶ����а�ȫ��δϵ
  TT_SafetyBeltSecondRight_On,  //�ڶ����Ұ�ȫ��δϵ
  TT_SafetyBeltThridLeft_On,    //��������ȫ��δϵ
  TT_SafetyBeltThridMid_On,     //�������а�ȫ��δϵ
  TT_SafetyBeltThridRight_On,   //�������Ұ�ȫ��δϵ
  TT_ThinIce_On,                //��������
  TT_48vSystem_On,              //48vϵͳָʾ��
  TT_StartStop_On,              //��ͣ����ָʾ�� & ����ָʾ��
  TT_StartStop_Fault,           //��ͣ����ָʾ�� & ����ϵͳ����ָʾ��

  TT_DoubleFlash_On,          //˫��ָʾ��
  TT_LowBeam_On,              //����ƿ���
  TT_KeyState_On,             //Կ��״̬
  TT_Charge_Fault,            //���ϵͳ���Ͼ����
  TT_InitiativeSpeed_On,      //��������ϵͳ״̬
  TT_ACCSystem_On,            //ACCϵͳ״̬
  TT_ACCSystem_Fault,         //ACCϵͳ���ϱ���
  TT_FCW_On,                  //FCW/AEBϵͳ״̬��ʾ/���ϱ���
  TT_FCW_Off,                 //FCW/AEBϵͳ�ر���ʾ
  TT_LDW_On,                  //LDWϵͳ״̬��ʾ/���ϱ���

  TT_LCDA_On,                 //LCDAϵͳ���ϱ���
  TT_LCDA_Off,                //LCDAϵͳ�ر���ʾ
  TT_IHC_On,                  //IHCϵͳ״̬��ʾ/���ϱ���
  TT_HDC_On,                  //HDCָʾ��
  TT_FrontRadar_On,           //ǰ�״�ָʾ��
  TT_AFS_On,                  //AFS����ָʾ��
  TT_SlowDown_On,             //����ָʾ��
  TT_SVS_On,                  //��������ָʾ��
  TT_External_Fault,          //�ⲿ�ƹ����
  TT_ExcessiveWeakBrake_On,   //�ƶ��Ŀ����ĥ�𱨾�


  TT_BackLeftSeatGreen_On,    //����Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackMidSeatGreen_On,     //�к��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackRightSeatGreen_On,   //�Һ��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackLeftSeatGray_On,     //����Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackMidSeatGray_On,      //�к��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackRightSeatGray_On,    //�Һ��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackLeftSeatRed_On,      //����Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackMidSeatRed_On,       //�к��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_BackRightSeatRed_On,     //�Һ��Ű�ȫ��δϵ-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_EPBWorkGreen_On,         //EPB����ָʾ�� & �Զ�פ��ָʾ��-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)

  TT_EPBWorkRed_On,             //EPB����ָʾ�� & �Զ�פ��ָʾ��-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_CoolantWarnBlue_On,      	//ˮ�¹��߱���ָʾ�� & �¶ȱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_CoolantWarnRed_On,         //ˮ�¹��߱���ָʾ�� & �¶ȱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_CruiseControlGreen_On,     //����Ѳ������״ָ̬ʾ�� & Ѳ������ָʾ��-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_CruiseControlGray_On,     	//����Ѳ������״ָ̬ʾ�� & Ѳ������ָʾ��-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_InitiativeSpeedGreen_On,     //��������ϵͳ״̬-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_InitiativeSpeedGrayLimit_On, //��������ϵͳ״̬-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_AccGreenSystem_On,           //ACCϵͳ״̬-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_AccGraySystem_On,            //ACCϵͳ״̬-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_FcwSystemWarnYellow_On,      //FCW/AEBϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)

  TT_FcwSystemWarnRed_On,    	//FCW/AEBϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_FcwSystemWarnGray_On,    //FCW/AEBϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_LdwSystemWarnGreen_On,   //LDWϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_LdwSystemWarnYellow_On,  //LDWϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_LdwSystemWarnGray_On,  	//LDWϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_IhcSystemWarnGreen_On,   //IHCϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_IhcSystemWarnYellow_On,  //IHCϵͳ״̬��ʾ/���ϱ���-�� (��SRD���ر�ע����ɫ�⣬��ʹ�ø�ID)
  TT_LeftFrontFog_On,           //��ǰ��ƿ���
  TT_RightFrontFog_On,		//��ǰ��ƿ���


  TT_BrakeLight_On,	//ɲ����

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

