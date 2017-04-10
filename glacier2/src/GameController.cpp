#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Actions.h"
#include "Console.h"
#include "ServiceLocator.h"
#include "Controllers.h"
#include "InputManager.h"
#include "Character.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  GameController::GameController(): mCharacter( nullptr )
  {
    resetActions();
  }

  void GameController::setCharacter( Character* character )
  {
    mCharacter = character;
  }

  void GameController::resetActions()
  {
    actions_.move = Character_Move_None;
    actions_.sidestep = Character_Sidestep_None;
    actions_.jump = Character_Jump_None;
    actions_.run = Character_Run_None;
    actions_.crouch = Character_Crouch_None;
    directional_ = Vector2::ZERO;
  }

  void GameController::prepare()
  {
    actions_.jump = Character_Jump_None;
    actions_.run = Character_Run_None;
    actions_.crouch = Character_Crouch_None;
  }

  void GameController::apply(
  CharacterMoveMode moveMode, const Vector3& direction )
  {
    if ( mCharacter )
      mCharacter->setActions( actions_, moveMode, direction, directional_ );
  }

}