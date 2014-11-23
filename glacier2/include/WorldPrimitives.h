#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysicsScene;

  namespace World {

    class Plane {
    protected:
      PhysicsScene* mScene;
      physx::PxRigidStatic* mActor;
      Ogre::Entity* mEntity;
      Ogre::MeshPtr mMesh;
      PCZSceneNode* mNode;
    public:
      Plane( PhysicsScene* scene, const Ogre::Plane& plane, const Real width, const Real height, const Vector3& position );
      ~Plane();
    };

  }

}