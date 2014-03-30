#include "StdAfx.h"
#include "Engine.h"
#include "WindowHandler.h"

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
    //
  }

  WindowHandler::~WindowHandler()
  {
    //
  }

}