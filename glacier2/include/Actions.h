#pragma once
#include "Utilities.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  enum BindAction {
    Action_None = 0,
    Action_Move_Forward,
    Action_Move_Backward,
    Action_Sidestep_Left,
    Action_Sidestep_Right,
    Action_Jump,
    Action_Crouch,
    Action_Run,
    Action_Zoom,
    Action_Rotate,
    Action_Toggle_Console = 900
  };

  enum CharacterMoveAction {
    Character_Move_None = 0,
    Character_Move_Forward,
    Character_Move_Backward
  };

  enum CharacterSidestepAction {
    Character_Sidestep_None = 0,
    Character_Sidestep_Left,
    Character_Sidestep_Right
  };

  enum CharacterJumpAction {
    Character_Jump_None = 0,
    Character_Jump_Keydown,
    Character_Jump_Keyup
  };

  enum CharacterRunAction {
    Character_Run_None = 0,
    Character_Run_Keydown,
    Character_Run_Keyup
  };

  enum CharacterCrouchAction {
    Character_Crouch_None = 0,
    Character_Crouch_Keydown,
    Character_Crouch_Keyup
  };

  struct ActionPacket {
  public:
    ActionPacket(): move( Character_Move_None ),
      sidestep( Character_Sidestep_None ),
      jump( Character_Jump_None ),
      run( Character_Run_None ),
      crouch( Character_Crouch_None ) {}
    CharacterMoveAction move;
    CharacterSidestepAction sidestep;
    CharacterJumpAction jump;
    CharacterRunAction run;
    CharacterCrouchAction crouch;
  };

}