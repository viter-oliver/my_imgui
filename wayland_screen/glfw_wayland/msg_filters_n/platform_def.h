#pragma once
#if defined(BUILD_DLL)
#define AFG_EXPORT __declspec( dllexport )
#elif defined(BUILD_WIN32)
#define AFG_EXPORT __declspec( dllimport )
#else
#define AFG_EXPORT 
#endif