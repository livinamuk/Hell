#include "hellpch.h"
#include "Physics.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "audio/Audio.h"

namespace HellEngine
{
	btDiscreteDynamicsWorld* Physics::s_dynamicsWorld;
	btAlignedObjectArray<btCollisionObject*> Physics::s_collisionObjects;
	btAlignedObjectArray<btRigidBody*> Physics::s_rigidBodies;
	GLDebugDrawer Physics::s_debugDraw;
	btDefaultCollisionConfiguration* Physics::s_collisionConfiguration;
	btCollisionDispatcher* Physics::s_dispatcher;
	btDbvtBroadphase* Physics::s_broadphase;
	btSequentialImpulseConstraintSolver* Physics::s_sol;
	btConstraintSolver* Physics::s_solver;
	btAlignedObjectArray<btCollisionShape*> Physics::s_collisionShapes;
	std::vector<glm::vec3> Physics::s_points;
	std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> Physics::s_objectsCollisions;
	CollisionPairs Physics::s_pairsLastUpdate;

	Ragdoll* Physics::m_ragdoll;

	void Physics::Init()
	{
		s_debugDraw.Init();
	}

	void Physics::RebuildWorld(House* house)
	{
		s_broadphase = new btDbvtBroadphase();
		s_collisionConfiguration = new btDefaultCollisionConfiguration();
		s_dispatcher = new btCollisionDispatcher(s_collisionConfiguration);
		s_solver = new btSequentialImpulseConstraintSolver;
		s_dynamicsWorld = new btDiscreteDynamicsWorld(s_dispatcher, s_broadphase, s_solver, s_collisionConfiguration);
		s_dynamicsWorld->setGravity(btVector3(0, -10, 0));
		s_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	
		CreateWorld();
		AddHouse(house);

		//m_ragdoll = new Ragdoll(s_dynamicsWorld, btVector3(0, 0, 0), 1.0f);
	}

	void Physics::DebugDraw(Shader* shader)
	{
		s_debugDraw.lines.clear();
		s_debugDraw.vertices.clear();
		Physics::s_dynamicsWorld->debugDrawWorld();

		for (int i = 0; i < s_debugDraw.lines.size() - 1; i++)
		{
			Vertex vert0, vert1;
			vert0.Position = s_debugDraw.lines[i].start_pos;
			vert0.Normal = s_debugDraw.lines[i].start_color;	// Yes. You are storing the color in the vertex normal spot.
			vert1.Position = s_debugDraw.lines[i].end_pos;
			vert1.Normal = s_debugDraw.lines[i].end_color;
			s_debugDraw.vertices.push_back(vert0);
			s_debugDraw.vertices.push_back(vert1);

		};

		glBindBuffer(GL_ARRAY_BUFFER, s_debugDraw.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_debugDraw.vertices.size(), &s_debugDraw.vertices[0], GL_STATIC_DRAW);

		glBindVertexArray(s_debugDraw.VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));


		shader->setVec3("color", s_debugDraw.lines[0].start_color);
		shader->setMat4("model", glm::mat4(1));
		glBindVertexArray(s_debugDraw.VAO);
		glDrawArrays(GL_LINES, 0, s_debugDraw.vertices.size());
	}

	void Physics::CreateWorld()
	{
		s_debugDraw.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		s_dynamicsWorld->setDebugDrawer(&s_debugDraw);
		s_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits);
	}

	void Physics::AddFloorsAndCeilingsToPhysicsWorld(House* house)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();
		for (Room& room : house->m_rooms)
		{
			float roomX = room.m_position.x;
			float roomZ = room.m_position.y;
			float roomWidth = room.m_size.x;
			float roomDepth = room.m_size.y;

			// Create the ground
			btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(roomWidth/2), btScalar(0.2), btScalar(roomDepth/2)));
			s_collisionShapes.push_back(groundShape);
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(roomX, -0.2, roomZ));
			btRigidBody* floor;

			btScalar mass(0.);
			btScalar friction = 0.5f;
			int group = CollisionGroups::HOUSE;
			int mask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
			floor = createRigidBody(mass, groundTransform, groundShape, friction, group, mask);
			floor->setCustomDebugColor(DEBUG_COLOR_GROUND);

			EntityData* entityData = new EntityData();
			entityData->name = "FLOOR";
			floor->setUserPointer(entityData);

		//	btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		//	collisionShape->setLocalScaling(btVector3(0.015, 0.15, 0.15));
		//	s_collisionShapes.push_back(collisionShape);
		




			/*// Floor
			btVector3 vertA, vertB, vertC, vertD;
			vertA = Util::glmVec3_to_btVec3(room.m_floor.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(room.m_floor.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(room.m_floor.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(room.m_floor.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);
			*/
			// Ceiling
		/*	vertA = Util::glmVec3_to_btVec3(room.ceiling.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(room.ceiling.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(room.ceiling.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(room.ceiling.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);*/
		}

		// Door floors
		for (Door& door : house->m_doors)
		{
			btVector3 vertA, vertB, vertC, vertD;
			vertA = Util::glmVec3_to_btVec3(door.m_floor.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(door.m_floor.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(door.m_floor.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(door.m_floor.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);
		}

		// Staircase
		for (Staircase& staircase : house->m_staircases)
		{
			// Staircase landings
			if (staircase.m_stepsInSecondSet > 0)
			{
				btVector3 vertA, vertB, vertC, vertD;
				vertA = Util::glmVec3_to_btVec3(staircase.m_landingFloor.worldSpaceCorners[0]);
				vertB = Util::glmVec3_to_btVec3(staircase.m_landingFloor.worldSpaceCorners[1]);
				vertC = Util::glmVec3_to_btVec3(staircase.m_landingFloor.worldSpaceCorners[2]);
				vertD = Util::glmVec3_to_btVec3(staircase.m_landingFloor.worldSpaceCorners[3]);
				triangleMesh->addTriangle(vertA, vertB, vertC);
				triangleMesh->addTriangle(vertC, vertD, vertA);
			}
			// Staircase bottom floor
			btVector3 vertA, vertB, vertC, vertD;
			vertA = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayFloor.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayFloor.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayFloor.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayFloor.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);

			// top doorway floor
			vertA = Util::glmVec3_to_btVec3(staircase.m_topDoorwayFloor.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(staircase.m_topDoorwayFloor.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(staircase.m_topDoorwayFloor.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(staircase.m_topDoorwayFloor.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);

			// Staircase bottom doorwayCeiling
			vertA = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayCeiling.worldSpaceCorners[0]);
			vertB = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayCeiling.worldSpaceCorners[1]);
			vertC = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayCeiling.worldSpaceCorners[2]);
			vertD = Util::glmVec3_to_btVec3(staircase.m_bottomDoorwayCeiling.worldSpaceCorners[3]);
			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);
		}



		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		s_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(triangleMeshShape);
		collisionObject->setWorldTransform(meshTransform);
		collisionObject->setFriction(0.5);
		collisionObject->setCustomDebugColor(DEBUG_COLOR_RAMP);

		//triangleMeshShape->setMargin(btScalar(0.01f));

		EntityData* entityData = new EntityData();
		entityData->name = "FLOOR";
		entityData->vectorIndex = 0;
		collisionObject->setUserPointer(entityData);

		s_dynamicsWorld->removeCollisionObject(collisionObject);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;

	//	s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	void Physics::AddCubesToPhysicsWorld(std::vector<Cube> cubes)
	{
		/*for (size_t i = 0; i < cubes.size(); i++)
		{
			btBoxShape* shape = new btBoxShape(Util::glmVec3_to_btVec3(cubes[i].m_transform.scale * glm::vec3(0.5f)));
			s_collisionShapes.push_back(shape);
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(Util::glmVec3_to_btVec3(cubes[i].m_transform.position));

			btRigidBody* rigidBody;
			btScalar mass = 0;
			rigidBody = createRigidBody(mass, transform, shape, 1.0f);
			rigidBody->setCustomDebugColor(DEBUG_COLOR_YELLOW);
		}*/
	}

	void Physics::AddStaircaseToPhysicsWorld(House* house)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (Staircase& staircase : house->m_staircases)
		{
			// steps
			for (size_t i = 0; i < staircase.m_stairMesh.indices.size(); i += 3) {
				btVector3 vertA = Util::glmVec3_to_btVec3(staircase.m_stairMesh.vertices[staircase.m_stairMesh.indices[i]].Position);
				btVector3 vertB = Util::glmVec3_to_btVec3(staircase.m_stairMesh.vertices[staircase.m_stairMesh.indices[i + 1]].Position);
				btVector3 vertC = Util::glmVec3_to_btVec3(staircase.m_stairMesh.vertices[staircase.m_stairMesh.indices[i + 2]].Position);
				triangleMesh->addTriangle(vertA, vertB, vertC);
			}
			// walls
			for (size_t i = 0; i < staircase.m_wallMesh.indices.size(); i += 3) {
				btVector3 vertA = Util::glmVec3_to_btVec3(staircase.m_wallMesh.vertices[staircase.m_wallMesh.indices[i]].Position);
				btVector3 vertB = Util::glmVec3_to_btVec3(staircase.m_wallMesh.vertices[staircase.m_wallMesh.indices[i + 1]].Position);
				btVector3 vertC = Util::glmVec3_to_btVec3(staircase.m_wallMesh.vertices[staircase.m_wallMesh.indices[i + 2]].Position);
				triangleMesh->addTriangle(vertA, vertB, vertC);
			}
			// ceilings
			for (size_t i = 0; i < staircase.m_ceilingMesh.indices.size(); i += 3) {
				btVector3 vertA = Util::glmVec3_to_btVec3(staircase.m_ceilingMesh.vertices[staircase.m_ceilingMesh.indices[i]].Position);
				btVector3 vertB = Util::glmVec3_to_btVec3(staircase.m_ceilingMesh.vertices[staircase.m_ceilingMesh.indices[i + 1]].Position);
				btVector3 vertC = Util::glmVec3_to_btVec3(staircase.m_ceilingMesh.vertices[staircase.m_ceilingMesh.indices[i + 2]].Position);
				triangleMesh->addTriangle(vertA, vertB, vertC);
			}
		}

		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		s_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(triangleMeshShape);
		collisionObject->setWorldTransform(meshTransform);
		collisionObject->setCustomDebugColor(btVector3(1, 0, 0));
		EntityData* entityData = new EntityData();
		entityData->name = "STAIRS";
		entityData->vectorIndex = 0;
		collisionObject->setUserPointer(entityData);

		collisionObject->setFriction(10);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::ENEMY;

	//	s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

		s_collisionObjects.push_back(collisionObject);

		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	}
	/*
	void Physics::AddStaircaseToPhysicsWorld(House* house)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (Staircase& staircase : house->staircases) {
			btVector3 vertA = Util::glmVec3_to_btVec3(staircase.worldSpaceCorners[0]);
			btVector3 vertB = Util::glmVec3_to_btVec3(staircase.worldSpaceCorners[1]);
			btVector3 vertC = Util::glmVec3_to_btVec3(staircase.worldSpaceCorners[2]);
			btVector3 vertD = Util::glmVec3_to_btVec3(staircase.worldSpaceCorners[3]);

			triangleMesh->addTriangle(vertA, vertB, vertC);
			triangleMesh->addTriangle(vertC, vertD, vertA);
		}

		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		m_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		m_triangleCollisionObject = new btCollisionObject();
		m_triangleCollisionObject->setCollisionShape(triangleMeshShape);
		m_triangleCollisionObject->setWorldTransform(meshTransform);
		m_triangleCollisionObject->setFriction(0);
		m_triangleCollisionObject->setCustomDebugColor(DEBUG_COLOR_RAMP);

		EntityData* entityData = new EntityData();
		entityData->name = "Staircase";
		entityData->vectorIndex = 0;
		m_triangleCollisionObject->setUserPointer(entityData);

		m_dynamicsWorld->addCollisionObject(m_triangleCollisionObject);
		m_triangleCollisionObject->setCollisionFlags(m_triangleCollisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}*/

	void Physics::AddWallsToPhysicsWorld(House* house)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (Room& room : house->m_rooms)
		{
			for (int i = 0; i < room.m_wallMesh.indices.size(); i += 3)
			{
				btVector3 vertA = Util::glmVec3_to_btVec3(room.m_wallMesh.vertices[room.m_wallMesh.indices[i]].Position);
				btVector3 vertB = Util::glmVec3_to_btVec3(room.m_wallMesh.vertices[room.m_wallMesh.indices[i + 1]].Position);
				btVector3 vertC = Util::glmVec3_to_btVec3(room.m_wallMesh.vertices[room.m_wallMesh.indices[i + 2]].Position);
				triangleMesh->addTriangle(vertA, vertB, vertC);
			}
		}

		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		s_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(triangleMeshShape);
		collisionObject->setWorldTransform(meshTransform);
		collisionObject->setFriction(0.2f);
		collisionObject->setCustomDebugColor(DEBUG_COLOR_WALL);
		EntityData* entityData = new EntityData();
		entityData->name = "WALL";
		entityData->vectorIndex = 0;
		collisionObject->setUserPointer(entityData);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::ENEMY | CollisionGroups::PROJECTILES;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);
		s_collisionObjects.push_back(collisionObject);
		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}


	btRigidBody* Physics::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction, int group, int mask)
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
		cInfo.m_friction = friction;

		btRigidBody* body = new btRigidBody(cInfo);
		//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
		btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
		body->setWorldTransform(startTransform);
#endif  //

		body->setUserIndex(-1);
		s_dynamicsWorld->addRigidBody(body, group, mask);

		return body;

	}

	void Physics::AddEntityToPhysicsWorld(Entity* entity)
	{
		//if (AssetManager::models.size() == 0)
		//	return;

		btTriangleMesh* triangleMesh = new btTriangleMesh();

		int indexCount = AssetManager::models[entity->m_modelID].m_meshes[0]->indices.size();


			for (int i = 0; i < indexCount ; i += 3)
			{
				std::vector<Vertex>* vertices = &AssetManager::models[entity->m_modelID].m_meshes[0]->vertices;
				std::vector<unsigned int>* indices = &AssetManager::models[entity->m_modelID].m_meshes[0]->indices;

				glm::vec3 scale = entity->m_transform.scale;
				btVector3 vertA = Util::glmVec3_to_btVec3(vertices->at(indices->at(i)).Position * scale);
				btVector3 vertB = Util::glmVec3_to_btVec3(vertices->at(indices->at(i+1)).Position * scale);
				btVector3 vertC = Util::glmVec3_to_btVec3(vertices->at(indices->at(i+2)).Position * scale);
				triangleMesh->addTriangle(vertA, vertB, vertC);
			}
	

		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		s_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(Util::glmVec3_to_btVec3(entity->m_transform.position));

		btQuaternion q = btQuaternion(entity->m_transform.rotation.y, 0, 0); // this is not always going to work. only for y it does.
		meshTransform.setRotation((q));

		//meshTransform.setFromOpenGLMatrix(glm::value_ptr(entity->m_transform.to_mat4()));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(triangleMeshShape);
		collisionObject->setWorldTransform(meshTransform);
		collisionObject->setFriction(0);
		collisionObject->setCustomDebugColor(btVector3(1, 0, 0));
		EntityData* entityData = new EntityData();
		entityData->name = "NEW MESH";
		entityData->vectorIndex = 0;
		collisionObject->setUserPointer(entityData);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::ENTITY | CollisionGroups::ENEMY;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

		s_collisionObjects.push_back(collisionObject);

		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}




	void Physics::AddAnimatedEntityToPhysicsWorld(AnimatedEntity* entity)
	{
		// this is a duplicate of the above. combined them somehow.
		// this is a duplicate of the above. combined them somehow.
		// this is a duplicate of the above. combined them somehow.
		// this is a duplicate of the above. combined them somehow.
		// this is a duplicate of the above. combined them somehow.
/*
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		int indexCount = AssetManager::models[entity->m_modelID].m_meshes[0]->indices.size();


		for (int i = 0; i < indexCount; i += 3)
		{
			std::vector<Vertex>* vertices = &AssetManager::models[entity->m_modelID].m_meshes[0]->vertices;
			std::vector<unsigned int>* indices = &AssetManager::models[entity->m_modelID].m_meshes[0]->indices;

			glm::vec3 scale = entity->m_transform.scale;
			btVector3 vertA = Util::glmVec3_to_btVec3(vertices->at(indices->at(i)).Position * scale);
			btVector3 vertB = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 1)).Position * scale);
			btVector3 vertC = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 2)).Position * scale);
			triangleMesh->addTriangle(vertA, vertB, vertC);
		}


		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		s_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(Util::glmVec3_to_btVec3(entity->m_transform.position));

		btQuaternion q = btQuaternion(entity->m_transform.rotation.y, 0, 0); // this is not always going to work. only for y it does.
		meshTransform.setRotation((q));

		//meshTransform.setFromOpenGLMatrix(glm::value_ptr(entity->m_transform.to_mat4()));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(triangleMeshShape);
		collisionObject->setWorldTransform(meshTransform);
		collisionObject->setFriction(0);
		collisionObject->setCustomDebugColor(btVector3(1, 0, 0));
		EntityData* entityData = new EntityData();
		entityData->name = "NEW MESH";
		entityData->vectorIndex = 0;
		collisionObject->setUserPointer(entityData);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::ENTITY | CollisionGroups::ENEMY;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

		s_collisionObjects.push_back(collisionObject);

		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);*/
	}



	void Physics::DeleteRigidBody(btRigidBody*& rigidBody)
	{
		delete rigidBody->getMotionState();
		delete rigidBody->getCollisionShape();
		s_dynamicsWorld->removeRigidBody(rigidBody);
		delete rigidBody;
		rigidBody = nullptr;
	}


	void Physics::Update(float deltaTime)
	{
		int maxSubSteps = 1;
		btScalar fixedTimeStep = btScalar(1.) / btScalar(60.);

		//maxSubSteps = 10;
		//fixedTimeStep = 1.0 / 240.0;
		s_dynamicsWorld->stepSimulation(deltaTime , maxSubSteps, fixedTimeStep);

		CheckForCollisionEvents();
	}

	void Physics::CheckForCollisionEvents() 
	{
		// keep a list of the collision pairs we
		// found during the current update
		CollisionPairs pairsThisUpdate;
	
		// iterate through all of the manifolds in the dispatcher
		for (int i = 0; i < s_dispatcher->getNumManifolds(); ++i) 
		{		
			// get the manifold
			btPersistentManifold* pManifold = s_dispatcher->getManifoldByIndexInternal(i);
			
			// ignore manifolds that have 
			// no contact points.
			if (pManifold->getNumContacts() > 0) 
			{
				// get the two rigid bodies involved in the collision
				const btRigidBody* pBody0 = static_cast<const btRigidBody*>(pManifold->getBody0());
				const btRigidBody* pBody1 = static_cast<const btRigidBody*>(pManifold->getBody1());
			
				// always create the pair in a predictable order
				// (use the pointer value..)
				bool const swapped = pBody0 > pBody1;
				const btRigidBody* pSortedBodyA = swapped ? pBody1 : pBody0;
				const btRigidBody* pSortedBodyB = swapped ? pBody0 : pBody1;
				
				// create the pair
				CollisionPair thisPair = std::make_pair(pSortedBodyA, pSortedBodyB);
				
				// insert the pair into the current list
				pairsThisUpdate.insert(thisPair);
				
				// if this pair doesn't exist in the list
				// from the previous update, it is a new
				// pair and we must send a collision event
				if (s_pairsLastUpdate.find(thisPair) == s_pairsLastUpdate.end()) 
				{				
					CollisionEvent((btRigidBody*)pBody0, (btRigidBody*)pBody1);
				}
			}
		}
		// create another list for pairs that
		// were removed this update
		CollisionPairs removedPairs;

		// this handy function gets the difference beween
		// two sets. It takes the difference between
		// collision pairs from the last update, and this 
		// update and pushes them into the removed pairs list
		std::set_difference(s_pairsLastUpdate.begin(), s_pairsLastUpdate.end(),
		pairsThisUpdate.begin(), pairsThisUpdate.end(),
		std::inserter(removedPairs, removedPairs.begin()));
		
		// iterate through all of the removed pairs
		// sending separation events for them
		for (CollisionPairs::const_iterator iter = removedPairs.begin(); iter != removedPairs.end(); ++iter) {
			SeparationEvent((btRigidBody*)iter->first, (btRigidBody*)iter->second);
		}

		// in the next iteration we'll want to
		// compare against the pairs we found
		// in this iteration
		s_pairsLastUpdate = pairsThisUpdate;
	}

	void Physics::CollisionEvent(btRigidBody* pBody0, btRigidBody* pBody1) 
	{
		// find the two colliding objects
		/*GameObject* pObj0 = FindGameObject(pBody0);
		GameObject* pObj1 = FindGameObject(pBody1);
		
		// exit if we didn't find anything
		if (!pObj0 || !pObj1) return;
		
		// set their colors to white
		pObj0->SetColor(btVector3(1.0, 1.0, 1.0));
		pObj1->SetColor(btVector3(1.0, 1.0, 1.0));*/
		char* name0;
		char* name1;

		EntityData* entityData = (EntityData*)pBody0->getUserPointer();
		if (entityData) {
			name0 = entityData->name;
		}
		EntityData* entityData1 = (EntityData*)pBody1->getUserPointer();
		if (entityData1) {
			name1 = entityData1->name;
		}

		bool shellCollision = false;
		bool floor = false;
		bool wall = false;

		if (name0 == "SHELL" || name1 == "SHELL")
			shellCollision = true;

		if (name0 == "FLOOR" || name1 == "FLOOR")
			floor = true;

		if (name0 == "WALL" || name1 == "WALL")
			wall = true;

		if (name0 == "DOOR" || name1 == "DOOR")
			wall = true;

		std::cout << "Collision between [" << name0 << "] and [" << name1 << "]\n";

		if (shellCollision)
		{
			if (wall)
				Audio::PlayAudio("ShellWallBounce.wav", 0.4f);

			if (floor) {
				Audio::PlayAudio("ShellFloorBounce.wav", 0.4f);		

				// This index of 1 is used to simulate rolling friction from this moment
				if (name0 == "SHELL")
					pBody0->setUserIndex(1); 
				if (name1 == "SHELL")
					pBody1->setUserIndex(1);
			}
		}



	}

	void Physics::SeparationEvent(btRigidBody* pBody0, btRigidBody* pBody1) 
	{
		/*
		// get the two separating objects
		GameObject* pObj0 = FindGameObject((btRigidBody*)pBody0);
		GameObject* pObj1 = FindGameObject((btRigidBody*)pBody1);
		
		// exit if we didn't find anything
		if (!pObj0 || !pObj1) return;
		
		// set their colors to black
		pObj0->SetColor(btVector3(0.0, 0.0, 0.0));
		pObj1->SetColor(btVector3(0.0, 0.0, 0.0));
		
		*/
	}

	/*::FindGameObject(btRigidBody* pBody) {
		// search through our list of gameobjects finding
		// the one with a rigid body that matches the given one
		for (GameObjects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter) {
			if ((*iter)->GetRigidBody() == pBody) {
				// found the body, so return the corresponding game object
				return *iter;
			}
		}
		return 0;
	}*/


	void Physics::AddHouse(House* house)
	{
		s_collisionObjects.clear();
		s_collisionShapes.clear();

		//m_triangleCollisionObject -

		if (house->m_staircases.size() > 0)
			AddStaircaseToPhysicsWorld(house);

		AddWallsToPhysicsWorld(house);
		AddFloorsAndCeilingsToPhysicsWorld(house);


		for (Entity& entity : house->m_entities)
			AddEntityToPhysicsWorld(&entity);


		//AddGroundToPhysicsWorld();

		// Add doors
		btVector3 collisionScale = btVector3(0.8f, 2, 0.04f);// Util::glmVec3_to_btVec3(boundingBox.baseTransform.scale);
		//btVector3 collisionScale = btVector3(1, 1, 1);

		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);

		for (int i = 0; i < house->m_doors.size(); i++)
		{
			Door* door = &house->m_doors[i];
			btTransform btTransform;
			btTransform.setIdentity();

			glm::vec3 pos = Util::GetTranslationFromMatrix(house->m_doors[i].m_rootTransform.to_mat4() * house->m_doors[i].m_doorTransform.to_mat4());
			//			glm::vec3 pos = Util::GetTranslationFromMatrix(house->m_doors[i].m_rootTransform.to_mat4());

			float x = pos.x;// door->m_rootTransform.position.x;// +door->m_doorTransform.position.x;
			float y = pos.y + 1;//->m_rootTransform.position.y + 1; // door->m_doorTransform.position.y + 1;
			float z = pos.z;// door->m_rootTransform.position.z;// +door->m_doorTransform.position.z;

			btTransform.setOrigin(btVector3(x, y, z));
			btTransform.setRotation(btQuaternion(door->m_rootTransform.rotation.y, 0, 0));

			int group = CollisionGroups::HOUSE;
			int mask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;

			door->m_rigidBody = createRigidBody(0.0, btTransform, collisionShape, 1.0, group, mask);
			door->m_rigidBody->setCustomDebugColor(DEBUG_COLOR_DOOR);

			// Entity Data
			EntityData* entityData = new EntityData();
			entityData->name = "DOOR";
			entityData->vectorIndex = i;
			door->m_rigidBody->setUserPointer(entityData);

			door->m_rigidBody->setCollisionFlags(door->m_rigidBody->getCollisionFlags() |
				btCollisionObject::CF_KINEMATIC_OBJECT |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			s_rigidBodies.push_back(door->m_rigidBody);
		}
	}

	void Physics::RebuildPhysicsWorld(House* house)
	{
		
		//m_dynamicsWorld.no
		//m_dynamicsWorld->removeCollisionObject(m_triangleCollisionObject);
	}

	glm::mat4 Physics::GetModelMatrixFromRigidBody(btRigidBody* rigidBody)
	{
		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform(trans);

		btScalar matrix[16];
		trans.getOpenGLMatrix(matrix);

		return Util::btScalar2mat4(matrix);
	}
}

