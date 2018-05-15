#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED

//# define JSON_IN_CPPTL 1

//#  define JSON_USE_CPPTL 1
//#  define JSON_USE_CPPTL_SMALLMAP 1
//#  define JSON_VALUE_USE_INTERNAL_MAP 1
//#  define JSON_USE_SIMPLE_INTERNAL_ALLOCATOR 1

#define JSON_USE_EXCEPTION 1

#ifdef JSON_IN_CPPTL
#include <cpptl/config.h>
#ifndef JSON_USE_CPPTL
#define JSON_USE_CPPTL 1
#endif
#endif

#ifdef JSON_IN_CPPTL
#define JSON_API CPPTL_API
#elif defined(JSON_DLL_BUILD)
#define JSON_API __declspec(dllexport)
#elif defined(JSON_DLL)
#define JSON_API __declspec(dllimport)
#else
#define JSON_API
#endif

//#define JSON_API JSONLIBPRODLLEXPORT


#endif // JSON_CONFIG_H_INCLUDED