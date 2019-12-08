#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Helpers/Camera.h"

#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

namespace Cronos {

	class EngineCamera : public Module, public Camera
	{
	public:

		//Module Methods
		EngineCamera(Application* app, bool start_enabled = true);
		~EngineCamera();

		virtual bool			OnStart() override;
		virtual update_status	OnUpdate(float dt) override;
		virtual update_status	OnPostUpdate(float dt) override;
		virtual bool			OnCleanUp() override;

		//Save/Load
		void SaveModuleData(json& JSONFile) const;
		void LoadModuleData(json& JSONFile);

	public:

		void ChangeProjection() { m_ChangeProj = true; }
		
	private:

		//Camera Methods
		glm::vec3 MouseRotation(const glm::vec3& pos, const glm::vec3& ref);

	private:

		//Camera Common data
		bool m_ChangeProj = false;
	};
}

#endif