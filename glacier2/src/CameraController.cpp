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
    persistentMovement_ = Vector3::ZERO;
    impulseMovement_ = Vector3::ZERO;
    movement_ = Vector3::ZERO;
  }

  void CameraController::updateMovement()
  {
    if ( !zooming_ )
      movement_ = persistentMovement_ + impulseMovement_;
    else
      movement_ = Vector3( persistentMovement_.x, 0.0f, persistentMovement_.y );
  }

  void CameraController::prepare()
  {
    impulseMovement_ = Vector3::ZERO;
    updateMovement();
  }

  void CameraController::apply()
  {
    if ( camera_ )
      camera_->applyMovement( movement_ );
  }

}