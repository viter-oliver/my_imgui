#include "fbxsdk.h"
#include "FBXVector2.h"

namespace FBXImporterUnmanaged
{
	FBXVector2 FBXVector2::zero( 0.0f, 0.0f );

	FBXVector2::FBXVector2()
		: x( 0.0f )
		, y( 0.0f )
	{
	}

	FBXVector2::FBXVector2( float x, float y )
		: x( x )
		, y( y )
	{
	}

	FBXVector2::FBXVector2( const FBXVector2& other )
		: x( other.x )
		, y( other.y )
	{
	}

	FBXVector2::FBXVector2( const FbxVector2& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
	{
	}

	FBXVector2::FBXVector2( const FbxDouble2& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
	{
	}

	FBXVector2::~FBXVector2()
	{
	}

	FBXVector2& FBXVector2::operator=( const FBXVector2& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	FBXVector2& FBXVector2::operator=( const FbxVector2& rhs )
	{
		x = static_cast<float>( rhs[0] );
		y = static_cast<float>( rhs[1] );
		return *this;
	}

	FBXVector2& FBXVector2::operator=( const FbxDouble2& rhs )
	{
		x = static_cast<float>( rhs[0] );
		y = static_cast<float>( rhs[1] );
		return *this;
	}
}