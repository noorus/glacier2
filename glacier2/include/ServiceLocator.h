#pragma once
#include "Services.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  class Graphics;
  class GUI;

  class Locator {
  private:
    static Memory* memoryService; //!< Currently provided memory service
    static Audio* audioService; //!< Currently provided audio service
    static NullAudio nullAudioService; //!< Default null audio service
    static Physics* physicsService; //!< Currently provided physics service
    static NullPhysics nullPhysicsService; //!< Default null physics service
    static Graphics* graphicsService;
    static GUI* guiService;
  public:
    static Memory& getMemory() { return *memoryService; }

    static void provideMemory( Memory* memory )
    {
      memoryService = memory;
    }

    static Audio& getAudio() { return *audioService; }

    static void provideAudio( Audio* audio )
    {
      audioService = audio ? audio : &nullAudioService;
    }

    static Physics& getPhysics() { return *physicsService; }

    static void providePhysics( Physics* physics )
    {
      physicsService = physics ? physics : &nullPhysicsService;
    }

    static Graphics& getGraphics() { return *graphicsService; }

    static void provideGraphics( Graphics* graphics )
    {
      graphicsService = graphics;
    }

    static GUI& getGUI() { return *guiService; }

    static void provideGUI( GUI* gui )
    {
      guiService = gui;
    }
  };

}