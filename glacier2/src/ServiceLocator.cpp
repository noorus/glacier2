#include "StdAfx.h"
#include "ServiceLocator.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  Memory* Locator::memoryService = nullptr;
  Graphics* Locator::graphicsService = nullptr;

  NullAudio Locator::nullAudioService;
  Audio* Locator::audioService = &Locator::nullAudioService;

  NullPhysics Locator::nullPhysicsService;
  Physics* Locator::physicsService = &Locator::nullPhysicsService;

}