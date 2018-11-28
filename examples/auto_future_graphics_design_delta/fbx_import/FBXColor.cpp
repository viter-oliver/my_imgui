#include "FBXColor.h"

namespace FBXImporterUnmanaged
{
	FBXColor FBXColor::black( 0.0f, 0.0f, 0.0f, 0.0f );

	FBXColor::FBXColor()
		: r( 0.0f )
		, g( 0.0f )
		, b( 0.0f )
		, a( 0.0f )
	{
	}

	FBXColor::FBXColor( float r, float g, float b, float a )
		: r( r )
		, g( g )
		, b( b )
		, a( a )
	{
	}

	FBXColor::FBXColor( const FBXColor& other )
		: r( other.r )
		, g( other.g )
		, b( other.b )
		, a( other.a )
	{
	}

	FBXColor::FBXColor( const FbxColor& other )
		: r( static_cast<float>( other.mRed ) )
		, g( static_cast<float>( other.mGreen ) )
		, b( static_cast<float>( other.mBlue ) )
		, a( static_cast<float>( other.mAlpha ) )
	{
	}

	FBXColor::~FBXColor()
	{
	}

	FBXColor& FBXColor::operator=( const FBXColor& rhs )
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	FBXColor& FBXColor::operator=( const FbxColor& rhs )
	{
		r = static_cast<float>( rhs.mRed );
		g = static_cast<float>( rhs.mGreen );
		b = static_cast<float>( rhs.mBlue );
		a = static_cast<float>( rhs.mAlpha );
		return *this;
	}
}