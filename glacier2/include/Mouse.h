#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "InputManager.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    class Device: public InputDevice, public Nil::MouseListener {
    public:
      typedef std::vector<BindAction> ButtonVector;
    protected:
      Nil::Mouse* mMouse;
      ButtonVector mButtons;
      ActionPacket mActions;
      bool mFocused;
    protected:
      void onMouseMoved(
        Nil::Mouse* mouse, const Nil::MouseState& state );
      void onMouseButtonPressed(
        Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
      void onMouseButtonReleased(
        Nil::Mouse* mouse, const Nil::MouseState& state, size_t button );
      void onMouseWheelMoved(
        Nil::Mouse* mouse, const Nil::MouseState& state );
    public:
      Device( LocalController* local, Nil::Mouse* mouse );
      virtual void prepare();
      virtual void onFocus( const bool focus );
      virtual const Type getType() { return Type_Mouse; }
      virtual ~Device();
    };

  }

}