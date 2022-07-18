#include <cassert>
#include <boost/lexical_cast.hpp>
#include "lua_interpreter.h"
#include "lua.hpp"
#include "af_type.h"

static lua_State* L=nullptr;
using namespace std;
using namespace zl_future;

static class manager_lua
{
public:
	manager_lua(){
		L = luaL_newstate();
		assert(L != nullptr&&"fail to create lua state!");
		luaL_openlibs(L);
	}
	~manager_lua(){
		lua_close(L);
	}
} s_manager_lua;
namespace lua_interpreter{
	const char* lua_func_head = "function calcu_property(";
	const char* lua_func_tail = "\nend";
	bool call_lua_fun(std::string& func_exp, var_unit& fun_retn, variable_list& vlist)
	{
		int ix=0;
		luaL_dostring(L, func_exp.c_str());
		lua_getglobal(L,"calcu_property");
		for (const auto& vr:vlist) {
			if ("int" == vr._type){
				int *pvalue=(int*)vr._value_addr;
				lua_pushinteger(L,*pvalue);
			} else if ("float" == vr._type){
				float* fvalue=(float*)vr._value_addr;
				lua_pushnumber(L,*fvalue);
			} else if("double" == vr._type){
				double* dvalue=(double*)vr._value_addr;
				lua_pushnumber(L,*dvalue);
			} else if("bool" == vr._type){
				bool* bvalue=(bool*)vr._value_addr;
				lua_pushboolean(L,*bvalue);
			} else if("char" == vr._type){
				char* pcvalue=(char*)vr._value_addr;
				lua_pushstring(L,pcvalue);
			} else if("af_vi2" == vr._type){
				af_vi2* pv2=(af_vi2*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushinteger(L,pv2->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushinteger(L,pv2->y);
				lua_settable(L,-3);
			} else if("af_vi3" == vr._type){
				af_vi3* pv3=(af_vi3*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushinteger(L,pv3->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushinteger(L,pv3->y);
				lua_settable(L,-3);
				lua_pushstring(L,"z");
				lua_pushinteger(L,pv3->z);
				lua_settable(L,-3);
			} else if("af_vi4" == vr._type){
				af_vi4* pv4=(af_vi4*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushinteger(L,pv4->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushinteger(L,pv4->y);
				lua_settable(L,-3);
				lua_pushstring(L,"z");
				lua_pushinteger(L,pv4->z);
				lua_settable(L,-3);
				lua_pushstring(L,"w");
				lua_pushinteger(L,pv4->w);
				lua_settable(L,-3);
			} else if("af_vec2" == vr._type){
				af_vec2* pv2=(af_vec2*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushnumber(L,pv2->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushnumber(L,pv2->y);
				lua_settable(L,-3);
			} else if("af_vec3" == vr._type){
				af_vec3* pv3=(af_vec3*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushnumber(L,pv3->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushnumber(L,pv3->y);
				lua_settable(L,-3);
				lua_pushstring(L,"z");
				lua_pushnumber(L,pv3->z);
				lua_settable(L,-3);
			} else if("af_vec4" == vr._type){
				af_vec4* pv4=(af_vec4*)vr._value_addr;
				lua_newtable(L);
				lua_pushstring(L,"x");
				lua_pushnumber(L,pv4->x);
				lua_settable(L,-3);
				lua_pushstring(L,"y");
				lua_pushnumber(L,pv4->y);
				lua_settable(L,-3);
				lua_pushstring(L,"z");
				lua_pushnumber(L,pv4->z);
				lua_settable(L,-3);
				lua_pushstring(L,"w");
				lua_pushnumber(L,pv4->w);
				lua_settable(L,-3);
			}
			ix++;
		}
		
		int rt=lua_pcall(L,ix,1,0);
		if(rt){
			printf("failing to call lua function because %s\n",lua_tostring(L,-1));
			lua_pop(L,1);
			return false;
		}
		auto& vr=fun_retn;
		if ("int" == vr._type && lua_isinteger(L,-1)){
			int *pvalue=(int*)vr._value_addr;
			*pvalue=lua_tointeger(L,-1);
		} else if ("float" == vr._type && lua_isnumber(L,-1)){
			float* fvalue=(float*)vr._value_addr;
			*fvalue=lua_tonumber(L,-1);
		} else if("double" == vr._type && lua_isnumber(L,-1)){
			double* dvalue=(double*)vr._value_addr;
			*dvalue=lua_tonumber(L,-1);
		} else if("bool" == vr._type && lua_isboolean(L,-1)){
			bool* bvalue=(bool*)vr._value_addr;
			*bvalue=lua_toboolean(L,-1);
		} else if("char" == vr._type && lua_isstring(L,-1)){
			char* pcvalue=(char*)vr._value_addr;
			const char* pscrc=lua_tostring(L,-1);
			strcpy(pcvalue,pscrc);
		} else if("af_vi2" == vr._type && lua_istable(L,-1)){
			af_vi2* pv2=(af_vi2*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv2->x=lua_tointeger(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv2->y=lua_tointeger(L,-1);
		} else if("af_vi3" == vr._type && lua_istable(L,-1)){
			af_vi3* pv3=(af_vi3*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv3->x=lua_tointeger(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv3->y=lua_tointeger(L,-1);
			lua_pushstring(L,"z");
			lua_gettable(L,-2);
			pv3->z=lua_tointeger(L,-1);
		} else if("af_vi4" == vr._type && lua_istable(L,-1)){
			af_vi4* pv4=(af_vi4*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv4->x=lua_tointeger(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv4->y=lua_tointeger(L,-1);
			lua_pushstring(L,"z");
			lua_gettable(L,-2);
			pv4->z=lua_tointeger(L,-1);
			lua_pushstring(L,"w");
			lua_gettable(L,-2);
			pv4->w=lua_tointeger(L,-1);
		} else if("af_vec2" == vr._type && lua_istable(L,-1)){
			af_vec2* pv2=(af_vec2*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv2->x=lua_tonumber(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv2->y=lua_tonumber(L,-1);
		} else if("af_vec3" == vr._type && lua_istable(L,-1)){
			af_vec3* pv3=(af_vec3*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv3->x=lua_tonumber(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv3->y=lua_tonumber(L,-1);
			lua_pushstring(L,"z");
			lua_gettable(L,-2);
			pv3->z=lua_tonumber(L,-1);
		} else if("af_vec4" == vr._type && lua_istable(L,-1)){
			af_vec4* pv4=(af_vec4*)vr._value_addr;
			lua_pushstring(L,"x");
			lua_gettable(L,-2);
			pv4->x=lua_tonumber(L,-1);
			lua_pop(L,1);
			lua_pushstring(L,"y");
			lua_gettable(L,-2);
			pv4->y=lua_tonumber(L,-1);
			lua_pushstring(L,"z");
			lua_gettable(L,-2);
			pv4->z=lua_tonumber(L,-1);
			lua_pushstring(L,"w");
			lua_gettable(L,-2);
			pv4->w=lua_tonumber(L,-1);
		}
		lua_settop(L,0);
		return true;
	}
}

