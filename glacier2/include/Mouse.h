#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "InputManager.h"
#include "Actions.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Mouse {

    class Device: public InputDevice, public Nil::MouseListener {
    public:
      typedef std::vector<BindAction> ButtonVector;
    protected:
      Nil::Mouse* mouse_;
      ButtonVector buttons_;
      ActionPacket actions_;
      MousePacket packet_;
      bool focused_;
    protected:
      void updatePacket( const Nil::MouseState& state );
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
      const MousePacket& getMousePacket() const { return packet_; }
      virtual void prepare();
      virtual void onFocus( const bool focus );
      virtual const Type getType() { return Type_Mouse; }
      virtual ~Device();
    };

  }

}