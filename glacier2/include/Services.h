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
      mDrivers.push_back( new Driver( 0, L"None" ) );
      mOutputTypes.push_back( new OutputType( 0, L"none", L"None" ) );
      mSpeakerModes.push_back( new SpeakerMode( 0, L"none", L"None" ) );
    }
    virtual void setMasterVolume( float volume ) { /* Nothing */ }
    virtual void setMusicVolume( float volume ) { /* Nothing */ }
    virtual void setEffectVolume( float volume ) { /* Nothing */ }
    virtual void initialize() { /* Nothing */ }
    virtual void shutdown() { /* Nothing */ }
    virtual void restart() { /* Nothing */ }
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