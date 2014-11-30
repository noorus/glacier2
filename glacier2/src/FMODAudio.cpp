#include "StdAfx.h"
#include "FMODAudio.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "FMODMusic.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

# define FMOD_FAILED(hr) (hr != FMOD_OK)

  // Sound engine constants ===================================================

  const wstring cUnknown = L"Unknown";
  const char* cFMODLogFile = "fmod.log";

  // Sound engine CVARs =======================================================

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

  // Sound engine =============================================================

  FMODAudio::FMODAudio( Engine* engine ): EngineComponent( engine ),
  mEventSystem( nullptr ), mSystem( nullptr ), mMusic( nullptr )
  {
    FMOD_RESULT hr;

    resetGroups();

    FMOD::Memory_Initialize( NULL, 0,
      FMODAudio::callbackFMODMemAlloc,
      FMODAudio::callbackFMODMemRealloc,
      FMODAudio::callbackFMODMemFree, FMOD_MEMORY_NORMAL );

    // Create event system instance
    hr = FMOD::EventSystem_Create( &mEventSystem );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to create FMOD EventSystem" );

    // Fetch underlying oldschool system
    hr = mEventSystem->getSystemObject( &mSystem );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to get FMOD System object" );

    // Set FMOD log file name
    FMOD_ADVANCEDSETTINGS advancedSettings = { NULL };
    advancedSettings.cbsize = sizeof( FMOD_ADVANCEDSETTINGS );
    advancedSettings.debugLogFilename = const_cast<char*>( cFMODLogFile );
    mSystem->setAdvancedSettings( &advancedSettings );

    // Check version
    unsigned int version;
    hr = mSystem->getVersion( &version );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to get FMOD System version" );
    if ( version != FMOD_VERSION )
      ENGINE_EXCEPT( "Bad FMOD runtime version" );

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
    FMOD_RESULT hr;

    mEngine->getConsole()->printf( Console::srcSound,
      L"Initializing FMOD Ex %x.%x.%x",
      HIWORD( FMOD_VERSION ), HIBYTE( LOWORD( FMOD_VERSION ) ),
      LOBYTE( LOWORD( FMOD_VERSION ) ) );

    refreshDrivers();
    refreshOutputTypes();
    refreshSpeakerModes();

    // Set output type
    setOutputType( stringToOutputType( g_CVar_fm_outputmode.getString() ) );

    // Set output device
    setDriver( g_CVar_fm_device.getInt() );

    // Get driver caps
    hr = mSystem->getDriverCaps( NULL, &mInfo.caps, &mInfo.rate, &mInfo.speakerMode );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to get driver caps" );

    // Print a warning and raise the DSP buffer if someone is poor enough to run on emulation
    if ( mInfo.caps & FMOD_CAPS_HARDWARE_EMULATED )
    {
      mEngine->getConsole()->printf( Console::srcSound,
        L"No hardware audio acceleration detected! Using software emulation!" );
      mSystem->setDSPBufferSize( 1024, 10 );
    }

    // Set speaker mode
    setSpeakerMode( stringToSpeakerMode( g_CVar_fm_speakermode.getString() ) );

    // Prepare flags
    FMOD_INITFLAGS fmodFlags = FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_OCCLUSION_LOWPASS | FMOD_INIT_HRTF_LOWPASS | FMOD_INIT_GEOMETRY_USECLOSEST;
    FMOD_EVENT_INITFLAGS eventFlags = FMOD_EVENT_INIT_NORMAL;

    // Initialize the system
    int channels = g_CVar_fm_maxchannels.getInt();
    hr =  mEventSystem->init( channels, fmodFlags, nullptr, eventFlags );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to initialize FMOD EventSystem" );

    // Fetch realized values
    hr = mSystem->getSpeakerMode( &mInfo.speakerMode );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "getSpeakerMode failed" );
    mSystem->getOutput( &mInfo.outputType );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "getOutput failed" );
    mSystem->getDriver( &mInfo.driver );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "getDriver failed" );

    if ( mSettings.outputType != 0 )
      for ( size_t i = 0; i < mOutputTypes.size(); i++ )
        if ( ((FMODOutputType*)mOutputTypes[i])->fmodValue == mInfo.outputType )
        {
          mSettings.outputType = (int)i;
          break;
        }

    if ( mSettings.driver != 0 )
      for ( size_t i = 0; i < mDrivers.size(); i++ )
        if ( ((FMODDriver*)mDrivers[i])->fmodValue == mInfo.driver )
        {
          mSettings.driver = (int)i;
          break;
        }

    if ( mSettings.speakerMode != 0 )
      for ( size_t i = 0; i < mSpeakerModes.size(); i++ )
        if ( ((FMODSpeakerMode*)mSpeakerModes[i])->fmodValue == mInfo.speakerMode )
        {
          mSettings.speakerMode = (int)i;
          break;
        }

    // Log to console for good measure
    mEngine->getConsole()->printf( Console::srcSound,
      L"Speaker mode: %s", speakerModeToDisplayString( mInfo.speakerMode ).c_str() );
    mEngine->getConsole()->printf( Console::srcSound,
      L"Output type: %s", outputTypeToDisplayString( mInfo.outputType ).c_str() );

    // Setup channel groups
    hr = mSystem->getMasterChannelGroup( &mMasterGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to get master channel group" );

    hr = mSystem->createChannelGroup( "music", &mMusicGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to create music channel group" );
    hr = mMasterGroup->addGroup( mMusicGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to assign music channel group" );

    hr = mSystem->createChannelGroup( "effect", &mEffectGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to create effect channel group" );
    hr = mMasterGroup->addGroup( mEffectGroup );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to assign effect channel group" );

    // Set initial volumes
    setMasterVolume( g_CVar_fm_volume.getFloat() );
    setMusicVolume( g_CVar_fm_bgvolume.getFloat() );
    setEffectVolume( g_CVar_fm_fxvolume.getFloat() );

    mMusic = new FMODMusic( this );
    Locator::provideMusic( mMusic );

    mEngine->operationContinueAudio();
  }

  void FMODAudio::setDriver( int index )
  {
    refreshDrivers();

    if ( index < 0 || index >= mDrivers.size() )
      index = 0;

    mSettings.driver = index;
    auto driver = (FMODDriver*)mDrivers[index];

    FMOD_RESULT hr = mSystem->setDriver( driver->fmodValue );
    if ( hr == FMOD_OK )
      return;

    mSettings.driver = 0;
    hr = mSystem->setDriver( 0 );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to set audio driver" );
  }

  void FMODAudio::setOutputType( int index )
  {
    if ( index < 0 || index >= mOutputTypes.size() )
      index = 0;

    mSettings.outputType = index;
    auto type = (FMODOutputType*)mOutputTypes[index];

    FMOD_RESULT hr = mSystem->setOutput( type->fmodValue );
    if ( hr == FMOD_OK )
      return;

    mSettings.outputType = 0;
    g_CVar_fm_outputmode.setValue( L"auto" );
    hr = mSystem->setOutput( FMOD_OUTPUTTYPE_AUTODETECT );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to set audio output type" );
  }

  void FMODAudio::setSpeakerMode( int index )
  {
    if ( index < 0 || index >= mSpeakerModes.size() )
      index = 0;

    mSettings.speakerMode = index;
    auto mode = (FMODSpeakerMode*)mSpeakerModes[index];

    FMOD_RESULT hr = mSystem->setSpeakerMode(
      mode->fmodValue != FMOD_SPEAKERMODE_MAX ? mode->fmodValue : mInfo.speakerMode );
    if ( hr == FMOD_OK )
      return;

    mSettings.speakerMode = 0;
    g_CVar_fm_speakermode.setValue( L"auto" );
    hr = mSystem->setSpeakerMode( mInfo.speakerMode );
    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to set audio speaker mode" );
  }

  void FMODAudio::applySettings( const Settings& settings )
  {
    mEngine->getConsole()->printf( Console::srcSound, L"Applying new sound settings..." );

    bool newOutput = ( settings.outputType != mSettings.outputType );
    bool newDriver = ( settings.driver != mSettings.driver );
    bool newSpeakers = ( settings.speakerMode != mSettings.speakerMode );

    if ( newOutput || newDriver || newSpeakers )
      shutdown();

    if ( newOutput )
      if ( settings.outputType >= 0 && settings.outputType < mOutputTypes.size() )
        g_CVar_fm_outputmode.setValue( mOutputTypes[settings.outputType]->shorthand );

    if ( newDriver )
      g_CVar_fm_device.setValue( settings.driver );

    if ( newSpeakers )
      if ( settings.speakerMode >= 0 && settings.speakerMode < mSpeakerModes.size() )
        g_CVar_fm_speakermode.setValue( mSpeakerModes[settings.speakerMode]->shorthand );

    if ( newOutput || newDriver || newSpeakers )
      initialize();
  }

  void FMODAudio::componentTick( GameTime tick, GameTime time )
  {
    mMusic->update( tick );
    mEventSystem->update();
  }

  void FMODAudio::shutdown()
  {
    mEngine->getConsole()->printf( Console::srcSound,
      L"Shutting down FMOD" );

    mEngine->operationSuspendAudio();

    SAFE_DELETE( mMusic );
    Locator::provideMusic( nullptr );

    if ( mEventSystem )
      mEventSystem->unload();

    if ( mSystem )
      mSystem->close();

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

    SAFE_RELEASE_FMOD( mEventSystem );
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

  void FMODAudio::refreshDrivers()
  {
    clearDrivers();

    mDrivers.push_back( new FMODDriver( 0, L"Automatic" ) );

    int count = 0;
    FMOD_RESULT hr = mSystem->getNumDrivers( &count );

    if ( FMOD_FAILED( hr ) )
      ENGINE_EXCEPT_FMOD( hr, "Failed to get driver count" );

    for ( int i = 0; i < count; i++ )
    {
      wchar_t deviceName[256];
      mSystem->getDriverInfoW( i, (short*)deviceName, 256, NULL );
      mDrivers.push_back( new FMODDriver( i, deviceName ) );
    }
  }

  void FMODAudio::refreshOutputTypes()
  {
    if ( mOutputTypes.empty() )
    {
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_AUTODETECT, L"auto", L"Automatic" ) );
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_NOSOUND, L"nosound", L"No sound" ) );
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_WINMM, L"winmm", L"Windows MultiMedia" ) );
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_DSOUND, L"dsound", L"DirectSound" ) );
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_WASAPI, L"wasapi", L"Windows Audio Session API" ) );
      mOutputTypes.push_back( new FMODOutputType( FMOD_OUTPUTTYPE_ASIO, L"asio", L"ASIO 2.0" ) );
    }
  }

  void FMODAudio::refreshSpeakerModes()
  {
    if ( mSpeakerModes.empty() )
    {
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_MAX, L"auto", L"Automatic" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_RAW, L"raw", L"Raw" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_MONO, L"mono", L"Mono (1.0)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_STEREO, L"stereo", L"Stereo (2.0)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_QUAD, L"quad", L"Quad (4.0)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_SURROUND, L"surround", L"Surround (5.0)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_5POINT1, L"surround51", L"Surround (5.1)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_7POINT1, L"surround71", L"Surround (7.1)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_SRS5_1_MATRIX, L"srs51matrix", L"Prologic/SRS Surround (5.1)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_DOLBY5_1_MATRIX, L"dolby51matrix", L"Dolby Surround (5.1)" ) );
      mSpeakerModes.push_back( new FMODSpeakerMode( FMOD_SPEAKERMODE_MYEARS, L"myears", L"MyEars HRTF Stereo (2.0)" ) );
    }
  }

  void FMODAudio::printDeviceList( Console* console )
  {
    if ( !mSystem )
      return;

    FMOD_OUTPUTTYPE output;
    if ( mSystem->getOutput( &output ) != FMOD_OK )
      return;

    console->printf( Console::srcSound,
      L"Available audio devices for %s:",
      outputTypeToDisplayString( output ).c_str() );

    for ( size_t i = 0; i < mDrivers.size(); i++ )
      console->printf( Console::srcSound, L"Device %i: %s",
        i, mDrivers[i]->name.c_str() );
  }

  // Console command callbacks ================================================

  bool FMODAudio::callbackMaxChannels( ConVar* variable, ConVar::Value oldValue )
  {
    int channels = boost::algorithm::clamp( variable->getInt(), 64, 4093 );
    variable->forceValue( channels );
    return true;
  }

  bool FMODAudio::callbackSpeakerMode( ConVar* variable, ConVar::Value oldValue )
  {
    return true;
  }

  bool FMODAudio::callbackOutputMode( ConVar* variable, ConVar::Value oldValue )
  {
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

  int FMODAudio::stringToSpeakerMode( const wstring& mode )
  {
    for ( size_t i = 0; i < mSpeakerModes.size(); i++ )
    {
      if ( boost::iequals( mode, mSpeakerModes[i]->shorthand ) )
        return (int)i;
    }
    return 0;
  }

  const wstring& FMODAudio::speakerModeToDisplayString( const FMOD_SPEAKERMODE mode )
  {
    for ( auto it : mSpeakerModes )
    {
      if ( ((FMODSpeakerMode*)it)->fmodValue == mode )
        return it->name;
    }
    return cUnknown;
  }

  int FMODAudio::stringToOutputType( const wstring& type )
  {
    for ( size_t i = 0; i < mOutputTypes.size(); i++ )
    {
      if ( boost::iequals( type, mOutputTypes[i]->shorthand ) )
        return (int)i;
    }
    return 0;
  }

  const wstring& FMODAudio::outputTypeToDisplayString( const FMOD_OUTPUTTYPE type )
  {
    for ( auto it : mOutputTypes )
    {
      if ( ((FMODOutputType*)it)->fmodValue == type )
        return it->name;
    }
    return cUnknown;
  }

}