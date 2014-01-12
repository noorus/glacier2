#include "StdAfx.h"
#include "Engine.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "Scripting.h"
#include "Graphics.h"
#include "Physics.h"
#include "Utilities.h"
#include "Exception.h"
#include "Sound.h"
#include "Game.h"

Glacier::Engine* gEngine = nullptr;

namespace Glacier {

  // Engine timing values =====================================================

  GameTime Engine::fTime = 0.0;
  GameTime Engine::fTimeDelta = 0.0;
  GameTime Engine::fTimeAccumulator = 0.0;
  GameTime Engine::fLogicStep = 1.0 / 60.0;

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
  ENGINE_DECLARE_CONCMD( screenshot,
    L"Save a screenshot to working directory.", Engine::callbackScreenshot );
  ENGINE_DECLARE_CONCMD( quit,
    L"Quit.", Engine::callbackQuit );

  Engine::Engine( HINSTANCE instance ):
  mConsole( nullptr ), mScripting( nullptr ), mGraphics( nullptr ),
  mProcess( NULL ), mThread( NULL ), mInstance( instance ),
  mSignal( Signal_None ), mVersion( 0, 1, 1 ), mConsoleWindow( nullptr ),
  mGame( nullptr )
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
    privileges->PrivilegeCount = cPrivileges;
    for ( int i = 0; i < cPrivileges; i++ )
    {
      if ( !LookupPrivilegeValueW( NULL, wantedPrivileges[i], &privileges->Privileges[i].Luid ) )
        return;
      privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
    }
    AdjustTokenPrivileges( token, FALSE, privileges, 0, NULL, NULL );
    free( privileges );
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
      getConsole()->errorPrintf( L"Failed to set thread affinity mask" );
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

  void Engine::initialize()
  {
    // Get process & thread handles
    mProcess = GetCurrentProcess();
    mThread = GetCurrentThread();

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

    // Create subsystems
    mScripting = new Scripting( this );
    mGraphics = new Graphics( this );
    mPhysics = new Physics( this );
    mSound = new Sound( this );
    mGame = new Game( this );
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
        mPhysics->componentTick( fLogicStep, fTime );
        mGame->componentTick( fLogicStep, fTime );
        mSound->componentTick( fLogicStep, fTime );
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

    gEngine->mSignal = Signal_Stop;
  }

  void Engine::shutdown()
  {
    SAFE_DELETE( mGame );
    SAFE_DELETE( mSound );
    SAFE_DELETE( mPhysics );
    SAFE_DELETE( mGraphics );
    SAFE_DELETE( mScripting );
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