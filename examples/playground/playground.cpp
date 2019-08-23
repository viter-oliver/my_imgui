// playground.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "aes.h"
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
#include <fstream>
#include <stddef.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <chrono>
#include <sstream>
extern BOOL GetMacByCmd(char *lpszMac);
using namespace std;
struct stt
{
	int x, y, z;
	stt(){}
	stt(int a, int b, int c) :x(a), y(b), z(c){}
};
int sss[2][4][4] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 
11, 12, 13, 14, 15, 16, 101, 102, 103, 104, 105, 106, 107, 108, 109,
1010, 1011, 1012, 1013, 1014, 1015, 1016 };
class MyClass
{
	BYTE et = 1;
	BYTE test_byts[2];
public:
	MyClass(){}
	MyClass(int){}
	MyClass(int,int){}

	virtual ~MyClass(){}
	virtual void func_test(){
		printf("mycl\n");
	}

};
//BYTE MyClass::test_byts[2] = { 0, 1 };
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

class dlg1
{
	int _slider1_value, _slider2_value;
   void button1_open(){}
	void button2_open(){}
	void slider1_scroll(int){}
	void slider2_scroll(int){}
};
class dlg2
{
	int  _slider3_value, _slider4_value;
	void button3_open(){}
	void button4_open(){}

	void slider3_scroll(int){}
	void slider4_scroll(int){}
};
class dashboard_controller
{
	dlg1 _dlg1;
	dlg2 _dlg2;
};
struct test_pods 
{
	int x1, x2;
};
void test_input_pod(test_pods&& tpd)
{
	tpd.x1 = 1;
}

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

	void test_vect(vector<int> iv)
	{
		for (auto& iu:iv)
		{
			cout << iu << endl;
		}
	}
#include <random>
	int roll_a_fair_die()
	{
		static std::default_random_engine e{};
		static std::uniform_int_distribution<int> d{ 1, 6 };
		return d(e);
	}	struct stp0
	{
		int j;
		virtual ~stp0()
		{
			j = 0;
		}
	};
	struct stp1 :public stp0
	{
		~stp1()
		{
			j = 2;
		}
	};
	struct test_cons 
	{
		int _ivalue;
	};

	struct test_cons_host
	{
		test_cons _tcons;
		struct iner_str 
		{
			int _iivalue;
			iner_str(test_cons& tcons)
			{
				_iivalue = 10;
				tcons._ivalue = _iivalue;
			}
		};
		iner_str _instr { _tcons };
		test_cons_host() {
			iner_str ssss(_tcons);
		}

	};
	int test_ct_value = 10;
	const int test_const()
	{
		test_ct_value = 5;
		return 1;
	}
	static const int test_cnt_triger = test_const();

	const char hex_enc_table[] = "abcdefghijklmnop";
	void convert_binary_to_string(char* pbin, int len, string& out_str)
	{
		out_str.resize(len * 2);
		int ix = 0;
		for (; ix < len; ix++)
		{
			auto bin_value = *(unsigned char*)pbin;
			out_str[ix * 2] = hex_enc_table[bin_value >> 4];
			out_str[ix * 2 + 1] = hex_enc_table[bin_value & 0x0f];
			pbin++;
		}
	}

	void convert_string_to_binary(string& in_str, string& out_bin)
	{
		auto out_size = in_str.size() / 21;
		out_bin.resize(out_size);
		int ix = 0;
		for (; ix < out_size; ix++)
		{
			unsigned char high = in_str[ix * 2] - 'a';
			unsigned char low = in_str[ix * 2 + 1] - 'a';
			out_bin[ix] = high << 4 | low;
		}
	}
#include <codecvt>
	std::wstring utf8ToWstring(const std::string& str)
	{
		std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
		return strCnv.from_bytes(str);
	}

	void print(unsigned char* state, int len)
	{
		int i;
		for (i = 0; i < len; i++)
		{
			if (i % 16 == 0) printf("\n");
			//		printf("%s%X ",state[i]>15 ? "" : "0", state[i]);
			printf("%d  ", (int)(state[i] & 0xff));
		}
		printf("\n");
	}
int _tmain(int argc, _TCHAR* argv[])
{
	string emptystr;
	wstring empth = utf8ToWstring(emptystr);
	char test_bin[] = { 0xab,0x01, 0x02, 0x0a, 0x1a, 0x1b };
	string test_bin_out;
	convert_binary_to_string(test_bin, sizeof(test_bin), test_bin_out);
	test_pods ffff;
	test_input_pod({ 1, 2 });
	test_input_pod(std::move(ffff));
	vector<stt> vstt;
	stt st0{ 1, 2, 3 };
	stt st1{ 4, 5, 6 };
	stt st2{ 1, 2, 3 };
	vstt.push_back(st0);
	vstt.push_back(st1);
	vstt.push_back(st2);
	cout << vstt.size() << endl;
	test_vect({ 0, 2, 3, 4 });
	vector<stt>* pvstt =&vstt; 
	cout << pvstt->size() << endl;
	cout << (*pvstt)[0].x << endl;
	cout << (*pvstt)[0].y << endl;
	cout << (*pvstt)[0].z << endl;

	pvstt = new(&vstt)vector<stt>(3);
	cout << pvstt->size() << endl;
	cout << (*pvstt)[0].x << endl;
	cout << (*pvstt)[0].y << endl;
	cout << (*pvstt)[0].z << endl;

	stt* pst0 = new(&st0) stt();
	cout << pst0->x << endl;
	cout << pst0->y << endl;
	cout << pst0->z << endl;

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
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(1.f, 1.f, 1.f));
	model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));

	glm::mat4 proj = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	model = proj*model;

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
	struct test_cvt
	{
		vector<int > vint;
	}; 
	test_cvt tcvt;
	cout << sizeof(tcvt) << endl;
	tcvt.vint.push_back(1);
	tcvt.vint.push_back(2);
	cout << sizeof(tcvt) << endl;
	cout << " rand0:";
	for (int ii = 0; ii < 100; ii++)
	{
		cout << rand()<<" ";
		if (ii&& ii % 20 == 0)
		{
			cout << endl;
		}
	}
	cout << endl;
	for (auto i = 1; i <= 100;i++)
	{
		if (i%2==0||i%3==0&&(printf("i/3=%d\n",i/3),true))
		{
			printf("i=%d\n", i);
		}
	}
	random_device rd;
	mt19937 mt(rd());
	cout << " rand1:";
	for (int ii = 0; ii < 100; ii++)
	{
		cout << mt()<<" ";
		if (ii&&ii % 20 == 0)
		{
			cout << endl;
		}
	}
	cout << endl;
	cout << " rand2:";
	for (int ii = 0; ii < 100; ii++)
	{
		cout << roll_a_fair_die() << " ";
		if (ii&&ii % 20 == 0)
		{
			cout << endl;
		}
	}	
	const int nrolls = 10000;  // number of experiments
	const int nstars = 95;     // maximum number of stars to distribute
	const int nintervals = 10; // number of intervals

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	std::uniform_real_distribution<double> distribution2(-25.0, 25.0);

	int p1[nintervals] = {};

	for (int i = 0; i < nrolls; ++i) {
		double number = distribution(generator);
		++p1[int(nintervals*number)];
	}

	std::cout << "uniform_real_distribution (0.0,1.0):" << std::endl;
	std::cout << std::fixed; std::cout.precision(1);

	for (int i = 0; i < nintervals; ++i) {
		std::cout << float(i) / nintervals << "-" << float(i + 1) / nintervals << ": ";
		std::cout << std::string(p1[i] * nstars / nrolls, '*') << std::endl;
	}
	std::cout << "uniform_real_distribution (0-25.0, 25.0):" << std::endl;
	for (int ii = 0; ii <100;ii++)
	{
		std::cout << distribution2(generator) << endl;
	}
	vector<shared_ptr<stp0>> vstp;
	vstp.emplace_back(make_shared<stp1>());
	vstp.erase(vstp.begin()+0);
	std::vector<int> vtest_swap;
	for (auto i = 0; i < 100;i++)
	{
		vtest_swap.emplace_back(i);
	}
	auto ifd = find_if(vtest_swap.begin(), vtest_swap.end(), [](int& ivalue){return ivalue == 50; });
	cout << "ifd:" << *ifd << endl;
	swap(*ifd, *(ifd - 1));
	for (auto& iu:vtest_swap)
	{
		cout << "v:" << iu << endl;
	}
	fstream fstr("ab.txt", ios::in | ios::out | ios::app);
	fstr.seekg(0, fstr.end);
	if (fstr.is_open())
	{
		for (auto& iu : vtest_swap)
		{
			fstr << "v:" << iu << endl;
		}
	}
	union test_endian
	{
		unsigned char buff[4];
		int rvalue;
	};
	test_endian adtest;
	adtest.rvalue = 0x12345678;
	for (int ii = 0; ii < 4;++ii)
	{
		printf("buff%d:0x%x\n", ii, adtest.buff[ii]);
	}
	adtest.rvalue = 0x123456;
	for (int ii = 0; ii < 4; ++ii)
	{
		printf("buff%d:0x%x\n", ii, adtest.buff[ii]);
	}
	adtest.rvalue = 0x1234;
	for (int ii = 0; ii < 4; ++ii)
	{
		printf("buff%d:0x%x\n", ii, adtest.buff[ii]);
	}
	struct test_this 
	{
		virtual void printf_this()
		{
			printf("test_this this=%x\n", this);
		}
	};
	struct test_this1:public test_this
	{
		void printf_this()
		{
			test_this::printf_this();

			printf("test_this1 this=%x\n", this);
		}

	};
	struct test_this2 :public test_this1
	{
		void printf_this()
		{
			test_this1::printf_this();

			printf("test_this2 this=%x\n", this);
		}

	};
	test_this2 ts2;
	ts2.printf_this();

	int iij{ 0 };
	cout << __cplusplus << endl;
#define SEQ (int aw=0)(int ax=1)(int ay=2)(int az=3)

#define MACRO(r, data, elem)  elem;

	BOOST_PP_SEQ_FOR_EACH(MACRO, _, SEQ) // expands to w_ x_ y_ z_
		cout << "aw:" << aw << endl;
	cout << "ax:" << ax << endl;
	cout << "ay:" << ay << endl;
	cout << "az:" << az << endl;

#define _STR(x) #x
#define STR(x) _STR(x)
#define GET_ELE(r,data,ele) STR(BOOST_PP_TUPLE_ELEM(2,0,ele)),STR(BOOST_PP_TUPLE_ELEM(2,1,ele)),
#define GEN_V(r,data,elem) BOOST_PP_TUPLE_ELEM(2,0,elem) BOOST_PP_TUPLE_ELEM(2,1,elem);
#define DEF_STRUCT(stname,vname,...) struct stname {BOOST_PP_SEQ_FOR_EACH(GEN_V,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	stname(){/*char* ele_name_list[]={BOOST_PP_SEQ_FOR_EACH(GET_ELE,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))};*/}\
	};\
	stname vname;
	/*char* ele_name_list[]={BOOST_PP_SEQ_FOR_EACH(GET_ELE,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))*/ 
	DEF_STRUCT(test_pp, my_testpp, (int, _i), (float, _f), (double, _d))
		my_testpp._d = 0;
#define ELELIST (int,hh) (float,ff) (double,dd)
	/*char* ssddd[] = { BOOST_PP_SEQ_FOR_EACH(GET_ELE,_,ELELIST) };
		cout << "value?:" << ssddd[0] << endl;
		cout << "value?:" << ssddd[1] << endl;
		cout << "value?:" << ssddd[2] << endl;
		cout << "value?:" << ssddd[3] << endl;
		cout << "value?:" << ssddd[4] << endl;
		cout << "value?:" << ssddd[5] << endl;
		*/
#define GET_SEQ(stname,...) char* stname= STR(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__));
	GET_SEQ(astr, int, (int i), (float))
	cout << astr << endl;
#define ID_OP(_, func, elem) func(elem)
#define APPLY_TO_ALL(func, ...)                \
    BOOST_PP_SEQ_FOR_EACH(                     \
        ID_OP, func,                           \
        BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)  \
    )

	// example call:

#define SomeTransformation(x) #x // stringize the argument

	char* nstr=APPLY_TO_ALL(SomeTransformation, 1, 2, 3); // expands to "1" "2" "3"
	cout << nstr << endl;
	struct test_iner_class
	{
		struct  iner_class
		{
			int a;
			int b;
			int c;
			iner_class()
			{
				a = 1;
				b = 2;
				c = 3;
				printf("this=%x\n", this);
			}
			iner_class(int i1, int i2, int i3) :a(i1), b(i2), c(i3){}
		};
		iner_class my_ineral;
		iner_class my_ineral2{ 1, 2, 3 };
		test_iner_class()
		{
			void* piner = &my_ineral;
			void* pp;
			pp = piner;
			printf("piner=%x\n", piner);
			int el1 = *((int*)piner);
			printf("el1=%d\n", el1);
			//printf("intsize=%d", sizeof(int));
			int* pel2 = (int*)((char*)piner + sizeof(int));
			int el2 = *(pel2);
			printf("el2=%d\n", el2);
			int* pel3 = (int*)((char*)pel2 + sizeof(int));
			int el3 = *(pel3);
			printf("el3=%d\n", el3);


		}
	};
	test_iner_class tic;
	char str_test[200];
	int jjj = sizeof(str_test);
	cout << "jjj=" << jjj << endl;
	char dddd[] = "adadads";		
	char a[20] = { 1, 2, 3 };

#pragma pack(1)  
	struct s_test_offset{
		int i[2];//8
		char c[2];//10
		double d;//10
		int e;//18
		char a[2];//24
		char tt_2_1[2];//26
		int ia[2];//34

		s_test_offset()
			:ia{ }
		{
		}
	};
	printf("size double=%d\n", sizeof(double));
	printf("size float=%d\n", sizeof(float));

	printf("offsets: i=%d; c=%d; d=%d;e=%d; a=%d;tt_2_1=%d;ia=%d\n",
		offsetof(s_test_offset, i[2]),
		offsetof(s_test_offset, c[2]),
		offsetof(s_test_offset, d),
		offsetof(s_test_offset, e),
		offsetof(s_test_offset, a[2]),
		offsetof(s_test_offset, tt_2_1[2]),
		offsetof(s_test_offset, ia[2])
		);
	int d_offset = offsetof(s_test_offset, d);
	s_test_offset _s_test_offset;
	_s_test_offset.i[0] = 1;
	_s_test_offset.c[0] = 'a';
	_s_test_offset.d = 10.0;
	_s_test_offset.a[0] = 'a';
	_s_test_offset.a[1] = 'b';
	_s_test_offset.a[2] = 'c';
	char* d_address = (char*)&_s_test_offset + d_offset;
	*(double*)d_address = 20.0;
	printf("sizeof(struct s)=%d\n", sizeof(struct s_test_offset));
	string test_a_str("12345678");
	auto apos = test_a_str.find('a');
	cout << "apos=" << apos << endl;
	char str_a[20];
	cout << "size str_a=" << sizeof(str_a) << endl;
	cout << "size str_a=" << sizeof(str_a[20]) << endl;
	cout << "size char=" << sizeof(char) << endl;
	cout << "size bool=" << sizeof(bool) << endl;

	auto pos6 = test_a_str.find('6');
	string test_sbstr = test_a_str.substr(0, pos6);
	cout << "test_sbstr:" << test_sbstr << endl;
	int aaa;
	cout << "test_ct_value=" << test_ct_value << endl;
	unsigned char tst_char[] = { 0x7, 0x80, 0x2, 0xd0, };
	unsigned int iwth =(tst_char[0]*0x100) + tst_char[1];
	unsigned int ihth = (tst_char[2]*0x100)+ tst_char[3];
	cout << "iwth:" << iwth << endl;
	cout << "ihth:" << ihth << endl;
	printf("iwth=0x%x\n", iwth);
	printf("ihth=0x%x\n", ihth);
	unsigned int* piwh = &iwth;
	unsigned short test_short = 0;
	printf("test_short=%d\n", test_short);
	wstring test_omit;
	test_omit= L"¡­";
	test_omit;
	short testid = -1;
	printf("testid=%d\n", testid);
	short* ptestid = &testid;
	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	struct test_bit
	{
		u8 hi_bit : 1;
		u8 hi_bit2 : 2;
		u8 hi_bit3 : 3;
		u8 hi_bit4 : 2;
	}tbit = {1,2,3,0};
	void* ptbit = &tbit;
	int test_multi_array[3][5] = {{ 0,1, 2,3,4}, { 5, 6, 7,8,9}, { 10,11,12,13,14 }};
	char sp_test[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8' };
	int ii = 255;
	struct buff_data
	{
		u8 data_h : 4;
		u8 data_l : 4;
	};
	u8 bdd = 0xc2;
	buff_data* pbuff = (buff_data*)&bdd;
	struct buff_data_ex
	{
		u8 data_h ;
		u8 data_l;
		u8 data_h1;
		u8 data_l2;

	};
	buff_data_ex bddd = { 0, 1, 0x1d, 0xce };
	u32* pbdd = (u32*)&bddd;
	printf("ddd=0x%x\n", *pbdd);
	printf("sizeof u32 is %d",sizeof *pbdd);
	/*const int buff_len = 256;
	char cbuff[buff_len] = { 0 };
	while (true)
	{
	int len = read(stdin, cbuff, buff_len);
	}*/
	sprintf_s(sp_test,10, "%d", ii);
	/*const char* str_st = "road.fbx 0x701";
	char str_buff[50] = { 0 };
	unsigned short test_sh;
	sscanf(str_st, "%s %x",str_buff, &test_sh);
	*/
	unsigned short test_shd = 0x701;
	unsigned char test_over = 0xff;
	test_over++;
	unsigned char* pshd = (unsigned char*)&test_shd;
	unsigned char shd1 = *pshd++;
	unsigned char shd2 = *pshd;
	string ikey("max&maj20190815x");
	string crp_text = "2019081520190815";
	unsigned char input[] = { 84, 103, 107, 115, 32, 105, 115, 32, 97, 32, 116, 101, 115, 116, 33, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 116, 101, 115, 116, 33, 84, 104 // , 105, 115, 32, 105, 115, 32, 97, 46, 122
};
	BYTE testinputs[] = { 0xe1, 0x04, 0xe5, 0x5b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0xa3, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  };
	unsigned char iv[] = { 103, 35, 148, 239, 76, 213, 47, 118, 255, 222, 123, 176, 106, 134, 98, 92 };
	unsigned char key[] = { 143, 194, 34, 208, 145, 203, 230, 143, 177, 246, 97, 206, 145, 92, 255, 84 };
	unsigned char output[100] = { 0 };
	unsigned char temp[100] = { 0 };
	BYTE plainText[100] = { 0 };
	//	unsigned char 
	AESModeOfOperation moo;
	//moo.set_key((unsigned char*)ikey.c_str());
	moo.set_key((BYTE*)ikey.c_str());
	moo.set_mode(MODE_CBC);
	moo.set_iv(iv);
	int olen = sizeof input;

	memcpy(temp, input, sizeof input);
	int len = moo.Encrypt(temp, olen, output);
	printf("len = %d\n", len);
	printf("output");
	print(output, len);
	printf("\n\nDecrypt----------\n");
	len = moo.Decrypt(output, len, plainText);
	printf("len = %d\n", len);
	printf("input");
	print(plainText, len);
	unsigned char ixxx = 0x2 << 4 | 0xa;
	char lpszMac[128];
	memset(lpszMac, 0x00, sizeof(lpszMac));

	//»ñÈ¡MAC
	/**/GetMacByCmd(lpszMac);
	printf("mac:%s\n", lpszMac);
	return 0;
}

