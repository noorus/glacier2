#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Actions.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "InputManager.h"
#include "Camera.h"

namespace Glacier {

  CameraController::CameraController(): mCamera( nullptr ),
  mZooming( false ), mRotating( false )
  {
    resetMovement();
  }

  void CameraController::setCamera( GameCamera* camera )
  {
    mCamera = camera;
  }

  void CameraController::resetMovement()
  {
    mPersistentMovement = Vector3::ZERO;
    mImpulseMovement = Vector3::ZERO;
    mMovement = Vector3::ZERO;
  }

  void CameraController::updateMovement()
  {
    if ( !mZooming )
      mMovement = mPersistentMovement + mImpulseMovement;
    else
      mMovement = Vector3( mPersistentMovement.x, 0.0f, mPersistentMovement.y );
  }

  void CameraController::prepare()
  {
    mImpulseMovement = Vector3::ZERO;
    updateMovement();
  }

  void CameraController::apply()
  {
    if ( mCamera )
      mCamera->applyMovement( mMovement );
  }

}