#ifndef _MODEL_H_
#define _MODEL_H_

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "VertexArray.h"
#include "GameObject/Components/MeshComponent.h"
#include "GameObject/GameObject.h"
#include "Renderer/Textures.h"
#include "Renderer/Shaders.h"

#include <Assimp/include/cimport.h>
#include <Assimp/include/cfileio.h>
#include <Assimp/include/Importer.hpp>
#include <Assimp/include/scene.h>
#include <Assimp/include/postprocess.h>


namespace Cronos {

	class CronosPrimitive;

	/*struct CronosVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};*/

	//class CronosMesh
	//{
	//public:
	//	
	//	CronosMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<Texture*>& textures);
	//	~CronosMesh();

	//	void Draw(Shader* shader, bool bindShader);
	//	void DrawVerticesNormals();
	//	void DrawPlanesNormals();

	//	void ScaleMesh(glm::vec3 ScaleMagnitude);
	//	void MoveMesh(glm::vec3 MoveAxis, float moveMagnitude);
	//	void RotateMesh(float RotDegrees, glm::vec3 RotAxis, glm::vec3 OwnAxis);

	//	const std::vector<Texture*>& GetTexturesVector() const { return m_TexturesVector; }
	//	const std::vector<CronosVertex> GetVertexVector() const { return m_VertexVector; }
	//	const std::vector<uint> GetIndexVector() const { return m_IndicesVector; }
	//	
	//	const glm::mat4 GetTransformation() const { return m_Transformation; }
	//	void SetTextures(std::vector<Texture*>& newTexture, TextureType textureType);

	//private:

	//	void SetupMesh();
	//	void DecomposeTransformation();

	//	std::vector<Texture*> m_TexturesVector;
	//	std::vector<CronosVertex> m_VertexVector;
	//	std::vector<uint> m_IndicesVector;

	//	VertexArray* m_MeshVAO = nullptr;
	//	VertexBuffer* m_MeshVBO = nullptr;
	//	IndexBuffer* m_MeshIBO = nullptr;

	//	glm::mat4 m_Transformation = glm::mat4(1.0f); // your transformation matrix.
	//	glm::vec3 m_Scale;
	//	glm::quat m_Rotation;
	//	glm::vec3 m_Translation;
	//};


	class CronosModel
	{
		//friend class AssimpCronosTranslator;
		friend class CronosPrimitive;

	public:
		
		CronosModel(const std::string& filepath);
		CronosModel(CronosPrimitive* primitive) {}
		~CronosModel();

		/*void Draw(Shader* shader, bool bindShader);
		void DrawVerticesNormals();
		void DrawPlanesNormals();
		void DrawModelAxis();

		void ScaleModel(glm::vec3 ScaleMagnitude);
		void MoveModel(glm::vec3 MoveAxis, float moveMagnitude);
		void RotateModel(float RotDegrees, glm::vec3 RotAxis);
		
		const glm::vec3 GetModelAxis() const { return m_ModelAxis; }
		const glm::mat4 GetTransformation() const { return m_Transformation; }*/

	private:
		
		//void CalculateModelAxis();

		//std::vector<CronosMesh*> m_ModelMeshesVector;
		std::string m_ModelDirectoryPath;
		glm::vec3 m_ModelAxis;

		glm::mat4 m_Transformation = glm::mat4(1.0f);
		glm::vec3 m_ModelMaxVertexPos;
		glm::vec3 m_ModelMinVertexPos;
	};


	class AssimpCronosTranslator
	{
	public:
	//	friend class CronosModel;

		AssimpCronosTranslator(/*CronosModel* Cr_Model*/);
		GameObject* LoadModel(const std::string& filepath);

		void ProcessAssimpNode(aiNode* as_node, const aiScene* as_scene, GameObject* motherGameObj);
		void ProcessCronosMesh(aiMesh* as_mesh, const aiScene* as_scene, GameObject* motherGameObj);

		std::vector<Texture*> LoadTextures(aiMaterial *material, aiTextureType Texturetype, TextureType texType, GameObject* motherGameObj);

	private:
		//GameObject* m_CronosModel = nullptr;
	};

}
#endif