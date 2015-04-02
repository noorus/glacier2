#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Actions.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "InputManager.h"
#include "Character.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  LocalController::LocalController(): mMode( Mode_KeyboardAndMouse )
  {
    //
  }

  void LocalController::updateInputMode( InputDevice* device )
  {
    Mode checkMode;

    if ( device->getType() == InputDevice::Type_Mouse
      || device->getType() == InputDevice::Type_Keyboard )
      checkMode = Mode_KeyboardAndMouse;
    else
      checkMode = Mode_Gamepad;

    if ( checkMode != mMode )
    {
      mMode = checkMode;
      resetActions();
      mRotating = false;
      mZooming = false;
      gEngine->getConsole()->printf( Console::srcInput,
        L"Switched input mode: %s",
        mMode == Mode_KeyboardAndMouse ? L"Keyboard & Mouse" : L"Gamepad" );
    }
  }

  bool LocalController::shouldIgnoreInput( InputDevice* from )
  {
    if ( ( from->getType() == InputDevice::Type_Keyboard
      || from->getType() == InputDevice::Type_Mouse )
      && mMode == Mode_KeyboardAndMouse )
      return false;
    else if ( from->getType() == InputDevice::Type_Gamepad && mMode == Mode_Gamepad )
      return false;
    else
      return true;
  }

  void LocalController::beginAction( InputDevice* device,
  const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    switch ( action )
    {
      case Action_Move_Forward:
        if ( mActions.move == Character_Move_None )
          mActions.move = Character_Move_Forward;
        break;
      case Action_Move_Backward:
        if ( mActions.move == Character_Move_None )
          mActions.move = Character_Move_Backward;
        break;
      case Action_Sidestep_Left:
        if ( mActions.sidestep == Character_Sidestep_None )
          mActions.sidestep = Character_Sidestep_Left;
        break;
      case Action_Sidestep_Right:
        if ( mActions.sidestep == Character_Sidestep_None )
          mActions.sidestep = Character_Sidestep_Right;
        break;
      case Action_Jump:
        mActions.jump = Character_Jump_Keydown;
        break;
      case Action_Run:
        mActions.run = Character_Run_Keydown;
        break;
      case Action_Crouch:
        mActions.crouch = Character_Crouch_Keydown;
        break;
      case Action_Rotate:
        mRotating = true;
        break;
      case Action_Zoom:
        mZooming = true;
        break;
    }
  }

  void LocalController::endAction( InputDevice* device,
  const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    switch ( action )
    {
      case Action_Move_Forward:
        if ( mActions.move == Character_Move_Forward )
          mActions.move = Character_Move_None;
        break;
      case Action_Move_Backward:
        if ( mActions.move == Character_Move_Backward )
          mActions.move = Character_Move_None;
        break;
      case Action_Sidestep_Left:
        if ( mActions.sidestep == Character_Sidestep_Left )
          mActions.sidestep = Character_Sidestep_None;
        break;
      case Action_Sidestep_Right:
        if ( mActions.sidestep == Character_Sidestep_Right )
          mActions.sidestep = Character_Sidestep_None;
        break;
      case Action_Jump:
        mActions.jump = Character_Jump_Keyup;
        break;
      case Action_Run:
        mActions.run = Character_Run_Keyup;
        break;
      case Action_Crouch:
        mActions.crouch = Character_Crouch_Keyup;
        break;
      case Action_Rotate:
        mRotating = false;
        break;
      case Action_Zoom:
        mZooming = false;
        break;
    }
  }

  const LocalController::Mode& LocalController::getMode()
  {
    return mMode;
  }

  void LocalController::applyZoom( InputDevice* device, const Real zoom )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    mImpulseMovement.z += zoom;
    updateMovement();
  }

  void LocalController::directionalMovement( InputDevice* device,
  const Vector2& directional )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    mDirectional = directional;

    if ( !mDirectional.isZeroLength() && mActions.move == Character_Move_None )
      mActions.move = Character_Move_Forward;
    else if ( mDirectional.isZeroLength() && mActions.move == Character_Move_Forward )
      mActions.move = Character_Move_None;
  }

  void LocalController::cameraMouseMovement( InputDevice* device,
  const Vector3& movement )
  {
    if ( shouldIgnoreInput( device ) )
      return;

    if ( mRotating )
    {
      mImpulseMovement.x += movement.x;
      mImpulseMovement.y += movement.y;
      updateMovement();
    }
    else if ( mZooming )
    {
      mImpulseMovement.z += movement.z;
      updateMovement();
    }
  }

  void LocalController::cameraPersistentMovement( InputDevice* device,
  const Vector2& movement )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    mPersistentMovement.x = -movement.x;
    mPersistentMovement.y = movement.y;
    updateMovement();
  }

  void LocalController::prepare()
  {
    GameController::prepare();
    CameraController::prepare();
  }

  void LocalController::apply()
  {
    CameraController::apply();

    CharacterMoveMode moveMode = (
      mMode == Mode_KeyboardAndMouse
      ? CharacterMoveMode::Mode_Impulse
      : CharacterMoveMode::Mode_Directional );

    GameController::apply( moveMode, -mCamera->getDirection() );
  }

}