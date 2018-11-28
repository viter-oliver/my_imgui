#pragma once

#include "FBXImporterCommon.h"
#include "RCPtr.h"
#include "FBXVector2.h"
#include "FBXVector3.h"
#include "FBXVector4.h"
#include "FBXColor.h"
#include "FBXMatrix4x4.h"

namespace FBXImporterUnmanaged
{
	class FBXValue;
	typedef RCPtr<FBXValue> FBXValuePtr;

	class FBXValue : public RCObject
	{
	public:
		enum ValueType
		{
			Bool,
			Int,
			Float,
			Double,
			Vector2,
			Vector3,
			Vector4,
			String,
			Color,
			Matrix4x4
		};

	public:
		static FBXValuePtr CreateBool();
		static FBXValuePtr CreateInt();
		static FBXValuePtr CreateFloat();
		static FBXValuePtr CreateDouble();
		static FBXValuePtr CreateVector2();
		static FBXValuePtr CreateVector3();
		static FBXValuePtr CreateVector4();
		static FBXValuePtr CreateString();
		static FBXValuePtr CreateColor();
		static FBXValuePtr CreateMatrix4x4();

		FBXValue();
		virtual ~FBXValue();

		virtual ValueType GetType() const = 0;

		virtual void Set( const bool& value ) { assert( 0 ); }
		virtual void Set( const int& value ) { assert( 0 ); }
		virtual void Set( const float& value ) { assert( 0 ); }
		virtual void Set( const double& value ) { assert( 0 ); }
		virtual void Set( const FBXVector2& value ) { assert( 0 ); }
		virtual void Set( const FBXVector3& value ) { assert( 0 ); }
		virtual void Set( const FBXVector4& value ) { assert( 0 ); }
		virtual void Set( const std::string& value ) { assert( 0 ); }
		virtual void Set( const FBXColor& value ) { assert( 0 ); }
		virtual void Set( const FBXMatrix4x4& value ) { assert( 0 ); }

		virtual void Get( bool& value ) { assert( 0 ); }
		virtual void Get( int& value ) { assert( 0 ); }
		virtual void Get( float& value ) { assert( 0 ); }
		virtual void Get( double& value ) { assert( 0 ); }
		virtual void Get( FBXVector2& value ) { assert( 0 ); }
		virtual void Get( FBXVector3& value ) { assert( 0 ); }
		virtual void Get( FBXVector4& value ) { assert( 0 ); }
		virtual void Get( std::string& value ) { assert( 0 ); }
		virtual void Get( FBXColor& value ) { assert( 0 ); }
		virtual void Get( FBXMatrix4x4& value ) { assert( 0 ); }
	};

	template<typename T>
	class FBXValueConcrete : public FBXValue
	{
		ValueType	type;
		T			value;

	public:
		FBXValueConcrete( ValueType type ) : type( type ) {};
		virtual ~FBXValueConcrete() {};

		inline virtual ValueType GetType() const { return type; }
		inline virtual void Set( const T& v ) { value = v; }
		inline virtual void Get( T& v ) { v = value; }
	};

	typedef FBXValueConcrete<bool> FBXValueBool;
	typedef FBXValueConcrete<int> FBXValueInt;
	typedef FBXValueConcrete<float> FBXValueFloat;
	typedef FBXValueConcrete<double> FBXValueDouble;
	typedef FBXValueConcrete<FBXVector2> FBXValueVector2;
	typedef FBXValueConcrete<FBXVector3> FBXValueVector3;
	typedef FBXValueConcrete<FBXVector4> FBXValueVector4;
	typedef FBXValueConcrete<std::string> FBXValueString;
	typedef FBXValueConcrete<FBXColor> FBXValueColor;
	typedef FBXValueConcrete<FBXMatrix4x4> FBXValueMatrix4x4;
}