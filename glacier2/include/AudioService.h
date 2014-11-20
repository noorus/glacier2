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
      wstring name;
      Driver( const wstring& _name ): name( _name ) {}
      virtual ~Driver() {}
    };
    typedef std::vector<Driver*> DriverVector;

    // Audio output type entry
    struct OutputType {
    public:
      wstring shorthand;
      wstring name;
      OutputType( const wstring& _shorthand, const wstring& _name ):
        shorthand( _shorthand ), name( _name ) {}
      virtual ~OutputType() {}
    };
    typedef std::vector<OutputType*> OutputTypeVector;

    // Audio speaker mode entry
    struct SpeakerMode {
    public:
      wstring shorthand;
      wstring name;
      SpeakerMode( const wstring& _shorthand, const wstring& _name ):
        shorthand( _shorthand ), name( _name ) {}
      virtual ~SpeakerMode() {}
    };
    typedef std::vector<SpeakerMode*> SpeakerModeVector;

    // Audio settings
    struct Settings {
      int driver;
      int outputType;
      int speakerMode;
      Settings(): driver( 0 ), outputType( 0 ), speakerMode( 0 ) {}
    };
  protected:
    DriverVector mDrivers;
    OutputTypeVector mOutputTypes;
    SpeakerModeVector mSpeakerModes;
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
    virtual void applySettings( const Settings& settings ) = 0;
    virtual const Settings& getSettings() { return mSettings; }
    virtual const DriverVector& getDrivers() { return mDrivers; }
    virtual const OutputTypeVector& getOutputTypes() { return mOutputTypes; }
    virtual const SpeakerModeVector& getSpeakerModes() { return mSpeakerModes; }
    virtual ~Audio()
    {
      clearDrivers();
      clearOutputTypes();
      clearSpeakerModes();
    }
  };

}