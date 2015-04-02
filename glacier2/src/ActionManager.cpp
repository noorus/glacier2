#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "ActionManager.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "GUI.h"

namespace Glacier {

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
    if ( !mZooming )
      mMovement = mPersistentMovement + mImpulseMovement;
    else
      mMovement = Vector3( mPersistentMovement.x, 0.0f, mPersistentMovement.y * 0.025f );
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
    mPersistentMovement.x = -vecMovement.x;
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
  mCameraController( nullptr )
  {
    assert( mEngine );
    mCameraController = new CameraController( mEngine );
  }

  ActionManager::~ActionManager()
  {
    SAFE_DELETE( mCameraController );
  }

}