#pragma once
#include "Types.h"
#include "Entity.h"
#include "EntityRegistry.h"
#include "Actions.h"
#include "Character.h"
#include "AIAgent.h"
#include "AIFiniteStateMachine.h"
#include "FOVCone.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Dummy: public Character, public AI::Agent {
  friend class EntityFactories;
  friend class PlayerCharacterInputComponent;
  private:
    static EntityBaseData baseData;
  protected:
    Ogre::Item* mItem;
    Ogre::MeshPtr mMesh;
    Ogre::SceneNode* mEyeNode;
    AI::FiniteStateMachine mStates;
    FOVCone mFovCone;
    Dummy( World* world );
    virtual ~Dummy();
  public:
    virtual AICharacterInputComponent* getInput();
    FOVCone& getFOVCone() throw();
    virtual void spawn( const Vector3& position, const Quaternion& orientation );
    virtual void think( const GameTime delta );
    virtual Ogre::MovableObject* getMovable();
  };

}