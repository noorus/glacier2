#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  void Print( const v8::FunctionCallbackInfo<v8::Value>& args )
  {
    wstring msg;
    bool first = true;
    for ( int i = 0; i < args.Length(); i++ )
    {
      v8::HandleScope handleScope( args.GetIsolate() );
      if ( first )
        first = false;
      else
        msg.append( L" " );
      v8::String::Value str( args[i] );
      const wchar_t* cstr = (const wchar_t*)*str;
      if ( cstr )
        msg.append( cstr );
    }
    if ( gEngine && !msg.empty() )
      gEngine->getConsole()->printf( Console::srcScripting, msg.c_str() );
  }

  Script::Script( v8::Isolate* isolation ): mIsolate( isolation )
  {
    v8::HandleScope handleScope( mIsolate );

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
    global->Set(
      v8::String::NewFromUtf8( mIsolate, "print" ),
      v8::FunctionTemplate::New( mIsolate, Print ) );

    v8::Handle<v8::Context> context = v8::Context::New( mIsolate,
      nullptr, global );

    mContext.Reset( mIsolate, context );
  }

  bool Script::compile( const wstring& source )
  {
    v8::HandleScope handleScope( mIsolate );
    v8::Context::Scope contextScope( v8::Handle<v8::Context>::New( mIsolate, mContext ) );
    v8::TryCatch tryCatch;

    v8::Handle<v8::Script> script = v8::Script::Compile(
      v8::String::NewFromTwoByte( mIsolate, (uint16_t*)source.c_str() ) );

    if ( script.IsEmpty() )
    {
      v8::String::Value errorValue( tryCatch.Exception() );
      v8::Handle<v8::Message> errorMessage = tryCatch.Message();
      if ( !errorMessage.IsEmpty() )
      {
      }
      return false;
    }

    mScript.Reset( mIsolate, script );

    return true;
  }

  bool Script::execute()
  {
    if ( mScript.IsEmpty() )
      return false;

    v8::HandleScope handleScope( mIsolate );
    v8::Context::Scope contextScope( v8::Handle<v8::Context>::New( mIsolate, mContext ) );
    v8::TryCatch tryCatch;

    v8::Local<v8::Script> script = v8::Local<v8::Script>::New( mIsolate, mScript );
    v8::Handle<v8::Value> result = script->Run();
    if ( result.IsEmpty() )
    {
      return false;
    }

    return true;
  }

  Script::~Script()
  {
    mScript.Reset();
    mContext.Reset();
  }

}