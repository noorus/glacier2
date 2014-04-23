#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"
#include "JSUtil.h"

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

  Script::Script( const wstring& name, Scripting* host ):
  mName( name ) , mHost( host )
  {
  }

  bool Script::compile()
  {
    HandleScope handleScope( mHost->getIsolation() );
    Context::Scope contextScope( Handle<Context>::New( mHost->getIsolation(), mHost->getContext() ) );
    TryCatch tryCatch;

    DataStreamPtr stream = mHost->openScriptFile( Ogre::UTFString( mName ) );

    v8::ScriptOrigin origin( JS::Util::allocString( mName ) );

    Handle<v8::Script> script = v8::Script::Compile(
      JS::Util::allocString( stream->getAsString() ),
      &origin );

    stream->close();

    if ( script.IsEmpty() )
    {
      if ( tryCatch.HasCaught() )
        reportException( tryCatch );
      return false;
    }

    mScript.Reset( mHost->getIsolation(), script );

    return true;
  }

  void Script::reportException( const v8::TryCatch& tryCatch )
  {
    HandleScope handleScope( mHost->getIsolation() );

    v8::String::Value exceptionString( tryCatch.Exception() );
    Local<v8::Message> message = tryCatch.Message();
    if ( message.IsEmpty() )
    {
      gEngine->getConsole()->errorPrintf( L"v8 Exception: %s", *exceptionString );
    }
    else
    {
      v8::String::Value scriptName( message->GetScriptResourceName() );
      v8::String::Value sourceLine( message->GetSourceLine() );
      gEngine->getConsole()->errorPrintf( L"v8 Exception in %s:", *scriptName );
      gEngine->getConsole()->errorPrintf( L"%s", *exceptionString );
      gEngine->getConsole()->errorPrintf( L"On line %d: %s", message->GetLineNumber(), *sourceLine );

      /*int frameCount = message->GetStackTrace()->GetFrameCount();
      for ( int i = 0; i < frameCount; i++ )
      {
        Local<v8::StackFrame> frame = message->GetStackTrace()->GetFrame( i );
        v8::String::Value functionName( frame->GetFunctionName() );
        gEngine->getConsole()->errorPrintf( L"[Frame %d] %s", i, *functionName );
      }*/
    }
  }

  bool Script::execute()
  {
    if ( mScript.IsEmpty() )
      return false;

    HandleScope handleScope( mHost->getIsolation() );
    Context::Scope contextScope( Handle<Context>::New( mHost->getIsolation(), mHost->getContext() ) );
    TryCatch tryCatch;

    Local<v8::Script> script = Local<v8::Script>::New( mHost->getIsolation(), mScript );
    Handle<Value> result = script->Run();
    if ( tryCatch.HasCaught() )
      reportException( tryCatch );
    if ( result.IsEmpty() )
    {
      return false;
    }

    return true;
  }

  Script::~Script()
  {
    mScript.Reset();
  }

}