#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "ActionManager.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "GUI.h"

namespace Glacier {

  // ActionManager globals ====================================================

  KeyBindTable ActionManager::mKeyTable;

  // CameraController class ===================================================

  CameraController::CameraController( Engine* engine ):
  mEngine( engine ), mZooming( false ), mRotating( false )
  {
    resetMovement();
  }

  void CameraController::resetMovement()
  {
    mPersistentMovement = Vector3::ZERO;
    mImpulseMovement = Vector3::ZERO;
    mMovement = Vector3::ZERO;
  }

  void CameraController::prepare()
  {
    mImpulseMovement = Vector3::ZERO;
    updateMovement();
  }

  void CameraController::updateMovement()
  {
    //if ( !mZooming )
      mMovement = mPersistentMovement + mImpulseMovement;
    //else {
    //  mMovement = Vector3( mPersistentMovement.x, 0.0f, mPersistentMovement.y * 0.025f );
    //}
  }

  void CameraController::onMouseButtonPressed( size_t button )
  {
    if ( button == 0 )
      mRotating = true;
    if ( button == 1 )
      mZooming = true;
  }

  void CameraController::onMouseButtonReleased( size_t button )
  {
    if ( button == 0 )
      mRotating = false;
    if ( button == 1 )
      mZooming = false;
  }

  void CameraController::applyMouseMovement( const Nil::MouseState& state )
  {
    if ( mRotating )
    {
      mImpulseMovement.x += state.mMovement.relative.x;
      mImpulseMovement.y += state.mMovement.relative.y;
      updateMovement();
    }
    else if ( mZooming )
    {
      mImpulseMovement.z += state.mMovement.relative.y * 0.2f;
      updateMovement();
    }
  }

  void CameraController::applyMouseWheel( const Nil::MouseState& state )
  {
    mImpulseMovement.z += (Real)( state.mWheel.relative / WHEEL_DELTA ) * 0.2f;
    updateMovement();
  }

  void CameraController::setPersistentMovement( const Vector2& vecMovement )
  {
    mPersistentMovement.x = vecMovement.x;
    mPersistentMovement.y = vecMovement.y;
    updateMovement();
  }

  void CameraController::setZooming( const bool zoom )
  {
    mZooming = zoom;
    updateMovement();
  }

  CameraController::~CameraController()
  {
    //
  }

  // ActionManager class ======================================================

  ActionManager::ActionManager( Engine* pEngine ): mEngine( pEngine ),
  mCameraController( nullptr ), mDirectional( Vector2::ZERO )
  {
    assert( mEngine );
    mCameraController = new CameraController( mEngine );
    clearActions();
    mKeyTable.keyboard[0x57] = Action_Move_Forward;
    mKeyTable.keyboard[0x53] = Action_Move_Backward;
    mKeyTable.keyboard[0x41] = Action_Sidestep_Left;
    mKeyTable.keyboard[0x44] = Action_Sidestep_Right;
    mKeyTable.keyboard[0x20] = Action_Jump;
    mKeyTable.keyboard[0xA0] = Action_Run;
    mKeyTable.keyboard[0xA2] = Action_Crouch;
    mKeyTable.keyboard[0xDC] = Action_Toggle_Console;
  }

  void ActionManager::clearActions()
  {
    mActions.move      = Player_Move_None;
    mActions.sidestep  = Player_Sidestep_None;
    mActions.jump      = Player_Jump_None;
    mActions.run       = Player_Run_None;
    mActions.crouch    = Player_Crouch_None;
  }

  void ActionManager::clearActivators()
  {
    //
  }

  void ActionManager::prepare()
  {
    // Clear these every new frame
    mActions.jump = Player_Jump_None;
    mActions.run = Player_Run_None;
    mActions.crouch = Player_Crouch_None;
    mCameraController->prepare();

    // HACK HACK HACK
    /*if ( !mDirectional.isZeroLength() && mActions.move == Player_Move_None )
      mActions.move = Player_Move_Forward;
    else if ( mDirectional.isZeroLength() && mActions.move == Player_Move_Forward )
      mActions.move = Player_Move_None;*/
  }

  void ActionManager::cancelAllInput()
  {
    mActions.move = Player_Move_None;
    mActions.sidestep = Player_Sidestep_None;
  }

  void ActionManager::processBindPress( const BindAction& action )
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
      case Action_Zoom:
        mCameraController->setZooming( true );
      break;
      case Action_Toggle_Console:
        //mEngine->toggleConsole();
      break;
    }
  }

  void ActionManager::processBindRelease( const BindAction& action )
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
      case Action_Zoom:
        mCameraController->setZooming( false );
      break;
    }
  }

  void ActionManager::onMouseMoved( const Nil::MouseState& state )
  {
    if ( !Locator::getGUI().injectMouseMove( state ) )
      mCameraController->applyMouseMovement( state );
  }

  void ActionManager::onMouseButtonPressed( const Nil::MouseState& state, size_t button )
  {
    if ( !Locator::getGUI().injectMousePress( state, button ) )
    {
      BindAction nBind = ActionManager::mKeyTable.mouse[button];
      if ( nBind != Action_None ) {
        processBindPress( nBind );
        return;
      } else
        mCameraController->onMouseButtonPressed( button );
    }
  }

  void ActionManager::onMouseButtonReleased( const Nil::MouseState& state, size_t button )
  {
    if ( !Locator::getGUI().injectMouseRelease( state, button ) )
    {
      BindAction nBind = ActionManager::mKeyTable.mouse[button];
      if ( nBind != Action_None ) {
        processBindRelease( nBind );
        return;
      } else
        mCameraController->onMouseButtonReleased( button );
    }
  }

  void ActionManager::onMouseWheelMoved( const Nil::MouseState& state )
  {
    mCameraController->applyMouseWheel( state );
  }

  void ActionManager::onKeyPressed( const Nil::VirtualKeyCode keycode )
  {
    BindAction action = ActionManager::mKeyTable.keyboard[keycode];
    if ( action == Action_None )
      return;

    processBindPress( action );
  }

  void ActionManager::onKeyRepeat( const Nil::VirtualKeyCode keycode )
  {
    //
  }

  void ActionManager::onKeyReleased( const Nil::VirtualKeyCode keycode )
  {
    BindAction action = ActionManager::mKeyTable.keyboard[keycode];
    if ( action == Action_None )
      return;

    processBindRelease( action );
  }

  ActionManager::~ActionManager()
  {
    SAFE_DELETE( mCameraController );
  }

}