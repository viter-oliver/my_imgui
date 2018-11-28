#include "fbxsdk.h"
#include "FBXVector3.h"

namespace FBXImporterUnmanaged
{
	FBXVector3 FBXVector3::zero( 0.0f, 0.0f, 0.0f );

	FBXVector3::FBXVector3()
		: x( 0.0f )
		, y( 0.0f )
		, z( 0.0f )
	{
	}

	FBXVector3::FBXVector3( float x, float y, float z )
		: x( x )
		, y( y )
		, z( z )
	{
	}

	FBXVector3::FBXVector3( const FBXVector3& other )
		: x( other.x )
		, y( other.y )
		, z( other.z )
	{
	}

	FBXVector3::FBXVector3( const FbxVector4& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
		, z( static_cast<float>( other[2] ) )
	{
	}

	FBXVector3::FBXVector3( const FbxDouble3& other )
		: x( static_cast<float>( other[0] ) )
		, y( static_cast<float>( other[1] ) )
		, z( static_cast<float>( other[2] ) )
	{
	}

	FBXVector3::~FBXVector3()
	{
	}

	FBXVector3& FBXVector3::operator=( const FBXVector3& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	FBXVector3& FBXVector3::operator=( const FbxVector4& rhs )
	{
		x = static_cast<float>( rhs[0] );
		y = static_cast<float>( rhs[1] );
		z = static_cast<float>( rhs[2] );
		return *this;
	}

	FBXVector3 FBXVector3::operator+( const FBXVector3& rhs ) const
	{
		return FBXVector3( x + rhs.x, y + rhs.y, z + rhs.z );
	}

	FBXVector3 FBXVector3::operator-( const FBXVector3& rhs ) const
	{
		return FBXVector3( x - rhs.x, y - rhs.y, z - rhs.z );
	}

	float FBXVector3::operator*( const FBXVector3& rhs ) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	FBXVector3 FBXVector3::operator*( float s ) const
	{
		return FBXVector3( x * s, y * s, z * s );
	}

	FBXVector3 FBXVector3::operator^( const FBXVector3& rhs ) const
	{
		return FBXVector3( y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x );
	}

	FBXVector3& FBXVector3::Normalize()
	{
		float det = sqrt( x * x + y * y + z * z );
		x /= det;
		y /= det;
		z /= det;
		return *this;
	}
}