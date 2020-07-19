#pragma once
#include "header.h"
#include "AnimatedEntity.h"
#include "Physics/Ragdoll.h"

namespace HellEngine
{
    class EnemyCharacter : public AnimatedEntity
    {
    public:
        EnemyCharacter();
        void NewRagdollFromAnimatedTransforms();
        void AnimatedFromRagdoll();
        //void Draw(Shader* shader, glm::mat4 modelMatrix);

        Ragdoll* m_ragdoll;
        bool m_animatedWithRagdoll = true;

    };
}  