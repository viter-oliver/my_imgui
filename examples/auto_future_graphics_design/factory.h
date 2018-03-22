#pragma once
#include <map>
#include <string>
#include <functional>
#include <memory>
#include "control_common_def.h"
struct factory
{
	template<typename T>
	struct register_t
	{
		register_t(const std::string& key)
		{
			factory::get().map_.emplace(key, [] { return new T(); });
		}

		template<typename... Args>
		register_t(const std::string& key, Args... args)
		{
			factory::get().map_.emplace(key, [&] { return new T(args...); });
		}
	};

	inline base_ui_component* produce(const std::string& key)
	{
		if (map_.find(key) == map_.end())
			throw std::invalid_argument("the message key is not exist!");

		return map_[key]();
	}

	std::unique_ptr<base_ui_component> produce_unique(const std::string& key)
	{
		return std::unique_ptr<base_ui_component>(produce(key));
	}

	std::shared_ptr<base_ui_component> produce_shared(const std::string& key)
	{
		return std::shared_ptr<base_ui_component>(produce(key));
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