#include "StdAfx.h"
#include "Engine.h"
#include "Console.h"
#include "Scripting.h"
#include "Graphics.h"
#include "Utilities.h"

Glacier::Engine* gEngine = nullptr;

namespace Glacier {

  Engine::Engine( HINSTANCE instance ):
  mConsole( nullptr ), mScripting( nullptr ), mGraphics( nullptr ),
  mProcess( NULL ), mThread( NULL ), mInstance( instance ),
  mSignal( Signal_None )
  {
    initialize();
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
    while ( ( engineMask & processMask ) == 0 ) {
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

    adjustPrivileges();
    fixupThreadAffinity();

    mScripting = new Scripting();
    mGraphics = new Graphics();
  }

  void Engine::run()
  {
    while ( mSignal != Signal_Stop )
    {
      return;
    }
  }

  void Engine::shutdown()
  {
    SAFE_DELETE( mGraphics );
    SAFE_DELETE( mScripting );
    SAFE_DELETE( mConsole );

    if ( mThread )
      CloseHandle( mThread );
    if ( mProcess )
      CloseHandle( mProcess );
  }

  Engine::~Engine()
  {
    shutdown();
  }

  Console* Engine::getConsole()
  {
    return mConsole;
  }

  Scripting* Engine::getScripting()
  {
    return mScripting;
  }

  Graphics* Engine::getGraphics()
  {
    return mGraphics;
  }

}