#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Actions.h"
#include "ConsoleWindow.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Gamepad.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  InputManager::InputManager( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window ):
  EngineComponent( engine ), system_( nullptr ), takingInput_( false ), localController_( nullptr )
  {
    localController_ = new LocalController();

    // Greetings
    mEngine->getConsole()->printf( Console::srcInput,
      L"Initializing input" );

    // Get window handle
    HWND windowHandle = NULL;
    window->getCustomAttribute( "WINDOW", &windowHandle );
    if ( !windowHandle )
      ENGINE_EXCEPT( "Could not retrieve render window handle" );

    // Create input system
    system_ = new Nil::System( instance, windowHandle,
      Nil::Cooperation_Foreground, this );

    // Enable all initially connected devices
    for ( auto device : system_->getDevices() )
      device->enable();
  }

  void InputManager::componentTick( GameTime tick, GameTime time )
  {
    localController_->prepare();

    for ( auto mouse : mice_ )
      mouse.second->prepare();
    for ( auto keyboard : keyboards_ )
      keyboard.second->prepare();
    for ( auto gamepad : gamepads_ )
      gamepad.second->prepare();

    system_->update();

    localController_->apply();
  }

  void InputManager::onInputFocus( const bool focus )
  {
    takingInput_ = focus;

    for ( auto mouse : mice_ )
      mouse.second->onFocus( focus );
    for ( auto keyboard : keyboards_ )
      keyboard.second->onFocus( focus );
    // for ( auto gamepad : mGamepads )
    //   gamepad.second->onFocus( focus );
  }

  LocalController* InputManager::getLocalController()
  {
    return localController_;
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

    if ( mice_.find( device->getStaticID() ) != mice_.end() )
      delete mice_[device->getStaticID()];

    auto mouse = new Mouse::Device( localController_, instance );
    mice_[device->getStaticID()] = mouse;
    instance->addListener( mouse );
  }

  void InputManager::onKeyboardEnabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled keyboard: %S", device->getName().c_str() );

    if ( keyboards_.find( device->getStaticID() ) != keyboards_.end() )
      delete keyboards_[device->getStaticID()];

    auto keyboard = new Keyboard::Device( localController_, instance );
    keyboards_[device->getStaticID()] = keyboard;
    instance->addListener( keyboard );
  }

  void InputManager::onControllerEnabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Enabled controller: %S", device->getName().c_str() );

    if ( gamepads_.find( device->getStaticID() ) != gamepads_.end() )
      delete gamepads_[device->getStaticID()];

    auto gamepad = new Gamepad::Device( localController_, instance );
    gamepads_[device->getStaticID()] = gamepad;
    instance->addListener( gamepad );
  }

  void InputManager::onMouseDisabled( Nil::Device* device, Nil::Mouse* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled mouse: %S", device->getName().c_str() );

    if ( mice_.find( device->getStaticID() ) != mice_.end() )
      delete mice_[device->getStaticID()];
  }

  void InputManager::onKeyboardDisabled( Nil::Device* device, Nil::Keyboard* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled keyboard: %S", device->getName().c_str() );

    if ( keyboards_.find( device->getStaticID() ) != keyboards_.end() )
      delete keyboards_[device->getStaticID()];
  }

  void InputManager::onControllerDisabled( Nil::Device* device, Nil::Controller* instance )
  {
    mEngine->getConsole()->printf( Console::srcInput,
      L"Disabled controller: %S", device->getName().c_str() );

    if ( gamepads_.find( device->getStaticID() ) != gamepads_.end() )
      delete gamepads_[device->getStaticID()];
  }

  InputManager::~InputManager()
  {
    SAFE_DELETE( system_ );
    SAFE_DELETE( localController_ );
  }

}