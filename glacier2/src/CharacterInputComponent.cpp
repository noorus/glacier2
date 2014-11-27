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

  const Real cAcceleration = 10.0f;
  const Real cDeceleration = 5.0f;

  const Real cTimeToRun = 0.2f;
  const Real cTimeToSlowdown = 0.2f;
  const Real cTimeToCrouch = 0.4f;
  const Real cTimeToUncrouch = 0.2f;

  inline Real quickSpline( Real value )
  {
    Real squared = value * value;
    return ( 3.0f * squared - 2.0f * squared * value );
  }

  CharacterInputComponent::CharacterInputComponent( Character* character ):
  mCharacter( character ), mCrouchKeyed( false ), mRunKeyed( false ),
  mRunTime( 0.0f ), mCrouchTime( 0.0f )
  {
    //
  }

  void CharacterInputComponent::calculateAffectors( const ActionPacket& action, GameTime delta )
  {
    CharacterMoveData& move = mCharacter->mMove;

    float accelerator = cAcceleration * delta;
    float decelerator = cDeceleration * delta;

    // forward & backward
    switch ( action.move )
    {
      case Player_Move_Forward:
        move.forward += accelerator;
        move.backward -= decelerator;
        break;
      case Player_Move_Backward:
        move.backward += accelerator;
        move.forward -= decelerator;
        break;
      case Player_Move_None:
        move.forward -= decelerator;
        move.backward -= decelerator;
        break;
    }

    // sidestepping
    switch ( action.sidestep )
    {
      case Player_Sidestep_Left:
        move.left += accelerator;
        move.right -= decelerator;
        break;
      case Player_Sidestep_Right:
        move.right += accelerator;
        move.left -= decelerator;
        break;
      case Player_Sidestep_None:
        move.left -= decelerator;
        move.right -= decelerator;
    }

    // running
    if ( mRunKeyed )
    {
      if ( mRunTime > cTimeToRun )
        move.run = 1.0f;
      else
        move.run = quickSpline( mRunTime / cTimeToRun );
    }
    else if ( mRunTime > 0.0f )
      move.run = quickSpline( mRunTime / cTimeToSlowdown );
    else
      move.run = 0.0f;

    // crouching
    if ( mCrouchKeyed )
    {
      if ( mCrouchTime > cTimeToCrouch )
        move.crouch = 1.0f;
      else
        move.crouch = quickSpline( mCrouchTime / cTimeToCrouch );
    }
    else if ( mCrouchTime > 0.0f )
      move.crouch = quickSpline( mCrouchTime / cTimeToUncrouch );
    else
      move.crouch = 0.0f;

    // clamping
    move.forward  = boost::algorithm::clamp( move.forward, 0.0f, 1.0f );
    move.backward = boost::algorithm::clamp( move.backward, 0.0f, 1.0f );
    move.left     = boost::algorithm::clamp( move.left, 0.0f, 1.0f );
    move.right    = boost::algorithm::clamp( move.right, 0.0f, 1.0f );

    move.updateBits();
  }

  void CharacterInputComponent::handleCrouching( const ActionPacket& action, const GameTime delta )
  {
    CharacterMoveData& move = mCharacter->mMove;

    if ( action.crouch == Player_Crouch_Keydown )
    {
      mCrouchKeyed = true;
      mCrouchTime = 0.0f;
    }
    else if ( action.crouch == Player_Crouch_Keyup )
    {
      mCrouchKeyed = false;
      if ( mCrouchTime > cTimeToUncrouch )
        mCrouchTime = cTimeToUncrouch;
    }

    if ( mCrouchKeyed )
      mCrouchTime += delta;
    else if ( mCrouchTime > 0.0f )
      mCrouchTime -= delta;
    if ( mCrouchTime < 0.0f )
      mCrouchTime = 0.0f;
  }

  void CharacterInputComponent::handleRunning( const ActionPacket& action, const GameTime delta )
  {
    CharacterMoveData& move = mCharacter->mMove;

    if ( action.run == Player_Run_Keydown )
    {
      mRunKeyed = true;
      mRunTime = 0.0f;
    }
    else if ( action.run == Player_Run_Keyup )
    {
      mRunKeyed = false;
      if ( mRunTime > cTimeToSlowdown )
        mRunTime = cTimeToSlowdown;
    }
    if ( mRunKeyed )
      mRunTime += delta;
    else if ( mRunTime > 0.0f )
      mRunTime -= delta;
    if ( mRunTime < 0.0f )
      mRunTime = 0.0f;
  }

  void CharacterInputComponent::handleJumping( const ActionPacket& action )
  {
    CharacterMoveData& move = mCharacter->mMove;

    if ( action.jump == Player_Jump_Keydown && mCharacter->isOnGround() )
      move.jumpImpulse = true;
  }

  void CharacterInputComponent::update( const ActionPacket& action, GameTime delta )
  {
    CharacterMoveData& move = mCharacter->mMove;

    move.direction = Vector3::UNIT_X;
    move.mode = CharacterMoveData::ControlMode_Impulse;

    handleCrouching( action, delta );
    handleRunning( action, delta );
    calculateAffectors( action, delta );
    handleJumping( action );
  }

  CharacterInputComponent::~CharacterInputComponent()
  {
    //
  }

}