#include "FBXMatrix4x4.h"

namespace FBXImporterUnmanaged
{
	FBXMatrix4x4 FBXMatrix4x4::zero = FBXMatrix4x4();
	FBXMatrix4x4 FBXMatrix4x4::identity = FBXMatrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 1.0f );

	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

	FBXMatrix4x4::FBXMatrix4x4()
	{
	}

	FBXMatrix4x4::FBXMatrix4x4( float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33 )
		: m00( m00 ), m01( m01 ), m02( m02 ), m03( m03 )
		, m10( m10 ), m11( m11 ), m12( m12 ), m13( m13 )
		, m20( m20 ), m21( m21 ), m22( m22 ), m23( m23 )
		, m30( m30 ), m31( m31 ), m32( m32 ), m33( m33 )
	{
	}

	FBXMatrix4x4::FBXMatrix4x4( const FBXMatrix4x4& other )
	{
		m00 = other.m00;
		m01 = other.m01;
		m02 = other.m02;
		m03 = other.m03;

		m10 = other.m10;
		m11 = other.m11;
		m12 = other.m12;
		m13 = other.m13;

		m20 = other.m20;
		m21 = other.m21;
		m22 = other.m22;
		m23 = other.m23;

		m30 = other.m30;
		m31 = other.m31;
		m32 = other.m32;
		m33 = other.m33;
	}

	FBXMatrix4x4::FBXMatrix4x4( const FbxDouble4x4& other )
	{
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; ++j )
			{
				(*this)[ i * 4 + j ] = static_cast<float>( other[i][j] );
			}
		}
	}

	FBXMatrix4x4::~FBXMatrix4x4()
	{
	}

	FBXMatrix4x4& FBXMatrix4x4::operator=( const FBXMatrix4x4& rhs )
	{
		m00 = rhs.m00;
		m01 = rhs.m01;
		m02 = rhs.m02;
		m03 = rhs.m03;
			  
		m10 = rhs.m10;
		m11 = rhs.m11;
		m12 = rhs.m12;
		m13 = rhs.m13;
			  
		m20 = rhs.m20;
		m21 = rhs.m21;
		m22 = rhs.m22;
		m23 = rhs.m23;
			  
		m30 = rhs.m30;
		m31 = rhs.m31;
		m32 = rhs.m32;
		m33 = rhs.m33;

		return *this;
	}

	FBXMatrix4x4& FBXMatrix4x4::operator=( const FbxDouble4x4& rhs )
	{
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; ++j )
			{
				(*this)[ i * 4 + j ] = static_cast<float>( rhs[i][j] );
			}
		}

		return *this;
	}

	const float& FBXMatrix4x4::operator[]( int index ) const
	{
		return static_cast<const float*>( &m00 )[index];
	}

	float& FBXMatrix4x4::operator[]( int index )
	{
		return static_cast<float*>( &m00 )[index];
	}
}