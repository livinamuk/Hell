#pragma once
#include "Header.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	class AnimatedEntity
	{
	public: // methods
		AnimatedEntity();
		void Update(float deltatime);
		//void Draw(Shader* shader);
		void Draw(Shader* shader, glm::mat4 modelMatrix);
		void SetSkinnedModel(const char* skinnedModelName);
		void SetMaterial(const char* MeshName, const char* MaterialName);
		void PlayAnimation(const char* animationName, bool loop);
		glm::mat4 GetCameraMatrix();
		SkinnedModel* GetSkinnedModel();
		bool IsAnimationComplete();
		bool IsSpecificAnimationComplete(const char* animationName);
		void ResetAnimationTimer();
		void PauseOnFinalFrame();
		void SetModelScale(float scale);
		//void FlipModelUpAxis(bool flip);
		void SetAnimationToBindPose();
		void PauseAnimation();

	public:	// fields
		Transform m_worldTransform;
		Transform m_modelTransform;
		//Transform m_skeletonTransform;
		std::vector<int> m_MaterialIDs; // one for each mesh
		std::vector<glm::mat4> m_animatedTransforms;
		std::vector<glm::mat4> m_animatedDebugTransforms_Animated;
		//std::vector<glm::mat4> m_animatedDebugTransforms_BindPose;
		int m_currentAnimationIndex;
		float m_currentAnimationTime;
		float m_currentAnimationDuration;

	private: // methods
		void UpdateAnimation(float deltatime);

	private: // fields
		bool m_loopCurrentAnimation;
		float m_animationSpeed;
		bool m_animationIsComplete;
		int m_skinnedModelID = -1;		
		bool m_pause = false;
	};
}