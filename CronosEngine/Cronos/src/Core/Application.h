#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Module.h"

#include "Providers/SystemInfo.h"

#include "Providers/Globals.h" //TODO: DEL

#include "Modules/SDLWindow.h"
#include "Modules/Input.h"
#include "Modules/Audio.h"
#include "Modules/Scene.h"
#include "Modules/EngineCamera.h"
#include "Modules/Filesystem.h"
#include "Modules/TextureManager.h"

#include "Renderer/GLRenderer3D.h"
#include "ImGui/ImGuiLayer.h"

#include "Timers/Timer.h"

#include "json/json.hpp"
using json = nlohmann::json;


namespace Cronos {

	class Application {

	public: //Modules

		SDLWindow* window;
		Input* input;
		Audio* audio;
		Scene* scene;
		GLRenderer3D* renderer3D;
		EngineCamera* engineCamera;
		ImGuiLayer* EditorGUI;
		Filesystem* filesystem;
		TextureManager* textureManager;



		RNGen m_RandomNumGenerator; //Random number generator

	private: //Time stuff

		std::vector<Module*> m_ModulesList;

		//Timestep stuff
		float	m_Timestep = 0.0f;
		Timer	mt_LastFrameTime;

		//Framerate Debug Info.
		Timer	mt_StartingTime; //Time since starting NEEDS TO BE STARTED!!!!!!
		Timer	mt_LastSecFrameTime; //Timer to check the frames in a second (at each second) NEEDS TO BE STARTED!!!!!
		uint	m_FrameCount = 0; //Frames since starting
		uint32	m_LastSecFrameCount = 0; //Frames in last second
		uint32	m_PREV_LastSecFrameCount = 0; //Stored frames of the previous-to-last second's frames
		float	m_AverageFPS = 0.0f;

		//FPS CAP
		int m_CappedMS = -1;
		int m_FPSCap = -1;

	public: //Serialization & Config

		mutable json m_JSONConfigFile;
		mutable bool m_MustLoad = false;
		mutable bool m_MustSave = false;
		mutable Timer mt_SaveTimer;
		float SaveTime;

		std::string m_DefaultConfigurationFilepath;
		std::string m_AppName;
		std::string m_AppVersion;
		std::string m_AppOrganization;
		std::string m_AppAuthors;

	private:

		void AddModule(Module* mod);
		void PrepareUpdate();
		void FinishUpdate();

		//SERIALIZATION & CONFIG READING
		void LoadJsonFile(const char* filePath);
		void SaveJsonFile(const char* filePath) const;
		
	public:

		Application(int FPSCap = -1);
		~Application();

		bool			OnInit();
		update_status	OnUpdate();
		bool			OnCleanUp();

	public:
		
		const void		RequestBrowser(const char* WebDirection);
		void			SetFPSCap(int FPScap);
		void			SetAppTitle(const std::string& name);
		void			SetAppVersion(const std::string& version);
		void			SetAppOrganization(const std::string& org);
		void			SetAppAuthors(const std::string& authors);

		//SERIALIZATION & CONFIG READING
		void SaveEngineData() const;
		void LoadEngineData() const;

	public:

		//'Getters'
		inline const float GetDeltaTime()				const		{	return	m_Timestep;					}
		inline const float GetLastFrameMS()				const		{	return	(m_Timestep * 1000.0f);		}
		inline const uint32 GetFramesInLastSecond()		const		{	return	m_PREV_LastSecFrameCount;	}
		inline const float GetAverageFPS()				const		{	return	m_AverageFPS;				}

		inline const int GetFPSCap()					const		{ return m_FPSCap; }

		inline const std::string GetAppTitle()			const		{ return m_AppName; }
		inline const std::string GetAppVersion()		const		{ return m_AppVersion; }
		inline const std::string GetAppOrganization()	const		{ return m_AppOrganization; }
		inline const std::string GetAppAuthors()		const		{ return m_AppAuthors; }

	};

	extern Application* App;
}

#endif
