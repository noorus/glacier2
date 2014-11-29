#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"
#include "Keyboard.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Keyboard {

    Device::Device( Nil::Keyboard* keyboard ): mKeyboard( keyboard )
    {
      resetBinds();
      clearActions();

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
      const BindAction& action = mKeyBindTable[keycode];

      gEngine->getConsole()->printf( Console::srcInput, L"Key press: 0x%X", keycode );

      switch ( action )
      {
        case Action_Move_Forward:
          if ( mActions.move == Player_Move_None )
            mActions.move = Player_Move_Forward;
          break;
        case Action_Move_Backward:
          if ( mActions.move == Player_Move_None )
            mActions.move = Player_Move_Backward;
          break;
        case Action_Sidestep_Left:
          if ( mActions.sidestep == Player_Sidestep_None )
            mActions.sidestep = Player_Sidestep_Left;
          break;
        case Action_Sidestep_Right:
          if ( mActions.sidestep == Player_Sidestep_None )
            mActions.sidestep = Player_Sidestep_Right;
          break;
        case Action_Jump:
          mActions.jump = Player_Jump_Keydown;
          break;
        case Action_Run:
          mActions.run = Player_Run_Keydown;
          break;
        case Action_Crouch:
          mActions.crouch = Player_Crouch_Keydown;
          break;
      }

      // Missing: zoom, console
    }

    void Device::onKeyRepeat( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      const BindAction& action = mKeyBindTable[keycode];
    }

    void Device::onKeyReleased( Nil::Keyboard* keyboard, const Nil::VirtualKeyCode keycode )
    {
      const BindAction& action = mKeyBindTable[keycode];

      switch ( action )
      {
        case Action_Move_Forward:
          if ( mActions.move == Player_Move_Forward )
            mActions.move = Player_Move_None;
          break;
        case Action_Move_Backward:
          if ( mActions.move == Player_Move_Backward )
            mActions.move = Player_Move_None;
          break;
        case Action_Sidestep_Left:
          if ( mActions.sidestep == Player_Sidestep_Left )
            mActions.sidestep = Player_Sidestep_None;
          break;
        case Action_Sidestep_Right:
          if ( mActions.sidestep == Player_Sidestep_Right )
            mActions.sidestep = Player_Sidestep_None;
          break;
        case Action_Jump:
          mActions.jump = Player_Jump_Keyup;
          break;
        case Action_Run:
          mActions.run = Player_Run_Keyup;
          break;
        case Action_Crouch:
          mActions.crouch = Player_Crouch_Keyup;
          break;
      }

      // Missing: zoom, console
    }

    void Device::prepare()
    {
      mActions.jump = Player_Jump_None;
      mActions.run = Player_Run_None;
      mActions.crouch = Player_Crouch_None;
      /*if ( !mDirectional.isZeroLength() && mActions.move == Player_Move_None )
      mActions.move = Player_Move_Forward;
      else if ( mDirectional.isZeroLength() && mActions.move == Player_Move_Forward )
      mActions.move = Player_Move_None;*/
    }

    void Device::cancelAllInput()
    {
      mActions.move = Player_Move_None;
      mActions.sidestep = Player_Sidestep_None;
    }

    void Device::clearActions()
    {
      mActions.move = Player_Move_None;
      mActions.sidestep = Player_Sidestep_None;
      mActions.jump = Player_Jump_None;
      mActions.run = Player_Run_None;
      mActions.crouch = Player_Crouch_None;
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