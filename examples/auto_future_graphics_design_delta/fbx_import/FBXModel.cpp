#include "FBXModel.h"

namespace FBXImporterUnmanaged
{
	FBXModelPtr FBXModel::Create()
	{
		FBXModel* newModel = new FBXModel();
		assert( newModel );
		return FBXModelPtr( newModel );
	}

	FBXModel::FBXModel()
	{
	}

	FBXModel::~FBXModel()
	{
	}

	void FBXModel::AddChild( FBXModelPtr child )
	{
		if( !child.IsNull() )
		{
			childs.push_back( child );
		}
	}

	void FBXModel::ResizeVertex( int count )
	{
		vertice.resize( count );		
	}

	void FBXModel::ResizeColor( int count )
	{
		colors.resize( count );
	}

	void FBXModel::ResizeUV( int layerCount, int count )
	{
		uvs.resize( layerCount );
		for( size_t i = 0; i < uvs.size(); ++i )
		{
			uvs[i].resize( count );
		}
	}

	void FBXModel::ResizeNormal( int count )
	{
		normals.resize( count );
	}

	void FBXModel::ResizeTangent( int count )
	{
		tangents.resize( count );
	}

	void FBXModel::ResizeBinormal( int count )
	{
		binormals.resize( count );
	}

	void FBXModel::ResizeMaterial( int count )
	{
		materials.resize( count );
	}
}