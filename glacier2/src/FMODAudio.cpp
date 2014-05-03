#include "StdAfx.h"
#include "FMODAudio.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

# define FMOD_FAILED(hr) (hr != FMOD_OK)

  // Sound engine constants ===================================================

  const wstring cUnknown = L"Unknown";
  const char* cFMODLogFile = "fmod.log";

  const int cFMODSpeakerModeCount = 10;
  const FMODAudio::SpeakerMode cFMODSpeakerModes[cFMODSpeakerModeCount] =
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
  const FMODAudio::OutputType cFMODOutputTypes[cFMODOutputTypeCount] =
  {
    { FMOD_OUTPUTTYPE_NOSOUND, L"nosound", L"No sound" },
    { FMOD_OUTPUTTYPE_WINMM, L"winmm", L"Windows MultiMedia" },
    { FMOD_OUTPUTTYPE_DSOUND, L"dsound", L"DirectSound" },
    { FMOD_OUTPUTTYPE_WASAPI, L"wasapi", L"Windows Audio Session API" },
    { FMOD_OUTPUTTYPE_ASIO, L"asio", L"ASIO 2.0" }
  };

  // Sound engine CVARs =======================================================

  ENGINE_DECLARE_CONVAR_WITH_CB( fm_volume, L"Master volume.",
    1.0f, FMODAudio::callbackMasterVolume );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_bgvolume, L"Background music volume.",
    1.0f, FMODAudio::callbackMusicVolume );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_fxvolume, L"Sound effect volume.",
    1.0f, FMODAudio::callbackEffectVolume );
  ENGINE_DECLARE_CONVAR( fm_device,
    L"Audio output device index. 0 = System default. Use fm_listdevices for a list of devices.", 0 );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_maxchannels,
    L"Maximum number of virtual sound channels available. Does not equal the amount of actual voices playing.",
    256, FMODAudio::callbackMaxChannels );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_speakermode,
    L"Speaker setup on the system. Valid values are \"auto\",\"mono\",\"stereo\",\"quad\" (4.0),\"surround\" (5.0),\"surround51\" (5.1),\"surround71\" (7.1),\"prologic\" (prologic surround encoding).",
    L"auto", FMODAudio::callbackSpeakerMode );
  ENGINE_DECLARE_CONVAR_WITH_CB( fm_outputmode,
    L"Audio output mode. Valid values are \"nosound\" (No output),\"auto\" (System default),\"winmm\" (Windows MultiMedia),\"dsound\" (DirectSound),\"wasapi\" (WASAPI),\"asio\" (ASIO 2.0).",
    L"auto", FMODAudio::callbackOutputMode );
  ENGINE_DECLARE_CONCMD( fm_restart,
    L"Restart the sound subsystem to apply changes to speaker/output setup.",
    FMODAudio::callbackAudioRestart );
  ENGINE_DECLARE_CONCMD( fm_listdevices,
    L"Lists all available audio devices for the current output mode.",
    FMODAudio::callbackListDevices );

  // Sound engine info struct =================================================

  const wstring& FMODAudio::Info::outputTypeString()
  {
    return outputTypeToDisplayString( outputType );
  }

  const wstring& FMODAudio::Info::speakerModeString()
  {
    return speakerModeToDisplayString( speakerMode );
  }

  // Sound engine =============================================================

  FMODAudio::FMODAudio( Engine* engine ): EngineComponent( engine ),
  mEventSystem( nullptr ), mSystem( nullptr )
  {
    resetGroups();

    FMOD::Memory_Initialize( NULL, 0,
      FMODAudio::callbackFMODMemAlloc,
      FMODAudio::callbackFMODMemRealloc,
      FMODAudio::callbackFMODMemFree, FMOD_MEMORY_NORMAL );

    initialize();
  }

  void FMODAudio::resetGroups()
  {
    mMasterGroup = nullptr;
    mMusicGroup = nullptr;
    mEffectGroup = nullptr;
  }

  void FMODAudio::initialize()
  {
    // Greetings
    FMOD_RESULT hr;
    mEngine->getConsole()->printf( Console::srcSound,
      L"Initializing sound system..." );

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
    mEngine->getConsole()->printf( Console::srcSound,
      L"Using FMOD Ex %x.%x.%x",
      HIWORD( version ), HIBYTE( LOWORD( version ) ),
      LOBYTE( LOWORD( version ) ) );

    // Set output mode
    auto outputType = stringToOutputType( g_CVar_fm_outputmode.getString() );
    if ( outputType == FMOD_OUTPUTTYPE_MAX )
    {
      mEngine->getConsole()->errorPrintf(
        L"Invalid value for fm_outputmode, reverting to auto!" );
      g_CVar_fm_outputmode.setValue( L"auto" );
    }
    else if ( outputType != FMOD_OUTPUTTYPE_MAX + 1 )
    {
      // Only call setOutput if value is _not_ "auto"!
      hr = mSystem->setOutput( outputType );
      if ( FMOD_FAILED( hr ) )
        ENGINE_EXCEPT_FMOD( hr, L"Failed to set output mode" );
    }

    // Set output device
    int driverCount;
    hr = mSystem->getNumDrivers( &driverCount );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to get driver count" );
    int driver = g_CVar_fm_device.getInt();
    if ( driver > driverCount ) {
      mEngine->getConsole()->errorPrintf(
        L"Invalid value for fm_device, reverting to 0!" );
      g_CVar_fm_device.setValue( 0 );
    } else if ( driver > 0 ) {
      // Only call setDriver if value is _not_ 0!
      hr = mSystem->setDriver( driver - 1 );
      if ( FMOD_FAILED( hr ) )
        ENGINE_EXCEPT_FMOD( hr, L"Failed to set output device" );
    }

    // Get driver caps
    hr = mSystem->getDriverCaps( NULL, &mInfo.caps, &mInfo.rate, &mInfo.speakerMode );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to get driver caps" );

    // Print a warning and raise the DSP buffer if someone is poor enough to run on emulation
    if ( mInfo.caps & FMOD_CAPS_HARDWARE_EMULATED )
    {
      mEngine->getConsole()->printf( Console::srcSound,
        L"No hardware audio acceleration detected! Using software emulation!" );
      mSystem->setDSPBufferSize( 1024, 10 );
    }

    // Set speaker mode
    auto speakerMode = stringToSpeakerMode( g_CVar_fm_speakermode.getString() );
    if ( speakerMode == FMOD_SPEAKERMODE_MAX )
    {
      mEngine->getConsole()->errorPrintf(
        L"Invalid value for fm_speakermode, reverting to auto!" );
      g_CVar_fm_speakermode.setValue( L"auto" );
      speakerMode = mInfo.speakerMode;
    }
    else if ( speakerMode == FMOD_SPEAKERMODE_MAX + 1 )
    {
      // Auto means the speaker mode currently used by the OS.
      speakerMode = mInfo.speakerMode;
    }
    hr = mSystem->setSpeakerMode( speakerMode );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to set speaker mode" );

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

    // Setup channel groups
    hr = mSystem->getMasterChannelGroup( &mMasterGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to get master channel group" );

    hr = mSystem->createChannelGroup( "music", &mMusicGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to create music channel group" );
    hr = mMasterGroup->addGroup( mMusicGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to assign music channel group" );

    hr = mSystem->createChannelGroup( "effect", &mEffectGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to create effect channel group" );
    hr = mMasterGroup->addGroup( mEffectGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, L"Failed to assign effect channel group" );

    // Set initial volumes
    setMasterVolume( g_CVar_fm_volume.getFloat() );
    setMusicVolume( g_CVar_fm_bgvolume.getFloat() );
    setEffectVolume( g_CVar_fm_fxvolume.getFloat() );

    mEngine->operationContinueAudio();
  }

  void FMODAudio::componentTick( GameTime tick, GameTime time )
  {
    mEventSystem->update();
  }

  void FMODAudio::shutdown()
  {
    mEngine->getConsole()->printf( Console::srcSound,
      L"Shutting down FMOD..." );

    mEngine->operationSuspendAudio();

    if ( mEventSystem )
      mEventSystem->unload();

    SAFE_RELEASE_FMOD( mEventSystem );

    resetGroups();
  }

  void FMODAudio::restart()
  {
    shutdown();
    initialize();
  }

  FMODAudio::~FMODAudio()
  {
    shutdown();
  }

  void FMODAudio::setMasterVolume( float volume )
  {
    if ( mMasterGroup )
      mMasterGroup->setVolume( volume );
  }

  void FMODAudio::setMusicVolume( float volume )
  {
    if ( mMusicGroup )
      mMusicGroup->setVolume( volume );
  }

  void FMODAudio::setEffectVolume( float volume )
  {
    if ( mEffectGroup )
      mEffectGroup->setVolume( volume );
  }

  void FMODAudio::printDeviceList( Console* console )
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

  // Console command callbacks ================================================

  bool FMODAudio::callbackMasterVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    if ( gEngine && gEngine->getSound() )
      gEngine->getSound()->setMasterVolume( volume );
    return true;
  }

  bool FMODAudio::callbackMusicVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    if ( gEngine && gEngine->getSound() )
      gEngine->getSound()->setMusicVolume( volume );
    return true;
  }

  bool FMODAudio::callbackEffectVolume( ConVar* variable, ConVar::Value oldValue )
  {
    float volume = boost::algorithm::clamp( variable->getFloat(), 0.0f, 1.0f );
    if ( gEngine && gEngine->getSound() )
      gEngine->getSound()->setEffectVolume( volume );
    return true;
  }

  bool FMODAudio::callbackMaxChannels( ConVar* variable, ConVar::Value oldValue )
  {
    int channels = boost::algorithm::clamp( variable->getInt(), 64, 4093 );
    variable->forceValue( channels );
    return true;
  }

  bool FMODAudio::callbackSpeakerMode( ConVar* variable, ConVar::Value oldValue )
  {
    auto mode = FMODAudio::stringToSpeakerMode( variable->getString() );
    if ( mode == FMOD_SPEAKERMODE_MAX )
    {
      if ( gEngine && gEngine->getConsole() )
        gEngine->getConsole()->errorPrintf(
          L"Bad value \"%s\" given for fm_speakermode.",
          variable->getString().c_str() );
      return false;
    }
    return true;
  }

  bool FMODAudio::callbackOutputMode( ConVar* variable, ConVar::Value oldValue )
  {
    auto type = FMODAudio::stringToOutputType( variable->getString() );
    if ( type == FMOD_OUTPUTTYPE_MAX )
    {
      if ( gEngine && gEngine->getConsole() )
        gEngine->getConsole()->errorPrintf(
        L"Bad value \"%s\" given for fm_outputmode.",
        variable->getString().c_str() );
      return false;
    }
    return true;
  }

  void FMODAudio::callbackAudioRestart( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getSound() )
      return;

    gEngine->getSound()->restart();
  }

  void FMODAudio::callbackListDevices( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getSound() )
      return;

    gEngine->getSound()->printDeviceList( console );
  }

  // Memory allocation callbacks ==============================================

  void* FMODAudio::callbackFMODMemAlloc( unsigned int size,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    return Locator::getMemory().alloc( Memory::Sector_Audio, size );
  }

  void* FMODAudio::callbackFMODMemRealloc( void* mem, unsigned int size,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    return Locator::getMemory().realloc( Memory::Sector_Audio, mem, size );
  }

  void FMODAudio::callbackFMODMemFree( void* mem,
  FMOD_MEMORY_TYPE type, const char* source )
  {
    Locator::getMemory().free( Memory::Sector_Audio, mem );
  }

  // Conversion utilities =====================================================

  FMOD_SPEAKERMODE FMODAudio::stringToSpeakerMode( const wstring& mode )
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

  const wstring& FMODAudio::speakerModeToDisplayString( const FMOD_SPEAKERMODE mode )
  {
    for ( int i = 0; i < cFMODSpeakerModeCount; i++ )
    {
      if ( cFMODSpeakerModes[i].index == mode )
        return cFMODSpeakerModes[i].name;
    }
    return cUnknown;
  }

  FMOD_OUTPUTTYPE FMODAudio::stringToOutputType( const wstring& type )
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

  const wstring& FMODAudio::outputTypeToDisplayString( const FMOD_OUTPUTTYPE type )
  {
    for ( int i = 0; i < cFMODOutputTypeCount; i++ )
    {
      if ( cFMODOutputTypes[i].index == type )
        return cFMODOutputTypes[i].name;
    }
    return cUnknown;
  }

}