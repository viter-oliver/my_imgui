
#include <stdio.h>
#include<string>
#include <stdexcept>

#include "lua.hpp"
lua_State* L;
struct st{
	int x;
	int y;
};
char* lua_str_fun = 
R"(function calcul(s,v0,v1)
  s.x = s.x + v0
  s.y = s.y + v1
  return s  
end)";
char* lua_str_fun0 =
R"(function calcul(s0,s1)
	s0.x=s0.x+s1.x
	s0.y=s0.y+s1.y
return s0
end)";
//����ص�
int LuaErrorCallBack(lua_State *L)
{
	lua_Debug debug = {};
	//���������ĵĵ��ò㼶
	int rank = 0;
	//����ȡlua�׳��Ĵ���ֱ����ȡ��
	while (lua_getstack(L, rank, &debug)) {
		rank++;
	}
	//�ж�ջ���Ƿ�Ϊ�ַ�������
	int type = lua_type(L, -1);
	if (type != 4)
		return 0;
	std::string error = lua_tostring(L, -1);
	std::string errorShort = debug.short_src;
	int errorLine = debug.currentline;
	std::string errorMsg = "error:" + error + ",errorShort:" + errorShort
		+ ",line:" + std::to_string(errorLine);
	//��������Ϣѹ��ջ
	lua_pushstring(L, errorMsg.c_str());
	return 1;
}
void stackDump(lua_State* L){
	int top = lua_gettop(L);
	for (int i = 1; i <= top;i++)
	{
		int t = lua_type(L,i);
		switch (t){
		case LUA_TSTRING:
			printf("'%s'\n", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf(lua_toboolean(L, i) ? "true\n" : "false\n");
			break;
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(L, i));
			break;
		default:
			printf("%s\n", lua_typename(L, t));
			break;
		}
		printf(" ");
	}
	printf("\n");
}
int main(int argc, char* argv[])
{
	//printf("hello world");
	L = luaL_newstate();
	luaL_openlibs(L);
	st s = {1,2};
#if 1
	auto call_rt=luaL_dostring(L, lua_str_fun);
	if (call_rt)
	{
		printf("error:%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		lua_close(L);
		return 0;
	}
	int basic_type=lua_getglobal(L, "calcul");
	int v0 = 1, v1 = 3;
	//lua_pushcfunction(L, LuaErrorCallBack);
	int callback = lua_gettop(L);
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushinteger(L, s.x);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushinteger(L, s.y);
	lua_settable(L, -3);
	lua_pushinteger(L, v0);
	lua_pushinteger(L, v1);
	//lua_call(L, 3, 1);
	int ret=lua_pcall(L,3,1,0);
	if (ret)
	{
		printf("error:%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		//lua_close(L);
		return 0;
	}
	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	s.x = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	s.y = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_settop(L, 0);
#endif
	auto call_rt2 = luaL_dostring(L, lua_str_fun0);
	if (call_rt2)
	{
		lua_pop(L, 1);
		lua_close(L);
		return 0;
	}
	int basic_type2 = lua_getglobal(L, "calcul");
	int callback3 = lua_gettop(L);
	st s1 = { 3, 4 };
	lua_newtable(L);
	lua_pushstring(L,"x");
	lua_pushinteger(L,s.x);
	lua_settable(L,-3);
	lua_pushstring(L,"y");
	lua_pushinteger(L,s.y);
	lua_settable(L,-3);
	lua_newtable(L);
	lua_pushstring(L,"x");
	lua_pushinteger(L,s1.x);
	lua_settable(L,-3);
	lua_pushstring(L,"y");
	lua_pushinteger(L,s1.y);
	lua_settable(L,-3);
	stackDump(L);
	int ret2=lua_pcall(L,2,1,0);
	if (ret2)
	{
		printf("error2:%s\n",lua_tostring(L,-1));
		lua_pop(L,1);
		lua_close(L);
		return 0;
	}
	lua_pushstring(L,"x");
	lua_gettable(L,-2);
	int rx=lua_tointeger(L,-1);
	lua_pop(L,1);
	lua_pushstring(L,"y");
	lua_gettable(L,-2);
	int ry=lua_tointeger(L,-1);
	lua_pop(L,1);	
	lua_close(L);
	return 0;
}