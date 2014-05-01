#pragma once
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Locator {
  private:
    static Audio* audioService;
    static NullAudio nullAudioService;
  public:
    static Audio& getAudio() { return *audioService; }

    static void provideAudio( Audio* audio )
    {
      audioService = audio ? audio : &nullAudioService;
    }
  };

}