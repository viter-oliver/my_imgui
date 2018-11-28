#pragma once

#include "FBXImporterCommon.h"
#include "fbxsdk.h"

namespace FBXImporterUnmanaged
{
	class FBXVector2
	{
	public:
		float x;
		float y;

		static FBXVector2 zero;

		FBXVector2();
		FBXVector2( float x, float y );
		FBXVector2( const FBXVector2& other );
		FBXVector2( const FbxVector2& other );
		FBXVector2( const FbxDouble2& other );

		virtual ~FBXVector2();

		FBXVector2& operator=( const FBXVector2& rhs );
		FBXVector2& operator=( const FbxVector2& rhs );
		FBXVector2& operator=( const FbxDouble2& rhs );
	};
}