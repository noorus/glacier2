#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Entity;
  struct EntityBaseData;
  class World;

  class Cube: public Entity {
  friend class EntityFactories;
  private:
    static EntityBaseData baseData;
  protected:
    physx::PxRigidDynamic* mActor;
    Ogre::Entity* mEntity;
    Ogre::MeshPtr mMesh;
    Cube( World* world );
    virtual ~Cube();
  public:
    virtual void spawn( const Vector3& position, const Quaternion& orientation );
    virtual void think();
  };

}