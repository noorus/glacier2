#pragma once
#include "Console.h"
#include "EngineComponent.h"
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_EXTERN_CONVAR( fm_device );
  ENGINE_EXTERN_CONVAR( fm_maxchannels );
  ENGINE_EXTERN_CONVAR( fm_speakermode );
  ENGINE_EXTERN_CONVAR( fm_outputmode );
  ENGINE_EXTERN_CONCMD( fm_restart );

  class FMODAudio: public Audio, public EngineComponent {
  public:
    struct Info {
      FMOD_CAPS caps;
      FMOD_OUTPUTTYPE outputType;
      FMOD_SPEAKERMODE speakerMode;
      int rate;
      int driver;
    };
    struct FMODDriver : public Audio::Driver
    {
      int fmodValue;
      FMODDriver( int _value, const wstring& _name ):
        Audio::Driver( _name ), fmodValue( _value ) {}
    };
    struct FMODOutputType: public Audio::OutputType
    {
      FMOD_OUTPUTTYPE fmodValue;
      FMODOutputType( FMOD_OUTPUTTYPE _value, const wstring& _shorthand, const wstring& _name ):
        Audio::OutputType( _shorthand, _name ), fmodValue( _value ) {}
    };
    struct FMODSpeakerMode: public Audio::SpeakerMode
    {
      FMOD_SPEAKERMODE fmodValue;
      FMODSpeakerMode( FMOD_SPEAKERMODE _value, const wstring& _shorthand, const wstring& _name ):
        Audio::SpeakerMode( _shorthand, _name ), fmodValue( _value ) {}
    };
  protected:
    Info mInfo;
    FMOD::EventSystem* mEventSystem;
    FMOD::System* mSystem;
    FMOD::ChannelGroup* mMasterGroup;
    FMOD::ChannelGroup* mMusicGroup;
    FMOD::ChannelGroup* mEffectGroup;
    void resetGroups();
    void printDeviceList( Console* console );
  protected:
    // Memory callbacks
    static void* F_CALLBACK callbackFMODMemAlloc( unsigned int size,
      FMOD_MEMORY_TYPE type, const char* source );
    static void* F_CALLBACK callbackFMODMemRealloc( void* mem,
      unsigned int size, FMOD_MEMORY_TYPE type, const char* source );
    static void F_CALLBACK callbackFMODMemFree( void* mem,
      FMOD_MEMORY_TYPE type, const char* source );
    // Conversion utilities
    int stringToSpeakerMode( const wstring& mode );
    const wstring& speakerModeToDisplayString(
      const FMOD_SPEAKERMODE mode );
    int stringToOutputType( const wstring& type );
    const wstring& outputTypeToDisplayString(
      const FMOD_OUTPUTTYPE type );
    void setDriver( int index );
    void setOutputType( int index );
    void setSpeakerMode( int index );
    void refreshDrivers();
    void refreshOutputTypes();
    void refreshSpeakerModes();
  public:
    // Console callbacks
    static void callbackAudioRestart( Console* console, ConCmd* command,
      StringVector& arguments );
    static void callbackListDevices( Console* console, ConCmd* command,
      StringVector& arguments );
    static bool callbackMaxChannels( ConVar* variable, ConVar::Value oldValue );
    static bool callbackSpeakerMode( ConVar* variable, ConVar::Value oldValue );
    static bool callbackOutputMode( ConVar* variable, ConVar::Value oldValue );
    virtual void setMasterVolume( float volume );
    virtual void setMusicVolume( float volume );
    virtual void setEffectVolume( float volume );
  public:
    FMODAudio( Engine* engine );
    virtual void initialize();
    virtual void shutdown();
    virtual void restart();
    virtual void applySettings( const Settings& settings );
    const Info& getInfo() { return mInfo; }
    virtual void componentTick( GameTime tick, GameTime time );
    virtual ~FMODAudio();
  };

}