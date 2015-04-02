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

  CharacterInputComponent::CharacterInputComponent( Character* character ):
  mCharacter( character )
  {
    //
  }

  CharacterInputComponent::~CharacterInputComponent()
  {
    //
  }

}