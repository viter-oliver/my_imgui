
#include "PageManager.h"

#include "af_bind.h"


PageManager::~PageManager()
{
	for (auto n = mmapPageDlgBase.begin(); n != mmapPageDlgBase.end();n++)
	{
		if (n->second)
		{
			delete n->second;
			n->second = NULL;
		}
	}
}

void PageManager::pageSwitchTo(std::string &str)
{
	//if (str)
	printf("str:%s\n", str.c_str());
	//从容器中搜索
	if (mmapFtBasePageDlg.find(str) != mmapFtBasePageDlg.end())
	{
		//mmapFtBasePageDlg[str]
		//第一次切换page
		if (pagePtrCur == nullptr)
		{
			auto ptr = mmapFtBasePageDlg[str];
			ptr->set_visible(true);
			pagePtrCur = ptr;
		}
		else
		{
			pagePtrCur->set_visible(false);
			auto ptr = mmapFtBasePageDlg[str];
			ptr->set_visible(true);
			pagePtrCur = ptr;
		}
	}
	else
		return;
}

void PageManager::pageSwitchTo(EmPageID id)
{
	//pageSwitchTo(std::string(emToStr(id)));
	printf("PageManager::pageSwitchTo id=%d\n", id);
	if (mmapFtBasePageID.find(id) != mmapFtBasePageID.end())
	{
		//mmapFtBasePageDlg[str]

		bool b = false;
		set_property_aliase_value(mmapPageIDStr[current_id].c_str(),&b);
		b = true;
		set_property_aliase_value(mmapPageIDStr[id].c_str(), &b);

		auto ptr = mmapFtBasePageID[id];
		if (ptr)
		{
			current_id = id;
		}
		else
		{
			printf("page id=%d, can not find ,check in mmapPageDlgBase\n", id);
		}
	}
		

	//在类中进行切换
	//1.在容器找到
	if (mmapPageDlgBase.find(id) != mmapPageDlgBase.end())
	{
		if (pagePtrBaseCur)
		{
			pagePtrBaseCur->onToLeaveStage();
			auto next = mmapPageDlgBase[id];
			next->onToBeginStage();
			pagePtrBaseCur = mmapPageDlgBase[id];
		}

	}		
	else//在容器中没有找到
	{

		PageDlgBase *p = nullptr;

		p = static_cast<PageDlgBase *>(CObjectFactory::createObject(GET_INSTANCE_FACTORY_MAP[id].c_str()));
		if (p == nullptr)
			return;
		mmapPageDlgBase.insert(std::pair<EmPageID, PageDlgBase*>(id, p));
		p->Init();

		if (pagePtrBaseCur)
			pagePtrBaseCur->onToLeaveStage();
		p->onToBeginStage();
		pagePtrBaseCur = p;
		current_id = id;
	}

	
}

void PageManager::hideCurrentPage(void)
{
	if (isHideCurrentDlg)
		return;

	//node invisable
	if (mmapFtBasePageID.find(current_id) != mmapFtBasePageID.end())
	{
		//mmapFtBasePageDlg[str]

		bool b = false;
		set_property_aliase_value(mmapPageIDStr[current_id].c_str(), &b);
	}

	if (mmapPageDlgBase.find(current_id) != mmapPageDlgBase.end())
	{
		mmapPageDlgBase[current_id]->onToLeaveStage();
	}

	isHideCurrentDlg = true;

}

void PageManager::switchToCurrentDlg(void)
{
	if (mmapFtBasePageID.find(current_id) != mmapFtBasePageID.end())
	{
		bool b = true;
		set_property_aliase_value(mmapPageIDStr[current_id].c_str(), &b);
	}

	if (mmapPageDlgBase.find(current_id) != mmapPageDlgBase.end())
	{
		mmapPageDlgBase[current_id]->onToBeginStage();
	}
	isHideCurrentDlg = false;
}

void PageManager::switchToLowlvPage(void)
{

	if (isBigNavi)
	{

	}
	else if (isEasyNavi)
	{

	}
	else
	{
		if (low_id != TC_menu || isHideMidePage)
		{
			current_id_pre = current_id;
			pageSwitchTo(TC_menu);
			low_id = TC_menu;
			isHideMidePage = false;
		}
	}
}

void PageManager::hideLowlvPage(void)
{
	switch (low_id)
	{
	case setting_3rd_easy_map:
		break;
	case setting_3rd_big_map:
		break;
	case page_adas:
		break;
	case TC_menu:
		pageSwitchTo(current_id_pre);
		low_id = id_none;
		break;

	default:
		break;
	}
}

void PageManager::hideMidPage(void)
{
	if (current_id == TC_menu || current_id == main_menu || current_id == setting_3rd_easy_map || current_id == setting_3rd_big_map)
	{
		isHideCurrentDlg = false;
		hideCurrentPage();
		isHideMidePage = true;
	}
}

void PageManager::showWithShowPage(void)
{
	if (isFirstPop)
	{
		low_id = current_id;
		isFirstPop = false;
	}

		//pageSwitchTo(setting_3rd_big_map);
	if (PageBigNavi::getInstance()->needShow)
	{
		PageBigNavi::getInstance()->show();
	}
	else if (PageAdas::getInstance()->needShow)
	{
		PageAdas::getInstance()->show();
	}
	else
	{
		pageSwitchTo(TC_menu);
	}
}

void PageManager::hideWithShowPage(bool have_pop/*=true*/)
{
	if (have_pop)
	{
		if (PageBigNavi::getInstance()->getShowSts())
			PageBigNavi::getInstance()->hide();
		else if (PageAdas::getInstance()->getShowSts())
			PageAdas::getInstance()->hide();
		else
		{
			if (isFirstPop)
			{
				low_id = current_id;
				isFirstPop = false;
			}
			//printf("11111111low id=%d\n", low_id);
			pageSwitchTo(id_none);
		}
	}
	else
	{
		PageBigNavi::getInstance()->hide();
		PageAdas::getInstance()->hide();
		if (PageBigNavi::getInstance()->needShow)
			PageBigNavi::getInstance()->show();
		else if (PageAdas::getInstance()->needShow)
			PageAdas::getInstance()->show();
		else
		{
			low_id = low_id == id_none ? TC_menu : low_id;
			//printf("222222low id=%d\n", low_id);
			pageSwitchTo(low_id);
			isFirstPop = true;
		}

	}

}

void PageManager::pageSwitchToWithShow(EmPageID id)
{
	switch (id)
	{
	case id_none:
		break;
	case setting_3rd_easy_map:
		break;
	case setting_3rd_big_map:
		break;
	case page_adas:
		break;
	case TC_menu:
		break;
	default:
		break;
	}
}

void PageManager::currentDlgDealKey(EmKey key, Em_key_state state)
{
	if (mmapPageDlgBase.find(current_id) != mmapPageDlgBase.end())
	{
		if (mmapPageDlgBase[current_id])
		{
			mmapPageDlgBase[current_id]->key_event(key, state);
			return;
		}
	}

	if (PageBigNavi::getInstance()->getShowSts())
		PageBigNavi::getInstance()->key_event(key, state);
	else 	if (PageAdas::getInstance()->getShowSts())
		PageAdas::getInstance()->key_event(key, state);
}