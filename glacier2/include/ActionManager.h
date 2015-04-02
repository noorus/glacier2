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

  enum PlayerMoveAction {
    Player_Move_None = 0,
    Player_Move_Forward,
    Player_Move_Backward
  };

  enum PlayerSidestepAction {
    Player_Sidestep_None = 0,
    Player_Sidestep_Left,
    Player_Sidestep_Right
  };

  enum PlayerJumpAction {
    Player_Jump_None = 0,
    Player_Jump_Keydown,
    Player_Jump_Keyup
  };

  enum PlayerRunAction {
    Player_Run_None = 0,
    Player_Run_Keydown,
    Player_Run_Keyup
  };

  enum PlayerCrouchAction {
    Player_Crouch_None = 0,
    Player_Crouch_Keydown,
    Player_Crouch_Keyup
  };

  struct ActionPacket {
  public:
    PlayerMoveAction move;
    PlayerSidestepAction sidestep;
    PlayerJumpAction jump;
    PlayerRunAction run;
    PlayerCrouchAction crouch;
  };

}