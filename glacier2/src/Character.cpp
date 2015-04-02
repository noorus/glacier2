#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "Character.h"
#include "World.h"
#include "Entity.h"
#include "Actions.h"
#include "InputManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Character::Character( World* world, const EntityBaseData* baseData ):
  Entity( world, baseData ),
  mInput( nullptr ), mPhysics( nullptr ), mMovement( nullptr )
  {
    mInput = new PlayerCharacterInputComponent( this );
  }

  void Character::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Entity::spawn( position, orientation );

    mPhysics = new CharacterPhysicsComponent( mWorld, position, mHeight, mRadius );
    mMovement = new CharacterMovementComponent( this );
  }

  void Character::setActions( const ActionPacket& actions,
  const CharacterMoveMode mode, const Vector3& direction,
  const Vector2& directional )
  {
    mActions = actions;
    mMove.moveMode = mode;
    mMove.direction = direction;
    mMove.directional = directional;
  }

  void Character::think( const GameTime delta )
  {
    mInput->update( mActions, delta );

    if ( mPhysics && mMovement )
    {
      mMovement->generate( mMove, delta, mPhysics );
      mPhysics->update();

      mNode->setPosition( mPhysics->getPosition() );
    }
  }

  void Character::onHitGround()
  {
    mFlags[Flag_On_Ground] = true;
  }

  void Character::onLeaveGround()
  {
    mFlags[Flag_On_Ground] = false;
  }

  const bool Character::isOnGround()
  {
    return mFlags[Flag_On_Ground];
  }

  Character::~Character()
  {
    SAFE_DELETE( mMovement );
    SAFE_DELETE( mPhysics );
    SAFE_DELETE( mInput );
  }

}