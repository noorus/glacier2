#include "StdAfx.h"
#include "ServiceLocator.h"

namespace Glacier {

  Memory* Locator::memoryService = nullptr;

  NullAudio Locator::nullAudioService;
  Audio* Locator::audioService = &Locator::nullAudioService;

  NullPhysics Locator::nullPhysicsService;
  Physics* Locator::physicsService = &Locator::nullPhysicsService;

}