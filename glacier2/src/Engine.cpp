#include "StdAfx.h"
#include "Engine.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "Scripting.h"
#include "Graphics.h"
#include "PhysXPhysics.h"
#include "Utilities.h"
#include "Exception.h"
#include "FMODAudio.h"
#include "Game.h"
#include "WindowHandler.h"
#include "Input.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

Glacier::Engine* gEngine = nullptr;

namespace Glacier {

  Engine::Options::Options():
  noAudio( false ), noPhysics( false )
  {
    //
  }

  // Engine timing values =====================================================

  GameTime Engine::fTime = 0.0;
  GameTime Engine::fTimeDelta = 0.0;
  GameTime Engine::fTimeAccumulator = 0.0;
  GameTime Engine::fLogicStep = 1.0 / 60.0;

  const char* cMainThreadName = "Gcr2 Main Thread";

  // Engine version struct ====================================================

  Engine::Version::Version( uint32_t major_, uint32_t minor_, uint32_t build_ ):
  compiler( _COMPILER ), profile( _PROFILE ),
  major( major_ ), minor( minor_ ), build( build_ )
  {
    WCHAR temp[100];
    swprintf_s( temp, 100, L"Glacier² Game Engine v%d.%d.%d %s", major, minor, build, profile.c_str() );
    title = temp;
    swprintf_s( temp, 100, L"%S %S", __DATE__, __TIME__ );
    compiled = temp;
    swprintf_s( temp, 100, L"Compiled on %s (%s)", compiled.c_str(), compiler.c_str() );
    subtitle = temp;
  }

  // Engine class =============================================================

  ENGINE_DECLARE_CONCMD( version,
    L"Print engine version.", Engine::callbackVersion );
  ENGINE_DECLARE_CONCMD( memstat,
    L"Print memory usage statistics.", Engine::callbackMemstat );
  ENGINE_DECLARE_CONCMD( screenshot,
    L"Save a screenshot to working directory.", Engine::callbackScreenshot );
  ENGINE_DECLARE_CONCMD( quit,
    L"Quit.", Engine::callbackQuit );

  Engine::Engine( HINSTANCE instance ):
  mConsole( nullptr ), mScripting( nullptr ), mGraphics( nullptr ),
  mProcess( NULL ), mThread( NULL ), mInstance( instance ),
  mSignal( Signal_None ), mVersion( 0, 1, 1 ), mConsoleWindow( nullptr ),
  mGame( nullptr ), mWindowHandler( nullptr ), mInput( nullptr ),
  mAudio( nullptr ), mPhysics( nullptr )
  {
  }

  const unsigned long cPrivileges = 1;

  void Engine::adjustPrivileges()
  {
    HANDLE token;
    if ( !OpenProcessToken( mProcess, TOKEN_ADJUST_PRIVILEGES, &token ) )
      return;
    LPCWSTR wantedPrivileges[cPrivileges] = {
      SE_LOCK_MEMORY_NAME
    };
    auto privileges = (PTOKEN_PRIVILEGES)malloc(
      FIELD_OFFSET( TOKEN_PRIVILEGES, Privileges[cPrivileges] ) );
    if ( privileges )
    {
      privileges->PrivilegeCount = cPrivileges;
      for ( int i = 0; i < cPrivileges; i++ )
      {
        if ( !LookupPrivilegeValueW( NULL, wantedPrivileges[i], &privileges->Privileges[i].Luid ) )
          return;
        privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
      }
      AdjustTokenPrivileges( token, FALSE, privileges, 0, NULL, NULL );
      free( privileges );
    }
    CloseHandle( token );
  }

  void Engine::fixupThreadAffinity()
  {
    DWORD_PTR processMask, systemMask;

    if ( !GetProcessAffinityMask( mProcess, &processMask, &systemMask ) )
    {
      getConsole()->printf( Console::srcEngine,
        L"Could not query process affinity mask - thread affinity may fluctuate" );
      return;
    }

    if ( processMask == 0 )
      processMask = 1;

    uint32_t core = 1;
    DWORD_PTR engineMask = 1;

    while ( ( engineMask & processMask ) == 0 )
    {
      core++;
      engineMask <<= 1;
    }

    uint32_t cores = Utilities::hammingWeight64( processMask );

    getConsole()->printf( Console::srcEngine,
      L"Fixating engine main thread to core %d (out of %d)", core, cores );

    if ( !SetThreadAffinityMask( mThread, engineMask ) )
      getConsole()->errorPrintf( Console::srcEngine,
        L"Failed to set thread affinity mask" );
  }

  void Engine::operationSuspendVideo()
  {
    if ( mGame )
      mGame->suspendState();
  }

  void Engine::operationContinueVideo()
  {
    if ( mGame )
      mGame->resumeState();
  }

  void Engine::operationSuspendAudio()
  {
    if ( mGame )
      mGame->suspendState();
  }

  void Engine::operationContinueAudio()
  {
    if ( mGame )
      mGame->resumeState();
  }

  void Engine::operationSuspendPhysics()
  {
    if ( mGame )
      mGame->suspendState();
  }

  void Engine::operationContinuePhysics()
  {
    if ( mGame )
      mGame->resumeState();
  }

  void Engine::registerResources( ResourceGroupManager& manager )
  {
    Graphics::registerResources( manager );
    Scripting::registerResources( manager );
  }

  void Engine::unregisterResources( ResourceGroupManager& manager )
  {
    Scripting::unregisterResources( manager );
    Graphics::unregisterResources( manager );
  }

  void Engine::initialize( const Options& options )
  {
    // Get process & thread handles
    mProcess = GetCurrentProcess();
    mThread = GetCurrentThread();

    Utilities::debugSetThreadName( GetCurrentThreadId(), cMainThreadName );

    // Setup console
    mConsole = new Console( this );
    mConsoleWindow = new ConsoleWindowThread( mInstance, mConsole );
    mConsoleWindow->start();

    // Print engine info
    mConsole->printf( Console::srcEngine, getVersion().title.c_str() );
    mConsole->printf( Console::srcEngine, getVersion().subtitle.c_str() );

    adjustPrivileges();

    // Fetch HPC frequency
    if ( !QueryPerformanceFrequency( &mHPCFrequency ) )
      ENGINE_EXCEPT_W32( L"Couldn't query HPC frequency" );

    mWindowHandler = new WindowHandler( this );
    mGraphics = new Graphics( this, mWindowHandler );

    mScripting = new Scripting( this );
    mScripting->simpleExecute( L"initialization.js" );

    mInput = new Input( this, mInstance, mGraphics->getWindow() );

    if ( !options.noPhysics )
    {
      mPhysics = new PhysXPhysics( this );
      Locator::providePhysics( mPhysics );
    }
    else
      mConsole->printf( Console::srcEngine, L"Init: Skipping physics" );

    if ( !options.noAudio )
    {
      mAudio = new FMODAudio( this );
      Locator::provideAudio( mAudio );
    }
    else
      mConsole->printf( Console::srcEngine, L"Init: Skipping audio" );

    mGame = new Game( this );
  }

  void Engine::signalStop()
  {
    mSignal = Signal_Stop;
  }

  void Engine::triggerFatalError( FatalError error )
  {
    // TODO: Try to save state and stuff here, before going down
    if ( mConsole )
    {
      mConsole->errorPrintf( Console::srcEngine,
        L"Shutting down due to a fatal error." );
    }

    signalStop();
  }

  void Engine::run()
  {
    fixupThreadAffinity();

    LARGE_INTEGER timeCurrent;
    LARGE_INTEGER tickDelta;
    LARGE_INTEGER timeNew;

    QueryPerformanceCounter( &timeCurrent );

    fTime = 0.0;
    fTimeAccumulator = 0.0;

    while ( mSignal != Signal_Stop )
    {
      mConsole->componentPreUpdate( fTime );
      mGraphics->componentPreUpdate( fTime );

      QueryPerformanceCounter( &timeNew );

      tickDelta.QuadPart = timeNew.QuadPart - timeCurrent.QuadPart;
      timeCurrent = timeNew;

      fTimeDelta = (GameTime)tickDelta.QuadPart / (GameTime)mHPCFrequency.QuadPart;
      fTimeAccumulator += fTimeDelta;
      while ( fTimeAccumulator >= fLogicStep )
      {
        if ( mPhysics )
          mPhysics->componentTick( fLogicStep, fTime );
        mInput->componentTick( fLogicStep, fTime );
        mGame->componentTick( fLogicStep, fTime );
        if ( mAudio )
          mAudio->componentTick( fLogicStep, fTime );
        fTime += fLogicStep;
        fTimeAccumulator -= fLogicStep;
      }
      fTimeDelta -= fTimeAccumulator;
      if ( fTimeDelta > 0.0 )
      {
        mGraphics->componentPostUpdate( fTimeDelta, fTime );
      }
    }
  }

  void Engine::callbackVersion( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine )
      return;

    console->printf( Console::srcEngine, gEngine->getVersion().title.c_str() );
  }

  void Engine::callbackMemstat( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    console->printf( Console::srcEngine, L"Memory provider: %s",
      Locator::getMemory().getProviderName().c_str() );
    console->printf( Console::srcEngine, L"Generic usage: %.2fMB",
      (float)Locator::getMemory().getMemoryUsage( Memory::Sector_Generic ) / 1048576.0f );
    console->printf( Console::srcEngine, L"Physics usage: %.2fMB",
      (float)Locator::getMemory().getMemoryUsage( Memory::Sector_Physics ) / 1048576.0f );
    console->printf( Console::srcEngine, L"Audio usage: %.2fMB",
      (float)Locator::getMemory().getMemoryUsage( Memory::Sector_Audio ) / 1048576.0f );
  }

  void Engine::callbackScreenshot( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine || !gEngine->getGraphics() )
      return;

    // TODO:LOW maybe pause execution and timing,
    // since this can cause a tiny hiccup
    gEngine->getGraphics()->screenshot();
  }

  void Engine::callbackQuit( Console* console, ConCmd* command,
  StringVector& arguments )
  {
    if ( !gEngine )
      return;

    gEngine->signalStop();
  }

  void Engine::shutdown()
  {
    SAFE_DELETE( mGame );

    if ( mAudio )
    {
      SAFE_DELETE( mAudio );
      Locator::provideAudio( nullptr );
    }

    if ( mPhysics )
    {
      SAFE_DELETE( mPhysics );
      Locator::providePhysics( nullptr );
    }

    SAFE_DELETE( mInput );
    SAFE_DELETE( mScripting );
    SAFE_DELETE( mGraphics );
    SAFE_DELETE( mWindowHandler );
    SAFE_DELETE( mConsoleWindow );
    SAFE_DELETE( mConsole );

    SAFE_CLOSE_HANDLE( mThread );
    SAFE_CLOSE_HANDLE( mProcess );
  }

  Engine::~Engine()
  {
    shutdown();
  }

}