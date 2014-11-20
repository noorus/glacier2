#include "StdAfx.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "InputManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  WindowHandler::WindowHandler( Engine* engine ): EngineComponent( engine )
  {
    //
  }

  void WindowHandler::windowMoved( Ogre::RenderWindow* rw )
  {
    //
  }

  void WindowHandler::windowResized( Ogre::RenderWindow* rw )
  {
    //
  }

  bool WindowHandler::windowClosing( Ogre::RenderWindow* rw )
  {
    return true;
  }

  void WindowHandler::windowClosed( Ogre::RenderWindow* rw )
  {
    mEngine->signalStop();
  }

  void WindowHandler::windowFocusChange( Ogre::RenderWindow* rw )
  {
    // This might break; It seems that when WM_ACTIVATE generates this event,
    // the GetFocus() return hasn't yet changed. So the statuses act reversed.
    HWND windowHandle = NULL;
    rw->getCustomAttribute( "WINDOW", &windowHandle );
    if ( GetFocus() != windowHandle )
      mEngine->getInput()->onInputFocus( true );
    else
      mEngine->getInput()->onInputFocus( false );
  }

  WindowHandler::~WindowHandler()
  {
    //
  }

}