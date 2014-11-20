#pragma once
#include "Types.h"
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class InputManager: public EngineComponent, public Nil::SystemListener,
    public Nil::MouseListener, public Nil::KeyboardListener,
    public Nil::ControllerListener {
  protected:
    Nil::System* mSystem;
    bool mTakingInput;
  protected:
    // System handlers
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
    // Mouse handlers
    virtual void onMouseMoved(
      Nil::Mouse* mouse, const Nil::MouseState& state );
    virtual void onMouseButtonPressed(
      Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
    virtual void onMouseButtonReleased(
      Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
    virtual void onMouseWheelMoved(
      Nil::Mouse* mouse, const Nil::MouseState& state );
    // Keyboard handlers
    virtual void onKeyPressed(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    virtual void onKeyRepeat(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    virtual void onKeyReleased(
      Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    // Controller handlers
    virtual void onControllerButtonPressed( Nil::Controller* controller,
      const Nil::ControllerState& state, size_t button );
    virtual void onControllerButtonReleased( Nil::Controller* controller,
      const Nil::ControllerState& state, size_t button );
    virtual void onControllerAxisMoved( Nil::Controller* controller,
      const Nil::ControllerState& state, size_t axis );
    virtual void onControllerSliderMoved( Nil::Controller* controller,
      const Nil::ControllerState& state, size_t slider );
    virtual void onControllerPOVMoved( Nil::Controller* controller,
      const Nil::ControllerState& state, size_t pov );
  public:
    InputManager( Engine* engine, HINSTANCE instance, Ogre::RenderWindow* window );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void onInputFocus( const bool focus );
    virtual ~InputManager();
  };

  //! @}

}