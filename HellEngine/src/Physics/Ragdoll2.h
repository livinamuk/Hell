#pragma once

#include "rapidjson/document.h"
#include <rapidjson/filereadstream.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include "Header.h"
#include "Helpers/Util.h"
#include "Physics/Physics.h"

#include "Animation/SkinnedModel.h"
#include "Helpers/FileImporter.h"

namespace HellEngine
{
	class Ragdoll2
	{

		struct RigidComponent
		{
			int ID;
			std::string name, correspondingJointName, shapeType;
			glm::mat4 restMatrix;
			glm::vec3 scaleAbsoluteVector;
			glm::vec3 boxExtents, offset;
			float capsuleLength, capsuleRadius;
			btQuaternion rotation;
			btRigidBody* rigidBody;

			float mass, friction, restitution, linearDamping, angularDamping, sleepThreshold;
		};

		struct JointComponent
		{
			std::string name;
			int parentID, childID;
			glm::mat4 parentFrame, childFrame;

			// Drive component
			float drive_angularDamping, drive_angularStiffness, drive_linearDampening, drive_linearStiffness;
			glm::mat4 target;

			// Limit component
			float twist, swing1, swing2, limit_angularStiffness, limit_angularDampening, limit_linearStiffness, limit_linearDampening;
			btConeTwistConstraint* coneTwist;
			//btGeneric6DofConstraint* coneTwist;
			glm::vec3 limit;
			bool enabled;
		};

	public:
		Ragdoll2();
		Ragdoll2(std::string filename);
		void RemovePhysicsObjects();
		std::vector<RigidComponent> m_rigidComponents;
		std::vector<JointComponent> m_jointComponents;
		SkinnedModel* m_skinnedModel;

		std::vector<glm::mat4> m_animatedTransforms;
		std::vector<glm::mat4> m_animatedDebugTransforms_Animated;

		void UpdateBoneTransform(SkinnedModel* skinnedModel, std::vector<glm::mat4>& Transforms, std::vector<glm::mat4>& DebugAnimatedTransforms);

		RigidComponent* GetRigidByName(std::string name);

		bool DisableSkinning;
	};
}