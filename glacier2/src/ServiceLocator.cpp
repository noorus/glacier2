#include "StdAfx.h"
#include "ServiceLocator.h"

namespace Glacier {

  NullAudio Locator::nullAudioService;
  Audio* Locator::audioService = &Locator::nullAudioService;
  Memory* Locator::memoryService = nullptr;

}