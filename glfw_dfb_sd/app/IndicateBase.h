#pragma once

#include "WarringLightId.h"
#include <string>
#include <stdio.h>
#include <map>
#include "msg_host_n.h"

using namespace auto_future_utilities;

extern void register_tellte(void);

#define  REG_TT(id,lsts,str,psts)   \
		SingIndicate::getInstance().map_indicate[id]=new DataIndicate(id,lsts,str,psts);\
		SingIndicate::getInstance().map_sts[id]=lsts;  \
		g_msg_host.attach_monitor(str, [&](u8*pbuff, int len){   \
		static int tmp = 0;  \
	if (pbuff == NULL || tmp == *pbuff)  \
		return;\
	if (*pbuff <= blink_2hz) \
	{  \
		tmp = *pbuff; \
		deal_show_indicate(id, TT_light_sts(*pbuff)); \
	}  \
});   \
   

class SingIndicate
{
public:
	~SingIndicate();
	SingIndicate(const SingIndicate&) = delete;

	struct DataIndicate
	{
		TTBaseID id;
		TT_light_sts sts;
		TT_power_sts pow_sts;
		std::string name;
		DataIndicate(TTBaseID _id, TT_light_sts _sts, const char * str, TT_power_sts psts = TT_POWER_ING_ON)
		{
			id = _id;
			sts = _sts;
			pow_sts = psts;
			name = std::string("show_TT_")+std::string(str);
		}
	};

	static SingIndicate & getInstance()
	{
		static SingIndicate instance;
		return instance;
	};

	void indicate_init(void);
	void reg_telltle_light(void);

	void call_back_ms(void);

	void deal_with_1hz(bool visable);

	void deal_with_2hz(bool visable);

	void deal_with_hz_sts(TT_light_sts sts,bool visable);

	void need_change(TT_light_sts sts);

	bool is_self_check_id(TTBaseID id);

	void update_indicate(TTBaseID id, TT_light_sts sts);

	void register_light_timer(void);

	void self_check(void);

	void deal_indicate(TTBaseID id, TT_light_sts sts);

	void deal_show_indicate(TTBaseID id, TT_light_sts sts);

	void pow_sts_change(TT_power_sts sts);
#ifdef WIN32
	void test_indicate(void);
#endif

public:
	std::map<TTBaseID, DataIndicate *>map_indicate;
	std::map<TTBaseID, TT_light_sts> map_sts;

	bool is_self_check{ true };
	int m_cout_1_hz{ 0 };
	int m_cout_2_hz{ 0 };

private:
	SingIndicate(){};
};


