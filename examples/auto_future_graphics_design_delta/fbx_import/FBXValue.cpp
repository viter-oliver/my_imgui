#include "FBXValue.h"

namespace FBXImporterUnmanaged
{
	FBXValuePtr FBXValue::CreateBool()
	{
		FBXValue* newValue = new FBXValueBool( FBXValue::Bool );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateInt()
	{
		FBXValue* newValue = new FBXValueInt( FBXValue::Int );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateFloat()
	{
		FBXValue* newValue = new FBXValueFloat( FBXValue::Float );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateDouble()
	{
		FBXValue* newValue = new FBXValueDouble( FBXValue::Double );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateVector2()
	{
		FBXValue* newValue = new FBXValueVector2( FBXValue::Vector2 );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateVector3()
	{
		FBXValue* newValue = new FBXValueVector3( FBXValue::Vector3 );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateVector4()
	{
		FBXValue* newValue = new FBXValueVector4( FBXValue::Vector4 );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateString()
	{
		FBXValue* newValue = new FBXValueString( FBXValue::String );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateColor()
	{
		FBXValue* newValue = new FBXValueColor( FBXValue::Color );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValuePtr FBXValue::CreateMatrix4x4()
	{
		FBXValue* newValue = new FBXValueMatrix4x4( FBXValue::Matrix4x4 );
		assert( newValue );
		return FBXValuePtr( newValue );
	}

	FBXValue::FBXValue()
	{
	}

	FBXValue::~FBXValue()
	{
	}
}