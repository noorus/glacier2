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
    InputDevice( local ), keyboard_( keyboard ), focused_( true )
    {
      resetBinds();

      keyBindTable_[0x57] = Action_Move_Forward;
      keyBindTable_[0x53] = Action_Move_Backward;
      keyBindTable_[0x41] = Action_Sidestep_Left;
      keyBindTable_[0x44] = Action_Sidestep_Right;
      keyBindTable_[0x20] = Action_Jump;
      keyBindTable_[0xA0] = Action_Run;
      keyBindTable_[0xA2] = Action_Crouch;
      keyBindTable_[0xDC] = Action_Toggle_Console;
    }

    void Device::onKeyPressed( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !focused_ )
        return;

      const BindAction& action = keyBindTable_[keycode];

      if ( action != Action_None )
        controller_->beginAction( this, action );
    }

    void Device::onKeyRepeat( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !focused_ )
        return;

      const BindAction& action = keyBindTable_[keycode];
    }

    void Device::onKeyReleased( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      if ( !focused_ )
        return;

      const BindAction& action = keyBindTable_[keycode];

      if ( action != Action_None )
        controller_->endAction( this, action );
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      focused_ = focus;

      /*if ( !focus )
      {
        mActions.move = Player_Move_None;
        mActions.sidestep = Player_Sidestep_None;
      }*/
    }

    void Device::resetBinds()
    {
      memset( keyBindTable_, Action_None, 0xFF * sizeof( BindAction ) );
    }

    Device::~Device()
    {
      //
    }

  }

}