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
    mActions.move = Character_Move_None;
    mActions.sidestep = Character_Sidestep_None;
    mActions.jump = Character_Jump_None;
    mActions.run = Character_Run_None;
    mActions.crouch = Character_Crouch_None;
    mDirectional = Vector2::ZERO;
  }

  void GameController::prepare()
  {
    mActions.jump = Character_Jump_None;
    mActions.run = Character_Run_None;
    mActions.crouch = Character_Crouch_None;
  }

  void GameController::apply(
  CharacterMoveMode moveMode, const Vector3& direction )
  {
    if ( mCharacter )
      mCharacter->setActions( mActions, moveMode, direction, mDirectional );
  }

}