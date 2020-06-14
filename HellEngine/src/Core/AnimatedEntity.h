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
		void PlayAnimation(const char* animationName, bool loop);
		glm::mat4 GetCameraMatrix();
		bool IsAnimationComplete();
		bool IsSpecificAnimationComplete(const char* animationName);
		void ResetAnimationTimer();
		void PauseOnFinalFrame();

	public:	// fields
		Transform m_transform;
		int m_skinnedModelID = -1;
		std::vector<int> m_meshMaterialIDs;
		std::vector<glm::mat4> m_animatedTransforms;
		int m_currentAnimationIndex;
		float m_currentAnimationTime;
		float m_currentAnimationDuration;

	private: // methods
		void UpdateAnimation(float deltatime);

	private: // fields
		bool m_loopCurrentAnimation;
		float m_animationSpeed;
		Transform m_worldTransform;
		bool m_animationIsComplete;
	};
}