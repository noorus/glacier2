#include "StdAfx.h"
#include "Engine.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "Scripting.h"
#include "Graphics.h"
#include "Utilities.h"
#include "Exception.h"

Glacier::Engine* gEngine = nullptr;

namespace Glacier {

  // Engine timing values

  GameTime  Engine::fTime = 0.0;
  LocalTime Engine::fTimeDelta = 0.0;
  GameTime  Engine::fTimeAccumulator = 0.0;
  LocalTime Engine::fLogicStep = 1.0 / 60.0;

  // Engine version struct

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

  // Engine class

  Engine::Engine( HINSTANCE instance ):
  mConsole( nullptr ), mScripting( nullptr ), mGraphics( nullptr ),
  mProcess( NULL ), mThread( NULL ), mInstance( instance ),
  mSignal( Signal_None ), mVersion( 0, 1, 1 ), mConsoleWindow( nullptr )
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
    DWORD_PTR engineMask = 1; // target 1st core
    while ( ( engineMask & processMask ) == 0 )
    {
      core++;
      engineMask <<= 1;
    }
    uint32_t cores = Utilities::hammingWeight64( processMask );
    getConsole()->printf( Console::srcEngine,
      L"Fixating engine main thread to core %d (out of %d)", core, cores );
    if ( !SetThreadAffinityMask( mThread, engineMask ) )
      getConsole()->printf( Console::srcEngine, L"Failed to set thread affinity mask" );
  }

  void Engine::initialize()
  {
    mProcess = GetCurrentProcess();
    mThread = GetCurrentThread();

    mConsole = new Console();
    mConsoleWindow = new ConsoleWindow( mInstance, mConsole );

    // Print engine info
    mConsole->printf( Console::srcEngine, getVersion().title.c_str() );
    mConsole->printf( Console::srcEngine, getVersion().subtitle.c_str() );

    adjustPrivileges();
    fixupThreadAffinity();

    // Fetch HPC frequency
    if ( !QueryPerformanceFrequency( &qwiTimerFrequency ) )
      ENGINE_EXCEPT_W32( L"Couldn't query HPC frequency" );

    mScripting = new Scripting();
    mGraphics = new Graphics();
  }

  void Engine::run()
  {
    while ( mSignal != Signal_Stop )
    {
      mConsoleWindow->step();
    }
  }

  void Engine::shutdown()
  {
    SAFE_DELETE( mGraphics );
    SAFE_DELETE( mScripting );
    SAFE_DELETE( mConsoleWindow );
    SAFE_DELETE( mConsole );

    if ( mThread )
      CloseHandle( mThread );
    if ( mProcess )
      CloseHandle( mProcess );
  }

  Engine::~Engine()
  {
  }

}