#include "FBXMaterial.h"

namespace FBXImporterUnmanaged
{
	FBXMaterialPtr FBXMaterial::Create()
	{
		FBXMaterial* newMaterial = new FBXMaterial();
		assert( newMaterial );
		return FBXMaterialPtr( newMaterial );
	}

	FBXMaterial::FBXMaterial()
	{
	}

	FBXMaterial::~FBXMaterial()
	{
	}

	void FBXMaterial::AddValue( const char* key, FBXValuePtr value )
	{
		semantics.erase( key );
		semantics.insert( std::unordered_map<std::string, FBXValuePtr>::value_type( key, value ) );
	}

	void FBXMaterial::RemoveValue( const char* key )
	{
		semantics.erase( key );
	}

	FBXValuePtr FBXMaterial::FindValue( const char* key ) const
	{
		std::unordered_map<std::string, FBXValuePtr>::const_iterator it = semantics.find( key );
		return it != semantics.end() ? it->second : FBXValuePtr::Null;
	}

	void FBXMaterial::SetBool( const char* key, const bool& value )
	{
		FBXValuePtr v = FBXValue::CreateBool();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetInt( const char* key, const int& value )
	{
		FBXValuePtr v = FBXValue::CreateInt();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetFloat( const char* key, const float& value )
	{
		FBXValuePtr v = FBXValue::CreateFloat();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetDouble( const char* key, const double& value )
	{
		FBXValuePtr v = FBXValue::CreateDouble();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetVector2( const char* key, const FBXVector2& value )
	{
		FBXValuePtr v = FBXValue::CreateVector2();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetVector3( const char* key, const FBXVector3& value )
	{
		FBXValuePtr v = FBXValue::CreateVector3();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetVector4( const char* key, const FBXVector4& value )
	{
		FBXValuePtr v = FBXValue::CreateVector4();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetString( const char* key, const char* value )
	{
		FBXValuePtr v = FBXValue::CreateString();
		if( !v.IsNull() )
		{
			v->Set( std::string( value ) );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetColor( const char* key, const FBXColor& value )
	{
		FBXValuePtr v = FBXValue::CreateColor();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	void FBXMaterial::SetMatrix4x4( const char* key, const FBXMatrix4x4& value )
	{
		FBXValuePtr v = FBXValue::CreateMatrix4x4();
		if( !v.IsNull() )
		{
			v->Set( value );
		}
		AddValue( key, v );
	}

	bool FBXMaterial::GetBool( const char* key, const bool& defaultValue /*= false*/ )
	{
		FBXValuePtr v = FindValue( key );
		bool ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	int FBXMaterial::GetInt( const char* key, const int& defaultValue /*= 0*/ )
	{
		FBXValuePtr v = FindValue( key );
		int ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	float FBXMaterial::GetFloat( const char* key, const float& defaultValue /*= 0.0f*/ )
	{
		FBXValuePtr v = FindValue( key );
		float ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	double FBXMaterial::GetDouble( const char* key, const double& defaultValue /*= 0.0*/ )
	{
		FBXValuePtr v = FindValue( key );
		double ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	FBXVector2 FBXMaterial::GetVector2( const char* key, const FBXVector2& defaultValue /*= FBXVector2::zero*/ )
	{
		FBXValuePtr v = FindValue( key );
		FBXVector2 ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	FBXVector3 FBXMaterial::GetVector3( const char* key, const FBXVector3& defaultValue /*= FBXVector3::zero*/ )
	{
		FBXValuePtr v = FindValue( key );
		FBXVector3 ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	FBXVector4 FBXMaterial::GetVector4( const char* key, const FBXVector4& defaultValue /*= FBXVector4::zero*/ )
	{
		FBXValuePtr v = FindValue( key );
		FBXVector4 ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	const char* FBXMaterial::GetString( const char* key, const char* defaultValue /*= ""*/ )
	{
		FBXValuePtr v = FindValue( key );
		static std::string value = defaultValue;
		value.clear();
		if( !v.IsNull() )
		{
			v->Get( value );
		}

		return value.c_str();
	}

	FBXColor FBXMaterial::GetColor( const char* key, const FBXColor& defaultValue /*= FBXColor::black*/ )
	{
		FBXValuePtr v = FindValue( key );
		FBXColor ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}

	FBXMatrix4x4 FBXMaterial::GetMatrix4x4( const char* key, const FBXMatrix4x4& defaultValue /*= FBXMatrix4x4::zero*/ )
	{
		FBXValuePtr v = FindValue( key );
		FBXMatrix4x4 ret = defaultValue;
		if( !v.IsNull() )
		{
			v->Get( ret );
		}
		return ret;
	}
}