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
      Ogre::Item* mItem;
      PhysicsScene* mScene;
      physx::PxRigidActor* mActor;
      SceneNode* mNode;
      Primitive( PhysicsScene* scene );
    public:
      Ogre::Item* getItem() { return mItem; }
      virtual ~Primitive();
    };

    class Plane: public Primitive {
    protected:
      Ogre::MeshPtr mMesh;
    public:
      Plane( PhysicsScene* scene, const Ogre::Plane& plane, const Real width, const Real height, const Vector3& position, const Real u = 1.0f, const Real v = 1.0f );
      virtual ~Plane();
    };

    class Box: public Primitive {
    protected:
      Ogre::MeshPtr mMesh;
    public:
      Box( PhysicsScene* scene, const Vector3& size, const Vector3& position, const Quaternion& orientation );
      virtual ~Box();
    };

  }

}