#include "Providers/cnpch.h"
#include "mmgr/mmgr.h"

#include "Application.h"
#include "Scene.h"

#include "Renderer/Buffers.h"

namespace Cronos {

	Scene::Scene(Application* app, bool start_enabled) : Module(app, "Module Scene", start_enabled)
	{
	}

	Scene::~Scene()
	{}

	// Load assets
	bool Scene::OnStart()
	{
		LOG("Loading Intro assets");
		bool ret = true;

		m_FloorPlane = Plane(0.0f, 1.0f, 0.0f, 0.0f); //Express the normal (0 centered)
		m_FloorPlane.axis = true; //Enable axis render

		App->engineCamera->Move(vec3(1.0f, 1.0f, 0.0f)); //Camera begins one unit up in Y and one unit to the right
		App->engineCamera->LookAt(vec3(0.0f, 0.0f, 0.0f)); //To look at center


		//---------------- TEST ----------------//
		float cbeVertices[3 * 8] = {
			-2.0f,	-2.0f,	-2.0f, //1
			 2.0f,	-2.0f,	-2.0f, //2
			 2.0f,	 2.0f,	-2.0f, //3
			-2.0f,	 2.0f,	-2.0f, //4
			-2.0f,	 2.0f,	 2.0f, //5
			 2.0f,	 2.0f,	 2.0f, //6
			 2.0f,	-2.0f,	 2.0f, //7
			-2.0f,	-2.0f,	 2.0f  //8
		};	// x	  y		  z
		uint cbeIndices[6 * 6] = {
			0, 1, 2, 2, 3, 0, //F1
			0, 1, 6, 6, 7, 0, //F2
			6, 5, 4, 4, 7, 6, //F3
			6, 5, 2, 2, 1, 6, //F4
			2, 5, 4, 4, 3, 2, //F5
			4, 3, 0, 0, 7, 4  //F6
		};

		//uint va;
		//glCreateVertexArrays(1, &va);
		//glBindVertexArray(va);
		//
		//uint vb;
		//glCreateBuffers(1, &vb);
		//glBindBuffer(GL_ARRAY_BUFFER, vb);
		//glBufferData(GL_ARRAY_BUFFER, 3*8* sizeof(float), cbeVertices, GL_STATIC_DRAW);
		//
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		//
		//uint ib;
		//glCreateBuffers(1, &ib);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 6 * sizeof(uint), cbeIndices, GL_STATIC_DRAW);
		//
		//glBindVertexArray(va);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

		VAO = new VertexArray();

		VertexBuffer* VBO = new VertexBuffer(cbeVertices, sizeof(cbeVertices));
		VBO->SetLayout({ {Cronos::VertexDataType::VEC3F, "a_Position"} });
		VAO->AddVertexBuffer(*VBO);
		//
		IndexBuffer* IBO = new IndexBuffer(cbeIndices, sizeof(cbeIndices));
		VAO->AddIndexBuffer(*IBO);

		//glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		VAO->~VertexArray();
		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{


		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// "Floor" Plane
		m_FloorPlane.Render();
		//glDrawArrays(GL_TRIANGLES, 0, 8);
		VAO->Bind();
		//Cube cbe = Cube(5, 5, 5);
		//cbe.Render();

		//glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr);
		//REMEMBER THAT CULL FACE IS ACTIVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr/* VAO->GetIndexBuffer()*/);

		return UPDATE_CONTINUE;
	}

	// PreUpdate
	update_status Scene::OnPreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	// UPostUpdate
	update_status Scene::OnPostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

}
