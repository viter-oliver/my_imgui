#pragma once

#include "FBXImporterCommon.h"
#include "RCPtr.h"
#include "FBXValue.h"

namespace FBXImporterUnmanaged
{
	class FBXMaterial;
	typedef RCPtr<FBXMaterial> FBXMaterialPtr;
	
	class FBXMaterial : public RCObject
	{
		std::string name;
		std::unordered_map<std::string, FBXValuePtr> semantics;

		void AddValue( const char* key, FBXValuePtr value );
		void RemoveValue( const char* key );
		FBXValuePtr FindValue( const char* key ) const;

	public:
		static FBXMaterialPtr Create();
		FBXMaterial();
		virtual ~FBXMaterial();

		inline void SetName( const char* name ) { this->name = name; }
		void SetBool( const char* key, const bool& value );
		void SetInt( const char* key, const int& value );
		void SetFloat( const char* key, const float& value );
		void SetDouble( const char* key, const double& value );
		void SetVector2( const char* key, const FBXVector2& value );
		void SetVector3( const char* key, const FBXVector3& value );
		void SetVector4( const char* key, const FBXVector4& value );
		void SetString( const char* key, const char* value );
		void SetColor( const char* key, const FBXColor& value );
		void SetMatrix4x4( const char* key, const FBXMatrix4x4& value );

		inline const char* GetName() const { return name.c_str(); }
		bool GetBool( const char* key, const bool& defaultValue = false );
		int GetInt( const char* key, const int& defaultValue = 0 );
		float GetFloat( const char* key, const float& defaultValue = 0.0f );
		double GetDouble( const char* key, const double& defaultValue = 0.0 );
		FBXVector2 GetVector2( const char* key, const FBXVector2& defaultValue = FBXVector2::zero );
		FBXVector3 GetVector3( const char* key, const FBXVector3& defaultValue = FBXVector3::zero );
		FBXVector4 GetVector4( const char* key, const FBXVector4& defaultValue = FBXVector4::zero );
		const char* GetString( const char* key, const char* defaultValue = "" );
		FBXColor GetColor( const char* key, const FBXColor& defaultValue = FBXColor::black );
		FBXMatrix4x4 GetMatrix4x4( const char* key, const FBXMatrix4x4& defaultValue = FBXMatrix4x4::zero );

		inline bool Exist( const char* key ) const { return !( FindValue( key ).IsNull() ); }
	};
}