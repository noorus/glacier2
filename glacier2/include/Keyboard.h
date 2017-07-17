#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "InputManager.h"
#include "Actions.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Input
  //! @{

  namespace Keyboard {

    class Device: public InputDevice, public Nil::KeyboardListener {
    protected:
      Nil::Keyboard* keyboard_;
      BindAction keyBindTable_[0xFF];
      bool focused_;
    protected:
      void onKeyPressed(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
      void onKeyRepeat(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
      void onKeyReleased(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    public:
      Device( LocalController* local, Nil::Keyboard* keyboard );
      virtual void prepare();
      virtual void onFocus( const bool focus );
      virtual const Type getType() { return Type_Keyboard; }
      void resetBinds();
      virtual ~Device();
    };

  }

  //! @}

  //! @}

}