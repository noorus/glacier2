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

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Real cStickyTerrainExtraGravity = 8.0f;
  const Real cJumpVelocity = 4.0f;

  void CharacterJump::begin( Vector3& velocity, Vector3& directional, const GameTime time )
  {
    mJumping = true;
    mInAir = true;
    if ( velocity.y > 0.0f )
      velocity.y = 0.0f;
    mBaseVelocity = directional;
    velocity += Vector3( 0.0f, cJumpVelocity, 0.0f );
    velocity += directional;
  }

  void CharacterJump::generate( Vector3& velocity, const GameTime time )
  {
    //
  }

  void CharacterJump::landed( Vector3& velocity )
  {
    velocity -= mBaseVelocity;
    mInAir = false;
    end();
  }

  void CharacterJump::end()
  {
    mJumping = false;
  }

  CharacterMovementComponent::CharacterMovementComponent( Character* character ):
  mCharacter( character ), mVelocity( Vector3::ZERO )
  {
    //
  }

  void CharacterMovementComponent::generate(
  CharacterMoveData& move, const GameTime delta, CharacterPhysicsComponent* physics )
  {
    // Reset displacement
    mDisplacement = Vector3::ZERO;

    // Add gravity
    Vector3 gravity = mCharacter->getWorld()->getPhysics()->getGravityVector();
    gravity *= (Real)delta;
    mVelocity += gravity;

    // Make forward vector
    Vector3 forward = move.direction;
    forward.y = 0.0f;
    forward.normalise();

    // Copy forward vector as facing direction
    move.facing = forward;

    // Make up vector
    static Vector3 up( Vector3::UNIT_Y );

    // Make right vector
    Vector3 right = forward.crossProduct( up );
    right.normalise();

    // Calculate directional impulse
    Vector3 directional( Vector3::ZERO );

    if ( move.moveMode == CharacterMoveData::Mode_Impulse )
    {
      // Impulse movements
      if ( move.affectors[CharacterMoveData::Affector_Forward] )
        directional += ( forward * move.forward );
      if ( move.affectors[CharacterMoveData::Affector_Backward] )
        directional -= ( forward * move.backward );
      if ( move.affectors[CharacterMoveData::Affector_Right] )
        directional += ( right * move.right );
      if ( move.affectors[CharacterMoveData::Affector_Left] )
        directional -= ( right * move.left );
    }
    else if ( move.moveMode == CharacterMoveData::Mode_Directional )
    {
      // Directional movements
      if ( !move.directional.isZeroLength() && move.affectors[CharacterMoveData::Affector_Forward] )
      {
        Vector2 dir2d = move.directional.normalisedCopy();
        Vector2 fwd2d( Vector2::UNIT_Y );
        Quaternion rotation( dir2d.angleTo( fwd2d ), Vector3::NEGATIVE_UNIT_Y );
        directional = rotation * forward;
        directional *= move.directional.length();
        directional *= move.forward;
      }
    }

    // Normalise if length > 1
    Real length = directional.length();
    if ( length > 1.0f )
      directional /= length;

    // Multiply by speed
    directional *= move.speed;

    // Handle jumping
    if ( move.jump.jumping() ) {
      move.jump.generate( mVelocity, delta );
    } else {
      if ( move.jumpImpulse ) {
        move.jump.begin( mVelocity, directional, delta );
        move.jumpImpulse = false;
      }
    }
    
    // Final change in velocity
    Vector3 velocityDelta( Vector3::ZERO );

    // Do sticky terrain magic
    if ( mCharacter->isOnGround() && !move.jump.jumping() )
    {
      Vector3 position = physics->getPosition();
      Vector3 newPosition = position + ( directional * (Real)delta );
      auto query = physics->groundQuery( newPosition );
      if ( query.hit )
      {
        Vector3 target = query.position;
        target.y += physics->getOffsetFromGround();
        Vector3 stickyDirectional = directional;
        stickyDirectional.y = target.y - position.y;
        if ( length < 1.0f )
          stickyDirectional *= length;
        velocityDelta = stickyDirectional;
      }
      else
        velocityDelta = directional;
      // Throw in some extra gravity to avoid stuttering issues
      velocityDelta.y -= cStickyTerrainExtraGravity;
      // Update velocity
      mVelocity += velocityDelta;
    }
    
    // Make displacement
    mDisplacement = mVelocity * (Real)delta;

    // Send to physical controller, get results
    auto lastFlags = physics->getLastCollisionFlags();
    auto flags = physics->move( mDisplacement, delta );

    // On ground, subtract directional impulse for infinite friction
    if ( mCharacter->isOnGround() )
      mVelocity -= velocityDelta;

    // Hit something below
    if ( flags & physx::PxControllerFlag::eCOLLISION_DOWN )
    {
      if ( !( lastFlags & physx::PxControllerFlag::eCOLLISION_DOWN ) )
      {
        mCharacter->onHitGround();
        if ( move.jump.jumping() )
          move.jump.landed( mVelocity );
      }
      mVelocity = 0.0f;
    }
    else if ( lastFlags & physx::PxControllerFlag::eCOLLISION_DOWN )
    {
      mCharacter->onLeaveGround();
    }

    // Copy velocity to movedata
    move.velocity = mVelocity;
  }

}