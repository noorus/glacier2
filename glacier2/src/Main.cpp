#include "StdAfx.h"
#include "Exception.h"
#include "GlacierMemory.h"
#include "Engine.h"
#include "Console.h"
#include "Win32.h"

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
  return _vsnprintf(pDestination, n, pFormat, arguments);
}

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

  Glacier::Win32::Win32::instance().initialize();
  Glacier::Win32::ErrorDialog::Context error( hInstance );
  error.title = L"Fatal engine error occurred";

  // TODO: Add SEH guard to neatly handle access violations

  try
  {
    gEngine = new Glacier::Engine( hInstance );
    gEngine->initialize();
    gEngine->run();
    gEngine->shutdown();
    SAFE_DELETE( gEngine );
  }
  catch ( Glacier::Exception& e )
  {
    error.subtitle = L"Program fault: Glacier² engine exception";
    error.body = e.getFullDescription();
    Glacier::Win32::ErrorDialog dialog( error );
  }
  catch ( Ogre::Exception& e )
  {
    error.subtitle = L"Program fault: Ogre3D runtime exception";
    error.body = Glacier::Utilities::utf8ToWide( e.getFullDescription() );
    Glacier::Win32::ErrorDialog dialog( error );
  }
  catch ( std::exception& e )
  {
    error.subtitle = L"Program fault: StdLib runtime exception";
    error.body = Glacier::Utilities::utf8ToWide( e.what() );
    Glacier::Win32::ErrorDialog dialog( error );
  }
  catch ( ... )
  {
    error.subtitle = L"Program fault: Unknown exception";
    error.body = L"An un-identified exception occurred in the program code.";
    Glacier::Win32::ErrorDialog dialog( error );
  }

  Glacier::Win32::Win32::instance().shutdown();

  return EXIT_SUCCESS;
}