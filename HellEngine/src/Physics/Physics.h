#pragma once
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
#include "bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "bullet/src/BulletDynamics/Character/btKinematicCharacterController.h"

//#include "Platform/OpenGL/BoundingPlane.h"
//#include "HellEngine/Components/House.h"

#include "Renderer/GLDebugDraw.h"
#include "Renderer/Cube.h"
#include "House/House.h"
#include "Core/AnimatedEntity.h"

#include "Ragdoll.h"
#include <set>
#include <iterator>
#include <algorithm>

namespace HellEngine
{
	typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionPair;
	typedef std::set<CollisionPair> CollisionPairs;

	class Physics
	{
	public: // fields
		static CollisionPairs s_pairsLastUpdate;

		static void CheckForCollisionEvents();
		static void CollisionEvent(btRigidBody* pBody0, btRigidBody* pBody1);
		static void SeparationEvent(btRigidBody* pBody0, btRigidBody* pBody1);

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
		static std::vector<glm::vec3> s_points;
		static std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> s_objectsCollisions;

		static Ragdoll* m_ragdoll;

	public: // methods 

		static void Init();
		static void RebuildWorld(House* house);

		static void CreateWorld();
		static void AddHouse(House* house);
		static void Update(float deltaTime);
		static void DebugDraw(Shader* shader);

		static btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction, int group, int mask);
		//static void AddRigidBody(btRigidBody* rigidBody);

		static void AddEntityToPhysicsWorld(Entity* entity);
		static void AddAnimatedEntityToPhysicsWorld(AnimatedEntity* entity);
		static void DeleteRigidBody(btRigidBody*& rigidBody);

		static void RebuildPhysicsWorld(House* house);

		static glm::mat4 GetModelMatrixFromRigidBody(btRigidBody* rigidBody);

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

	class OpenGLMotionState : public btDefaultMotionState 
	{
		public:
			OpenGLMotionState(const btTransform& transform) : btDefaultMotionState(transform) {}
		
			void GetWorldTransform(btScalar* transform) 
			{
				btTransform trans;
				getWorldTransform(trans);
				trans.getOpenGLMatrix(transform);
		}
	};

}