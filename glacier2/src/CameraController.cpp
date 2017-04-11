#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Actions.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "InputManager.h"
#include "Camera.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  CameraController::CameraController(): camera_( nullptr ),
  zooming_( false ), rotating_( false )
  {
    resetMovement();
  }

  void CameraController::setCamera( GameCamera* camera )
  {
    camera_ = camera;
  }

  void CameraController::resetMovement()
  {
    persistentRotation_ = Vector3::ZERO;
    impulseRotation_ = Vector3::ZERO;
    rotation_ = Vector3::ZERO;
  }

  void CameraController::updateMovement()
  {
    if ( !zooming_ )
      rotation_ = persistentRotation_ + impulseRotation_;
    else
      rotation_ = Vector3( persistentRotation_.x, 0.0f, persistentRotation_.y );
  }

  void CameraController::prepare()
  {
    impulseRotation_ = Vector3::ZERO;
    updateMovement();
  }

  void CameraController::apply()
  {
    if ( camera_ ) {
      camera_->applyRotation( rotation_ );
      camera_->applyEdgeScrolling( cameraEdgeScroll_ );
    }
  }

}