#include "StdAfx.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "Character.h"
#include "World.h"
#include "Entity.h"
#include "Actions.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  AICharacterInputComponent::AICharacterInputComponent( Character* character ):
  CharacterInputComponent( character )
  {
    //
  }

  void AICharacterInputComponent::update( const ActionPacket& action, GameTime delta )
  {
    //
  }

  AICharacterInputComponent::~AICharacterInputComponent()
  {
    //
  }

}