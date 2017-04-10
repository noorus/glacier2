#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Mouse.h"
#include "ServiceLocator.h"
#include "Controllers.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    Device::Device( LocalController* local, Nil::Mouse* mouse ):
    InputDevice( local ), mouse_( mouse ), focused_( true )
    {
      buttons_.resize( mouse_->getState().mButtons.size(), Action_None );

      buttons_[0] = Action_FirstClick;
      buttons_[1] = Action_SecondClick;
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      focused_ = focus;
    }

    void Device::onMouseButtonPressed( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMousePress( state, button ) )
      //   return;

      const BindAction& action = buttons_[button];
      if ( action != Action_None )
        controller_->beginAction( this, action );
    }

    void Device::onMouseButtonReleased( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMouseRelease( state, button ) )
      //   return;

      const BindAction& action = buttons_[button];
      if ( action != Action_None )
        controller_->endAction( this, action );
    }

    void Device::onMouseMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMouseMove( state ) )
      //   return;

      Vector3 movement(
        (Real)state.mMovement.relative.x,
        (Real)state.mMovement.relative.y,
        0.0f );

      controller_->cameraMouseMovement( this, movement );
    }

    void Device::onMouseWheelMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMouseMove( state ) )
      //   return;

      controller_->applyZoom( this, Real( state.mWheel.relative / WHEEL_DELTA ) );
    }

    Device::~Device()
    {
      //
    }

  }

}