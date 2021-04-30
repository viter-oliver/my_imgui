#ifndef PAGEMANAGER__H_
#define  PAGEMANAGER__H_

#include "ft_base.h"
#include <map>
#include "AppDef.h"
#include "NormalDlg.h"

#define  emToStr(x) #x

class PageManager
{
public:
	~PageManager();
	PageManager(const PageManager&) = delete;
	static PageManager * getInstance()
	{
		static PageManager instance;
		return &instance;
	};
	void init(void){ mmapFtBasePageDlg.clear(); mmapFtBasePageID.clear(); mmapPageIDStr.clear(); };
	EmPageID getCurrentPageId(void){ return current_id; };
	std::map<std::string, ft_base*> mmapFtBasePageDlg;
	std::map<EmPageID, ft_base*> mmapFtBasePageID;
	std::map<EmPageID, std::string> mmapPageIDStr;
	void pageSwitchTo(std::string &str);
	void pageSwitchTo(EmPageID id);
	void hideCurrentPage(void);
	void switchToCurrentDlg(void);
	
	void switchToLowlvPage(void);	
	void hideLowlvPage(void);

	void hideMidPage(void);

	void showWithShowPage(void);

	void hideWithShowPage(bool have_pop=true);

	void pageSwitchToWithShow(EmPageID id);

	void currentDlgDealKey(EmKey key, Em_key_state state);

private:
	
	PageManager(){};

	ft_base * pagePtrCur = nullptr;
	PageDlgBase *pagePtrBaseCur = nullptr;
	EmPageID current_id{ TC_menu };
	EmPageID current_id_pre{ TC_menu };

	EmPageID low_id{ TC_menu };

	std::map<EmPageID, PageDlgBase *>mmapPageDlgBase;
	bool isHideCurrentDlg{ false };
	bool isHideMidePage{ false };

	bool isFirstPop{true};
	bool isEasyNavi{ false };
	bool isBigNavi{ false };
	bool isAdas{ false };
};


#endif