#include "fbxsdk.h"
#include "FBXImporter.h"
#include "FBXVector2.h"
#include "FBXVector3.h"
#include "FBXVector4.h"

namespace FBXImporterUnmanaged
{
	FBXImporter::FBXImporter()
	{
	}

	FBXImporter::~FBXImporter()
	{
	}

	FBXScenePtr FBXImporter::ImportFile( const char* path )
	{
		// fbx manager
		FbxManager* manager = FbxManager::Create();

		// fbx iosettings
		FbxIOSettings* ioSettings = FbxIOSettings::Create( manager, IOSROOT );
		ioSettings->SetBoolProp( IMP_FBX_MATERIAL, true );
		ioSettings->SetBoolProp( IMP_FBX_TEXTURE, true );
		manager->SetIOSettings( ioSettings );

		// fbx importer
		FbxImporter* importer = FbxImporter::Create( manager, "" );
		bool ret = importer->Initialize( path, -1, ioSettings );
		if( !ret )
		{
			printf( "Initialize importer failed\n" );
			importer->Destroy();
			manager->Destroy();
			return 0;
		}

		// fbx scene
		FbxScene* fbxScene = FbxScene::Create( manager, "" );
		if( !importer->Import( fbxScene ) )
		{
			fbxScene->Destroy();
			importer->Destroy();
			manager->Destroy();
			return 0;
		}

		// import scene
		FBXScenePtr scene = FBXScene::Create();
		importScene( scene, fbxScene, manager );

		fbxScene->Destroy();
		importer->Destroy();
		manager->Destroy();

		return scene;
	}

	bool FBXImporter::importScene( FBXScenePtr scene, FbxScene* fbxScene, FbxManager* manager )
	{
		// convert to Unity unit system
		if( fbxScene->GetGlobalSettings().GetSystemUnit().GetScaleFactor() != 0.01f )
		{
			FbxSystemUnit::m.ConvertScene( fbxScene );
		}

		// import global settings
		FBXGlobalSettingsPtr globalSettings = FBXGlobalSettings::Create();
		scene->SetGlobalSettings( globalSettings );
		if( !importGlobalSettings( globalSettings, fbxScene->GetGlobalSettings() ) )
		{
			return false;
		}

		// import node
		FBXModelPtr model = FBXModel::Create();
		scene->SetModel( model );

		FbxNode* root = fbxScene->GetRootNode();
		if( !importNode( model, fbxScene->GetRootNode(), manager ) )
		{
			return false;
		}

		// import rigging
		importRigging( model, fbxScene );

		return true;
	}

	bool FBXImporter::importGlobalSettings( FBXGlobalSettingsPtr globalSettings, FbxGlobalSettings& fbxGlobalSettings )
	{
		// import unit scalar factor
		globalSettings->SetUnitScaleFactor( static_cast<float>( fbxGlobalSettings.GetSystemUnit().GetScaleFactor() ) );
		globalSettings->SetOriginalUnitScaleFactor( static_cast<float>( fbxGlobalSettings.GetOriginalSystemUnit().GetScaleFactor() ) );

		// import coordinate system
		FbxAxisSystem::ECoordSystem coordinateSystem = fbxGlobalSettings.GetAxisSystem().GetCoorSystem();
		switch( coordinateSystem )
		{
			case FbxAxisSystem::ECoordSystem::eLeftHanded: globalSettings->SetCoordinateSystem( FBXGlobalSettings::CoordinateSystem::LeftHand ); break;
			case FbxAxisSystem::ECoordSystem::eRightHanded: globalSettings->SetCoordinateSystem( FBXGlobalSettings::CoordinateSystem::RightHand ); break;
			default:
			{
			}
		}

		// import up and front vector
		int sign = 1;
		int fbxOriginalUp = fbxGlobalSettings.GetOriginalUpAxis();
		globalSettings->SetOriginalUpVector( ( FBXGlobalSettings::UpVector )fbxOriginalUp );

		FbxAxisSystem::EUpVector fbxUp = fbxGlobalSettings.GetAxisSystem().GetUpVector( sign );
		switch( fbxUp )
		{
			case FbxAxisSystem::EUpVector::eXAxis: globalSettings->SetUpVector( FBXGlobalSettings::UpVector::X ); break;
			case FbxAxisSystem::EUpVector::eYAxis: globalSettings->SetUpVector( FBXGlobalSettings::UpVector::Y ); break;
			case FbxAxisSystem::EUpVector::eZAxis: globalSettings->SetUpVector( FBXGlobalSettings::UpVector::Z ); break;
			default:
			{
			}
		}

		FbxAxisSystem::EFrontVector fbxFront = fbxGlobalSettings.GetAxisSystem().GetFrontVector( sign );
		switch( fbxFront )
		{
			case FbxAxisSystem::EFrontVector::eParityOdd: globalSettings->SetFrontVector( FBXGlobalSettings::FrontVector::Odd ); break;
			case FbxAxisSystem::EFrontVector::eParityEven: globalSettings->SetFrontVector( FBXGlobalSettings::FrontVector::Even ); break;
			default:
			{
			}
		}

		//import light,camera
		globalSettings->QSetAmbientColor(fbxGlobalSettings.GetAmbientColor());
		globalSettings->QSetDefaultCamera(fbxGlobalSettings.GetDefaultCamera());

		return true;
	}

	bool FBXImporter::importNode( FBXModelPtr model, FbxNode* node, FbxManager* manager )
	{
		// import name
		model->SetName( node->GetName() );
		// import transform
		importTransform( model, node, manager );

		// import material
		for( int iMaterial = 0; iMaterial < node->GetMaterialCount(); ++iMaterial )
		{
			FbxSurfaceMaterial* fbxMaterial = node->GetMaterial( iMaterial );
			importMaterial( model, fbxMaterial, iMaterial );
		}

		// import attributes
		for( int j = 0; j < node->GetNodeAttributeCount(); ++j )
		{
			FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex( j );
			switch( attribute->GetAttributeType() )
			{
				case FbxNodeAttribute::eMesh:
				{
					printf("\n------------------------mesh name:%s------------------------\n", node->GetName());
					if( !importMesh( model, static_cast<FbxMesh*>( attribute ), manager ) )
					{
						printf( "import mesh failed node = %s\n", node->GetName() );
					}
				}
				break;
				default:
				{
				}
			}
		}

		// import childs
		for( int i = 0; i < node->GetChildCount(); ++i )
		{
			FbxNode* child = node->GetChild( i );
			FBXModelPtr childModel = FBXModel::Create();
			if( importNode( childModel, child, manager ) )
			{
				model->AddChild( childModel );
			}
		}

		return true;
	}

	bool FBXImporter::importTransform( FBXModelPtr model, FbxNode* node, FbxManager* manager )
	{
		model->SetPivotRotation( FBXVector3( node->GetRotationPivot( FbxNode::EPivotSet::eSourcePivot ) ) );
		model->SetRotationOffset( node->GetRotationOffset( FbxNode::EPivotSet::eSourcePivot ) );
		model->SetPreRotation( node->GetPreRotation( FbxNode::EPivotSet::eSourcePivot ) );
		model->SetPostRotation( node->GetPostRotation( FbxNode::EPivotSet::eSourcePivot ) );
		model->SetPivotScale( FBXVector3( node->GetScalingPivot( FbxNode::EPivotSet::eSourcePivot ) ) );
		model->SetScalingOffset( node->GetScalingOffset( FbxNode::EPivotSet::eSourcePivot ) );
		model->SetGeometryTranslation( node->GeometricTranslation.Get() );
		model->SetGeometryTranslation( node->GeometricRotation.Get() );


		model->SetTranslation( FBXVector3( node->LclTranslation.Get() ) );
		model->SetRotation( FBXVector3( node->LclRotation.Get() ) );
		model->SetScale( FBXVector3( node->LclScaling.Get() ) );

		return true;
	}

	bool FBXImporter::importMesh( FBXModelPtr model, FbxMesh* fbxMesh, FbxManager* manager )
	{
		if( fbxMesh->RemoveBadPolygons() < 0 )
		{
			return false;
		}

		//add is Mesh
		model->SetIsMesh(true);
		++m_nMeshCount;

		FbxGeometryConverter triangleConverter( manager );

		// Must do this before triangulating the mesh due to an FBX bug in TriangulateMeshAdvance
		int layerSmoothCount = fbxMesh->GetLayerCount( FbxLayerElement::eSmoothing );
		for( int i = 0; i < layerSmoothCount; ++i )
		{
			FbxLayerElementSmoothing const* SmoothingInfo = fbxMesh->GetLayer( 0 )->GetSmoothing();
			if( SmoothingInfo && SmoothingInfo->GetMappingMode() != FbxLayerElement::eByPolygon )
			{
				triangleConverter.ComputePolygonSmoothingFromEdgeSmoothing( fbxMesh, i );
			}
		}

		// trianglate
		if( !fbxMesh->IsTriangleMesh() )
		{
			// triangulate mesh
			FbxMesh* convertedMesh = (FbxMesh*)triangleConverter.Triangulate( fbxMesh, true );
			if( convertedMesh && convertedMesh->GetAttributeType() == FbxNodeAttribute::eMesh )
			{
				fbxMesh = convertedMesh;
			}
		}

		// import vertice element data
		std::vector<int> indice;
		int polygonCount = fbxMesh->GetPolygonCount();
		indice.resize( polygonCount * 3 );
		for( int iPolygon = 0; iPolygon < polygonCount; ++iPolygon )
		{
			int polygonVertexCount = 3;
			for( int iPolygonVertex = 0; iPolygonVertex < polygonVertexCount; ++iPolygonVertex )
			{
				int iControlPoint = fbxMesh->GetPolygonVertex( iPolygon, iPolygonVertex );
				
				//add custom
				model->SetCustomIndex(iControlPoint);
				
				int iVertex = iPolygon * 3 + iPolygonVertex;

				// import vertex
				FBXVector3 vertex = fbxMesh->GetControlPointAt( iControlPoint );
				model->AddVertex(vertex); //import all vertex

				// import color
				FBXColor color;
				const int iColor = 0;
				const FbxGeometryElementVertexColor* fbxColors = fbxMesh->GetElementVertexColor( iColor );
				if( fbxColors )
				{
					switch( fbxColors->GetMappingMode() )
					{
						case FbxGeometryElement::eByControlPoint:
						{
							switch( fbxColors->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									color = fbxColors->GetDirectArray().GetAt( iControlPoint );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									int colorIndex = fbxColors->GetIndexArray().GetAt( iControlPoint );
									color = fbxColors->GetDirectArray().GetAt( colorIndex );
								}
								default:
								{
								}
							}
						}
						break;
						case FbxGeometryElement::eByPolygonVertex:
						{
							switch( fbxColors->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									color = fbxColors->GetDirectArray().GetAt( iVertex );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									int colorIndex = fbxColors->GetIndexArray().GetAt( iVertex );
									color = fbxColors->GetDirectArray().GetAt( colorIndex );
								}
								default:
								{
								}
							}
						}
						break;
					}
					model->AddColor(color);
				}// finish color

				// import uvs
				std::vector<FBXVector2> uvs;

				//目前不清楚>1的时候uv坐标为什么大于1或者小于0，暂时放下
				int iUVLayer = 0;
				//for (int iUVLayer = 0; iUVLayer < fbxMesh->GetElementUVCount(); ++iUVLayer)
				FBXVector2 _cusUV;


				{
					const FbxGeometryElementUV* fbxUVs = fbxMesh->GetElementUV( iUVLayer );
					if( fbxUVs )
					{
						// only support mapping mode eByPolygonVertex and eByControlPoint
						switch( fbxUVs->GetMappingMode() )
						{
							case FbxGeometryElement::eByControlPoint:
							{
								switch( fbxUVs->GetReferenceMode() )
								{
									case FbxGeometryElement::eDirect:
									{
										_cusUV = fbxUVs->GetDirectArray().GetAt(iControlPoint);
										uvs.push_back( fbxUVs->GetDirectArray().GetAt( iControlPoint ) );
									}
									break;
									case FbxGeometryElement::eIndexToDirect:
									{
										_cusUV = fbxUVs->GetDirectArray().GetAt(fbxUVs->GetIndexArray().GetAt(iControlPoint));
										uvs.push_back( fbxUVs->GetDirectArray().GetAt( fbxUVs->GetIndexArray().GetAt( iControlPoint ) ) );
									}
									break;
								}
							}
							break;
							case FbxGeometryElement::eByPolygonVertex:
							{
								switch( fbxUVs->GetReferenceMode() )
								{
									case FbxGeometryElement::eDirect:
									{
										_cusUV = fbxUVs->GetDirectArray().GetAt(iVertex);
										uvs.push_back( fbxUVs->GetDirectArray().GetAt( iVertex ) );
									}
									break;
									case FbxGeometryElement::eIndexToDirect:
									{
										_cusUV = fbxUVs->GetDirectArray().GetAt(fbxUVs->GetIndexArray().GetAt(iVertex));
										uvs.push_back( fbxUVs->GetDirectArray().GetAt( fbxUVs->GetIndexArray().GetAt( iVertex ) ) );
									}
									break;
								}
							}
							break;
						}
					}
					model->AddCustomUV(_cusUV);
				} // finish uvs

				// import normals
				FBXVector3 normal;
				const int iNormal = 0;
				const FbxGeometryElementNormal* fbxNormals = fbxMesh->GetElementNormal( iNormal );
				if( fbxNormals )
				{
					switch( fbxNormals->GetMappingMode() )
					{
						case FbxGeometryElement::eByControlPoint:
						{
							switch( fbxNormals->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									normal = fbxNormals->GetDirectArray().GetAt( iControlPoint );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									normal = fbxNormals->GetDirectArray().GetAt( fbxNormals->GetIndexArray().GetAt( iControlPoint ) );
								}
							}
						}
						break;
						case FbxGeometryElement::eByPolygonVertex:
						{
							switch( fbxNormals->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									normal = fbxNormals->GetDirectArray().GetAt( iVertex );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									normal = fbxNormals->GetDirectArray().GetAt( fbxNormals->GetIndexArray().GetAt( iVertex ) );
								}
							}
						}
						break;
					}
					model->AddNormal(normal);
				} // finish normal

				// import tangent
				FBXVector4 tangent;
				const int iTangent = 0;
				const FbxGeometryElementTangent* fbxTangents = fbxMesh->GetElementTangent( iTangent );
				if( fbxTangents )
				{
					switch( fbxTangents->GetMappingMode() )
					{
						case FbxGeometryElement::eByControlPoint:
						{
							switch( fbxTangents->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									tangent = fbxTangents->GetDirectArray().GetAt( iControlPoint );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									tangent = fbxTangents->GetDirectArray().GetAt( fbxTangents->GetIndexArray().GetAt( iControlPoint ) );
								}
							}
						}
						break;
						case FbxGeometryElement::eByPolygonVertex:
						{
							switch( fbxTangents->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									tangent = fbxTangents->GetDirectArray().GetAt( iVertex );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									tangent = fbxTangents->GetDirectArray().GetAt( fbxTangents->GetIndexArray().GetAt( iVertex ) );
								}
							}
						}
						break;
					}
				} // finish tangent
				
				// import binormal
				FBXVector4 binormal;
				const int iBinormal = 0;
				const FbxGeometryElementBinormal* fbxBinormals = fbxMesh->GetElementBinormal( iBinormal );
				if( fbxBinormals )
				{
					switch( fbxBinormals->GetMappingMode() )
					{
						case FbxGeometryElement::eByControlPoint:
						{
							switch( fbxBinormals->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									binormal = fbxBinormals->GetDirectArray().GetAt( iControlPoint );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									binormal = fbxBinormals->GetDirectArray().GetAt( fbxBinormals->GetIndexArray().GetAt( iControlPoint ) );
								}
							}
						}
						break;
						case FbxGeometryElement::eByPolygonVertex:
						{
							switch( fbxBinormals->GetReferenceMode() )
							{
								case FbxGeometryElement::eDirect:
								{
									binormal = fbxBinormals->GetDirectArray().GetAt( iVertex );
								}
								break;
								case FbxGeometryElement::eIndexToDirect:
								{
									binormal = fbxBinormals->GetDirectArray().GetAt( fbxBinormals->GetIndexArray().GetAt( iVertex ) );
								}
							}
						}
						break;
					}
				} // finish binormal

				int sameNormalIndex = -1;
				int indexMapCount = model->GetIndexMapCount( iControlPoint );

				for( int iIndexMap = 0; iIndexMap < indexMapCount; ++iIndexMap )
				{
					int iIndexComp = model->GetIndexMap( iControlPoint, iIndexMap );
					FBXVector3 normalComp = model->GetNormal( iIndexComp );
					if( normalComp == normal )
					{
						sameNormalIndex = iIndexComp;
						break;
					}
				}

				if( sameNormalIndex < 0 )
				{
					int nextIndex = model->GetVertexCount();
					//model->AddVertex( vertex );

					//model->AddColor( color );
					for( int iUVLayer = 0; iUVLayer < static_cast<int>( uvs.size() ); ++iUVLayer )
					{
						//model->AddUV( iUVLayer, uvs[iUVLayer] );
					}
					//model->AddNormal( normal );
					model->AddTangent( tangent );
					model->AddBinormal( binormal );

					indice[iVertex] = nextIndex;
					model->SetIndexMap( iControlPoint, nextIndex );
				}
				else
				{
					indice[iVertex] = sameNormalIndex;
				}

			} // finish polygon vertex
		} // finish polygon

		// import mesh material
		importMeshMaterial( model, fbxMesh );

		// generate submesh
		int materialCount = model->GetMaterialCount();
		if( materialCount > 0 )
		{
			for( int iMaterial = 0; iMaterial < materialCount; ++iMaterial )
			{
				int materialPolygonCount = model->GetMaterialPolygonCount( iMaterial );
				for( int iMaterialPolygonIndex = 0; iMaterialPolygonIndex < materialPolygonCount; ++iMaterialPolygonIndex )
				{
					int iPolygon = model->GetMaterialPolygon( iMaterial, iMaterialPolygonIndex );

					int polygonIndice0 = indice[iPolygon * 3];
					int polygonIndice1 = indice[iPolygon * 3 + 1];
					int polygonIndice2 = indice[iPolygon * 3 + 2];

					model->AddIndex( iMaterial, polygonIndice0 );
					model->AddIndex( iMaterial, polygonIndice1 );
					model->AddIndex( iMaterial, polygonIndice2 );
				}
			}
		}
		else
		{
			for( int iPolygon = 0; iPolygon < fbxMesh->GetPolygonCount(); ++iPolygon )
			{
				int polygonIndice0 = indice[iPolygon * 3];
				int polygonIndice1 = indice[iPolygon * 3 + 1];
				int polygonIndice2 = indice[iPolygon * 3 + 2];

				model->AddIndex( 0, polygonIndice0 );
				model->AddIndex( 0, polygonIndice1 );
				model->AddIndex( 0, polygonIndice2 );
			}
		}

		return true;
	}

	bool FBXImporter::importMeshMaterial( FBXModelPtr model, FbxMesh* fbxMesh )
	{
		bool allSameMaterial = true;
		for( int iMaterial = 0; iMaterial < fbxMesh->GetElementMaterialCount(); ++iMaterial )
		{
			FbxGeometryElementMaterial* fbxMaterial = fbxMesh->GetElementMaterial( iMaterial );
			if( fbxMaterial->GetMappingMode() == FbxGeometryElementMaterial::EMappingMode::eByPolygon )
			{
				allSameMaterial = false;
				break;
			}
		}

		if( allSameMaterial )
		{
			for( int iMaterial = 0; iMaterial < fbxMesh->GetElementMaterialCount(); ++iMaterial )
			{
				FbxGeometryElementMaterial* fbxElementMaterial = fbxMesh->GetElementMaterial( iMaterial );
				if( fbxElementMaterial->GetMappingMode() == FbxGeometryElementMaterial::EMappingMode::eAllSame )
				{
					for( int iPolygon = 0; iPolygon < fbxMesh->GetPolygonCount(); ++iPolygon )
					{
						model->AddMaterialPolygons( iMaterial, iPolygon );
					}
				}
			}
		}
		else
		{
			for( int iPolygon = 0; iPolygon < fbxMesh->GetPolygonCount(); ++iPolygon )
			{
				for( int iMaterial = 0; iMaterial < fbxMesh->GetElementMaterialCount(); ++iMaterial )
				{
					FbxGeometryElementMaterial* fbxElementMaterial = fbxMesh->GetElementMaterial( iMaterial );
					if( fbxElementMaterial->GetMappingMode() == FbxGeometryElementMaterial::EMappingMode::eByPolygon && fbxElementMaterial->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
					{					
						model->AddMaterialPolygons( fbxElementMaterial->GetIndexArray().GetAt( iPolygon ), iPolygon );
					}
				}
			}
		}

		return true;
	}

	bool FBXImporter::importMaterial( FBXModelPtr model, FbxSurfaceMaterial* fbxMaterial, int iMaterial )
	{
		FBXMaterialPtr material = FBXMaterial::Create();

		const FbxImplementation* fbxImpl = GetImplementation( fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL );
		if( !fbxImpl )
		{
			fbxImpl = GetImplementation( fbxMaterial, FBXSDK_IMPLEMENTATION_CGFX );
		}

		if( fbxImpl )
		{
			const FbxBindingTable* bindingTable = fbxImpl->GetRootTable();
			bindingTable->DescAbsoluteURL.Get();
			for( int iEntry = 0; iEntry < static_cast<int>( bindingTable->GetEntryCount() ); ++iEntry )
			{
				const FbxBindingTableEntry& entry = bindingTable->GetEntry( iEntry );
				FbxProperty fbxProp;
				if( strcmp( FbxPropertyEntryView::sEntryType, entry.GetEntryType( true ) ) == 0 )
				{
					fbxProp = fbxMaterial->FindPropertyHierarchical( entry.GetSource() );
					if( !fbxProp.IsValid() )
					{
						fbxProp = fbxMaterial->RootProperty.FindHierarchical( entry.GetSource() );
					}
				}
				else if( strcmp( FbxConstantEntryView::sEntryType, entry.GetEntryType( true ) ) == 0 )
				{
					fbxProp = fbxImpl->GetConstants().FindHierarchical( entry.GetSource() );
				}

				if( fbxProp.IsValid() )
				{
					if( fbxProp.GetSrcObjectCount<FbxTexture>() > 0 )
					{
						importTextures( material, fbxProp );
					}
					else
					{
						importParameters( material, fbxProp );
					}
				}
			}
		}
		else
		{
			if( fbxMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) )
			{
				FbxSurfacePhong* fbxPhong = static_cast<FbxSurfacePhong*>( fbxMaterial );
				material->SetString( "ReflectionModel", "Phong" );
				material->SetVector3( "Emissive", FBXVector3( fbxPhong->Emissive.Get() ) );
				material->SetFloat( "EmissiveFactor", static_cast<float>( fbxPhong->EmissiveFactor ) );
				material->SetVector3( "Ambient", FBXVector3( fbxPhong->Ambient.Get() ) );
				material->SetFloat( "AmbientFactor", static_cast<float>( fbxPhong->AmbientFactor ) );
				material->SetVector3( "Diffuse", FBXVector3( fbxPhong->Diffuse.Get() ) );
				material->SetFloat( "DiffuseFactor", static_cast<float>( fbxPhong->DiffuseFactor ) );
				material->SetVector3( "Specular", FBXVector3( fbxPhong->Specular.Get() ) );
				material->SetFloat( "SpecularFactor", static_cast<float>( fbxPhong->SpecularFactor ) );
				material->SetFloat( "Opacity", static_cast<float>( 1.0f - fbxPhong->TransparencyFactor.Get() ) );
				material->SetFloat( "Shiniess", static_cast<float>( fbxPhong->Shininess.Get() ) );
				material->SetFloat( "ReflectionFactor", static_cast<float>( fbxPhong->ReflectionFactor.Get() ) );
				FbxTexture* fbxTexture = static_cast<FbxTexture*>( fbxMaterial->GetSrcObject<FbxTexture>() );

				for( int iTextureLayer = 0; iTextureLayer < FbxLayerElement::sTypeTextureCount; ++iTextureLayer )
				{
					FbxProperty fbxProp = fbxMaterial->FindProperty( FbxLayerElement::sTextureChannelNames[iTextureLayer] );
					if( fbxProp.IsValid() && fbxProp.GetSrcObjectCount<FbxTexture>() > 0 )
					{
						importTextures( material, fbxProp );
					}
				}
			}
			else if( fbxMaterial->GetClassId().Is( FbxSurfaceLambert::ClassId ) )
			{
				FbxSurfaceLambert* fbxLambert = static_cast<FbxSurfaceLambert*>( fbxMaterial );
				material->SetString( "ReflectionModel", "Lambert" );
				material->SetVector3( "Emissive", FBXVector3( fbxLambert->Emissive.Get() ) );
				material->SetFloat( "EmissiveFactor", static_cast<float>( fbxLambert->EmissiveFactor ) );
				material->SetVector3( "Ambient", FBXVector3( fbxLambert->Ambient.Get() ) );
				material->SetFloat( "AmbientFactor", static_cast<float>( fbxLambert->AmbientFactor ) );
				material->SetVector3( "Diffuse", FBXVector3( fbxLambert->Diffuse.Get() ) );
				material->SetFloat( "DiffuseFactor", static_cast<float>( fbxLambert->DiffuseFactor ) );
				material->SetFloat( "Opacity", static_cast<float>( 1.0f - fbxLambert->TransparencyFactor.Get() ) );

				for( int iTextureLayer = 0; iTextureLayer < FbxLayerElement::sTypeTextureCount; ++iTextureLayer )
				{
					FbxProperty fbxProp = fbxMaterial->FindProperty( FbxLayerElement::sTextureChannelNames[iTextureLayer] );
					if( fbxProp.IsValid() && fbxProp.GetSrcObjectCount<FbxTexture>() > 0 )
					{
						importTextures( material, fbxProp );
					}
				}
			}
		}

		material->SetName( fbxMaterial->GetName() );
		model->SetMaterial( iMaterial, material );
		return true;
	}

	bool FBXImporter::importTextures( FBXMaterialPtr material, FbxProperty fbxProp )
	{
		for( int iTexture = 0; iTexture < fbxProp.GetSrcObjectCount<FbxFileTexture>(); ++iTexture )
		{
			FbxFileTexture* fbxTexture = fbxProp.GetSrcObject<FbxFileTexture>( iTexture );
			material->SetString("FbxTexture", fbxTexture->GetRelativeFileName() );
		}
		for( int iTexture = 0; iTexture < fbxProp.GetSrcObjectCount<FbxLayeredTexture>(); ++iTexture )
		{
			FbxLayeredTexture* fbxTexture = fbxProp.GetSrcObject<FbxLayeredTexture>( iTexture );
			material->SetString("FbxTexture", fbxTexture->GetName());
		}
		for( int iTexture = 0; iTexture < fbxProp.GetSrcObjectCount<FbxProceduralTexture>(); ++iTexture )
		{
			FbxProceduralTexture* fbxTexture = fbxProp.GetSrcObject<FbxProceduralTexture>( iTexture );
			material->SetString("FbxTexture", fbxTexture->GetName());
		}

		return true;
	}

	bool FBXImporter::importParameters( FBXMaterialPtr material, FbxProperty fbxProp )
	{
		FbxDataType fbxDataType = fbxProp.GetPropertyDataType();
		if( fbxDataType == FbxBoolDT )
		{
			material->SetBool( fbxProp.GetName(), fbxProp.Get<FbxBool>() );
		}
		else if( fbxDataType == FbxIntDT || fbxDataType == FbxEnumDT )
		{
			material->SetInt( fbxProp.GetName(), fbxProp.Get<FbxInt>() );
		}
		else if( fbxDataType == FbxFloatDT )
		{
			material->SetFloat( fbxProp.GetName(), fbxProp.Get<FbxFloat>() );
		}
		else if( fbxDataType == FbxDoubleDT )
		{
			material->SetDouble( fbxProp.GetName(), fbxProp.Get<FbxDouble>() );
		}
		else if( fbxDataType == FbxStringDT )
		{
			material->SetString( fbxProp.GetName(), fbxProp.Get<FbxString>() );
		}
		else if( fbxDataType == FbxDouble2DT )
		{
			material->SetVector2( fbxProp.GetName(), FBXVector2( fbxProp.Get<FbxDouble2>() ) );
		}
		else if( fbxDataType == FbxDouble3DT || fbxDataType == FbxColor3DT )
		{
			material->SetVector3( fbxProp.GetName(), FBXVector3( fbxProp.Get<FbxDouble3>() ) );
		}
		else if( fbxDataType == FbxDouble4DT || fbxDataType == FbxColor4DT )
		{
			material->SetVector4( fbxProp.GetName(), FBXVector4( fbxProp.Get<FbxDouble4>() ) );
		}
		else if( fbxDataType == FbxDouble4x4DT )
		{
			material->SetMatrix4x4( fbxProp.GetName(), FBXMatrix4x4( fbxProp.Get<FbxDouble4x4>() ) );
		}

		return true;
	}

	bool FBXImporter::importSkeleton( FBXModelPtr model, FbxSkeleton* fbxSkeleton )
	{
		return true;
	}

	bool FBXImporter::importRigging( FBXModelPtr model, FbxScene* scene )
	{
		for( int i = 0; i < scene->GetPoseCount(); ++i )
		{
			FbxPose* pose = scene->GetPose( i );
			printf( "Get pose name = %s, is rest pose = %s\n", pose->GetName(), pose->IsRestPose() ? "Yes" : "No" );
		}

		for( int i = 0; i < scene->GetCharacterCount(); ++i )
		{
			FbxCharacter* character = scene->GetCharacter( i );
			printf( "Get character name = %s\n", character->GetName() );
		}

		for( int i = 0; i < scene->GetCharacterPoseCount(); ++i )
		{
			FbxCharacterPose* pose = scene->GetCharacterPose( i );
			{
				printf( "Get character pose name = %s\n", pose->GetName() );
			}
		}

		return true;
	}
}
