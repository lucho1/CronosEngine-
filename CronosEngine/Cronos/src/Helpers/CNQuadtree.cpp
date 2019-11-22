#include "Providers/cnpch.h"
#include "CNQuadtree.h"

#include "Renderer/GLRenderer3D.h"
#include "Application.h"
#include "GameObject/Components/TransformComponent.h"

namespace Cronos {

	//Quadtree -----------------------------------------------------------------
	CnQuadtree::CnQuadtree(AABB space, int capacity)
	{
		m_Root = new CnQT_Node(space, NodeType::ROOT, capacity);
	}

	CnQuadtree::~CnQuadtree()
	{
	}

	void CnQuadtree::Draw()
	{
		m_Root->Draw();
	}

	void CnQuadtree::CleanUp()
	{
		m_Root->CleanUp();
	}

	bool CnQuadtree::Insert(GameObject* GObject)
	{
		return m_Root->Insert(GObject);
	}

	std::vector<GameObject*> CnQuadtree::GetObjectsContained(AABB cubicSpace)
	{
		return m_Root->GetObjectsContained(cubicSpace);
	}


	//Quadtree Node ------------------------------------------------------------
	CnQT_Node::CnQT_Node(AABB partitionSpace, NodeType nodeType, int capacity)
		: m_CubicSpace(partitionSpace), m_NodeType(nodeType), m_Capacity(capacity)
	{
	}

	CnQT_Node::~CnQT_Node()
	{
	}

	void CnQT_Node::CleanUp()
	{
		for (uint i = 0; i < m_ChildsQuantity; i++)
			m_Nodes[i].CleanUp();

		if (m_Nodes != nullptr)
			delete[] m_Nodes;
	}

	void CnQT_Node::Split()
	{
		m_Leaf = false;

		AABB children[4];

		glm::vec3 max, min;
		glm::vec3 c_Min = m_CubicSpace.getMin();
		glm::vec3 c_Max = m_CubicSpace.getMax();

		max = glm::vec3(c_Max.x, c_Max.y, c_Max.z);
		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Min.y, (c_Max.z + c_Min.z)/2.0f);

		//Set new node with those max and min
		children[0] = AABB(min, max);

		max = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Max.y, c_Max.z);
		min = glm::vec3(c_Min.x, c_Min.y, (c_Max.z + c_Min.z) / 2.0f);

		//Set new node with those max and min
		children[1] = AABB(min, max);

		max = glm::vec3(c_Max.x, c_Max.y, (c_Max.z + c_Min.z) / 2.0f);
		min = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Min.y, c_Min.z);

		//Set new node with those max and min
		children[2] = AABB(min, max);

		max = glm::vec3((c_Max.x + c_Min.x) / 2.0f, c_Max.y, (c_Max.z + c_Min.z) / 2.0f);
		min = glm::vec3(c_Min.x, c_Min.y, c_Min.z);

		//Set new node with those max and min
		children[3] = AABB(min, max);

		m_Nodes = new CnQT_Node[4];

		for (int i = 0; i < 4; ++i)
			m_Nodes[i] = CnQT_Node(children[i], NodeType::CHILD, m_Capacity);

		if (m_NodeType!= NodeType::ROOT)
			m_NodeType= NodeType::PARENT;

		m_ChildsQuantity = 4;
	}

	void CnQT_Node::Draw()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(0.5f);

		glColor3f(Red.r, Red.g, Red.b);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));
		App->renderer3D->DrawCube(m_CubicSpace.getMax(), m_CubicSpace.getMin());

		if (!m_Leaf)
			for (int i = 0; i < m_ChildsQuantity; ++i)
				m_Nodes[i].Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	bool CnQT_Node::Insert(GameObject* GObj)
	{
		AABB GOAABB = GObj->GetComponent<TransformComponent>()->GetAABB();
		if (m_CubicSpace.intersect(GOAABB) == AABB::OUTSIDE)
			return false;

		int nodesContained = 0, container = 0;

		if (m_NodeType == NodeType::PARENT || (m_NodeType == NodeType::ROOT && m_ChildsQuantity > 0))
		{
			for (uint i = 0; i < m_ChildsQuantity; i++)
			{
				if (m_Nodes[i].m_CubicSpace.intersect(GOAABB) != AABB::OUTSIDE)
				{
					nodesContained++;
					container = i;
				}
				if (nodesContained > 1)
					break;
			}

			if (nodesContained == 1)
				m_Nodes[container].Insert(GObj);
			else if (nodesContained > 1)
				GObjectsContained_Vector.push_back(GObj);

			return true;
		}
		else if (m_NodeType == NodeType::CHILD || (m_NodeType == NodeType::ROOT && m_ChildsQuantity == 0))
		{
			GObjectsContained_Vector.push_back(GObj);
			if (GObjectsContained_Vector.size() > m_Capacity)
			{
				Split();
				std::vector<GameObject*> newObjects = GObjectsContained_Vector;
				GObjectsContained_Vector.clear();

				for (uint j = 0; j < newObjects.size(); j++)
				{
					nodesContained = 0;
					container = 0;
					for (uint i = 0; i < m_ChildsQuantity; i++)
					{
						if (m_Nodes[i].m_CubicSpace.intersect(newObjects[j]->GetComponent<TransformComponent>()->GetAABB()) != AABB::OUTSIDE)
						{
							nodesContained++;
							container = i;
						}
						if(nodesContained > 1)
							break;
					}

					if (nodesContained == 1)
						m_Nodes[container].Insert(newObjects[j]);
					else if (nodesContained > 1)
						GObjectsContained_Vector.push_back(newObjects[j]);
				}
			}

			return true;
		}

		return false;
	}

	std::vector<GameObject*> CnQT_Node::GetObjectsContained(AABB cubicSpace)
	{
		std::vector<GameObject*> objectsInside;
		if (m_CubicSpace.intersect(cubicSpace) == AABB::OUTSIDE)
		{
			LOG("No Objects intersecting this cube! Return value empty");
			return objectsInside;
		}

		std::vector<GameObject*>::iterator it = GObjectsContained_Vector.begin();
		for (; it != GObjectsContained_Vector.end(); it++)
			objectsInside.push_back(*it);

		if (m_ChildsQuantity > 0)
		{
			std::vector<GameObject*> childrenObjects;
			for (uint i = 0; i < m_ChildsQuantity; i++)
			{
				std::vector<GameObject*>nodeObjs = m_Nodes[i].GetObjectsContained(cubicSpace);

				if(nodeObjs.size() > 0)
					childrenObjects.insert(childrenObjects.begin(), nodeObjs.begin(), nodeObjs.end());
			}

			if(childrenObjects.size() > 0)
				objectsInside.insert(objectsInside.begin(), childrenObjects.begin(), childrenObjects.end());
		}

		return objectsInside;
	}
}