#pragma once

enum EmPageID
{
	id_none,

	pop_win_page_lv2_3,
	blue_tooth_page,
	pop_win_page_lv1,

	main_menu,

	//2nd menu
	setting_menu_1st,
	media,
	fault_list,
	TC_in_main_menu,

	//3rd menu
	setting_2nd_navi,
	setting_2nd_reset_TC_data,
	setting_2nd_reset_factory,

	media_2nd_usb_music,
	media_2nd_bluetooth_music,
	media_2nd_locate_music,
	media_2nd_radio_fm,
	media_2nd_radio_am,
	//4th menu
	setting_3rd_reset_avg,
	setting_3rd_reset_tripa,

	setting_3rd_easy_map,
	setting_3rd_big_map,

	page_adas,
	TC_menu,

	page_id_max
};


enum EmKey
{
	key_space,
	key_menu,
	key_return,
	key_up,
	key_down,
	key_left,
	key_right,
	key_calling,
	key_voice,
	key_ok,
	key_value_max
};

enum Em_key_state
{
	key_state_none,
	key_state_press,
	key_state_up,
	key_state_long_press,
	key_state_state_max
};

enum Em_key_from{
	cluster, //仪表
	IVI,//中控
	from_max
};

enum Em_main_menu{
	main_menu_media,
	main_menu_setting,
	main_menu_tc,
	main_menu_fault_list,
	main_menu_max
};

enum Em_setting{
	setting_2nd_map,
	setting_2nd_reset,
	setting_2nd_factory,
	setting_2nd_max
};

enum Em_map{
	map_esy,
	map_big,
	map_max
};

enum Em_tc_trip{
	tc_tripa,
	tc_avg,
	tc_max
};

enum Em_decide{
	decide_ok,
	decide_cancle,
	decide_max
};

#define  Propety_def(type,name)  \
 private : \
		type m_##name; \
public:    \
 type get_##name() \
	{\
		 return m_##name; \
	}; \
	\
 void set_##name(type c)\
	{\
		m_##name = c;\
	 }; \

#define  DATA_STORE SingleDataStore::getInstance()
class SingleDataStore
{
public:
	SingleDataStore(const SingleDataStore&) = delete;
	static SingleDataStore & getInstance()
	{
		static SingleDataStore instance;
		return instance;
	};
	~SingleDataStore(){};
	Propety_def(std::string, song_name);
	Propety_def(bool, song_play);
	Propety_def(int , fm_freq);
	Propety_def(EmPageID, media_music_fm);

	void init(void){};
private:
	SingleDataStore(){
		m_song_name = std::string("");
		m_song_play = false;
		m_fm_freq = 0;
		m_media_music_fm = id_none;
	};
};


#include <map>
#include "NormalDlg.h"

typedef void* (*Constructor)();

#define  GET_INSTANCE_FACTORY_MAP CObjectFactory::getInstance().mmapPageIdToClassName

class CObjectFactory
{
public:
	~CObjectFactory()
	{
	};
	static void registerClass(std::string className, Constructor constructor )
	{
		constructors()[className] = constructor;
	}

	static void* createObject(const std::string& className)
	{
		Constructor constructor = nullptr;

		if (constructors().find(className) != constructors().end())
			constructor = constructors().find(className)->second;

		if (constructor == nullptr)
			return nullptr;

		return (*constructor)();
	}

	CObjectFactory(const CObjectFactory&) = delete;
	CObjectFactory& operator=(const CObjectFactory&) = delete;
	static CObjectFactory &getInstance()
	{
		static CObjectFactory cob;
		return cob;
	}
	 std::map<EmPageID, std::string> mmapPageIdToClassName;
private:
	CObjectFactory(){};
	inline static std::map<std::string, Constructor>& constructors()
	{
		static std::map<std::string, Constructor> instance;
		return instance;
	}
};

#define REGISTER_CLASS(id,class_name) \
class class_name##Helper { \
public: \
    class_name##Helper() \
    { \
        CObjectFactory::registerClass(#class_name, class_name##Helper::creatObjFunc); \
		if(GET_INSTANCE_FACTORY_MAP.find(id)==GET_INSTANCE_FACTORY_MAP.end()) \
						{ \
			GET_INSTANCE_FACTORY_MAP[id]=std::string(#class_name);\
		}\
		else {printf("this class [%s] is in mmapPageIdToClassName alreadly please check\n",#class_name);}\
    } \
    static void* creatObjFunc() \
    { \
        return new class_name; \
    } \
}; \
class_name##Helper class_name##helper;



