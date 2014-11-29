#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "InputManager.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Keyboard {

    class Device: public Nil::KeyboardListener {
    protected:
      Nil::Keyboard* mKeyboard;
      BindAction mKeyBindTable[0xFF];
      ActionPacket mActions;
    protected:
      void onKeyPressed(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
      void onKeyRepeat(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
      void onKeyReleased(
        Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode );
    public:
      Device( Nil::Keyboard* keyboard );
      void prepare();
      void cancelAllInput();
      void clearActions();
      void resetBinds();
      virtual ~Device();
    };

  }

}