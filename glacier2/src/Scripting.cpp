#include "StdAfx.h"
#include "Scripting.h"

namespace Glacier {

  Scripting::Scripting()
  {
    v8::V8::InitializeICU();
  }

  Scripting::~Scripting()
  {
    v8::V8::Dispose();
  }

  void Scripting::test()
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

}