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

  Script::Script( Scripting* host ): mHost( host )
  {
  }

  bool Script::compile( const Ogre::String& source )
  {
    HandleScope handleScope( mHost->getIsolation() );
    Context::Scope contextScope( Handle<Context>::New( mHost->getIsolation(), mHost->getContext() ) );
    TryCatch tryCatch;

    Handle<v8::Script> script = v8::Script::Compile( mHost->allocString( source ) );

    if ( script.IsEmpty() )
    {
      v8::String::Value errorValue( tryCatch.Exception() );
      Handle<v8::Message> errorMessage = tryCatch.Message();
      if ( !errorMessage.IsEmpty() )
      {
      }
      return false;
    }

    mScript.Reset( mHost->getIsolation(), script );

    return true;
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