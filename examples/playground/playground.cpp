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
	~MyClass(){}
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

class MyClass2 :public MyClass
{

	int ddd;
public:
	MyClass2(int g)
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
struct Person
{
	int _age;
	std::string _lname;
	std::string _fname;

	Person(int age, std::string&& lname, std::string&& fname) :
		_age(age), _lname(std::move(lname)), _fname(std::move(fname))
	{
		std::cout << "constructor called" << std::endl;
	}

	Person(const Person&& rhs) :
		_age(std::move(rhs._age)), _lname(std::move(rhs._lname)), _fname(std::move(rhs._fname))
	{
		std::cout << "move constructor called" << std::endl;
	}

	//default is an new C++ explicit instruction to the compiler to generate default constructor,
	//copy constructor or assignment operator for a class.

	Person& operator=(const Person& rhs) = default;

};

//compare between std::vector::push_back and std::vector::emplace_back

void TestEmplace()
{
	std::cout << "# PushBack" << std::endl;
	std::vector<Person> v;

	//to use push_back, it needs to pass object, thus, construct and move will be called.

	//v.push_back(Person(1, "Baby", "Caren"));

	for (Person& p : v)
	{
		std::cout << p._age << " : " << p._fname << " : " << p._lname << std::endl;
	}

	std::cout << "\n\n# Emplace" << std::endl;
	std::vector<Person> vEmplace;


	//emplace_back allow you construct on the fly, thus only constructor called. no move.

	vEmplace.emplace_back(18, "HighTeen", "Caren");

	for (Person& p : vEmplace)
	{
		std::cout << p._age << " : " << p._fname << " : " << p._lname << std::endl;
	}
}
template<typename T>
class ImVector
{
public:
	int                         Size;
	int                         Capacity;
	T*                          Data;

	typedef T                   value_type;
	typedef value_type*         iterator;
	typedef const value_type*   const_iterator;

	inline ImVector()           { Size = Capacity = 0; Data = NULL; }
	inline ~ImVector()          { if (Data) ImGui::MemFree(Data); }
	inline ImVector(const ImVector<T>& src)                     { Size = Capacity = 0; Data = NULL; operator=(src); }
	inline ImVector& operator=(const ImVector<T>& src)          { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(value_type)); return *this; }

	inline bool                 empty() const                   { return Size == 0; }
	inline int                  size() const                    { return Size; }
	inline int                  capacity() const                { return Capacity; }
	inline value_type&          operator[](int i)               { IM_ASSERT(i < Size); return Data[i]; }
	inline const value_type&    operator[](int i) const         { IM_ASSERT(i < Size); return Data[i]; }

	inline void                 clear()                         { if (Data) { Size = Capacity = 0; ImGui::MemFree(Data); Data = NULL; } }
	inline iterator             begin()                         { return Data; }
	inline const_iterator       begin() const                   { return Data; }
	inline iterator             end()                           { return Data + Size; }
	inline const_iterator       end() const                     { return Data + Size; }
	inline value_type&          front()                         { IM_ASSERT(Size > 0); return Data[0]; }
	inline const value_type&    front() const                   { IM_ASSERT(Size > 0); return Data[0]; }
	inline value_type&          back()                          { IM_ASSERT(Size > 0); return Data[Size - 1]; }
	inline const value_type&    back() const                    { IM_ASSERT(Size > 0); return Data[Size - 1]; }
	inline void                 swap(ImVector<value_type>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; value_type* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

	inline int          _grow_capacity(int sz) const            { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
	inline void         resize(int new_size)                    { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
	inline void         resize(int new_size, const value_type& v){ if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
	inline void         reserve(int new_capacity)
	{
		if (new_capacity <= Capacity)
			return;
		value_type* new_data = (value_type*)ImGui::MemAlloc((size_t)new_capacity * sizeof(value_type));
		if (Data)
			memcpy(new_data, Data, (size_t)Size * sizeof(value_type));
		ImGui::MemFree(Data);
		Data = new_data;
		Capacity = new_capacity;
	}

	// NB: &v cannot be pointing inside the ImVector Data itself! e.g. v.push_back(v[10]) is forbidden.
	inline void         push_back(const value_type& v)                  { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
	inline void         pop_back()                                      { IM_ASSERT(Size > 0); Size--; }
	inline void         push_front(const value_type& v)                 { if (Size == 0) push_back(v); else insert(Data, v); }
	inline iterator     erase(const_iterator it)                        { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(value_type)); Size--; return Data + off; }
	inline iterator     insert(const_iterator it, const value_type& v)  { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(value_type)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
	inline bool         contains(const value_type& v) const             { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
};
class test_type_name
{
public:
	virtual void fun()
	{
		cout << "just fun" << endl;
	}
	void print_tyep_name()
	{
		printf("typename:%s\n", typeid(*this).name());
	}
};
class test_tn :public test_type_name
{

};
class test_tn_tn :public test_tn
{

};
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
	
	MyClass2 mmg(1);
	
	ft.Register("MyClass", []{return new MyClass(); });
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

	std::cout << "sss is_trivial:" << std::is_trivially_copyable<sss>::value << std::endl;
	struct ImVec3
	{
		float x, y, z;
		ImVec3() { x = y = z = 0.0f; }
		ImVec3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
#ifdef IM_VEC3_CLASS_EXTRA          // Define constructor and implicit cast operators in imconfig.h to convert back<>forth from your math types and ImVec4.
		IM_VEC3_CLASS_EXTRA
#endif
	};
	struct ImVec2
	{
		float x, y;
		ImVec2() { x = y = 0.0f; }
		ImVec2(float _x, float _y) { x = _x; y = _y;  }
#ifdef IM_VEC3_CLASS_EXTRA          // Define constructor and implicit cast operators in imconfig.h to convert back<>forth from your math types and ImVec4.
		IM_VEC3_CLASS_EXTRA
#endif
	};
	std::cout << "ImVec3 is_trivially_copyable:" << std::is_trivially_copyable<ImVec3>::value << std::endl;
	std::cout << "vector<ImVec3> is_trivially_copyable:" << std::is_trivially_copyable<vector<ImVec3>>::value << std::endl;
	std::cout << "ImVector<ImVec3> is_trivially_copyable:" << std::is_trivially_copyable<ImVector<ImVec3>>::value << std::endl;
	std::cout << "IImVec3[50] is_trivially_copyable:" << std::is_trivially_copyable<ImVec3[50]>::value << std::endl;

	struct intl_pt
	{
		ImVec3 _scale;
		ImVec3 _rotation;
		ImVec2 _translation;
		//int _shader_instance_index;
		intl_pt() {}
	};
	std::cout << "intl_pt is_trivially_copyable:" << std::is_trivially_copyable<intl_pt>::value << std::endl;
	std::cout << "array<intl_pt,10> is_trivially_copyable:" << std::is_trivially_copyable<array<intl_pt,10>>::value << std::endl;
	//intl_pt dd[10];
     vector<int> vtest = { 1, 3, 4, 5 };
	for (auto& iv:vtest)
	{
		iv = iv + 2;
	}
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

	test_type_name* ptn = new test_tn_tn;
	ptn->print_tyep_name();
	test_tn_tn tttn;
	tttn.print_tyep_name();
	//vvtest.clear();
	//map<int, int> mp_int = { 10, 10 };

	return 0;
}

