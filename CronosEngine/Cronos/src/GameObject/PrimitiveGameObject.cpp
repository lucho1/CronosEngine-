#include "Providers/cnpch.h"
#include "PrimitiveGameObject.h"

#include "Application.h"

#include "Modules/Scene.h"
#include "Modules/Filesystem.h"

#include "GameObject/Components/TransformComponent.h"

#include "par_shapes/par_shapes.h"

namespace Cronos {

	static par_shapes_mesh* ParshapeMesh;

	// --------------------------------- PRIMITIVE MODEL ---------------------------------
	PrimitiveGameObject::PrimitiveGameObject(PrimitiveType primitve_type, const std::string& name, glm::vec3 size, glm::vec3 position, float radius, int figure_slices, int figure_stacks)
		: GameObject(name, App->m_RandomNumGenerator.GetIntRN(), "", true, position, glm::vec3(0.0f), size), m_PrimitiveType(primitve_type)
	{
		if (primitve_type == PrimitiveType::DISK || primitve_type == PrimitiveType::ROCK || primitve_type == PrimitiveType::SUBDIVIDED_SPHERE)
		{
			CRONOS_ASSERT(false, "Complex Primitives are not created thorugh this constructor!!");
			return;
		}

		m_IsPrimitive = true;

		switch (primitve_type)
		{
			default:
				CRONOS_ASSERT(false, "Invalid Primitive Type!!");
				return;
			case PrimitiveType::NONE:
				CRONOS_ASSERT(false, "Invalid Primitive Type!!");
				return;
			case PrimitiveType::EMPTY:
				ParshapeMesh = par_shapes_create_empty();
				LOG("Created Empty Primitive")
				return;
			//Very Simple primitives (for par shapes)
			case PrimitiveType::CUBE:
				ParshapeMesh = par_shapes_create_cube();
				break;
			case PrimitiveType::TETRAHEDRON:
				ParshapeMesh = par_shapes_create_tetrahedron();
				break;
			case PrimitiveType::OCTAHEDRON:
				ParshapeMesh = par_shapes_create_octahedron();
				break;
			case PrimitiveType::DODECAHEDRON:
				ParshapeMesh = par_shapes_create_dodecahedron();
				break;
			case PrimitiveType::ICOSAHEDRON:
				ParshapeMesh = par_shapes_create_icosahedron();
				break;
			//Simple primitives for par shapes (slices, stacks)
			case PrimitiveType::EMPTY_CYLINDER:
				ParshapeMesh = par_shapes_create_cylinder(figure_slices, figure_stacks);
				break;
			case PrimitiveType::CLOSED_CYLINDER:
				CreateCylinder(size, position, figure_slices, figure_stacks);
				LOG("Created Primitive of Radius %i and Size (%i, %i, %i) with %i Slices and %i Stacks", radius, size.x, size.y, size.z, figure_slices, figure_stacks);
				return;
			case PrimitiveType::EMPTY_CONE:
				ParshapeMesh = par_shapes_create_cone(figure_slices, figure_stacks);
				break;
			case PrimitiveType::CLOSED_CONE:
				CreateCone(size, position, figure_slices, figure_stacks);
				LOG("Created Primitive of Radius %i and Size (%i, %i, %i) with %i Slices and %i Stacks", radius, size.x, size.y, size.z, figure_slices, figure_stacks);
				return;
			case PrimitiveType::SPHERE:
				ParshapeMesh = par_shapes_create_parametric_sphere(figure_slices, figure_stacks);
				break;
			case PrimitiveType::SEMI_SPHERE:
				ParshapeMesh = par_shapes_create_hemisphere(figure_slices, figure_stacks);
				break;
			case PrimitiveType::PLANE:
				ParshapeMesh = par_shapes_create_plane(figure_slices, figure_stacks);
				break;
			case PrimitiveType::KLEIN_BOTTLE:
				ParshapeMesh = par_shapes_create_klein_bottle(figure_slices, figure_stacks);
				break;
			case  PrimitiveType::TREFOIL_KNOT:
				if (radius >= 0.5f && radius <= 3.0f) {
					ParshapeMesh = par_shapes_create_trefoil_knot(figure_slices, figure_stacks, radius);
					break;
				}
				else
					LOG("Couldn't Create Trefoil Knot! Radius Must be between 0.5 and 3.0!!"); return;
			case PrimitiveType::TORUS:
				if (radius >= 0.1f && radius <= 1.0f) {
					ParshapeMesh = par_shapes_create_torus(figure_slices, figure_stacks, radius);
					break;
				}
				else
					LOG("Couldn't Create Torus! Radius Must be between 0.1 and 1.0!!"); return;
		}


		ParShapeToPrimitive(size, position);
		LOG("Created Primitive of Radius %.2f and Size (%.2f, %.2f, %.2f) with %d Slices and %d Stacks", radius, size.x, size.y, size.z, figure_slices, figure_stacks);
	}

	//Translation from ParShape to Cronos Primitive
	void PrimitiveGameObject::ParShapeToPrimitive(glm::vec3 size, glm::vec3 position)
	{
		par_shapes_translate(ParshapeMesh, position.x, position.y, position.z);
		par_shapes_scale(ParshapeMesh, size.x, size.y, size.z);

		//Set up the physical mesh
		std::vector<CronosVertex>tmpVertexVector;

		uint j = 0;
		for (uint i = 0; i < ParshapeMesh->npoints; i++)
		{
			CronosVertex tmpVertex;

			float x = ParshapeMesh->points[j];
			float y = ParshapeMesh->points[j + 1];
			float z = ParshapeMesh->points[j + 2];

			tmpVertex.Position = glm::vec3(x, y, z);

			if (ParshapeMesh->normals != nullptr) {

				x = ParshapeMesh->normals[j];
				y = ParshapeMesh->normals[j + 1];
				z = ParshapeMesh->normals[j + 2];

				tmpVertex.Normal = glm::vec3(x, y, z);
			}

			if (ParshapeMesh->tcoords != nullptr && j <= ParshapeMesh->npoints * 2)
			{
				x = ParshapeMesh->tcoords[j];
				y = ParshapeMesh->tcoords[j + 1];
				tmpVertex.TexCoords = glm::vec2(x, y);
			}
			else
				tmpVertex.TexCoords = glm::vec2(0, 0);

			tmpVertexVector.push_back(tmpVertex);
			j += 3;
		}

		std::vector<uint> tmpIndicesVector;
		for (uint i = 0; i < (ParshapeMesh->ntriangles * 3); i++)
			tmpIndicesVector.push_back(ParshapeMesh->triangles[i]);

		//Now set up the mesh component for the game object
		MeshComponent* meshComp = ((MeshComponent*)(CreateComponent(ComponentType::MESH)));
		meshComp->r_mesh = new ResourceMesh(App->m_RandomNumGenerator.GetIntRN());
		ResourceMesh* rMesh = meshComp->r_mesh;

		rMesh->m_BufferSize[0] = ParshapeMesh->npoints;
		rMesh->m_BufferSize[1] = ParshapeMesh->npoints;
		rMesh->m_BufferSize[2] = ParshapeMesh->npoints;
		rMesh->m_BufferSize[3] = ParshapeMesh->ntriangles;

		rMesh->Position = new float[rMesh->m_BufferSize[0] * 3];
		memcpy(rMesh->Position, ParshapeMesh->points, sizeof(float)*rMesh->m_BufferSize[0] * 3);

		int tes = sizeof(ParshapeMesh->triangles);
		int tes2 = sizeof(uint);

		uint16_t* test = new uint16_t(rMesh->m_BufferSize[3]);

		rMesh->Index = new uint[rMesh->m_BufferSize[3]*3];
		//for(int i =0;i<)

		for (uint i = 0; i < (rMesh->m_BufferSize[3] * 3); i++)
			rMesh->Index[i] = ParshapeMesh->triangles[i];

	
		rMesh->Normal = new float[rMesh->m_BufferSize[1] * 3];
		if (ParshapeMesh->normals != nullptr)
			memcpy(rMesh->Normal, ParshapeMesh->normals, sizeof(float)*rMesh->m_BufferSize[1] * 3);
		else			
			std::fill_n(rMesh->Normal,rMesh->m_BufferSize[1] * 3, 0);		

		rMesh->TextureV = new float[rMesh->m_BufferSize[2] * 2];
		if(ParshapeMesh->tcoords != nullptr)
			memcpy(rMesh->TextureV, ParshapeMesh->tcoords, sizeof(float)*rMesh->m_BufferSize[0] * 2);
		else
			std::fill_n(rMesh->TextureV, rMesh->m_BufferSize[2] * 2, 0);

		rMesh->toCronosVertexVector();
		meshComp->SetupMesh(rMesh->getVector(), rMesh->getIndex());
		m_Components.push_back(meshComp);

		//Also, set the material component (setted to default material)
		MaterialComponent* matComp = (MaterialComponent*)(CreateComponent(ComponentType::MATERIAL));
		m_Components.push_back(matComp);
		
		//Set the GO AABB and finally push it to the mother's child list		
		rMesh->Position = new float[rMesh->m_BufferSize[0] * 3];
		
		float arrsize = rMesh->getVector().size();
		float3* verts = new float3[arrsize];
		for (uint i = 0; i < arrsize; i++)
		{
			glm::vec3 vec = rMesh->getVector()[i].Position;
			verts[i] = float3(vec.x, vec.y, vec.z);
		}
		
		math::AABB aabb;
		math::OBB oobb;

		math::float4x4 mat = math::float4x4::identity;
		mat.Set(glm::value_ptr(GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()));

		aabb.SetNegativeInfinity();
		aabb.SetFrom(verts, arrsize);
		oobb.SetFrom(aabb);
		oobb.Transform(mat);

		SetInitialAABB(aabb);
		SetOOBB(oobb);
		SetAABB(aabb);
		delete[] verts;
		
		App->filesystem->SaveOwnFormat(this);		
		LOG("Processed Primitive Mesh with %i Vertices %i Indices and %i Polygons (Triangles)", tmpVertexVector.size(), tmpIndicesVector.size(), j/3);
	}


	//---------------------------------------- Special Primitives Creation ----------------------------------------
	void PrimitiveGameObject::CreateCylinder(glm::vec3 size, glm::vec3 position, int figure_slices, int figure_stacks)
	{
		//Check if the size forms a circle (to directly do a rounded cylinder) or not (to do a rounded cylinder and then scale it)
		if (size.x == size.y &&  size.x == size.z)
		{
			//First, create a normal cylinder and put it at (0,0,0)
			par_shapes_mesh* Cyl_PrShM = par_shapes_create_cylinder(figure_slices, figure_stacks);
			par_shapes_translate(Cyl_PrShM, 0, 0, 0);
			par_shapes_scale(Cyl_PrShM, size.x, size.y, size.z);

			//Now create 2 disks around the cylinder (since x, y and z are the same, we can just pick x)
			float normal[3] = { 0, 0, 1 };
			float center_axis[3] = { 0, 0, size.z };
			float center_axis2[3] = { 0, 0, 1 };
			par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(size.x, figure_slices, center_axis, normal);
			par_shapes_mesh* Disk2_PrShM = par_shapes_create_disk(size.x, figure_slices, center_axis2, normal);

			//Rotate one of the disks (to make it see outside the cylinder) -- A translation is needed (I don't know why, ParShapes stuff \_O_/, guess it has to do wit Rot. Axis)
			float RotAxis[3] = { 1, 0, 0 };
			par_shapes_rotate(Disk2_PrShM, PI, RotAxis);
			par_shapes_translate(Disk2_PrShM, 0, 0, 1);

			//Finally, set the class' mesh to an Empty ParShape, merge to it the 3 meshes
			ParshapeMesh = par_shapes_create_empty();
			par_shapes_merge_and_free(ParshapeMesh, Cyl_PrShM);
			par_shapes_merge_and_free(ParshapeMesh, Disk_PrShM);
			par_shapes_merge_and_free(ParshapeMesh, Disk2_PrShM);
		}
		else
		{
			CreateCylinder(glm::vec3(1, 1, 1), position, figure_slices, figure_slices);
			//ScaleModel(size);
			return;
		}

		//At the end, call the translation function
		ParShapeToPrimitive(size, position);
	}

	void PrimitiveGameObject::CreateCone(glm::vec3 size, glm::vec3 position, int figure_slices, int figure_stacks)
	{
		//Same method for cylinder but with one disk
		if (size.x == size.y &&  size.x == size.z)
		{
			//First, create a normal cone and put it at (0,0,0)
			par_shapes_mesh* Cone_PrShM = par_shapes_create_cone(figure_slices, figure_stacks);
			par_shapes_translate(Cone_PrShM, 0, 0, 0);
			par_shapes_scale(Cone_PrShM, size.x, size.y, size.z);

			//Now create a disk
			float normal[3] = { 0, 0, 1 };
			float center_axis[3] = { 0, 0, 1 };
			par_shapes_mesh* Disk_PrShM = par_shapes_create_disk(size.x, figure_slices, center_axis, normal);

			//Rotate the disk to make it see outside the cone -- A translation is needed (I don't know why, ParShapes stuff \_O_/, guess it has to do wit Rot. Axis)
			float RotAx[3] = { 1, 0, 0 };
			par_shapes_rotate(Disk_PrShM, PI, RotAx);
			par_shapes_translate(Disk_PrShM, 0, 0, 1);

			//Finally, set the class' mesh to an Empty ParShape, merge to it the 3 meshes
			ParshapeMesh = par_shapes_create_empty();
			par_shapes_merge_and_free(ParshapeMesh, Cone_PrShM);
			par_shapes_merge_and_free(ParshapeMesh, Disk_PrShM);
		}
		else
		{
			CreateCone(glm::vec3(1, 1, 1), position, figure_slices, figure_slices);
			//ScaleModel(size);
			return;
		}

		//At the end, call the translation function
		ParShapeToPrimitive(size, position);
	}

	void PrimitiveGameObject::CreateRock(glm::vec3 size, glm::vec3 position, int seed, uint nSubdivisions)
	{
		if (m_PrimitiveType == PrimitiveType::EMPTY)
		{
			par_shapes_free_mesh(ParshapeMesh);
			LOG("Par Shape Empty mesh successfully freed");

			ParshapeMesh = par_shapes_create_rock(seed, nSubdivisions);
			m_PrimitiveType = PrimitiveType::ROCK;
			ParShapeToPrimitive(size, position);
			LOG("Created Rock with seed %i and %i Subdivisions", seed, nSubdivisions);
		}
		else
			LOG("Couldn't create Rock! It must be created from an Empty primitive type");
	}

	void PrimitiveGameObject::CreateSubdividedSphere(glm::vec3 size, glm::vec3 position, uint nSubdivisions)
	{
		if (m_PrimitiveType == PrimitiveType::EMPTY)
		{
			par_shapes_free_mesh(ParshapeMesh);
			LOG("Par Shape Empty mesh successfully freed");

			ParshapeMesh = par_shapes_create_subdivided_sphere(nSubdivisions);
			m_PrimitiveType = PrimitiveType::SUBDIVIDED_SPHERE;
			ParShapeToPrimitive(size, position);
			LOG("Created Subdivided Sphere with %i Subdivisions", nSubdivisions);
		}
		else
			LOG("Couldn't create Subdivided Sphere! It must be created from an Empty primitive type");
	}

	void PrimitiveGameObject::CreateDisk(glm::vec3 center, glm::vec3 size, float radius, int figure_slices)
	{
		if (m_PrimitiveType == PrimitiveType::EMPTY)
		{
			par_shapes_free_mesh(ParshapeMesh);
			LOG("Par Shape Empty mesh successfully freed");

			float center_arr[3] = { center.x, center.y, center.z };
			float normal[3] = { 0, 0, 1 };

			ParshapeMesh = par_shapes_create_disk(radius, figure_slices, center_arr, normal);

			m_PrimitiveType = PrimitiveType::DISK;
			ParShapeToPrimitive(size, center);
		}
		else
			LOG("Couldn't create Disk! It must be created from an Empty primitive type");
	}
}
