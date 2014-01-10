#include "StdAfx.h"
#include "Sound.h"
#include "Engine.h"
#include "Console.h"
#include "GlacierMemory.h"
#include "Exception.h"

namespace Glacier {

# define FMOD_FAILED(hr) (hr != FMOD_OK)

  // Sound engine constants ===================================================

  const wstring cUnknown = L"Unknown";
  const char* cFMODLogFile = "fmod.log";
  const int cFMODSpeakerModeCount = 10;
  const Sound::SpeakerMode cFMODSpeakerModes[cFMODSpeakerModeCount] =
  {
    { FMOD_SPEAKERMODE_RAW, L"raw", L"Raw" },
    { FMOD_SPEAKERMODE_MONO, L"mono", L"Mono (1.0)" },
    { FMOD_SPEAKERMODE_STEREO, L"stereo", L"Stereo (2.0)" },
    { FMOD_SPEAKERMODE_QUAD, L"quad", L"Quad (4.0)" },
    { FMOD_SPEAKERMODE_SURROUND, L"surround", L"Surround (5.0)" },
    { FMOD_SPEAKERMODE_5POINT1, L"surround51", L"Surround (5.1)" },
    { FMOD_SPEAKERMODE_7POINT1, L"surround71", L"Surround (7.1)" },
    { FMOD_SPEAKERMODE_SRS5_1_MATRIX, L"srs51matrix", L"Prologic/SRS Surround (5.1)" },
    { FMOD_SPEAKERMODE_DOLBY5_1_MATRIX, L"dolby51matrix", L"Dolby Surround (5.1)" },
    { FMOD_SPEAKERMODE_MYEARS, L"myears", L"MyEars HRTF Stereo (2.0)" }
  };
  const int cFMODOutputTypeCount = 5;
  const Sound::OutputType cFMODOutputTypes[cFMODOutputTypeCount] =
  {
    { FMOD_OUTPUTTYPE_NOSOUND, L"nosound", L"No sound" },
    { FMOD_OUTPUTTYPE_WINMM, L"winmm", L"Windows MultiMedia" },
    { FMOD_OUTPUTTYPE_DSOUND, L"dsound", L"DirectSound" },
    { FMOD_OUTPUTTYPE_WASAPI, L"wasapi", L"Windows Audio Session API" },
    { FMOD_OUTPUTTYPE_ASIO, L"asio", L"ASIO 2.0" }
  };

  // Sound engine CVARs =======================================================

  ENGINE_DECLARE_CONVAR( fm_bgvolume, L"Background music volume.", 1.0 );
  ENGINE_DECLARE_CONVAR( fm_fxvolume, L"Sound effect volume.", 1.0 );
  ENGINE_DECLARE_CONVAR( fm_device,
    L"Audio output device index. 0 = System default. Use fm_listdevices for a list of devices.", 0 );
  ENGINE_DECLARE_CONVAR( fm_maxchannels,
    L"Maximum number of virtual sound channels available. Does not equal the amount of actual voices playing.",
    256 );
  ENGINE_DECLARE_CONVAR( fm_speakermode,
    L"Speaker setup on the system. Valid values are \"auto\",\"mono\",\"stereo\",\"quad\" (4.0),\"surround\" (5.0),\"surround51\" (5.1),\"surround71\" (7.1),\"prologic\" (prologic surround encoding).",
    L"auto" );
  ENGINE_DECLARE_CONVAR( fm_outputmode,
    L"Audio output mode. Valid values are \"nosound\" (No output),\"auto\" (System default),\"winmm\" (Windows MultiMedia),\"dsound\" (DirectSound),\"wasapi\" (WASAPI),\"asio\" (ASIO 2.0).",
    L"auto" );
  ENGINE_DECLARE_CONCMD( fm_restart,
    L"Restart the sound subsystem to apply changes to speaker/output setup.",
    &Sound::callbackAudioRestart );
  ENGINE_DECLARE_CONCMD( fm_listdevices,
    L"Lists all available audio devices for the current output mode.",
    &Sound::callbackListDevices );

  // Sound engine info struct =================================================

  const wstring& Sound::Info::outputTypeString()
  {
    return outputTypeToDisplayString( outputType );
  }

  const wstring& Sound::Info::speakerModeString()
  {
    return speakerModeToDisplayString( speakerMode );
  }

  // Sound engine =============================================================

  Sound::Sound( Engine* engine ): EngineComponent( engine ),
  mEventSystem( nullptr ), mSystem( nullptr )
  {
    FMOD::Memory_Initialize( NULL, 0,
      Sound::callbackFMODMemAlloc,
      Sound::callbackFMODMemRealloc,
      Sound::callbackFMODMemFree, FMOD_MEMORY_NORMAL );

    soundInitialize();
  }

  void Sound::soundInitialize()
  {
    // Greetings
    FMOD_RESULT hr;
    mEngine->getConsole()->printf( Console::srcSound, L"Initializing sound system..." );

    // Create event system instance
    hr = FMOD::EventSystem_Create( &mEventSystem );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to create FMOD EventSystem" );

    // Fetch underlying oldschool system
    hr = mEventSystem->getSystemObject( &mSystem );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to get FMOD System object" );

    // Set FMOD log file name
    FMOD_ADVANCEDSETTINGS advancedSettings = { NULL };
    advancedSettings.cbsize = sizeof( FMOD_ADVANCEDSETTINGS );
    advancedSettings.debugLogFilename = const_cast<char*>( cFMODLogFile );
    mSystem->setAdvancedSettings( &advancedSettings );

    // Check version
    unsigned int version;
    hr = mSystem->getVersion( &version );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to get FMOD System version" );
    if ( version != FMOD_VERSION )
      ENGINE_EXCEPT( L"Bad FMOD version" );

    // Print some version info
    mEngine->getConsole()->printf( Console::srcSound, L"Using FMOD Ex %x.%x.%x",
      HIWORD(version), HIBYTE(LOWORD(version)), LOBYTE(LOWORD(version)) );

    // TODO: set output mode, output device, speaker mode, validity callbacks

    // Prepare flags
    FMOD_INITFLAGS fmodFlags = FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_OCCLUSION_LOWPASS | FMOD_INIT_HRTF_LOWPASS | FMOD_INIT_GEOMETRY_USECLOSEST;
    FMOD_EVENT_INITFLAGS eventFlags = FMOD_EVENT_INIT_NORMAL;

    // Initialize the system
    int channels = g_CVar_fm_maxchannels.getInt();
    hr =  mEventSystem->init( channels, fmodFlags, nullptr, eventFlags );

    if ( hr == FMOD_ERR_OUTPUT_CREATEBUFFER )
    {
      // Bad speaker mode! Try reverting to stereo.
      g_CVar_fm_speakermode.setValue( L"stereo" );
      mSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO );
      hr =  mEventSystem->init( channels, fmodFlags, nullptr, eventFlags );
    }

    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to initialize FMOD EventSystem" );

    // Fetch realized values
    mSystem->getSpeakerMode( &mInfo.speakerMode );
    mSystem->getOutput( &mInfo.outputType );
    mSystem->getDriver( &mInfo.driver );

    wchar_t driverName[256] = { '\0' };
    mSystem->getDriverInfoW( mInfo.driver, (short*)&driverName, 256, NULL );
    mInfo.driverName = driverName;

    // Log to console for good measure
    mEngine->getConsole()->printf( Console::srcSound,
      L"Speaker mode: %s", mInfo.speakerModeString().c_str() );
    mEngine->getConsole()->printf( Console::srcSound,
      L"Output type: %s", mInfo.outputTypeString().c_str() );
    mEngine->getConsole()->printf( Console::srcSound,
      L"Output device: %s", mInfo.driverName.c_str() );

    mEngine->operationContinueAudio();
  }

  void Sound::componentTick( GameTime tick, GameTime time )
  {
    mEventSystem->update();
  }

  void Sound::soundShutdown()
  {
    mEngine->getConsole()->printf( Console::srcSound, L"Shutting down FMOD" );

    mEngine->operationSuspendAudio();

    if ( mEventSystem )
      mEventSystem->unload();

    SAFE_RELEASE_FMOD( mEventSystem );
  }

  void Sound::soundRestart()
  {
    soundShutdown();
    soundInitialize();
  }

  Sound::~Sound()
  {
    soundShutdown();
  }

  void Sound::printDeviceList( Console* console )
  {
    if ( !mSystem )
      return;

    int count = 0;
    FMOD_OUTPUTTYPE output;
    if ( mSystem->getNumDrivers( &count ) != FMOD_OK )
      return;
    if ( mSystem->getOutput( &output ) != FMOD_OK )
      return;

    console->printf( Console::srcSound,
      L"Available audio devices for %s:",
      outputTypeToDisplayString( output ).c_str() );

    for ( int i = 0; i < count; i++ )
    {
      wchar_t deviceName[128];
      mSystem->getDriverInfoW( i, (short*)deviceName, 128, NULL );
      console->printf( Console::srcSound, L"Device %i: %s", i + 1, deviceName );
    }
  }

  // Conversion utilities =====================================================

  FMOD_SPEAKERMODE Sound::stringToSpeakerMode( const wstring& mode )
  {
    if ( boost::iequals( mode, L"auto" ) )
      return (FMOD_SPEAKERMODE)( FMOD_SPEAKERMODE_MAX + 1 );
    for ( int i = 0; i < cFMODSpeakerModeCount; i++ )
    {
      if ( boost::iequals( mode, cFMODSpeakerModes[i].shorthand ) )
        return cFMODSpeakerModes[i].index;
    }
    return FMOD_SPEAKERMODE_MAX;
  }

  const wstring& Sound::speakerModeToDisplayString( const FMOD_SPEAKERMODE mode )
  {
    for ( int i = 0; i < cFMODSpeakerModeCount; i++ )
    {
      if ( cFMODSpeakerModes[i].index == mode )
        return cFMODSpeakerModes[i].name;
    }
    return cUnknown;
  }

  FMOD_OUTPUTTYPE Sound::stringToOutputType( const wstring& type )
  {
    if ( boost::iequals( type, L"auto" ) )
      return (FMOD_OUTPUTTYPE)( FMOD_OUTPUTTYPE_MAX + 1 );
    for ( int i = 0; i < cFMODOutputTypeCount; i++ )
    {
      if ( boost::iequals( type, cFMODOutputTypes[i].shorthand ) )
        return cFMODOutputTypes[i].index;
    }
    return FMOD_OUTPUTTYPE_MAX;
  }

  const wstring& Sound::outputTypeToDisplayString( const FMOD_OUTPUTTYPE type )
  {
    for ( int i = 0; i < cFMODOutputTypeCount; i++ )
    {
      if ( cFMODOutputTypes[i].index == type )
        return cFMODOutputTypes[i].name;
    }
    return cUnknown;
  }

  // Console command callbacks ================================================

  void Sound::callbackAudioRestart( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getSound() )
      return;

    gEngine->getSound()->soundRestart();
  }

  void Sound::callbackListDevices( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getSound() )
      return;

    gEngine->getSound()->printDeviceList( console );
  }

  // Memory allocation callbacks ==============================================

  void* Sound::callbackFMODMemAlloc( unsigned int size,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    return Memory::instance().alloc( size );
  }

  void* Sound::callbackFMODMemRealloc( void* mem, unsigned int size,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    return Memory::instance().realloc( mem, size );
  }

  void Sound::callbackFMODMemFree( void* mem,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    Memory::instance().free( mem );
  }

}