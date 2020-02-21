#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include "Component.h"
#include "Renderer/Shaders.h"
#include "MaterialComponent.h"

namespace Cronos
{
	enum class LightType { NONE = -1, DIRECTIONAL, POINTLIGHT, SPOTLIGHT };

	struct DirectionalLight
	{
		glm::vec3 m_LightDirection = glm::vec3(0.0f);
		glm::vec3 m_LightColor = glm::vec3(0.0f);

		float m_LightIntensity = 1.0f;
	};

	struct PointLight
	{
		glm::vec3 m_LightPosition = glm::vec3(0.0f);
		glm::vec3 m_LightColor = glm::vec3(1.0f);

		float m_LightIntensity = 1.0f;

		float m_LightAttK = 1.0f;
		float m_LightAttL = 0.0f;
		float m_LightAttQ = 0.0f;
	};

	struct SpotLight
	{
		glm::vec3 m_LightPosition = glm::vec3(0.0f);
		glm::vec3 m_LightColor = glm::vec3(1.0f);
		glm::vec3 m_LightDirection = glm::vec3(0.0f);

		float m_LightIntensity = 1.0f;

		float m_LightAttK = 1.0f;
		float m_LightAttL = 0.0f;
		float m_LightAttQ = 0.0f;

		float m_InnerCutoffAngle = 12.5f; //Cosinus value
		float m_OuterCutoffAngle = 45.0f; //Cosinus value
	};


	class LightComponent : public Component
	{
	public:

		LightComponent(GameObject* attachedGO);
		~LightComponent();
		
		static ComponentType GetType() { return ComponentType::LIGHT; }
		
	public:

		//Setters
		void SetLightType(LightType type);
		void SetLightDirection(const glm::vec3& direction) { m_SLightComp.m_LightDirection = m_DLightComp.m_LightDirection = direction; }
		void SetLightColor(const glm::vec3& color);
		void SetLightIntensity(const float& intensity);
		void SetSpotlightInnerCutoff(const float& degreesAngle) { m_SLightComp.m_InnerCutoffAngle = degreesAngle; }
		void SetSpotlightOuterCutoff(const float& degreesAngle) { m_SLightComp.m_OuterCutoffAngle = degreesAngle; }

		void SetAttenuationFactors(const glm::vec3& attenuationFactorsKLQ)
		{
			m_PLightComp.m_LightAttK = m_SLightComp.m_LightAttK = attenuationFactorsKLQ.x;
			m_PLightComp.m_LightAttL = m_SLightComp.m_LightAttL = attenuationFactorsKLQ.y;
			m_PLightComp.m_LightAttQ = m_SLightComp.m_LightAttQ = attenuationFactorsKLQ.z;
		}

		//Getters
		inline const LightType GetLightType()				const { return m_LightType; }

		inline const glm::vec3 GetLightColor()				const { return (m_LightType == LightType::POINTLIGHT ? m_PLightComp.m_LightColor :
																			(m_LightType == LightType::SPOTLIGHT ? m_SLightComp.m_LightColor : m_DLightComp.m_LightColor)); }

		inline const float GetLightIntensity()				const { return (m_LightType == LightType::POINTLIGHT ? m_PLightComp.m_LightIntensity :
																			(m_LightType == LightType::SPOTLIGHT ? m_SLightComp.m_LightIntensity : m_DLightComp.m_LightIntensity)); }

		inline const glm::vec3 GetLightDirection()			const { return (m_LightType == LightType::POINTLIGHT ? glm::vec3(0.0f) :
																			(m_LightType == LightType::SPOTLIGHT ? m_SLightComp.m_LightDirection : m_DLightComp.m_LightDirection)); }

		inline const glm::vec3 GetLightAttenuationFactors()	const { return (m_LightType == LightType::POINTLIGHT ? glm::vec3(m_PLightComp.m_LightAttK, m_PLightComp.m_LightAttL, m_PLightComp.m_LightAttQ) :
																			(m_LightType == LightType::SPOTLIGHT ? glm::vec3(m_SLightComp.m_LightAttK, m_SLightComp.m_LightAttL, m_SLightComp.m_LightAttQ) : glm::vec3(0.0f))); }

		inline const float GetSpotlightInnerCutoff()		const { return m_SLightComp.m_InnerCutoffAngle; }
		inline const float GetSpotlightOuterCutoff()		const { return m_SLightComp.m_OuterCutoffAngle; }

	public:

		void SendUniformsLightData(Shader* shader, uint lightIndex);

		DirectionalLight m_DLightComp;
		PointLight m_PLightComp;
		SpotLight m_SLightComp;

	private:

		//void SetLightToZero(Shader* shader, uint lightIndex, LightType lType);

	private:

		LightType m_LightType = LightType::NONE;

		Material* m_LightMaterial = nullptr;

			

		bool m_ChangeLightType = false;
	};
}

#endif