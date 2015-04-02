#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Mouse.h"
#include "ServiceLocator.h"
#include "GUI.h"
#include "Controllers.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    Device::Device( LocalController* local, Nil::Mouse* mouse ):
    InputDevice( local ), mMouse( mouse ), mFocused( true )
    {
      mButtons.resize( mMouse->getState().mButtons.size(), Action_None );

      mButtons[0] = Action_Rotate;
      mButtons[1] = Action_Zoom;
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      mFocused = focus;
    }

    void Device::onMouseButtonPressed( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMousePress( state, button ) )
        return;

      const BindAction& action = mButtons[button];
      if ( action != Action_None )
        mController->beginAction( this, action );
    }

    void Device::onMouseButtonReleased( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseRelease( state, button ) )
        return;

      const BindAction& action = mButtons[button];
      if ( action != Action_None )
        mController->endAction( this, action );
    }

    void Device::onMouseMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseMove( state ) )
        return;

      Vector3 movement(
        (Real)state.mMovement.relative.x,
        (Real)state.mMovement.relative.y,
        0.0f );

      mController->cameraMouseMovement( this, movement );
    }

    void Device::onMouseWheelMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseMove( state ) )
        return;

      mController->applyZoom( this, Real( state.mWheel.relative / WHEEL_DELTA ) );
    }

    Device::~Device()
    {
      //
    }

  }

}