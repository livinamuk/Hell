#pragma once
#include "Header.h"

namespace HellEngine
{
	class Entity
	{
	public: // methods
		Entity();
		Entity(int modelID);
		void Update(float deltaTime);
		void Draw(Shader* shader);
		void Draw(Shader* shader, glm::mat4 modelMatrix);
		void SetAnimation(char* animationName, bool loop); // switches animation. 
		bool IsAnimationComplete();

	public:	// fields
		Transform m_transform;
		int m_modelID = -1;

		char* m_currentAnimationName = "UNDEFINED";
		bool m_loopAnimation = true;
		FbxTime m_currentAnimationTime;
		Animation* m_currentAnimation = NULL;
	};
}