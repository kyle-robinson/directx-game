#pragma once
#ifndef MODEL_H
#define MODEL_H

class Camera;
#include "Mesh.h"

/// <summary>
/// Load models from file using Assimp library.
/// Processing of individual meshes and textures is deferred to the appropriate classes.
/// </summary>
class Model
{
public:
	bool Initialize(
		const std::string& filePath,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		ConstantBuffer<CB_VS_matrix>& cb_vs_matrix);
	void Draw( const XMMATRIX& worldMatrix );
	static void BindMatrices( ID3D11DeviceContext* context, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix,
		const std::unique_ptr<Camera>& camera ) noexcept;
private:
	bool LoadModel( const std::string& filePath );
	void ProcessNode( aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix );
	Mesh ProcessMesh( aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix );
	TextureStorageType GetTextureStorageType( const aiScene* pScene, aiMaterial* pMaterial, uint32_t index, aiTextureType textureType );
	std::vector<Texture> LoadMaterialTextures( aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene );
	int GetTextureIndex( aiString* pStr );
private:
	std::string directory = "";
	std::vector<Mesh> meshes;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ConstantBuffer<CB_VS_matrix>* cb_vs_matrix = nullptr;
};

#endif