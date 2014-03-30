#pragma once
#include "Types.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Input: public EngineComponent, public Nil::SystemListener {
  protected:
    Nil::System* mSystem;
  protected:
    // Nil::SystemListener callbacks
    virtual void onDeviceConnected(
      Nil::Device* device );
    virtual void onDeviceDisconnected(
      Nil::Device* device );
    virtual void onMouseEnabled(
      Nil::Device* device, Nil::Mouse* instance );
    virtual void onKeyboardEnabled(
      Nil::Device* device, Nil::Keyboard* instance );
    virtual void onControllerEnabled(
      Nil::Device* device, Nil::Controller* instance );
    virtual void onMouseDisabled(
      Nil::Device* device, Nil::Mouse* instance );
    virtual void onKeyboardDisabled(
      Nil::Device* device, Nil::Keyboard* instance );
    virtual void onControllerDisabled(
      Nil::Device* device, Nil::Controller* instance );
  public:
    Input( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual ~Input();
  };

}