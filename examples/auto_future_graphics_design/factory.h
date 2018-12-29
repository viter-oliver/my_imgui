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
	#if defined(__QNXNTO__)
                   // cout<< "register control:"<<key<<endl;
                   auto& fmap=factory::get().map_;
                   auto itc=fmap.find(key);
                   if(itc==fmap.end())
                    {
                        // cout<<"begin reg:"<<key<<endl;
                        factory::get().map_[key]=[]{return new T();};
                        //cout<<"end reg:"<<key<<endl;
                    }
                   else{
                        // cout<< " fail to register control:"<<key<<endl;
                   }
        #else
			factory::get().map_.emplace(key, [] { return new T(); });
        #endif
		}
        #if 0
		template<typename... Args>
		register_t(const std::string& key, Args... args)
		{
			factory::get().map_.emplace(key, [&] { return new T(args...); });
		}
        #endif
	};

	inline base_ui_component* produce(const std::string& key)
	{
		if (map_.find(key) == map_.end())
			throw std::invalid_argument("the message key is not exist!");

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
	void iterate_types(function<void(string,function<base_ui_component*()>)> imp)
	{
		for (auto it:map_)
		{
			imp((it.first),it.second);
		}
	}

private:
	factory() {};
	factory(const factory&) = delete;
	factory(factory&&) = delete;


	std::map<std::string, std::function<base_ui_component*()>> map_;
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