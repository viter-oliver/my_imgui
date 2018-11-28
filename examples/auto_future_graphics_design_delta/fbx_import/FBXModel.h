#pragma once

#include "FBXImporterCommon.h"
#include "FBXColor.h"
#include "FBXVector2.h"
#include "FBXVector3.h"
#include "FBXVector4.h"
#include "FBXColor.h"
#include "RCPtr.h"
#include "FBXMaterial.h"

namespace FBXImporterUnmanaged
{
	class FBXModel;
	typedef RCPtr<FBXModel> FBXModelPtr;

	class FBXModel : public RCObject
	{
	private:

		std::string																name;
		std::vector<FBXModelPtr>												childs;

		// transform
		FBXVector3																rotationOffset;
		FBXVector3																scalingOffset;
		FBXVector3																pivotRotation;
		FBXVector3																pivotScale;
		FBXVector3																preRotation;
		FBXVector3																postRotation;
		FBXVector3																translation;
		FBXVector3																rotation;
		FBXVector3																scale;
		FBXVector3																geometryTranslation;
		FBXVector3																geometryRotation;
		FBXVector3																geometryScale;

		// mesh
		std::vector<FBXVector3>													vertice;				// vertice
		std::vector<FBXColor>													colors;					// color
		std::vector<std::vector<FBXVector2> >									uvs;					// uv layers
		std::vector<FBXVector3>													normals;				// iVertex to normal
		std::vector<FBXVector4>													tangents;				// iVertex to tangent
		std::vector<FBXVector4>													binormals;				// iVertex to binormal
		std::unordered_map<int, std::vector<int> >								indexMap;				// iControlPoint to iVertex

		// material
		std::vector<FBXMaterialPtr>												materials;				// materials
		std::unordered_map<int, std::vector<int> >								materialPolygons;		// iMaterial to iPolygons

		// indice
		std::vector<std::vector<int> >											indice;					// index of submesh

		//is mesh model
		bool                                                                    isMesh{false};
		std::vector<int>                                                        indexCustom;            //ploygon vertex index 

		std::vector<FBXVector2>                                                 customUV;

	public:
		static FBXModelPtr Create();
		FBXModel();
		virtual ~FBXModel();

		inline void SetName( const char* name ) { this->name = name; }
		inline const char* GetName() const { return name.c_str(); }
		inline int GetChildCount() const { return static_cast<int>( childs.size() ); }
		inline FBXModelPtr GetChild( int index ) { return index < static_cast<int>( childs.size() ) ? childs[index] : FBXModelPtr::Null; }
		void AddChild( FBXModelPtr child );

		void ResizeVertex( int count );
		void ResizeColor( int count );
		void ResizeUV( int layerCount, int count );
		void ResizeNormal( int count );
		void ResizeTangent( int count );
		void ResizeBinormal( int count );
		void ResizeMaterial( int count );

		inline void SetPivotRotation( const FBXVector3& value ) { pivotRotation = value; }
		inline void SetRotationOffset( const FBXVector3& value ) { rotationOffset = value; }
		inline void SetPreRotation( const FBXVector3& value ) { preRotation = value; }
		inline void SetPostRotation( const FBXVector3& value ) { postRotation = value; }
		inline void SetPivotScale( const FBXVector3& value ) { pivotScale = value; }
		inline void SetScalingOffset( const FBXVector3& value ) { scalingOffset = value; }

		inline void SetTranslation( const FBXVector3& value ) { translation = value; }
		inline void SetRotation( const FBXVector3& value ) { rotation = value; }
		inline void SetScale( const FBXVector3& value ) { scale = value; }

		inline void SetGeometryTranslation( const FBXVector3& value ) { geometryTranslation = value ; }
		inline void SetGeometryRotation( const FBXVector3& value ) { geometryRotation = value; }
		inline void SetGeometryScale( const FBXVector3& value ) { geometryScale = value; }

		inline void AddVertex( const FBXVector3& value ) { vertice.push_back( value ); }
		inline void AddIndex( int subMesh, int value )
		{
			if( subMesh >= static_cast<int>( indice.size() ) ) indice.resize( subMesh + 1 );
			indice[subMesh].push_back( value );
		}
		inline void AddColor( const FBXColor& value ) { colors.push_back( value ); }
		inline void AddUV( int layerIndex, const FBXVector2& value )
		{
			if( layerIndex >= static_cast<int>( uvs.size() ) ) uvs.resize( layerIndex + 1 );
			uvs[layerIndex].push_back( value );
		}

		//add custom uv
		inline void AddCustomUV(const FBXVector2& value){customUV.push_back(value);}
		inline int GetCustomUVCount(){ return customUV.size(); }
		inline FBXVector2 GetCustomUV(int index){ return customUV[index]; }

		inline void AddNormal( const FBXVector3& value ) { normals.push_back( value ); }
		inline void AddTangent( const FBXVector4& value ) { tangents.push_back( value ); }
		inline void AddBinormal( const FBXVector4& value ) { binormals.push_back( value ); }
		inline void AddMaterial( const FBXMaterialPtr& value ) { materials.push_back( value ); }

		inline void SetVertex( int index, const FBXVector3& value )
		{
			if( index >= static_cast<int>( vertice.size() ) ) vertice.resize( index + 1 );
			vertice[index] = value;
		}

		inline void SetColor( int index, const FBXColor& value )
		{
			if( index >= static_cast<int>( colors.size() ) ) colors.resize( index + 1 );
			colors[index] = value;
		}

		inline void SetUVLayer( int layerIndex )
		{
			if( layerIndex >= static_cast<int>( uvs.size() ) )
			{
				uvs.reserve( layerIndex + 1 );
			}
		}

		inline void SetUV( int layerIndex, int index, const FBXVector2 value )
		{
			if( layerIndex >= static_cast<int>( uvs.size() ) )
			{
				uvs.resize( layerIndex + 1 );
			}

			std::vector<FBXVector2>& uv = uvs[layerIndex];
			if( index >= static_cast<int>( uv.size() ) )
			{
				uv.resize( index + 1 );
			}
			uv[index] = value;
		}

		inline void SetNormal( int index, const FBXVector3& value )
		{
			if( index >= static_cast<int>( normals.size() ) )
			{
				normals.resize( index + 1 );
			}
			normals[index] = value;
		}

		inline void SetTangent( int index, const FBXVector4& value )
		{
			if( index >= static_cast<int>( tangents.size() ) )
			{
				tangents.resize( index + 1 );
			}
			tangents[index] = value;
		}

		inline void SetBinormal( int index, const FBXVector4& value )
		{
			if( index >= static_cast<int>( binormals.size() ) )
			{
				binormals.resize( index + 1);
			}
			binormals[index] = value;
		}

		inline void SetIndexMap( int iControlPoint, int iVertex )
		{
			if( indexMap.find( iControlPoint ) == indexMap.end() )
			{
				indexMap.insert( std::unordered_map<int, std::vector<int> >::value_type( iControlPoint, std::vector<int>() ) );
			}

			indexMap[iControlPoint].push_back( iVertex );
		}

		inline void SetMaterial( int index, FBXMaterialPtr value )
		{
			if( index >= static_cast<int>( materials.size() ) )
			{
				materials.resize( index + 1 );
			}
			materials[index] = value;
		}

		inline void AddMaterialPolygons( int iMaterial, int iPolygon )
		{
			if( materialPolygons.find( iMaterial ) == materialPolygons.end() )
			{
				materialPolygons.insert( std::unordered_map<int, std::vector<int> >::value_type( iMaterial, std::vector<int>() ) );
			}
			materialPolygons[iMaterial].push_back( iPolygon );
		}

		inline FBXVector3 GetPivotRotation() const { return pivotRotation; }
		inline FBXVector3 GetRotationOffset() const { return rotationOffset; }
		inline FBXVector3 GetPreRotation() const { return preRotation; }
		inline FBXVector3 GetPostRotation() const { return postRotation; }
		inline FBXVector3 GetPivotScale() const { return pivotScale; }
		inline FBXVector3 GetScalingOffset() const { return scalingOffset; }
		inline FBXVector3 GetTranslation() const { return translation; }
		inline FBXVector3 GetRotation() const { return rotation; }
		inline FBXVector3 GetScale() const { return scale; }
		inline FBXVector3 GetGeometryTranslation() const { return geometryTranslation; }
		inline FBXVector3 GetGeometryRotation() const { return geometryRotation; }
		inline FBXVector3 GetGeometryScale() const { return geometryScale; }

		inline FBXVector3 GetVertex( int index ) { return index < static_cast<int>( vertice.size() ) ? vertice[index] : FBXVector3::zero; }
		inline int GetIndex( int iMaterial, int index ) {
			return iMaterial < static_cast<int>( indice.size() ) && index < static_cast<int>( indice[iMaterial].size() ) ?
				indice[iMaterial][index] : -1;
		}

		inline FBXColor GetColor( int index ) { return index < static_cast<int>( colors.size() ) ? colors[index] : FBXColor::black; }
		inline FBXVector2 GetUV( int layerIndex, int index )
		{
			if( layerIndex < static_cast<int>( uvs.size() ) )
			{
				std::vector<FBXVector2>& uv = uvs[layerIndex];
				if( index < static_cast<int>( uv.size() ) )
				{
					return uv[index];
				}
			}
			return FBXVector2::zero;
		}
		inline FBXVector3 GetNormal( int index ) { return index < static_cast<int>( normals.size() ) ? normals[index] : FBXVector3::zero; }
		inline FBXVector4 GetTangent( int index ) { return index < static_cast<int>( tangents.size() ) ? tangents[index] : FBXVector4::zero; }
		inline FBXVector4 GetBinormal( int index ) { return index < static_cast<int>( binormals.size() ) ? binormals[index] : FBXVector4::zero; }

		inline int GetIndexMapCount( int iControlPoint )
		{
			std::unordered_map<int, std::vector<int> >::iterator it = indexMap.find( iControlPoint );
			if( it != indexMap.end() )
			{
				return static_cast<int>( it->second.size() );
			}

			return 0;
		}

		inline int GetIndexMap( int iControlPoint, int index )
		{
			std::unordered_map<int, std::vector<int> >::iterator it = indexMap.find( iControlPoint );
			if( it != indexMap.end() )
			{
				std::vector<int>& indice = it->second;
				if( index < static_cast<int>( indice.size() ) )
				{
					return indice[index];
				}
			}

			return -1;
		}

		inline FBXMaterialPtr GetMaterial( int index ) { return index < static_cast<int>( materials.size() ) ? materials[index] : FBXMaterialPtr::Null; }
		inline int GetMaterialPolygonCount( int iMaterial ) { return iMaterial < static_cast<int>( materialPolygons.size() ) ? static_cast<int>( materialPolygons[iMaterial].size() ) : 0; }
		inline int GetMaterialPolygon( int iMaterial, int index ) {
			return iMaterial < static_cast<int>( materialPolygons.size() )
				&& index < static_cast<int>( materialPolygons[iMaterial].size() ) ? materialPolygons[iMaterial][index] : -1;
		}

		inline int GetVertexCount() const { return static_cast<int>( vertice.size() ); }
		inline int GetIndiceCount( int iMaterial ) const { return iMaterial < static_cast<int>( indice.size() ) ? static_cast<int>( indice[iMaterial].size() ) : 0; }
		inline int GetColorCount() const { return static_cast<int>( colors.size() ); }
		inline int GetUVLayerCount() const { return static_cast<int>( uvs.size() ); }
		inline int GetUVCount( int layerIndex ) const { return layerIndex < static_cast<int>( uvs.size() ) ? static_cast<int>( uvs[layerIndex].size() ) : 0; }
		inline int GetNormalCount() const { return static_cast<int>( normals.size() ); }
		inline int GetTangentCount() const { return static_cast<int>( tangents.size() ); }
		inline int GetBinormalCount() const { return static_cast<int>( binormals.size() ); }
		inline int GetMaterialCount() const { return static_cast<int>( materials.size() ); }

		//add is Mesh
		inline void SetIsMesh(bool bRes){ isMesh = bRes; }
		inline bool GetIsMesh(){ return isMesh; }

		inline void SetCustomIndex(int index){ indexCustom.push_back(index); }
		inline int GetCustomIndex(int index){ return indexCustom[index]; }
		inline int GetCustomIndexCount(){ return indexCustom.size(); }

		inline void getTextureProperty(float &umin, float &urange, float &vmin, float &vrange)
		{ 
			float u_tmp_min = 0.f, u_tmp_max = 0.f, v_tmp_min = 0.f, v_tmp_max = 0.f;
			std::vector<FBXVector2>& uv = customUV;
			for (auto &it : uv)
			{
				u_tmp_min = u_tmp_min > it.x ? it.x : u_tmp_min;
				u_tmp_max = u_tmp_max < it.x ? it.x : u_tmp_max;
				v_tmp_min = v_tmp_min > it.y ? it.y : v_tmp_min;
				v_tmp_max = v_tmp_max < it.y ? it.y : v_tmp_max;
			}
			umin = u_tmp_min;
			urange = u_tmp_max - u_tmp_min;
			vmin = v_tmp_min;
			vrange = v_tmp_max - v_tmp_min;
		}
	};
}