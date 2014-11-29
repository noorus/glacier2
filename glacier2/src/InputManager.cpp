#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Gamepad.h"

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
    gEngine->getActionManager()->prepare();

    for ( auto mouse : mMice )
      mouse.second->prepare();
    for ( auto keyboard : mKeyboards )
      keyboard.second->prepare();
    for ( auto gamepad : mGamepads )
      gamepad.second->prepare();

    mSystem->update();
  }

  void InputManager::onInputFocus( const bool focus )
  {
    mTakingInput = focus;

    for ( auto mouse : mMice )
      mouse.second->onFocus( focus );
    for ( auto keyboard : mKeyboards )
      keyboard.second->onFocus( focus );
    for ( auto gamepad : mGamepads )
      gamepad.second->onFocus( focus );
  }

  void InputManager::onDeviceConnected( Nil::Device* device )
  {
    device->enable();
  }
  
  void InputManager::onDeviceDisconnected( Nil::Device* device )
  {
    device->disable();
  }

  void InputManager::onMouseEnabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled mouse: %S", device->getName().c_str() );

    if ( mMice.find( device->getStaticID() ) != mMice.end() )
      delete mMice[device->getStaticID()];

    auto mouse = new Mouse::Device( instance );
    mMice[device->getStaticID()] = mouse;
    instance->addListener( mouse );
  }

  void InputManager::onKeyboardEnabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled keyboard: %S", device->getName().c_str() );

    if ( mKeyboards.find( device->getStaticID() ) != mKeyboards.end() )
      delete mKeyboards[device->getStaticID()];

    auto keyboard = new Keyboard::Device( instance );
    mKeyboards[device->getStaticID()] = keyboard;
    instance->addListener( keyboard );
  }

  void InputManager::onControllerEnabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled controller: %S", device->getName().c_str() );

    if ( mGamepads.find( device->getStaticID() ) != mGamepads.end() )
      delete mGamepads[device->getStaticID()];

    auto gamepad = new Gamepad::Device( instance );
    mGamepads[device->getStaticID()] = gamepad;
    instance->addListener( gamepad );
  }

  void InputManager::onMouseDisabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled mouse: %S", device->getName().c_str() );

    if ( mMice.find( device->getStaticID() ) != mMice.end() )
      delete mMice[device->getStaticID()];
  }

  void InputManager::onKeyboardDisabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled keyboard: %S", device->getName().c_str() );

    if ( mKeyboards.find( device->getStaticID() ) != mKeyboards.end() )
      delete mKeyboards[device->getStaticID()];
  }

  void InputManager::onControllerDisabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled controller: %S", device->getName().c_str() );

    if ( mGamepads.find( device->getStaticID() ) != mGamepads.end() )
      delete mGamepads[device->getStaticID()];
  }

  InputManager::~InputManager()
  {
    SAFE_DELETE( mSystem );
  }

}