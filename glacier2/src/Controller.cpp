#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "ActionManager.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controller.h"

namespace Glacier {

  Controller::Controller(): mCamera( nullptr )
  {
    //
  }

  void Controller::setCamera( GameCamera* camera )
  {
    mCamera = camera;
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

  void LocalController::beginAction( InputDevice* device, const BindAction& action )
  {
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

  void LocalController::applyZoom( InputDevice* device, const Real zoom )
  {
    gEngine->getConsole()->printf( Console::srcInput, L"LocalController::applyZoom()" );
  }

  void LocalController::directionalMovement( InputDevice* device, const Vector2& directional )
  {
    gEngine->getConsole()->printf( Console::srcInput, L"LocalController::directionalMovement()" );
  }

  void LocalController::cameraMovement( InputDevice* device, const Vector2& movement )
  {
    gEngine->getConsole()->printf( Console::srcInput, L"LocalController::cameraMovement()" );
  }

}