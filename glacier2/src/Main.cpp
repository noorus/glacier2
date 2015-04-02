#include "StdAfx.h"
#include "Exception.h"
#include "Engine.h"
#include "Console.h"
#include "Win32.h"
#include "ServiceLocator.h"
#include "NedPoolMemory.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

using namespace Glacier;

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPWSTR lpCmdLine, int nCmdShow )
{
  UNREFERENCED_PARAMETER( hPrevInstance );
  UNREFERENCED_PARAMETER( lpCmdLine );
  UNREFERENCED_PARAMETER( nCmdShow );

  // Enable leak checking in debug builds
#if defined( _DEBUG ) || defined( DEBUG )
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  // CRT memory allocation breakpoints can be set here
  // _CrtSetBreakAlloc( x );

  // Initialize & provide the pooled memory service
  NedPoolMemory memoryService;
  Locator::provideMemory( &memoryService );

  // Parse command line arguments into engine options
  int argCount;
  wchar_t** arguments = CommandLineToArgvW( lpCmdLine, &argCount );
  if ( !arguments )
    return EXIT_FAILURE;

  Engine::Options options;

  int i = 0;
  while ( i < argCount )
  {
    if ( _wcsicmp( arguments[i], L"-noaudio" ) == 0 ) {
      options.noAudio = true;
    } else if ( _wcsicmp( arguments[i], L"-nophysics" ) == 0 ) {
      options.noPhysics = true;
    } else if ( _wcsicmp( arguments[i], L"-exec" ) == 0 ) {
      i++;
      if ( i < argCount )
        options.additionalExecs.push_back( arguments[i] );
    }
    i++;
  }

  LocalFree( arguments );

  Win32::Win32::instance().prepareProcess();
  Win32::Win32::instance().adjustPrivileges();

  // Init generic win32 stuff and our error dialog context
  Win32::Win32::instance().initialize();
  Win32::ErrorDialog::Context error( hInstance );
  error.title = L"Fatal engine error occurred";

  // TODO: Add SEH guard to neatly handle access violations

#ifndef _DEBUG
  try
  {
#endif
    gEngine = new Engine( hInstance );
    gEngine->initialize( options );
    gEngine->run();
    gEngine->shutdown();
    SAFE_DELETE( gEngine );
#ifndef _DEBUG
  }
  catch ( Exception& e )
  {
    error.subtitle = L"Program fault: Glacier² engine exception";
    error.body = Utilities::utf8ToWide( e.getFullDescription() );
    Win32::ErrorDialog dialog( error );
  }
  catch ( Ogre::Exception& e )
  {
    error.subtitle = L"Program fault: Ogre3D runtime exception";
    error.body = Utilities::utf8ToWide( e.getFullDescription() );
    Win32::ErrorDialog dialog( error );
  }
  catch ( Nil::Exception& e )
  {
    error.subtitle = L"Program fault: NIL runtime exception";
    error.body = Utilities::utf8ToWide( e.getFullDescription() );
    Win32::ErrorDialog dialog( error );
  }
  catch ( std::exception& e )
  {
    error.subtitle = L"Program fault: StdLib runtime exception";
    error.body = Utilities::utf8ToWide( e.what() );
    Win32::ErrorDialog dialog( error );
  }
  catch ( ... )
  {
    error.subtitle = L"Program fault: Unknown exception";
    error.body = L"An un-identified exception occurred in the program code.";
    Win32::ErrorDialog dialog( error );
  }
#endif

  Win32::Win32::instance().shutdown();

  return EXIT_SUCCESS;
}