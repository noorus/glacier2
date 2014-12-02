#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "CharacterPhysicsComponent.h"
#include "World.h"
#include "Entity.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_ENTITY( player, Character );

  const Real cPlayerHeight = 1.0f;
  const Real cPlayerRadius = 0.25f;

  Character::Character( World* world ): Entity( world, &baseData ),
  mInput( nullptr ), mPhysics( nullptr ), mMovement( nullptr )
  {
    mInput = new CharacterInputComponent( this );
  }

  const Ogre::MovableObject* Character::getMovable() const
  {
    return mEntity;
  }

  void Character::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Entity::spawn( position, orientation );

    mMesh = Procedural::CapsuleGenerator().setHeight( cPlayerHeight ).setRadius( cPlayerRadius ).realizeMesh( "playerStandCapsule" );

    mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
    mEntity->setMaterialName( "Debug/PlayerPhysicsCapsule" );
    mEntity->setCastShadows( false );
    mNode->attachObject( mEntity );

    mPhysics = new CharacterPhysicsComponent( mWorld, position, cPlayerHeight, cPlayerRadius );
    mMovement = new CharacterMovementComponent( this );
  }

  void Character::think( const GameTime delta )
  {
    mInput->update( gEngine->getActionManager()->getActions(), delta );

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
    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );
  }

}