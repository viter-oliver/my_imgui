// playground.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fab/fab.h"
#include <stdio.h>
#include <iostream>  
#include <vector>
#include <map>
#include <array>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace std;
struct stt
{
	int x, y, z;
};
int sss[2][4][4] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 
11, 12, 13, 14, 15, 16, 101, 102, 103, 104, 105, 106, 107, 108, 109,
1010, 1011, 1012, 1013, 1014, 1015, 1016 };
class MyClass
{
public:
	MyClass(){}
	MyClass(int){}
	MyClass(int,int){}

	virtual ~MyClass(){}
	virtual void func_test(){
		printf("mycl\n");
	}

private:

};
using namespace fab;
Factory<MyClass> ft;
template<typename T>
struct shader_assist
{
	shader_assist(string key){ ft.Register(key, []{return new T(); }); }
};
#define REG_SHADER(shd_type) shader_assist<shd_type> reg_##shd_type(#shd_type)


template<typename T>
struct shader_assist_int
{
	shader_assist_int(string key){ ft.Register(key, [](int ix,int iy){return new T(ix,iy); }); }
};
#define REG_SHADER_INT(shd_type) shader_assist_int<shd_type> reg_##shd_type(#shd_type)

class MyClass2 :public MyClass
{

	int ddd;
public:
	MyClass2(int g)
	{
		char nnn[20] = { 0 };
		ddd = g;
	}
	MyClass2(int g, char j)
	{
		char nnn[20] = { 0 };
		ddd = g;
	}
	MyClass2(MyClass2&){
		ddd = 0;
	}
	void func_test(){
      int m[3]  { 0, 1, 2 };
		printf("mycl2\n");
	}
	
	 
	//static constexpr int sss[10] = { 1, 2, 3 };
	//const char *tmp[3]{"dd", "bnbnb", "fhjgfjhgfhj"};
};
//REG_SHADER_INT(MyClass2);
class MyClass3 :public MyClass
{
	void func_test(){
		printf("MyClass3\n");
	}
};
REG_SHADER(MyClass3);
class testc
{
public:
	testc(){}
	virtual void func_test(){
		printf("mycl\n");
	}
	//string get_cname()
	//{
	//	return typeid(*this).name();
	//}
};
class t1estc0 :public testc
{
	int ggg;
public:
	t1estc0(int i){ ggg = i; }
	t1estc0(){
		printf("0 ");
	}
	t1estc0(t1estc0&){ printf("1 "); }
	
};
struct testalias
{
	int jj;
	int& ij = jj;
};



//class testc1 :public testc0
//{
//public:
//	testc1(long j){}
//	testc1(testc1&){}
//};
/*#include <windows.h>
#include <tchar.h>  
#include <locale.h>*/  
#include <ShlObj.h>
#include <typeinfo>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <memory>
#define eprintf(format, ...)   fprintf (stderr, format , __VA_ARGS__)


	MyClass2* create_myclass2(int i)
	{
		return new MyClass2(i); 
	}

int _tmain(int argc, _TCHAR* argv[])
{
	vector<stt> vstt;
	stt st0{ 1, 2, 3 };
	stt st1{ 4, 5, 6 };
	stt st2{ 1, 2, 3 };
	vstt.push_back(st0);
	vstt.push_back(st1);
	vstt.push_back(st2);
	//int (*s)[4]= sss;
	int *p = (int*)sss;
	for (int ix = 0; ix < 32;ix++)
	{
		printf("%d\n", *p++);
	}
	MyClass* pm = new MyClass2(1);
	printf("name:::%s\n",typeid(*pm).name());
	
	testc* pcn = new t1estc0(1);
	//testc* pcn2 = new testc1(1L);
	printf("name:%s\n", typeid(*pcn).name());
	//printf("name:%s\n", typeid(*pcn2).name());

	eprintf("");
	//mms->func_test();
	delete pm;

	ft.Register("MyClass2", create_myclass2);
	auto atest = ft.Create("MyClass2", 1);
	atest->func_test();
	//ft.Register("MyClass2", [](int i,char j)->MyClass2*{return new MyClass2(i,j); });
	//auto mc = ft.Create("MyClass2",1,2);
	//mc->func_test();
	int iy = -1;
	int ddd = iy > 0 ? 1 : 2 + 3;
	map<string, stt> mptext;
	mptext["x"].x = 0;
	mptext["x"].y= 0;

	mptext["y"].y = 1;
	mptext["z"].z = 2;
	static int ssy;
	struct sss
	{
		sss(){}

		void fun()
		{
			ssy = 1;
		}
	};

	std::cout << "MyClass2 is_trivially_copyable:" << std::is_trivially_copyable<MyClass2>::value << std::endl;
	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 ret = trans*glm::vec4(1000.0f, 100.0f, 1.0f, 1.0f);
	printf("%f,%f,%f\n", ret.x, ret.y, ret.z);
	std::cout << "glm::vec4 is_trivially_copyable:" << std::is_trivially_copyable<glm::vec4>::value << std::endl;


	testalias mya;
	mya.jj = 1111;
	printf("ij=%d\n", mya.ij);
	int tst_int;
	printf("%s\n",typeid(tst_int).name());
	char* tst_str = "gfsdgd";
	printf("%s\n", typeid(tst_str).name());
	const int& tint = tst_int;
	printf("%s\n", typeid(tint).name());
	float* pf = 0;
	float*& pr = pf;
	printf("%s\n", typeid(pf).name());
	printf("%s\n", typeid(pr).name());

	printf("%s\n", typeid(mya).name());
	printf("trans %s\n", typeid(trans).name());
	printf("ret %s\n", typeid(ret).name());

	//testc cn;
	//printf("name:%s\n", cn.get_cname());
	//testc0 cn0;
	//printf("name:%s\n", cn0.get_cname());
 //   testc1 cn1;
	//printf("name:%s\n", cn1.get_cname());

	std::vector<int> myvector = { 10, 20, 30 };


	auto it = myvector.emplace(myvector.begin() + 1, 100);
	myvector.emplace(it, 200);
	myvector.emplace(myvector.end(), 300);

	std::cout << "myvector contains:";
	for (auto& x : myvector)
		std::cout << ' ' << x;
	std::cout << '\n';

	struct test_cs
	{
		test_cs()
		{
			cout << "new test_cs" << endl; 
		}
		~test_cs()
		{ 
			cout << "delete test_cs" << endl; 
		}
	};
	vector<shared_ptr<test_cs*> > vtestcs;
	vtestcs.emplace_back(make_shared<test_cs*>(new test_cs()));
	//vtestcs.push_back(make_shared<>(test_cs()));
	vector<test_cs> vvtest;
	if (true)
	{
	    vvtest.emplace_back();
	    vvtest.emplace_back();
	    vvtest.emplace_back();

	}
	map<string, int> mp_test ;
	mp_test["positin"] = 10;
	mp_test["color"] = 11;
	mp_test["color2"] = 13;
	mp_test["postion2"] = 14;
	mp_test["texture"] = 15;
	mp_test["texture2"] = 16;
	for (auto& iit:mp_test)
	{
		cout << iit.first << endl;

	}


	return 0;
}

