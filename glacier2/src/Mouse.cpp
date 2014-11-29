#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"
#include "Mouse.h"
#include "ServiceLocator.h"
#include "GUI.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    Device::Device( Nil::Mouse* mouse ): mMouse( mouse ), mFocused( true )
    {
      mButtons.resize( mMouse->getState().mButtons.size(), Action_None );
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
        gEngine->getActionManager()->beginAction( action );
    }

    void Device::onMouseButtonReleased( Nil::Mouse* mouse, const Nil::MouseState& state, size_t button )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseRelease( state, button ) )
        return;

      const BindAction& action = mButtons[button];
      if ( action != Action_None )
        gEngine->getActionManager()->endAction( action );
    }

    void Device::onMouseMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseMove( state ) )
        return;
    }

    void Device::onMouseWheelMoved( Nil::Mouse* mouse, const Nil::MouseState& state )
    {
      if ( !mFocused )
        return;

      if ( Locator::getGUI().injectMouseMove( state ) )
        return;

      gEngine->getActionManager()->applyZoom( state.mWheel.relative / WHEEL_DELTA );
    }

    Device::~Device()
    {
      //
    }

  }

}