#pragma once
#include "Engine.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class WindowHandler: public EngineComponent, public Ogre::WindowEventListener {
  protected:
    void windowMoved( Ogre::RenderWindow* rw );
    void windowResized( Ogre::RenderWindow* rw );
    bool windowClosing( Ogre::RenderWindow* rw );
    void windowClosed( Ogre::RenderWindow* rw );
    void windowFocusChange( Ogre::RenderWindow* rw );
  public:
    WindowHandler( Engine* engine );
    virtual ~WindowHandler();
  };

}