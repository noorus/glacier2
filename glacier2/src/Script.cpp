#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using v8::Isolate;
  using v8::Handle;
  using v8::Persistent;
  using v8::Local;
  using v8::Context;
  using v8::TryCatch;
  using v8::HandleScope;
  using v8::FunctionCallbackInfo;
  using v8::ObjectTemplate;
  using v8::FunctionTemplate;
  using v8::Value;

  void Print( const FunctionCallbackInfo<Value>& args )
  {
    wstring msg;
    bool first = true;
    for ( int i = 0; i < args.Length(); i++ )
    {
      HandleScope handleScope( args.GetIsolate() );
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

  Script::Script( Isolate* isolation ): mIsolate( isolation )
  {
    HandleScope handleScope( mIsolate );

    Handle<ObjectTemplate> global = ObjectTemplate::New();
    global->Set(
      allocString( L"print" ),
      FunctionTemplate::New( mIsolate, Print ) );

    Handle<Context> context = Context::New( mIsolate,
      nullptr, global );

    mContext.Reset( mIsolate, context );
  }

  inline Local<v8::String> Script::allocString( const wstring& str )
  {
    return v8::String::NewFromTwoByte( mIsolate, (uint16_t*)str.c_str() );
  }

  bool Script::compile( const wstring& source )
  {
    HandleScope handleScope( mIsolate );
    Context::Scope contextScope( Handle<Context>::New( mIsolate, mContext ) );
    TryCatch tryCatch;

    Handle<v8::Script> script = v8::Script::Compile( allocString( source ) );

    if ( script.IsEmpty() )
    {
      v8::String::Value errorValue( tryCatch.Exception() );
      Handle<v8::Message> errorMessage = tryCatch.Message();
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

    HandleScope handleScope( mIsolate );
    Context::Scope contextScope( Handle<Context>::New( mIsolate, mContext ) );
    TryCatch tryCatch;

    Local<v8::Script> script = Local<v8::Script>::New( mIsolate, mScript );
    Handle<Value> result = script->Run();
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