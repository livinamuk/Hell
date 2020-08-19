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
	
	// Shapes
	btBoxShape* Physics::s_windowShape;
	btBoxShape* Physics::s_doorShape; 
	btBoxShape* Physics::s_groundShape; 
	btCollisionObject* Physics::s_WorldGroundCollisionObject;

	//std::vector<btCollisionObject*> Physics::m_WallCollisionObjects;

	Ragdoll* Physics::m_ragdoll;

	void Physics::Init()
	{
		s_debugDraw.Init();		
		s_broadphase = new btDbvtBroadphase();
		s_collisionConfiguration = new btDefaultCollisionConfiguration();
		s_dispatcher = new btCollisionDispatcher(s_collisionConfiguration);
		s_solver = new btSequentialImpulseConstraintSolver;

		s_dynamicsWorld = new btDiscreteDynamicsWorld(s_dispatcher, s_broadphase, s_solver, s_collisionConfiguration);
		s_dynamicsWorld->setGravity(btVector3(0, -10, 0));
		s_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());		
		s_dynamicsWorld->setDebugDrawer(&s_debugDraw);
		s_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_FastWireframe + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits);

		s_doorShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		s_windowShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		s_groundShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));

		s_doorShape->setLocalScaling(btVector3(DOOR_WIDTH, DOOR_HEIGHT, DOOR_DEPTH));
		s_windowShape->setLocalScaling(btVector3(btVector3(WINDOW_WIDTH_SINGLE, WINDOW_HEIGHT_SINGLE, 0.1f)));
		s_groundShape->setLocalScaling(btVector3(WORLD_GROUND_SIZE, WORLD_GROUND_HEIGHT, WORLD_GROUND_SIZE));

		CreateWorldGround();
	}

	void Physics::CreateWorldGround()
	{
		// Create the ground
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(0, -WORLD_GROUND_HEIGHT, 0));

		float friction = 0.5f;
		int collisionGroup = CollisionGroups::TERRAIN;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
		btCollisionShape* collisionShape = s_groundShape;
		PhysicsObjectType objectType = PhysicsObjectType::UNDEFINED;

		s_WorldGroundCollisionObject = Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, nullptr);
	}

	void Physics::RemoveHouse(House* house)
	{
		// Remove house
		for (Door& door : house->m_doors)
			door.RemoveCollisionObject();

		for (Window& window : house->m_windows)
			window.RemoveCollisionObject();

		for (Room& room : house->m_rooms)
			room.ResetWallSegmentsAndRemoveAssociatedCollisionObjects();

		for (Entity& entity : house->m_entities)
			entity.RemoveCollisionObject();
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

	void Physics::AddFloorsAndCeilingsToPhysicsWorld(House* house)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();
		for (size_t i = 0; i < house->m_rooms.size(); i++)
		{
			Room* room = &house->m_rooms[i];
			float roomX = room->m_position.x;
			float roomZ = room->m_position.y;
			float roomWidth = room->m_size.x;
			float roomDepth = room->m_size.y;

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
			entityData->type = PhysicsObjectType::FLOOR;
			entityData->ptr = &room->m_floor;

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
			//std::cout << Util::Vec3_to_String(door.m_floor.m_transform.position) << "\n";
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
		entityData->type = PhysicsObjectType::FLOOR;
		entityData->ptr = &house->m_rooms[0];									//// SUUPER TEMPRORARY
		collisionObject->setUserPointer(entityData);

		s_dynamicsWorld->removeCollisionObject(collisionObject);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}


	btCollisionObject* Physics::CreateCollisionObject(btTransform transform, btCollisionShape* collisionShape, PhysicsObjectType objectType, int collisionGroup, int collisionMask, float friction, btVector3 debugColor, void* parent)
	{
		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(collisionShape);
		collisionObject->setWorldTransform(transform);
		collisionObject->setCustomDebugColor(debugColor);

		EntityData* entityData = new EntityData();
		entityData->type = objectType;
		entityData->ptr = parent;
		collisionObject->setUserPointer(entityData);
		
		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		s_dynamicsWorld->addCollisionObject(collisionObject, collisionGroup, collisionMask);
		
		return collisionObject;
	}


	// THIS IS ONLY USED BY WALL SEGMENTS CURRENTLY
	/*btCollisionObject* Physics::AddWallSegment(const Transform& trans)
	{
		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(Util::glmVec3_to_btVec3(trans.scale));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(trans.position));
		transform.setRotation(Util::glmVec3_to_btQuat(trans.rotation));

		btTransform depthOffsetTransform;
		depthOffsetTransform.setIdentity();
		depthOffsetTransform.setOrigin(btVector3(0, 0, -WALL_DEPTH/2));

		btCollisionObject* collisionObject = new btCollisionObject();
		collisionObject->setCollisionShape(collisionShape);
		collisionObject->setWorldTransform(transform * depthOffsetTransform);
		collisionObject->setCustomDebugColor(DEBUG_COLOR_WALL);
		collisionObject->setFriction(0.2f);
		EntityData* entityData = new EntityData();
		entityData->type = PhysicsObjectType::WALL;
		entityData->ptr = nullptr;							// SUPER TOMPORARY ALSO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		collisionObject->setUserPointer(entityData);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::ENEMY | CollisionGroups::PROJECTILES;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);	
		s_collisionObjects.push_back(collisionObject);
		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

		return collisionObject;
	}*/

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
		entityData->type = PhysicsObjectType::STAIRS;
		entityData->ptr = nullptr;										/// SUPER TEMPORARY ???//////////////////////
		collisionObject->setUserPointer(entityData);

		collisionObject->setFriction(10);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::ENEMY;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);

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
		// Clean up
	/*	for (btCollisionObject* collisionObject : m_WallCollisionObjects)
			s_dynamicsWorld->removeCollisionObject(collisionObject);
		m_WallCollisionObjects.clear();
		*/
		return;
		/////////
		/////////
		/////////
		/////////
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
		entityData->type = PhysicsObjectType::WALL;
		entityData->ptr = nullptr; ///////////////////////////////////////////////////////////////// TEMP AF
		collisionObject->setUserPointer(entityData);

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::PLAYER | CollisionGroups::ENEMY | CollisionGroups::PROJECTILES;

		s_dynamicsWorld->addCollisionObject(collisionObject, group, mask);
		s_collisionObjects.push_back(collisionObject);
		collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
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

	void Physics::DeleteRigidBody(btRigidBody*& rigidBody)
	{
		//if (rigidBody->getMotionState())
			delete rigidBody->getMotionState();

		//if (rigidBody->getCollisionShape())
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

		PhysicsObjectType objectType0;
		PhysicsObjectType objectType1;

		EntityData* entityData = (EntityData*)pBody0->getUserPointer();
		if (entityData) {
			objectType0 = entityData->type;
		}
		EntityData* entityData1 = (EntityData*)pBody1->getUserPointer();
		if (entityData1) {
			objectType1 = entityData1->type;
		}

		bool collision_with_floor = false;
		bool collision_with_wall = false;

		if (objectType0 == PhysicsObjectType::FLOOR || objectType0 == PhysicsObjectType::FLOOR)
			collision_with_floor = true;

		if (objectType0 == PhysicsObjectType::WALL || objectType0 == PhysicsObjectType::WALL)
			collision_with_wall = true;

		if (objectType0 == PhysicsObjectType::DOOR || objectType0 == PhysicsObjectType::DOOR)
			collision_with_wall = true;

		std::cout << "Collision between [" << Util::PhysicsObjectEnumToString(objectType0) << "] and [" << Util::PhysicsObjectEnumToString(objectType1) << "]\n";


		// Shells
		int casingType = 0;

		if (objectType0 == PhysicsObjectType::SHELL_PROJECTILE) {
			pBody0->setUserIndex(1); // This index of 1 is used to simulate rolling friction from this moment
			casingType = entityData->enumValue;
		}
		else if (objectType1 == PhysicsObjectType::SHELL_PROJECTILE) {
			pBody1->setUserIndex(1);
			casingType = entityData1->enumValue;
		}
		else
			casingType = -1;

		if (casingType == CasingType::SHOTGUN_SHELL) {
			if (collision_with_wall)
				Audio::PlayAudio("ShellWallBounce.wav", 0.4f);

			if (collision_with_floor)
				Audio::PlayAudio("ShellFloorBounce.wav", 0.4f);
		}

		if (casingType == CasingType::BULLET_CASING) {
				Audio::PlayAudio("BuolletCasingBounce.wav", 0.4f);
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

	void Physics::SetRigidBodyWorldTransform(btRigidBody& rigidBody, Transform& transform)
	{
		// position and rotation
		btTransform btTrans;
		btTrans.setIdentity();
		btTrans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

		glm::quat r = glm::quat(transform.rotation);
		btTrans.setRotation(btQuaternion(r.x, r.y, r.z, r.w));

		rigidBody.setWorldTransform(btTrans);

		// scale
		rigidBody.getCollisionShape()->setLocalScaling(Util::glmVec3_to_btVec3(transform.scale));
	}

	void Physics::SetCollisionObjectWorldTransform(btCollisionObject* collisionObject, Transform& transform)
	{
		// position and rotation
		btTransform btTrans;
		btTrans.setIdentity();
		btTrans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

		glm::quat r = glm::quat(transform.rotation);
		btTrans.setRotation(btQuaternion(r.x, r.y, r.z, r.w));

		collisionObject->setWorldTransform(btTrans);

		// scale
		collisionObject->getCollisionShape()->setLocalScaling(Util::glmVec3_to_btVec3(transform.scale));
	}

	void Physics::SetCollisionObjectWorldTranslation(btCollisionObject* collisionObject, const glm::vec3& position, glm::vec3 orginOffset)
	{
		btTransform btTrans;
		btTrans = collisionObject->getWorldTransform();
		btTrans.setOrigin(Util::glmVec3_to_btVec3(position + orginOffset));
		collisionObject->setWorldTransform(btTrans);
	}

	void Physics::SetCollisionObjectWorldRotation(btCollisionObject* collisionObject, const glm::vec3& rotation)
	{
		btTransform btTrans;
		btTrans = collisionObject->getWorldTransform();
		btTrans.setRotation(Util::glmVec3_to_btQuat(rotation));
		collisionObject->setWorldTransform(btTrans);
	}
}

