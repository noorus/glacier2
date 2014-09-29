#pragma once
#include "Console.h"
#include "EngineComponent.h"
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_EXTERN_CONVAR( fm_volume );
  ENGINE_EXTERN_CONVAR( fm_bgvolume );
  ENGINE_EXTERN_CONVAR( fm_fxvolume );
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
      wstring driverName;
      const wstring& outputTypeString();
      const wstring& speakerModeString();
    };
    struct OutputType {
      FMOD_OUTPUTTYPE index;
      wstring shorthand;
      wstring name;
    };
    struct SpeakerMode {
      FMOD_SPEAKERMODE index;
      wstring shorthand;
      wstring name;
    };
  protected:
    Info mInfo;
    Audio::AudioDriverList mDrivers;
    Audio::OutputTypeList mOutputTypes;
    Audio::SpeakerModeList mSpeakerModes;
    FMOD::EventSystem* mEventSystem;
    FMOD::System* mSystem;
    FMOD::ChannelGroup* mMasterGroup;
    FMOD::ChannelGroup* mMusicGroup;
    FMOD::ChannelGroup* mEffectGroup;
    void resetGroups();
    void setMasterVolume( float volume );
    void setMusicVolume( float volume );
    void setEffectVolume( float volume );
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
    static FMOD_SPEAKERMODE stringToSpeakerMode( const wstring& mode );
    static const wstring& speakerModeToDisplayString(
      const FMOD_SPEAKERMODE mode );
    static FMOD_OUTPUTTYPE stringToOutputType( const wstring& type );
    static const wstring& outputTypeToDisplayString(
      const FMOD_OUTPUTTYPE type );
    void refreshDrivers();
    void refreshOutputTypes();
    void refreshSpeakerModes();
  public:
    // Console callbacks
    static void callbackAudioRestart( Console* console, ConCmd* command,
      StringVector& arguments );
    static void callbackListDevices( Console* console, ConCmd* command,
      StringVector& arguments );
    static bool callbackMasterVolume( ConVar* variable, ConVar::Value oldValue );
    static bool callbackMusicVolume( ConVar* variable, ConVar::Value oldValue );
    static bool callbackEffectVolume( ConVar* variable, ConVar::Value oldValue );
    static bool callbackMaxChannels( ConVar* variable, ConVar::Value oldValue );
    static bool callbackSpeakerMode( ConVar* variable, ConVar::Value oldValue );
    static bool callbackOutputMode( ConVar* variable, ConVar::Value oldValue );
  public:
    FMODAudio( Engine* engine );
    virtual void initialize();
    virtual void shutdown();
    virtual void restart();
    virtual const AudioDriverList& getDrivers();
    virtual const OutputTypeList& getOutputTypes();
    virtual const SpeakerModeList& getSpeakerModes();
    const Info& getInfo() { return mInfo; }
    virtual void componentTick( GameTime tick, GameTime time );
    virtual ~FMODAudio();
  };

}