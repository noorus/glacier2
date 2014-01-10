#pragma once
#include "Console.h"
#include "EngineComponent.h"

namespace Glacier {

  ENGINE_EXTERN_CONVAR( fm_bgvolume );
  ENGINE_EXTERN_CONVAR( fm_fxvolume );
  ENGINE_EXTERN_CONVAR( fm_device );
  ENGINE_EXTERN_CONVAR( fm_maxchannels );
  ENGINE_EXTERN_CONVAR( fm_speakermode );
  ENGINE_EXTERN_CONVAR( fm_outputmode );
  ENGINE_EXTERN_CONCMD( fm_restart );

  class Sound: public EngineComponent {
  public:
    struct Info {
      FMOD_OUTPUTTYPE outputType;
      FMOD_SPEAKERMODE speakerMode;
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
    FMOD::EventSystem* mEventSystem;
    FMOD::System* mSystem;
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
  public:
    // Console callbacks
    static void callbackAudioRestart( Console* console, ConCmd* command,
      StringVector& arguments );
    static void callbackListDevices( Console* console, ConCmd* command,
      StringVector& arguments );
  public:
    Sound( Engine* engine );
    void soundInitialize();
    void soundShutdown();
    void soundRestart();
    const Info& getInfo() { return mInfo; }
    virtual void componentTick( GameTime tick, GameTime time );
    virtual ~Sound();
  };

}