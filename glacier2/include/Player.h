#pragma once
#include "Types.h"
#include "Entity.h"
#include "EntityRegistry.h"
#include "Actions.h"
#include "Character.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Player: public Character {
  friend class EntityFactories;
  friend class PlayerCharacterInputComponent;
  private:
    static EntityBaseData baseData;
  protected:
    Ogre::Entity* mEntity;
    Ogre::MeshPtr mMesh;
    Player( World* world );
    virtual ~Player();
  public:
    virtual void spawn( const Vector3& position, const Quaternion& orientation );
    virtual void think( const GameTime delta );
    virtual const Ogre::MovableObject* getMovable() const throw();
  };

}