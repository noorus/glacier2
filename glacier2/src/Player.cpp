#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "Character.h"
#include "World.h"
#include "Entity.h"
#include "Actions.h"
#include "InputManager.h"
#include "Player.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_ENTITY( player, Player );

  Player::Player( World* world ): Character( world, &baseData )
  {
    //
  }

  void Player::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Character::spawn( position, orientation );
  }

  void Player::think( const GameTime delta )
  {
    Character::think( delta );
  }

  Player::~Player()
  {
    //
  }

}