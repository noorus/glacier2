#include "StdAfx.h"
#include "Input.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"

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
    mEngine->getActionManager()->prepare();
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
      L"Enabled mouse: %s", device->getName().c_str() );
    instance->addListener( mEngine->getActionManager() );
  }

  void Input::onKeyboardEnabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled keyboard: %s", device->getName().c_str() );
    instance->addListener( mEngine->getActionManager() );
  }

  void Input::onControllerEnabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled controller: %s", device->getName().c_str() );
    instance->addListener( this );
  }

  void Input::onMouseDisabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled mouse: %s", device->getName().c_str() );
  }

  void Input::onKeyboardDisabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled keyboard: %s", device->getName().c_str() );
  }

  void Input::onControllerDisabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled controller: %s", device->getName().c_str() );
  }

  void Input::onControllerButtonPressed( Nil::Controller* controller, const Nil::ControllerState& state, size_t button )
  {
    // mEngine->getConsole()->printf( Console::srcInput, L"controller button pressed" );
  }

  void Input::onControllerButtonReleased( Nil::Controller* controller, const Nil::ControllerState& state, size_t button )
  {
    // mEngine->getConsole()->printf( Console::srcInput, L"controller button released" );
  }

  void Input::onControllerAxisMoved( Nil::Controller* controller, const Nil::ControllerState& state, size_t axis )
  {
    // mEngine->getConsole()->printf( Console::srcInput, L"controller axis moved" );
  }

  void Input::onControllerSliderMoved( Nil::Controller* controller, const Nil::ControllerState& state, size_t slider )
  {
    // mEngine->getConsole()->printf( Console::srcInput, L"controller slider moved" );
  }

  void Input::onControllerPOVMoved( Nil::Controller* controller, const Nil::ControllerState& state, size_t pov )
  {
    // mEngine->getConsole()->printf( Console::srcInput, L"controller pov moved" );
  }

  Input::~Input()
  {
    SAFE_DELETE( mSystem );
  }

}