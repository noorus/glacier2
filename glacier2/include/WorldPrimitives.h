#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "ServiceLocator.h"
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class PhysicsScene;

  class Cube: public Entity {
  friend class EntityFactories;
  private:
    static EntityBaseData baseData;
  protected:
    PhysicsScene* mScene;
    physx::PxRigidDynamic* mActor;
    Ogre::Entity* mEntity;
    Ogre::MeshPtr mMesh;
    PCZSceneNode* mNode;
    Cube();
    virtual ~Cube();
  public:
    virtual void spawn( PhysicsScene* scene, const Vector3& position );
    virtual void think();
  };

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