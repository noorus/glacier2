#pragma once
#include "Types.h"
#include "Entity.h"
#include "EntityRegistry.h"
#include "Actions.h"
#include "Character.h"
#include "AIAgent.h"
#include "AIFiniteStateMachine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Dummy: public Character, public AI::Agent {
  friend class EntityFactories;
  friend class PlayerCharacterInputComponent;
  private:
    static EntityBaseData baseData;
  protected:
    Ogre::Entity* mEntity;
    Ogre::MeshPtr mMesh;
    AI::FiniteStateMachine mStates;
    Dummy( World* world );
    virtual ~Dummy();
  public:
    virtual void spawn( const Vector3& position, const Quaternion& orientation );
    virtual void think( const GameTime delta );
    virtual const Ogre::MovableObject* getMovable() const throw( );
  };

}