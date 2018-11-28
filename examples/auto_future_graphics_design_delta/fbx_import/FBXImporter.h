#pragma once

#include "FBXImporterCommon.h"
#include "FBXScene.h"

namespace FBXImporterUnmanaged
{
	class FBXImporter
	{
		bool importScene( FBXScenePtr scene, FbxScene* fbxScene, FbxManager* manager );
		bool importGlobalSettings( FBXGlobalSettingsPtr globalSettings, FbxGlobalSettings& fbxGlobalSettings );
		bool importNode( FBXModelPtr model, FbxNode* fbxNode, FbxManager* manager );
		bool importTransform( FBXModelPtr model, FbxNode* node, FbxManager* manager );
		bool importMesh( FBXModelPtr model, FbxMesh* fbxMesh, FbxManager* manager );
		bool importVertex( FbxMesh* mesh, int iControlPoint, int iPolygon, int iPolygonVertexf, int iVertex, FbxManager* manager );
		bool importMeshPolygonVertex( FBXModelPtr model, FbxMesh* mesh, int iControlPoint, int iPolygon, int iPolygonVertex, int iVertex, FbxManager* manager );
		bool importMeshMaterial( FBXModelPtr model, FbxMesh* fbxMesh );
		bool importMaterial( FBXModelPtr model, FbxSurfaceMaterial* fbxMaterial, int iMaterial );
		bool importTextures( FBXMaterialPtr material, FbxProperty fbxProp );
		bool importParameters( FBXMaterialPtr material, FbxProperty fbxProp );
		bool importSkeleton( FBXModelPtr model, FbxSkeleton* fbxSkeleton );
		bool importRigging( FBXModelPtr model, FbxScene* scene );

		int m_nMeshCount{0};

	public:
		FBXImporter();
		virtual ~FBXImporter();

		FBXScenePtr ImportFile( const char* path );

		int getMeshCount(){ return m_nMeshCount; }
	};
}