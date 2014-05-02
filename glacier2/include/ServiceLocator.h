#pragma once
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Locator {
  private:
    static Memory* memoryService; //!< Currently provided memory service
    static Audio* audioService; //!< Currently provided audio service
    static NullAudio nullAudioService; //!< Default null audio service
  public:
    static Memory& getMemory() { return *memoryService; }

    static void provideMemory( Memory* memory )
    {
      assert( memory );
      memoryService = memory;
    }

    static Audio& getAudio() { return *audioService; }

    static void provideAudio( Audio* audio )
    {
      audioService = audio ? audio : &nullAudioService;
    }
  };

}