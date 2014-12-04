#pragma once
#include "AudioService.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class NullAudio: public Audio {
  protected:
    Settings mSettings;
  public:
    NullAudio()
    {
      mDrivers.push_back( new Driver( L"None" ) );
      mOutputTypes.push_back( new OutputType( L"none", L"None" ) );
      mSpeakerModes.push_back( new SpeakerMode( L"none", L"None" ) );
    }
    virtual void applySettings( const Settings& settings ) { mSettings = settings; }
    virtual void setMasterVolume( float volume ) { /* Nothing */ }
    virtual void setMusicVolume( float volume ) { /* Nothing */ }
    virtual void setEffectVolume( float volume ) { /* Nothing */ }
    virtual void initialize() { /* Nothing */ }
    virtual void shutdown() { /* Nothing */ }
    virtual void restart() { /* Nothing */ }
  };

  class Music {
  public:
    virtual void beginScene() = 0;
    virtual void endScene() = 0;
  };

  class NullMusic: public Music {
  public:
    virtual void beginScene() { /* Nothing */ }
    virtual void endScene() { /* Nothing */ }
  };

  class Physics {
  public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void restart() = 0;
  };

  class NullPhysics: public Physics {
  public:
    virtual void initialize() { /* Nothing */ }
    virtual void shutdown() { /* Nothing */ }
    virtual void restart() { /* Nothing */ }
  };

  class Memory {
  public:
    enum Sector {
      Sector_Generic,
      Sector_Audio,
      Sector_Physics
    };
    virtual void* alloc( const Sector sector, size_t size, size_t alignment = 0Ui64 ) = 0;
    virtual void* realloc( const Sector sector, void* location, size_t size, size_t alignment = 0Ui64 ) = 0;
    virtual void free( const Sector sector, void* location ) = 0;
    virtual size_t getMemoryUsage( const Sector sector ) = 0;
    virtual const wstring& getProviderName() = 0;
  };

}