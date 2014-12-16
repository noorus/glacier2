#pragma once
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Graphics;
  class GUI;
  class EntityManager;
  class Music;
  class Colors;

  //! \class Locator
  //! Central game services locator.
  class Locator {
  private:
    static Memory* memoryService; //!< Currently provided memory service
    static Audio* audioService; //!< Currently provided audio service
    static NullAudio nullAudioService; //!< Default null audio service
    static Physics* physicsService; //!< Currently provided physics service
    static NullPhysics nullPhysicsService; //!< Default null physics service
    static Graphics* graphicsService; //!< Graphics component
    static GUI* guiService; //!< GUI component
    static EntityManager* entityManager; //!< World entities manager
    static NullMusic nullMusic; //!< Default null music service
    static Music* musicPlayer; //!< Music player
    static Colors colors; //!< Colors
  public:
    static const bool hasMemory() { return ( memoryService ? true : false ); }

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

    static EntityManager& getEntities() { return *entityManager; }

    static void provideEntities( EntityManager* entities )
    {
      entityManager = entities;
    }

    static Music& getMusic() { return *musicPlayer; }

    static void provideMusic( Music* player )
    {
      musicPlayer = player;
    }

    static Colors& getColors() { return colors; }
  };

}