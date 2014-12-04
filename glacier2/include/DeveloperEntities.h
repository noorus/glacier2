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
    public:
      enum Type {
        DevCube_025,
        DevCube_050
      };
    private:
      static EntityBaseData baseData;
    protected:
      physx::PxRigidDynamic* mActor;
      Ogre::Entity* mEntity;
      Ogre::MeshPtr mMesh;
      Type mType;
      DevCube( World* world );
      virtual ~DevCube();
    public:
      virtual const Ogre::MovableObject* getMovable() const throw();
      virtual void setType( const Type type );
      virtual void spawn( const Vector3& position, const Quaternion& orientation );
      virtual void think( const GameTime delta );
    };

  }

}