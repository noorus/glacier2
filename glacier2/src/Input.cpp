#include "StdAfx.h"
#include "Input.h"
#include "Engine.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Input::Input( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window ):
  EngineComponent( engine ), mSystem( nullptr )
  {
    // Greetings
    mEngine->getConsole()->printf( Console::srcInput,
      L"Initializing input system..." );

    // Get window handle
    HWND windowHandle = NULL;
    window->getCustomAttribute( "WINDOW", &windowHandle );
    if ( !windowHandle )
      ENGINE_EXCEPT( L"Could not retrieve render window handle" );

    // Create input system
    mSystem = new Nil::System( instance, windowHandle, this );

    // Enable all initially connected devices
    for ( auto device : mSystem->getDevices() )
      device->enable();
  }

  void Input::componentTick( GameTime tick, GameTime time )
  {
    mSystem->update();
  }

  void Input::onDeviceConnected( Nil::Device* device )
  {
    device->enable();
  }
  
  void Input::onDeviceDisconnected( Nil::Device* device )
  {
  }

  void Input::onMouseEnabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Mouse: %s", device->getName().c_str() );
  }

  void Input::onKeyboardEnabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Keyboard: %s", device->getName().c_str() );
  }

  void Input::onControllerEnabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Controller: %s", device->getName().c_str() );
  }

  void Input::onMouseDisabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled: %s", device->getName().c_str() );
  }

  void Input::onKeyboardDisabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled: %s", device->getName().c_str() );
  }

  void Input::onControllerDisabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled: %s", device->getName().c_str() );
  }

  Input::~Input()
  {
    SAFE_DELETE( mSystem );
  }

}