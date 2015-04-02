#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Keyboard.h"
#include "Controllers.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Keyboard {

    Device::Device( LocalController* local, Nil::Keyboard* keyboard ):
    InputDevice( local ), mKeyboard( keyboard ), mFocused( true )
    {
      resetBinds();

      mKeyBindTable[0x57] = Action_Move_Forward;
      mKeyBindTable[0x53] = Action_Move_Backward;
      mKeyBindTable[0x41] = Action_Sidestep_Left;
      mKeyBindTable[0x44] = Action_Sidestep_Right;
      mKeyBindTable[0x20] = Action_Jump;
      mKeyBindTable[0xA0] = Action_Run;
      mKeyBindTable[0xA2] = Action_Crouch;
      mKeyBindTable[0xDC] = Action_Toggle_Console;
    }

    void Device::onKeyPressed( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !mFocused )
        return;

      const BindAction& action = mKeyBindTable[keycode];

      if ( action != Action_None )
        mController->beginAction( this, action );
    }

    void Device::onKeyRepeat( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !mFocused )
        return;

      const BindAction& action = mKeyBindTable[keycode];
    }

    void Device::onKeyReleased( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !mFocused )
        return;

      const BindAction& action = mKeyBindTable[keycode];

      if ( action != Action_None )
        mController->endAction( this, action );
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      mFocused = focus;

      /*if ( !focus )
      {
        mActions.move = Player_Move_None;
        mActions.sidestep = Player_Sidestep_None;
      }*/
    }

    void Device::resetBinds()
    {
      memset( mKeyBindTable, Action_None, 0xFF * sizeof( BindAction ) );
    }

    Device::~Device()
    {
      //
    }

  }

}