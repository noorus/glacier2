#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Entity;
  struct EntityBaseData;
  class World;

  namespace Entities {

    class DevCube: public Entity {
    friend class EntityFactories;
    private:
      static EntityBaseData baseData;
    protected:
      physx::PxRigidDynamic* mActor;
      Ogre::Entity* mEntity;
      Ogre::MeshPtr mMesh;
      DevCube( World* world );
      virtual ~DevCube();
    public:
      virtual void spawn( const Vector3& position, const Quaternion& orientation );
      virtual void think( const GameTime delta );
    };

  }

}