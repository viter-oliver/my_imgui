#pragma once

#include "FBXImporterCommon.h"
#include "fbxsdk.h"

namespace FBXImporterUnmanaged
{
	class FBXVector3
	{
	public:
		static FBXVector3 zero;

	public:
		float x;
		float y;
		float z;

		FBXVector3();
		FBXVector3( float x, float y, float z );
		FBXVector3( const FBXVector3& other );
		FBXVector3( const FbxVector4& other );
		FBXVector3( const FbxDouble3& other );
		virtual ~FBXVector3();

		FBXVector3& operator=( const FBXVector3& rhs );
		FBXVector3& operator=( const FbxVector4& rhs );

		FBXVector3 operator+( const FBXVector3& rhs ) const;
		FBXVector3 operator-( const FBXVector3& rhs ) const;
		float operator*( const FBXVector3& rhs ) const;
		FBXVector3 operator*( float s ) const;
		FBXVector3 operator^( const FBXVector3& rhs ) const;

		inline bool operator==( const FBXVector3& rhs ) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
		inline bool operator!=( const FBXVector3& rhs ) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

		FBXVector3& Normalize();
	};
}