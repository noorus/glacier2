#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "ActionManager.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controller.h"
#include "InputManager.h"
#include "CharacterPhysicsComponent.h"

namespace Glacier {

  Controller::Controller(): mCamera( nullptr ), mCharacter( nullptr )
  {
    resetActions();
  }

  void Controller::setCamera( GameCamera* camera )
  {
    mCamera = camera;
  }

  void Controller::setCharacter( Character* character )
  {
    gEngine->getConsole()->printf( Console::srcInput, L"Controller: setting character 0x%X", character );
    mCharacter = character;
  }

  void Controller::resetActions()
  {
    mActions.move = Player_Move_None;
    mActions.sidestep = Player_Sidestep_None;
    mActions.jump = Player_Jump_None;
    mActions.run = Player_Run_None;
    mActions.crouch = Player_Crouch_None;
  }

  void Controller::prepare()
  {
    // mCameraController->prepare();

    mActions.jump = Player_Jump_None;
    mActions.run = Player_Run_None;
    mActions.crouch = Player_Crouch_None;

    /*if ( !mDirectional.isZeroLength() && mActions.move == Player_Move_None )
      mActions.move = Player_Move_Forward;
    else if ( mDirectional.isZeroLength() && mActions.move == Player_Move_Forward )
      mActions.move = Player_Move_None;*/
  }

  void Controller::apply()
  {
    if ( mCharacter )
      mCharacter->setActions( mActions );
  }

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

  void LocalController::beginAction( InputDevice* device, const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    switch ( action )
    {
      case Action_Move_Forward:
        if ( mActions.move == Player_Move_None )
          mActions.move = Player_Move_Forward;
        break;
      case Action_Move_Backward:
        if ( mActions.move == Player_Move_None )
          mActions.move = Player_Move_Backward;
        break;
      case Action_Sidestep_Left:
        if ( mActions.sidestep == Player_Sidestep_None )
          mActions.sidestep = Player_Sidestep_Left;
        break;
      case Action_Sidestep_Right:
        if ( mActions.sidestep == Player_Sidestep_None )
          mActions.sidestep = Player_Sidestep_Right;
        break;
      case Action_Jump:
        mActions.jump = Player_Jump_Keydown;
        break;
      case Action_Run:
        mActions.run = Player_Run_Keydown;
        break;
      case Action_Crouch:
        mActions.crouch = Player_Crouch_Keydown;
        break;
    }
  }

  void LocalController::endAction( InputDevice* device, const BindAction& action )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    switch ( action )
    {
      case Action_Move_Forward:
        if ( mActions.move == Player_Move_Forward )
          mActions.move = Player_Move_None;
        break;
      case Action_Move_Backward:
        if ( mActions.move == Player_Move_Backward )
          mActions.move = Player_Move_None;
        break;
      case Action_Sidestep_Left:
        if ( mActions.sidestep == Player_Sidestep_Left )
          mActions.sidestep = Player_Sidestep_None;
        break;
      case Action_Sidestep_Right:
        if ( mActions.sidestep == Player_Sidestep_Right )
          mActions.sidestep = Player_Sidestep_None;
        break;
      case Action_Jump:
        mActions.jump = Player_Jump_Keyup;
        break;
      case Action_Run:
        mActions.run = Player_Run_Keyup;
        break;
      case Action_Crouch:
        mActions.crouch = Player_Crouch_Keyup;
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

    // gEngine->getConsole()->printf( Console::srcInput, L"LocalController::applyZoom()" );
  }

  void LocalController::directionalMovement( InputDevice* device, const Vector2& directional )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    // gEngine->getConsole()->printf( Console::srcInput, L"LocalController::directionalMovement()" );
  }

  void LocalController::cameraMovement( InputDevice* device, const Vector2& movement )
  {
    updateInputMode( device );

    if ( shouldIgnoreInput( device ) )
      return;

    // gEngine->getConsole()->printf( Console::srcInput, L"LocalController::cameraMovement()" );
  }

}