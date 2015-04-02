#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "Controllers.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  namespace Mouse { class Device; }
  namespace Keyboard { class Device; }
  namespace Gamepad { class Device; }

  class InputDevice {
  public:
    enum Type {
      Type_Keyboard,
      Type_Mouse,
      Type_Gamepad
    };
  protected:
    LocalController* mController;
  public:
    InputDevice( LocalController* controller ): mController( controller ) {}
    virtual void prepare() = 0;
    virtual void onFocus( const bool focus ) = 0;
    virtual const Type getType() = 0;
    virtual LocalController* getController() { return mController; }
  };

  typedef std::map<Nil::DeviceID, Mouse::Device*> MouseMap;
  typedef std::map<Nil::DeviceID, Keyboard::Device*> KeyboardMap;
  typedef std::map<Nil::DeviceID, Gamepad::Device*> GamepadMap;

  class InputManager: public EngineComponent, public Nil::SystemListener {
  protected:
    Nil::System* mSystem;
    MouseMap mMice;
    KeyboardMap mKeyboards;
    GamepadMap mGamepads;
    bool mTakingInput;
    LocalController* mLocalController;
  protected:
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
    InputManager( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual LocalController* getLocalController();
    virtual void onInputFocus( const bool focus );
    virtual ~InputManager();
  };

  //! @}

}