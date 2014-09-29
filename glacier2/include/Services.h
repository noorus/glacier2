#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Audio {
  public:
    struct AudioDriverEntry {
    public:
      int index;
      wstring name;
    };
    struct OutputTypeEntry {
    public:
      int index;
      wstring name;
    };
    struct SpeakerModeEntry {
    public:
      int index;
      wstring name;
    };
    typedef std::list<AudioDriverEntry> AudioDriverList;
    typedef std::list<OutputTypeEntry> OutputTypeList;
    typedef std::list<SpeakerModeEntry> SpeakerModeList;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void restart() = 0;
    virtual const AudioDriverList& getDrivers() = 0;
    virtual const OutputTypeList& getOutputTypes() = 0;
    virtual const SpeakerModeList& getSpeakerModes() = 0;
  };

  class NullAudio: public Audio {
  protected:
    AudioDriverList emptyDriverList;
    OutputTypeList emptyOutputTypeList;
    SpeakerModeList emptySpeakerModeList;
  public:
    virtual void initialize() { /* Nothing */ }
    virtual void shutdown() { /* Nothing */ }
    virtual void restart() { /* Nothing */ }
    virtual const AudioDriverList& getDrivers() { return emptyDriverList; }
    virtual const OutputTypeList& getOutputTypes() { return emptyOutputTypeList; }
    virtual const SpeakerModeList& getSpeakerModes() { return emptySpeakerModeList; }
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

  //

}