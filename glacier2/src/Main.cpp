#include "StdAfx.h"
#include "Exception.h"
#include "GlacierMemory.h"
#include "Engine.h"
#include "Console.h"

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

  gEngine = new Glacier::Engine( hInstance );
  gEngine->run();
  SAFE_DELETE( gEngine );

  return EXIT_SUCCESS;
}