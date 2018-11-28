#pragma once

#include "FBXImporterCommon.h"
#include "fbxsdk.h"

namespace FBXImporterUnmanaged
{
	class FBXMatrix4x4
	{
	public:

		static FBXMatrix4x4 zero;
		static FBXMatrix4x4 identity;

		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;

		FBXMatrix4x4();
		FBXMatrix4x4( float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33 );
		FBXMatrix4x4( const FBXMatrix4x4& other );
		FBXMatrix4x4( const FbxDouble4x4& other );
		virtual ~FBXMatrix4x4();

		FBXMatrix4x4& operator=( const FBXMatrix4x4& rhs );
		FBXMatrix4x4& operator=( const FbxDouble4x4& rhs );
		const float& operator[]( int index ) const;
		float& operator[]( int index );
	};
}