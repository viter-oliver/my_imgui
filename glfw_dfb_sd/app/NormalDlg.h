
#pragma execution_character_set("utf-8")

#pragma once



#include "ft_base.h"
#include "AppDef.h"
#include "ft_video.h"

class PageDlgBase
{
public:
	PageDlgBase(){};
	virtual ~PageDlgBase(){};
	virtual void Init(void) = 0;
	virtual void onToBeginStage(void) = 0;
	virtual void onToLeaveStage(void) = 0;

	virtual bool key_event(EmKey key, Em_key_state state) = 0;

	friend class PageManager;
};
//行车电脑,瞬时油耗，
class PageTCDlg : public PageDlgBase
{
public:
	PageTCDlg(){};
	~PageTCDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	bool key_event(EmKey key, Em_key_state state);
	
	void show_up(bool);
private:
	bool isShowUp{ false };
};
class PageTCMainMenuDlg : public PageDlgBase
{
public:
	PageTCMainMenuDlg(){};
	~PageTCMainMenuDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	bool key_event(EmKey key, Em_key_state state);

	void show_up(bool);
private:
	bool isShowUp{ false };

};


//主菜单
class PageMainMenuDlg : public PageDlgBase
{
public:
	PageMainMenuDlg(){};
	~PageMainMenuDlg(){};
	void Init(void) override {};
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	bool key_event(EmKey key, Em_key_state state);
private:
	Em_main_menu id{ main_menu_media };
};
//设置界面
class PageSettingDlg : public PageDlgBase
{
public:
	PageSettingDlg(){};
	~PageSettingDlg(){};
	 void Init(void) override;
	 void onToBeginStage(void) override;
	 void onToLeaveStage(void) override;
	 void show_cursor(Em_setting id);

	 bool key_event(EmKey key, Em_key_state state);
private:
	Em_setting id{ setting_2nd_map };
};
//多媒体
class PageMediaDlg : public PageDlgBase
{
public:
	PageMediaDlg(){};
	~PageMediaDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);

	bool key_event(EmKey key, Em_key_state state);
private:
	//Em_setting id{ setting_2nd_map };
};
//多媒体音乐
class PageMediaMusicDlg : public PageDlgBase
{
public:
	PageMediaMusicDlg(){};
	~PageMediaMusicDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);

	bool key_event(EmKey key, Em_key_state state);
private:
	//Em_setting id{ setting_2nd_map };
};
//多媒体电台
class PageMediaRadioDlg : public PageDlgBase
{
public:
	PageMediaRadioDlg(){};
	~PageMediaRadioDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);

	bool key_event(EmKey key, Em_key_state state);
private:
	//Em_setting id{ setting_2nd_map };
};


//故障列表
class PageFaultListDlg : public PageDlgBase
{
public:
	PageFaultListDlg(){};
	~PageFaultListDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);

	bool key_event(EmKey key, Em_key_state state);
private:

};
//设置界面的导航
class PageNaviDlg : public PageDlgBase
{
public:
	PageNaviDlg(){};
	~PageNaviDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	void show_cursor(Em_map id);

	bool key_event(EmKey key, Em_key_state state);
private:
	Em_map id{ map_esy };
};

//清零行车电脑数据
class PageResetTcDlg : public PageDlgBase
{
public:
	PageResetTcDlg(){};
	~PageResetTcDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);
	void show_cursor(Em_tc_trip id);
	bool key_event(EmKey key, Em_key_state state);
private:
	Em_tc_trip id{ tc_tripa };

};

//清零tripa
class PageResetTripaDlg : public PageDlgBase
{
public:
	PageResetTripaDlg(){};
	~PageResetTripaDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);
	void show_cursor(Em_decide id);
	bool key_event(EmKey key, Em_key_state state);
private:
	Em_decide id{ decide_ok };

};

//清零avg

class PageResetAvgDlg : public PageDlgBase
{
public:
	PageResetAvgDlg(){};
	~PageResetAvgDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_setting id);
	void show_cursor(Em_decide id);
	bool key_event(EmKey key, Em_key_state state);
private:
	Em_decide id{ decide_ok };

};

//恢复出厂设置
class PageResetFactoryDlg : public PageDlgBase
{
public:
	PageResetFactoryDlg(){};
	~PageResetFactoryDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	//void show_cursor(Em_decide id);
	bool key_event(EmKey key, Em_key_state state);
private:
	Em_decide id{ decide_ok };
	bool is_show_ok{ false };
};

class PageNoneDlg :public PageDlgBase
{
public:
	PageNoneDlg(){};
	~PageNoneDlg(){};
	void Init(void) override;
	void onToBeginStage(void) override;
	void onToLeaveStage(void) override;
	bool key_event(EmKey key, Em_key_state state);
};


//全屏大导航
class PageBigNavi
{
public:
	~PageBigNavi(){};
	PageBigNavi(const PageBigNavi&) = delete;
	static PageBigNavi * getInstance()
	{
		static PageBigNavi instance;
		return &instance;
	};
	void init(void);
	void show(void);
	void hide(void);
	void key_event(EmKey key, Em_key_state state);
	bool getShowSts(void){ return visable; };
	bool needShow{ false };
	bool visable{ false };

private:
	PageBigNavi(){};
};
//ADAS
class PageAdas
{
public:
	~PageAdas(){};
	PageAdas(const PageAdas&) = delete;
	static PageAdas * getInstance()
	{
		static PageAdas instance;
		return &instance;
	};
	void init(void);
	void show(void);
	void hide(void);
	void key_event(EmKey key, Em_key_state state);
	bool getShowSts(void){ return visable; };
	bool needShow{ false };
	bool visable{ false };
private:
	PageAdas(){};
};



