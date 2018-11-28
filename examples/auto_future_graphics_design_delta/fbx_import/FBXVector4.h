#pragma once

#include "FBXImporterCommon.h"
#include "fbxsdk.h"

namespace FBXImporterUnmanaged
{
	class FBXVector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		static FBXVector4 zero;

		FBXVector4();
		FBXVector4( float x, float y, float z, float w );
		FBXVector4( const FBXVector4& other );
		FBXVector4( const FbxVector4& other );
		FBXVector4( const FbxDouble4& other );
		virtual ~FBXVector4();

		FBXVector4& operator=( const FBXVector4& rhs );
		FBXVector4& operator=( const FbxVector4& rhs );
		FBXVector4& operator=( const FbxDouble4& rhs );
	};
}