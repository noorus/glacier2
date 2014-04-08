#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "JSNatives.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Ogre::String cJSResourceGroup = "JavaScript";

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

  Scripting::Scripting( Engine* engine ): EngineComponent( engine ),
  mIsolate( nullptr )
  {
    mEngine->getConsole()->printf( Console::srcScripting,
      L"Initializing scripting..." );

    mEngine->getConsole()->printf( Console::srcScripting,
      L"Using V8 %S", v8::V8::GetVersion() );

    v8::V8::InitializeICU();

    mIsolate = v8::Isolate::GetCurrent();
    if ( !mIsolate )
      ENGINE_EXCEPT( L"Getting v8 default Isolate failed" );

    initialize();
  }

  v8::Isolate* Scripting::getIsolation()
  {
    return mIsolate;
  }

  v8::Persistent<v8::Context>& Scripting::getContext()
  {
    return mContext;
  }

  v8::Local<v8::String> Scripting::allocString( const wstring& str )
  {
    return v8::String::NewFromTwoByte( mIsolate, (uint16_t*)str.c_str() );
  }

  v8::Local<v8::String> Scripting::allocString( const Ogre::String& str )
  {
    return v8::String::NewFromUtf8( mIsolate, str.c_str() );
  }

  void Scripting::initialize()
  {
    v8::HandleScope handleScope( mIsolate );

    // Set up global object template
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
    global->Set(
      allocString( L"print" ),
      v8::FunctionTemplate::New( mIsolate, Print ) );

    // Initialize native classes in the global namespace
    JS::Vector3::initialize( global );

    // Create our one and only global context
    v8::Handle<v8::Context> context = v8::Context::New( mIsolate,
      nullptr, global );

    // Switch it in place
    mContext.Reset( mIsolate, context );
  }

  void Scripting::shutdown()
  {
    // Destroy the global context
    mContext.Reset();
  }

  void Scripting::registerResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcScripting,
      L"Registering resources..." );

    manager.addResourceLocation( "data\\js", "FileSystem", cJSResourceGroup );
  }

  void Scripting::unregisterResources( ResourceGroupManager& manager )
  {
    gEngine->getConsole()->printf( Console::srcScripting,
      L"Unregistering resources..." );

    manager.removeResourceLocation( "data\\js", cJSResourceGroup );
  }

  Scripting::~Scripting()
  {
    shutdown();
    v8::V8::Dispose();
  }

  void Scripting::test( const Ogre::String& filename )
  {
    Script script( this );
    DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource( filename, cJSResourceGroup );
    Ogre::String str = stream->getAsString();
    mEngine->getConsole()->printf( Console::srcScripting, L"name is %S", stream->getName().c_str() );
    stream->close();
    if ( script.compile( str ) )
      script.execute();
  }

}