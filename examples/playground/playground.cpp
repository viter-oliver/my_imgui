// playground.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fab/fab.h"
#include <vector>
#include <map>
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
	~MyClass(){}
	virtual void func_test(){
		printf("mycl\n");
	}

private:

};
class MyClass2:public MyClass
{
	int ddd;
public:
	MyClass2(int g){
		ddd = g;
	}
	MyClass2(MyClass2&){
		ddd = 0;
	}
	void func_test(){
		printf("mycl2\n");
	}
};

#include <typeinfo>

#define eprintf(format, ...)   fprintf (stderr, format , __VA_ARGS__)
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
	printf("name%s\n",typeid(*pm).name());
	
	eprintf("");
	//mms->func_test();
	delete pm;
	using namespace fab;
	MyClass2 mmg(1);
	Factory<MyClass> ft;
	ft.Register("MyClass2", [](int ss){return new MyClass2(ss); });
	ft.Register("MyClass3", [](MyClass2& ss){return new MyClass2(ss); });

	auto mc = ft.Create("MyClass3",mmg);
	mc->func_test();
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
		void fun()
		{
			ssy = 1;
		}
	};
	return 0;

}

