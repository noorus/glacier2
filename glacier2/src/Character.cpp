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
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Character::Character( World* world, const EntityBaseData* baseData,
  CharacterInputComponent* input ):
  Entity( world, baseData ),
  mInput( input ), mPhysics( nullptr ), mMovement( nullptr )
  {
    mFacing = Vector3::UNIT_Z;
  }

  void Character::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Entity::spawn( position, orientation );

    mPhysics = new CharacterPhysicsComponent( mWorld, position, mHeight, mRadius );
    mMovement = new CharacterMovementComponent( this );
  }

  const Vector3& Character::getLocalEyePosition() const
  {
    return mEyePosition;
  }

  const Vector3 Character::getWorldEyePosition() const
  {
    return mPhysics->getPosition() + mEyePosition;
  }

  const Radian& Character::getFieldOfView() const
  {
    return mFieldOfView;
  }

  const Vector3& Character::getFacing() const
  {
    return mFacing;
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
    if ( mInput )
      mInput->update( mActions, delta );

    if ( mPhysics && mMovement )
    {
      mMovement->generate( mMove, delta, mPhysics );
      mPhysics->update();
    }
  }

  void Character::visualize()
  {
    mNode->setPosition( mPhysics->getPosition() );
  }

  const bool Character::canSee( Entity* entity ) const
  {
    auto scene = Locator::getGraphics().getScene();
    auto movable = entity->getMovable();
    auto worldEye = getWorldEyePosition();
    auto halfFov = Degree( mFieldOfView.valueDegrees() / 2.0f );
    auto aabb = movable->getWorldAabbUpdated();

    if ( aabb.distance( worldEye ) > mViewDistance )
      return false;

    auto samplingPoints = Math::aabbCorners( aabb );
    for ( size_t i = 0; i < samplingPoints.size(); i++ )
    {
      auto sample = samplingPoints[i];
      if ( sample.distance( worldEye ) <= mViewDistance )
      {
        sample -= worldEye;
        if ( sample.angleBetween( mFacing ) <= halfFov )
        {
          Ogre::Ray ray( worldEye, sample );
          auto query = scene->createRayQuery( ray );

          // Mask out cameras, because 1) we don't care 2) their bounds will
          // be out of date, causing an assertion failure
          uint32_t mask = 0xFFFFFFFF;
          mask &= ~SceneQueryFlag_Camera;
          query->setQueryMask( mask );
          query->setSortByDistance( true );

          auto results = query->execute();
          for ( auto result : results )
          {
            if ( !result.movable )
              break;
            auto flags = result.movable->getQueryFlags();
            if ( ( flags & SceneQueryFlag_World ) != 0 )
              break;
            if ( result.movable->getId() == movable->getId() )
            {
              scene->destroyQuery( query );
              return true;
            }
          }
          scene->destroyQuery( query );
        }
      }
    }
    return false;
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