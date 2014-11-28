#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  InputManager::InputManager( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window ):
  EngineComponent( engine ), mSystem( nullptr ), mTakingInput( false )
  {
    // Greetings
    mEngine->getConsole()->printf( Console::srcInput,
      L"Initializing Nice Input Library" );

    // Get window handle
    HWND windowHandle = NULL;
    window->getCustomAttribute( "WINDOW", &windowHandle );
    if ( !windowHandle )
      ENGINE_EXCEPT( "Could not retrieve render window handle" );

    // Create input system
    mSystem = new Nil::System( instance, windowHandle,
      Nil::Cooperation_Foreground, this );

    // Enable all initially connected devices
    for ( auto device : mSystem->getDevices() )
      device->enable();
  }

  void InputManager::componentTick( GameTime tick, GameTime time )
  {
    mEngine->getActionManager()->prepare();
    mSystem->update();
  }

  void InputManager::onInputFocus( const bool focus )
  {
    mTakingInput = focus;

    if ( !mTakingInput )
      mEngine->getActionManager()->cancelAllInput();
  }

  void InputManager::onDeviceConnected( Nil::Device* device )
  {
    device->enable();
  }
  
  void InputManager::onDeviceDisconnected( Nil::Device* device )
  {
  }

  void InputManager::onMouseEnabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled mouse: %S", device->getName().c_str() );

    instance->addListener( this );
  }

  void InputManager::onKeyboardEnabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled keyboard: %S", device->getName().c_str() );

    instance->addListener( this );
  }

  void InputManager::onControllerEnabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled controller: %S", device->getName().c_str() );
  }

  void InputManager::onMouseDisabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled mouse: %S", device->getName().c_str() );
  }

  void InputManager::onKeyboardDisabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled keyboard: %S", device->getName().c_str() );
  }

  void InputManager::onControllerDisabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled controller: %S", device->getName().c_str() );
  }

  // Mouse

  void InputManager::onMouseMoved( Nil::Mouse* mouse,
  const Nil::MouseState& state )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onMouseMoved( state );
  }

  void InputManager::onMouseButtonPressed( Nil::Mouse* mouse,
  const Nil::MouseState& state, size_t button )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onMouseButtonPressed( state, button );
  }

  void InputManager::onMouseButtonReleased( Nil::Mouse* mouse,
  const Nil::MouseState& state, size_t button )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onMouseButtonReleased( state, button );
  }

  void InputManager::onMouseWheelMoved( Nil::Mouse* mouse,
  const Nil::MouseState& state )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onMouseWheelMoved( state );
  }

  // Keyboard
  
  void InputManager::onKeyPressed( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onKeyPressed( keycode );
  }

  void InputManager::onKeyRepeat( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onKeyRepeat( keycode );
  }

  void InputManager::onKeyReleased( Nil::Keyboard* keyboard,
  const Nil::VirtualKeyCode keycode )
  {
    if ( !mTakingInput )
      return;

    mEngine->getActionManager()->onKeyReleased( keycode );
  }

  InputManager::~InputManager()
  {
    SAFE_DELETE( mSystem );
  }

}