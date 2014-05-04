#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "ActionManager.h"
#include "Console.h"

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
    mKeyTable.nKeyboard[0x58] = Action_Move_Forward;
    mKeyTable.nKeyboard[0x53] = Action_Move_Backward;
    mKeyTable.nKeyboard[0x41] = Action_Sidestep_Left;
    mKeyTable.nKeyboard[0x44] = Action_Sidestep_Right;
    mKeyTable.nKeyboard[0x20] = Action_Jump;
    mKeyTable.nKeyboard[0xA0] = Action_Run;
    mKeyTable.nKeyboard[0xA2] = Action_Crouch;
    mKeyTable.nKeyboard[0xDC] = Action_Toggle_Console;
  }

  void ActionManager::clearActions()
  {
    mActions.nMove      = Player_Move_None;
    mActions.nSidestep  = Player_Sidestep_None;
    mActions.nJump      = Player_Jump_None;
    mActions.nRun       = Player_Run_None;
    mActions.nCrouch    = Player_Crouch_None;
  }

  void ActionManager::clearActivators()
  {
    //
  }

  void ActionManager::prepare()
  {
    // Clear these every new frame
    mActions.nJump = Player_Jump_None;
    mActions.nRun = Player_Run_None;
    mActions.nCrouch = Player_Crouch_None;
    mCameraController->prepare();

    // HACK HACK HACK
    if ( !mDirectional.isZeroLength() && mActions.nMove == Player_Move_None )
      mActions.nMove = Played_Move_Forward;
    else if ( mDirectional.isZeroLength() && mActions.nMove == Played_Move_Forward )
      mActions.nMove = Player_Move_None;
  }

  void ActionManager::processBindPress( const BindAction& action )
  {
    switch ( action )
    {
      case Action_Move_Forward:
        if ( mActions.nMove == Player_Move_None )
          mActions.nMove = Played_Move_Forward;
      break;
      case Action_Move_Backward:
        if ( mActions.nMove == Player_Move_None )
          mActions.nMove = Player_Move_Backward;
      break;
      case Action_Sidestep_Left:
        if ( mActions.nSidestep == Player_Sidestep_None )
          mActions.nSidestep = Player_Sidestep_Left;
      break;
      case Action_Sidestep_Right:
        if ( mActions.nSidestep == Player_Sidestep_None )
          mActions.nSidestep = Player_Sidestep_Right;
      break;
      case Action_Jump:
        mActions.nJump = Player_Jump_Keydown;
      break;
      case Action_Run:
        mActions.nRun = Player_Run_Keydown;
      break;
      case Action_Crouch:
        mActions.nCrouch = Player_Crouch_Keydown;
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
        if ( mActions.nMove == Played_Move_Forward )
          mActions.nMove = Player_Move_None;
      break;
      case Action_Move_Backward:
        if ( mActions.nMove == Player_Move_Backward )
          mActions.nMove = Player_Move_None;
      break;
      case Action_Sidestep_Left:
        if ( mActions.nSidestep == Player_Sidestep_Left )
          mActions.nSidestep = Player_Sidestep_None;
      break;
      case Action_Sidestep_Right:
        if ( mActions.nSidestep == Player_Sidestep_Right )
          mActions.nSidestep = Player_Sidestep_None;
      break;
      case Action_Jump:
        mActions.nJump = Player_Jump_Keyup;
      break;
      case Action_Run:
        mActions.nRun = Player_Run_Keyup;
      break;
      case Action_Crouch:
        mActions.nCrouch = Player_Crouch_Keyup;
      break;
      case Action_Zoom:
        mCameraController->setZooming( false );
      break;
    }
  }

  void ActionManager::onMouseMoved( Nil::Mouse* mouse,
  const Nil::MouseState& state )
  {
    mCameraController->applyMouseMovement( state );
  }

  void ActionManager::onMouseButtonPressed( Nil::Mouse* mouse,
  const Nil::MouseState& state, size_t button )
  {
    BindAction nBind = ActionManager::mKeyTable.nMouse[button];
    if ( nBind != Action_None ) {
      processBindPress( nBind );
      return;
    } else
      mCameraController->onMouseButtonPressed( button );
  }

  void ActionManager::onMouseButtonReleased( Nil::Mouse* mouse,
  const Nil::MouseState& state, size_t button )
  {
    BindAction nBind = ActionManager::mKeyTable.nMouse[button];
    if ( nBind != Action_None ) {
      processBindRelease( nBind );
      return;
    } else
      mCameraController->onMouseButtonReleased( button );
  }

  void ActionManager::onMouseWheelMoved( Nil::Mouse* mouse,
  const Nil::MouseState& state )
  {
    mCameraController->applyMouseWheel( state );
  }

  void ActionManager::onKeyPressed( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    BindAction action = ActionManager::mKeyTable.nKeyboard[keycode];
    if ( action == Action_None )
      return;
    processBindPress( action );
  }

  void ActionManager::onKeyRepeat( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    //
  }

  void ActionManager::onKeyReleased( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    BindAction action = ActionManager::mKeyTable.nKeyboard[keycode];
    if ( action == Action_None )
      return;
    processBindRelease( action );
  }

  ActionManager::~ActionManager()
  {
    SAFE_DELETE( mCameraController );
  }

}