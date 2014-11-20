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

  Script::Script( Scripting& host, const wstring& name ):
  mHost( host ), mName( name ), mStatus( Status_Uncompiled )
  {
  }

  const wstring& Script::getName()
  {
    return mName;
  }

  const bool Script::isSimple()
  {
    return true;
  }

  void Script::changeStatus( Status status )
  {
    mStatus = status;
  }

  bool Script::compile()
  {
    HandleScope handleScope( mHost.getIsolate() );

    Local<Context> context = Local<Context>::New( mHost.getIsolate(), mHost.getContext() );
    Context::Scope contextScope( context );

    TryCatch tryCatch;

    DataStreamPtr stream = mHost.openScriptFile( Ogre::UTFString( mName ) );

    v8::ScriptOrigin origin( JS::Util::allocString( mName ) );

    Handle<v8::Script> script = v8::Script::Compile(
      JS::Util::allocString( stream->getAsString() ),
      &origin );

    stream->close();

    if ( script.IsEmpty() )
    {
      changeStatus( Status_CompileError );
      if ( tryCatch.HasCaught() )
        reportException( tryCatch );
      return false;
    }

    mScript.Reset( mHost.getIsolate(), script );
    changeStatus( Status_Compiled );

    return true;
  }

  void Script::reportException( const v8::TryCatch& tryCatch )
  {
    HandleScope handleScope( mHost.getIsolate() );

    Glacier::Console* console = gEngine->getConsole();

    v8::String::Value exceptionString( tryCatch.Exception() );
    Local<v8::Message> message = tryCatch.Message();
    if ( message.IsEmpty() )
    {
      console->printf( Console::srcScripting, L"Exception: %s", *exceptionString );
    }
    else
    {
      v8::String::Value scriptName( message->GetScriptResourceName() );
      v8::String::Value sourceLine( message->GetSourceLine() );
      console->printf( Console::srcScripting, L"V8 Script exception!" );
      console->printf( Console::srcScripting, L"●   %s", *exceptionString );
      console->printf( Console::srcScripting, L"In %s line %d:", *scriptName, message->GetLineNumber() );
      wstring trimmedLine( (const wchar_t*)*sourceLine );
      boost::trim( trimmedLine );
      console->printf( Console::srcScripting, L"→   %s", trimmedLine.c_str() );

      console->printf( Console::srcScripting, L"Stack trace:" );

      int frameCount = message->GetStackTrace()->GetFrameCount();
      for ( int i = 0; i < frameCount; i++ )
      {
        Local<v8::StackFrame> frame = message->GetStackTrace()->GetFrame( i );
        v8::String::Value functionName( frame->GetFunctionName() );
        v8::String::Value script( frame->GetScriptName() );
        
        if ( functionName.length() )
          console->printf( Console::srcScripting, L"→   in function %s() (%s line %d)", *functionName, *script, frame->GetLineNumber() );
        else
          console->printf( Console::srcScripting, L"→   in %s line %d", *script, frame->GetLineNumber() );
      }
    }
  }

  bool Script::execute()
  {
    v8::Isolate* isolate = mHost.getIsolate();

    if ( mScript.IsEmpty() )
      return false;

    changeStatus( Status_Executing );

    HandleScope handleScope( isolate );
    Local<Context> context = Local<Context>::New( isolate, mHost.getContext() );
    Context::Scope contextScope( context );

    TryCatch tryCatch;

    Local<v8::Script> script = Local<v8::Script>::New( isolate, mScript );

    Handle<Value> result = script->Run();

    if ( tryCatch.HasCaught() )
    {
      changeStatus( Status_RuntimeError );
      reportException( tryCatch );
    }

    if ( result.IsEmpty() )
      return false;

    changeStatus( Status_Compiled );

    return true;
  }

  Script::~Script()
  {
    mScript.Reset();
  }

}