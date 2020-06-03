#pragma once
#include "Header.h"

namespace HellEngine
{
	class Entity
	{
	public: // methods
		Entity();
		Entity(char* name);
		Entity(int modelID);
		void Update(float deltaTime);
		void DrawEntity(Shader* shader);
		void Draw(Shader* shader);
		void Draw(Shader* shader, glm::mat4 modelMatrix);
		void SetAnimation(char* animationName, bool loop); // switches animation. 
		bool IsAnimationComplete();


	public:	// fields
		Transform m_transform;
		int m_modelID = -1;
		int m_materialID = -1;


		char* m_currentAnimationName = "UNDEFINED";
		bool m_loopAnimation = true;
		FbxTime m_currentAnimationTime;
		Animation* m_currentAnimation = NULL;

		char* m_tag = "Entity";
	};
}