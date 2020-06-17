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

        Ragdoll* m_ragdoll;

    };
}  