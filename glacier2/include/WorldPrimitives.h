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
      virtual void draw() = 0;
      virtual ~Primitive();
    };

    class Grid: public Primitive {
    protected:
      SceneManager* scene_;
      SceneNode* node_;
      ManualObject* obj_;
    public:
      Grid( PhysicsScene* scene, const Ogre::Plane& plane, const Real width, const Real height, const Vector3& position, const Real u = 1.0f, const Real v = 1.0f );
      virtual void draw();
      virtual ~Grid();
    };

    class Box: public Primitive {
    protected:
      Ogre::MeshPtr mMesh;
    public:
      Box( PhysicsScene* scene, const Vector3& size, const Vector3& position, const Quaternion& orientation );
      virtual void draw();
      virtual ~Box();
    };

  }

}