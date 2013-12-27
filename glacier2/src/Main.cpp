#include "StdAfx.h"

void v8test()
{
  v8::Isolate* isolate = v8::Isolate::GetCurrent();

  v8::HandleScope handle_scope( isolate );

  v8::Handle<v8::Context> context = v8::Context::New( isolate );

  v8::Context::Scope context_scope( context );

  v8::Handle<v8::String> source = v8::String::NewFromUtf8( isolate, "'Hello' + ', World!'" );

  v8::Handle<v8::Script> script = v8::Script::Compile( source );

  v8::Handle<v8::Value> result = script->Run();

  v8::String::Value out( result );

  MessageBoxW( 0, (LPCWSTR)*out, NULL, MB_OK );
}

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPWSTR lpCmdLine, int nCmdShow )
{
  UNREFERENCED_PARAMETER( hPrevInstance );
  UNREFERENCED_PARAMETER( lpCmdLine );
  UNREFERENCED_PARAMETER( nCmdShow );

  // Enable leak checking in debug builds
#if defined( _DEBUG ) | defined( DEBUG )
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  // CRT memory allocation breakpoints can be set here
  // _CrtSetBreakAlloc( x );

  v8::V8::InitializeICU();
  v8test();
  v8::V8::Dispose();

  return EXIT_SUCCESS;
}