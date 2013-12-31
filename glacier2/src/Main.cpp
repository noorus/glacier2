#include "StdAfx.h"
#include <EASTL/hash_map.h>
#include <EASTL/string.h>

void* operator new[](size_t size, const char* pName, int flags,
unsigned debugFlags, const char* file, int line)
{
  return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
  EASTL_ASSERT(alignment <= 8);
  return malloc(size);
}

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
  return _vsnprintf(pDestination, n, pFormat, arguments);
}

void v8_test()
{
  v8::Isolate* isolate = v8::Isolate::GetCurrent();

  v8::HandleScope handle_scope( isolate );

  v8::Handle<v8::Context> context = v8::Context::New( isolate );

  v8::Context::Scope context_scope( context );

  v8::Handle<v8::String> source = v8::String::NewFromUtf8( isolate, "'V8' + ' works'" );

  v8::Handle<v8::Script> script = v8::Script::Compile( source );

  v8::Handle<v8::Value> result = script->Run();

  v8::String::Value out( result );

  MessageBoxW( 0, (LPCWSTR)*out, NULL, MB_OK );
}

void eastl_test_hash_map()
{
  eastl::hash_map<eastl::string, eastl::string> map;
  map["key1"] = "value1";
  map["key2"] = "value2";
  EASTL_ASSERT(map.find("key1")->second == "value1");
}

void eastl_test_string()
{
  eastl::string str;
  str += "testing ";
  str += "simple ";
  str += "string ";
  str += "concat";
  EASTL_ASSERT(str == "testing simple string concat");

  str.sprintf("%d", 20);
  EASTL_ASSERT(str == "20");
}

const unsigned long g_nPrivileges = 1;

void adjustPrivileges()
{
  HANDLE token;
  HANDLE process = GetCurrentProcess();
  if ( OpenProcessToken( process, TOKEN_ADJUST_PRIVILEGES, &token ) )
  {
    LPCWSTR wantedPrivileges[g_nPrivileges] = {
      SE_LOCK_MEMORY_NAME
    };
    PTOKEN_PRIVILEGES privileges = (PTOKEN_PRIVILEGES)malloc(
      FIELD_OFFSET( TOKEN_PRIVILEGES, Privileges[g_nPrivileges] ) );
    privileges->PrivilegeCount = g_nPrivileges;
    for ( int i = 0; i < g_nPrivileges; i++ )
    {
      if ( !LookupPrivilegeValueW( NULL, wantedPrivileges[i], &privileges->Privileges[i].Luid ) )
        return;
      privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
    }
    AdjustTokenPrivileges( token, FALSE, privileges, sizeof( privileges ), NULL, NULL );
    free( privileges );
  }
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

  adjustPrivileges();

  v8::V8::InitializeICU();
  v8_test();
  v8::V8::Dispose();

  eastl_test_hash_map();
  eastl_test_string();

  return EXIT_SUCCESS;
}