#pragma once
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_EXTERN_CONVAR( fm_volume );
  ENGINE_EXTERN_CONVAR( fm_bgvolume );
  ENGINE_EXTERN_CONVAR( fm_fxvolume );

  //! \class Audio
  //! Virtual base class for game audio services.
  class Audio {
  public:
    // Audio driver entry
    struct Driver {
    public:
      int index;
      wstring name;
      Driver( int _index, const wstring& _name ):
        index( _index ), name( _name ) {}
      virtual ~Driver() {}
    };
    typedef std::list<Driver*> DriverList;

    // Audio output type entry
    struct OutputType {
    public:
      int index;
      wstring shorthand;
      wstring name;
      OutputType( int _index, const wstring& _shorthand, const wstring& _name ):
        index( _index ), shorthand( _shorthand ), name( _name ) {}
      virtual ~OutputType() {}
    };
    typedef std::list<OutputType*> OutputTypeList;

    // Audio speaker mode entry
    struct SpeakerMode {
    public:
      int index;
      wstring shorthand;
      wstring name;
      SpeakerMode( int _index, const wstring& _shorthand, const wstring& _name ):
        index( _index ), shorthand( _shorthand ), name( _name ) {}
      virtual ~SpeakerMode() {}
    };
    typedef std::list<SpeakerMode*> SpeakerModeList;

    // Audio settings entry
    struct Settings {
      int driver;
      int outputType;
      int speakerMode;
      Settings(): driver( 0 ), outputType( 0 ), speakerMode( 0 ) {}
    };
  protected:
    DriverList mDrivers;
    OutputTypeList mOutputTypes;
    SpeakerModeList mSpeakerModes;
    Settings mSettings;
  public:
    virtual void setMasterVolume( float volume ) = 0;
    virtual void setMusicVolume( float volume ) = 0;
    virtual void setEffectVolume( float volume ) = 0;
    static bool callbackMasterVolume( ConVar* variable, ConVar::Value oldValue );
    static bool callbackMusicVolume( ConVar* variable, ConVar::Value oldValue );
    static bool callbackEffectVolume( ConVar* variable, ConVar::Value oldValue );
  public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void restart() = 0;
    virtual void clearDrivers()
    {
      for ( auto it : mDrivers )
        delete it;
      mDrivers.clear();
    }
    virtual void clearOutputTypes()
    {
      for ( auto it : mOutputTypes )
        delete it;
      mOutputTypes.clear();
    }
    virtual void clearSpeakerModes()
    {
      for ( auto it : mSpeakerModes )
        delete it;
      mSpeakerModes.clear();
    }
    virtual const Settings& getSettings() { return mSettings; }
    virtual const DriverList& getDrivers() { return mDrivers; }
    virtual const OutputTypeList& getOutputTypes() { return mOutputTypes; }
    virtual const SpeakerModeList& getSpeakerModes() { return mSpeakerModes; }
    virtual ~Audio()
    {
      clearDrivers();
      clearOutputTypes();
      clearSpeakerModes();
    }
  };

}