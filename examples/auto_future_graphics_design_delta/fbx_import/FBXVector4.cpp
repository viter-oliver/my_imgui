#include "fbxsdk.h"
#include "FBXVector4.h"

namespace FBXImporterUnmanaged
{
	FBXVector4 FBXVector4::zero( 0.0f, 0.0f, 0.0f, 0.0f );

	FBXVector4::FBXVector4()
		: x( 0.0f )
		, y( 0.0f )
		, z( 0.0f )
		, w( 0.0f )
	{
	}

	FBXVector4::FBXVector4( float x, float y, float z, float w )
		: x( x )
		, y( y )
		, z( z )
		, w( w )
	{
	}

	FBXVector4::FBXVector4( const FBXVector4& other )
		: x( other.x )
		, y( other.y )
		, z( other.z )
		, w( other.w )
	{
	}

	FBXVector4::FBXVector4( const FbxVector4& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
		, z( static_cast<float>( other[2] ) )
		, w( static_cast<float>( other[3] ) )
	{
	}

	FBXVector4::FBXVector4( const FbxDouble4& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
		, z( static_cast<float>( other[2] ) )
		, w( static_cast<float>( other[3] ) )
	{
	}

	FBXVector4::~FBXVector4()
	{
	}

	FBXVector4& FBXVector4::operator=( const FBXVector4& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}

	FBXVector4& FBXVector4::operator=( const FbxVector4& rhs )
	{
		x = static_cast<float>( rhs[0] );
		y = static_cast<float>( rhs[1] );
		z = static_cast<float>( rhs[2] );
		w = static_cast<float>( rhs[3] );
		return *this;
	}

	FBXVector4& FBXVector4::operator=( const FbxDouble4& rhs )
	{
		x = static_cast<float>( rhs[0] );
		y = static_cast<float>( rhs[1] );
		z = static_cast<float>( rhs[2] );
		w = static_cast<float>( rhs[3] );
		return *this;
	}
}