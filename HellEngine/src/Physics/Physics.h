#pragma once
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
#include "bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "bullet/src/BulletDynamics/Character/btKinematicCharacterController.h"

//#include "Platform/OpenGL/BoundingPlane.h"
//#include "HellEngine/Components/House.h"

#include "Renderer/GLDebugDraw.h"
#include "Renderer/Cube.h"
#include "House/House.h"

namespace HellEngine
{
	class Physics
	{
	public: // fields

		static GLDebugDrawer s_debugDraw;
		static btDefaultCollisionConfiguration* s_collisionConfiguration;
		static btCollisionDispatcher* s_dispatcher;
		static btDbvtBroadphase* s_broadphase;
		static btSequentialImpulseConstraintSolver* s_sol;
		static btConstraintSolver* s_solver;
		static btAlignedObjectArray<btCollisionShape*> s_collisionShapes;
		static btDiscreteDynamicsWorld* s_dynamicsWorld;
		static btAlignedObjectArray<btRigidBody*> s_rigidBodies;
		static btAlignedObjectArray<btCollisionObject*> s_collisionObjects;
		static btCollisionObject* s_triangleCollisionObject;
		static std::vector<glm::vec3> s_points;
		static std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> s_objectsCollisions;

	public: // methods 

		static void Init();
		static void RebuildWorld(House* house);

		static void CreateWorld();
		static void AddHouse(House* house);
		static void Update(float deltaTime);
		static void DebugDraw(Shader* shader);

		static btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction);
		static void AddRigidBody(btRigidBody* rigidBody);

		static void RebuildPhysicsWorld(House* house);

	public: // Methods
		//void AddGroundToPhysicsWorld();
		static void AddWallsToPhysicsWorld(House* house);
		static void AddStaircaseToPhysicsWorld(House* house);
		static void AddFloorsAndCeilingsToPhysicsWorld(House* house);
		static void AddCubesToPhysicsWorld(std::vector<Cube> cubes);

		//public: // static fields
		//static btBoxShape* collisionShapeBox;

		//public: // static methods

	};
}