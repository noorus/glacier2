#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Mouse.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "Graphics.h"
#include "Win32.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    Device::Device( LocalController* local, Nil::Mouse* mouse ):
    InputDevice( local ), mouse_( mouse ), focused_( true )
    {
      buttons_.resize( mouse_->getState().mButtons.size(), Action_None );

      buttons_[0] = Action_Select;
      buttons_[1] = Action_Command;
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      focused_ = focus;
    }

    void Device::updatePacket( const Nil::MouseState& state )
    {
      // well this sucks, but we don't get absolute positions from the event APIs

      POINT pt;
      Win32::Win32::getCursorPosition( gEngine->getGraphics()->getRenderWindowHandle(), pt );
      packet_.absolute_.x = (Real)pt.x;
      packet_.absolute_.y = (Real)pt.y;
      packet_.relative_.x = (Real)state.mMovement.relative.x;
      packet_.relative_.y = (Real)state.mMovement.relative.y;
    }

    void Device::onMouseButtonPressed( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMousePress( state, button ) )
      //   return;

      const BindAction& action = buttons_[button];
      if ( action != Action_None ) {
        updatePacket( state );
        controller_->beginAction( this, action );
      }
    }

    void Device::onMouseButtonReleased( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMouseRelease( state, button ) )
      //   return;

      const BindAction& action = buttons_[button];
      if ( action != Action_None ) {
        updatePacket( state );
        controller_->endAction( this, action );
      }
    }

    void Device::onMouseMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !focused_ )
        return;

      // if ( Locator::getGUI().injectMouseMove( state ) )
      //   return;

      updatePacket( state );

      controller_->mouseMovement( this, packet_ );
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