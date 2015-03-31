#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysicsScene;

  namespace Primitives {

    class Primitive {
    protected:
      Ogre::Entity* mEntity;
      PhysicsScene* mScene;
      physx::PxRigidActor* mActor;
      PCZSceneNode* mNode;
      Primitive( PhysicsScene* scene );
    public:
      Ogre::Entity* getEntity() { return mEntity; }
    };

    class Plane: public Primitive {
    protected:
      Ogre::MeshPtr mMesh;
    public:
      Plane( PhysicsScene* scene, const Ogre::Plane& plane, const Real width, const Real height, const Vector3& position );
      ~Plane();
    };

    class Box: public Primitive {
    protected:
      Ogre::MeshPtr mMesh;
    public:
      Box( PhysicsScene* scene, const Vector3& size, const Vector3& position, const Quaternion& orientation );
      ~Box();
    };

  }

}