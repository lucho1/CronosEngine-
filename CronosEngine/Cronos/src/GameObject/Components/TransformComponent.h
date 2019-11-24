#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"
#include "MaterialComponent.h"

#include "GameObject/PrimitiveGameObject.h"

#include "Helpers/AABB.h"
#include "Helpers/OBB.h"

namespace Cronos {

	class GameObject;

	/*class BoundingBox
	{
	public:

		BoundingBox() {}
		BoundingBox(GameObject* GameObjectAttached);
		void Update(float dt);

		void CleanUp()
		{
			RELEASE(AABBox);
			RELEASE(OBBox);
		}

		bool IsNull()
		{
			if (this == nullptr)
				return true;
			if(AABBox == nullptr || OBBox == nullptr)
				return true;
			return false;
		}

	private:

		PrimitiveGameObject* AABBox = nullptr;
		PrimitiveGameObject* OBBox = nullptr;

		GameObject* GOAttached = nullptr;
	};*/

	class TransformComponent : public Component
	{
	public:

		TransformComponent(GameObject* attachedGO, bool active = true);
		~TransformComponent();

		virtual void Update(float dt) override;

		//Type
		static ComponentType GetType() { return ComponentType::TRANSFORM; };

		//AABBs & Axis
		void SetAABB(const glm::vec3& minVec, const glm::vec3& maxVec) { m_ContainerAABBCube = AABB(minVec, maxVec); }
		void SetAABB(const AABB& aabbCube) { m_ContainerAABBCube = AABB(aabbCube); }
		
		bool &SetDrawAxis() { return DrawAxis; }
		void DrawCentralAxis();

		const inline AABB GetAABB() const { return m_ContainerAABBCube; }
		const inline glm::vec3 GetCentralAxis() const { return m_ContainerAABBCube.getCenter(); }

	public:

		//This is to SET the game object to a desired transformation
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void SetOrientation(glm::vec3 euler_angles);
		
		//This is to ADD the game object a transformation
		void Move(glm::vec3 translation);
		void Scale(glm::vec3 scale);
		void Rotate(glm::vec3 euler_angles);

		//Getters
		const inline glm::vec3 GetTranslation() const { return m_Translation; };
		const inline glm::vec3 GetScale() const { return m_Scale; };
		const inline glm::vec3 GetOrientation() const { return glm::degrees(m_EulerAngles); };

		const inline glm::mat4 GetLocalTranformationMatrix() const { return m_LocalTransformationMatrix; }
		const inline glm::mat4 GetGlobalTranformationMatrix() const { return m_GlobalTransformationMatrix; }

	private:

		//Decompose the matrix in the position, scale vectors and orientation quaternion
		void UpdateTransform();

	public:

		bool DrawAxis = false;

	private:

		glm::mat4 m_LocalTransformationMatrix;
		glm::mat4 m_GlobalTransformationMatrix;

		glm::vec3 m_Translation;
		glm::vec3 m_Scale;

		glm::quat m_Orientation;
		glm::vec3 m_EulerAngles;

		//AABB Cube
		AABB m_ContainerAABBCube;
		glm::vec3 AABBPos = glm::vec3(0.0f);
		glm::vec3 AABBScale = glm::vec3(1.0f);
		glm::quat AABBOrientation = glm::quat(1.0f, glm::vec3(0.0f));

		OBB m_ContainerOOBB;
		//BoundingBox* TrueAABB;
	};

}

#endif // !_COMPONENTTRANSFORM_H_
