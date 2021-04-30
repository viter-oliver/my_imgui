#pragma once
#include <stdexcept>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include "control_common_def.h"
#include <iostream>
using namespace auto_future;
struct factory
{
	template<typename T>
	struct register_t
	{
		register_t(const std::string& key)
		{

                   // cout<< "register control:"<<key<<endl;
                   auto& fmap=factory::get().map_;
                   auto itc=fmap.find(key);
                   if(itc==fmap.end())
                    {
                       // printf("begin reg:%s\n",key.c_str());
                        factory::get().map_[key]=[]{return new T();};
                        //cout<<"end reg:"<<key<<endl;
                    }
                   else{
                         //printf(" fail to register control:%s\n",key.c_str());
                   }
       
		}
        #if 1
		template<typename... Args>
		register_t(const std::string& key, Args... args)
		{
			
             auto& fmap=factory::get().map_;
                   auto itc=fmap.find(key);
                   if(itc==fmap.end())
                    {
                        printf("XXbegin reg:%s\n",key.c_str());
                        factory::get().map_.emplace(key, [&] { return new T(args...); });
                        //cout<<"end reg:"<<key<<endl;
                    }
                   else{
                         printf(" XXfail to register control:%s\n",key.c_str());
                   }
		}
        #endif
	};

	inline base_ui_component* produce(const std::string& key)
	{
		if (map_.find(key) == map_.end())
		 {
		        std::string exp_arg=key+" is not exist!";
                    printf("exp:%s\n",exp_arg.c_str());
                    for(auto& icl:map_)
                    {
                       printf("key:%s\n",icl.first.c_str());
                    }
			//throw std::invalid_argument(exp_arg);
                     return map_[std::string("ft_base")]();
		 }
		return map_[key]();
	}
    #if 0
	template<typename... Args>
	base_ui_component* produce(const std::string& key, Args&&... args)
	{
		if (map_.find(key) == map_.end())
			throw std::invalid_argument("the message key is not exist!");

		return map_[key](std::forward<Args>(args)...);
	}
    #endif
	std::unique_ptr<base_ui_component> produce_unique(const std::string& key)
	{
		return std::unique_ptr<base_ui_component>(produce(key));
	}

	std::shared_ptr<base_ui_component> produce_shared(const std::string& key)
	{
		return std::shared_ptr<base_ui_component>(produce(key));
	}
	void register_res_constrained(std::string cname, function<bool()>res_constrained)
	{
		auto& ac_map = factory::get().map_ac_;
		auto itc = ac_map.find(cname);
		if (itc!=ac_map.end())
		{
			printf("%s has existed!\n", cname.c_str());
		}
		else
		{
			ac_map[cname] = res_constrained;
		}
	}
	void iterate_types(function<void(string,function<base_ui_component*()>)> imp)
	{
		for (auto it:map_)
		{
			bool be_valid = true;
			auto iac = map_ac_.find(it.first);
			if (iac!=map_ac_.end())
			{
				be_valid = iac->second();
			}
			imp((it.first),be_valid?it.second:nullptr);
		}
	}

private:
	factory() {};
	factory(const factory&) = delete;
	factory(factory&&) = delete;


	std::map<std::string, std::function<base_ui_component*()>> map_;
	std::map<std::string, std::function<bool()>> map_ac_;
public:
	inline static factory& get()
	{
		static factory instance;
		return instance;
	}
};
//std::map<std::string, std::function<base_ui_component*()>> factory::map_;

#define REGISTER_CONTROL_VNAME(T) reg_control_##T##_
#define REGISTER_CONTROL_BYKEY(T, key, ...) static factory::register_t<T> REGISTER_CONTROL_VNAME(T)(key, ##__VA_ARGS__);

#define REGISTER_CONTROL(T,...)  REGISTER_CONTROL_BYKEY(T,#T,##__VA_ARGS__)