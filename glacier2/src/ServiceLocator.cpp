#include "StdAfx.h"
#include "ServiceLocator.h"
#include "PhysXPhysics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Memory* Locator::memoryService = nullptr;
  Graphics* Locator::graphicsService = nullptr;
  EntityManager* Locator::entityManager = nullptr;
  PhysXPhysics* Locator::physicsService = nullptr;
  HUD* Locator::hud = nullptr;

  NullAudio Locator::nullAudioService;
  Audio* Locator::audioService = &Locator::nullAudioService;

  NullMusic Locator::nullMusic;
  Music* Locator::musicPlayer = &Locator::nullMusic;

  Colors Locator::colors;
}