#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "JSNatives.h"
#include "JSUtil.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Ogre::String cJSResourceGroup = "JavaScript";

  Scripting::Scripting( Engine* engine ): EngineComponent( engine ),
  mIsolate( nullptr )
  {
    mEngine->getConsole()->printf( Console::srcScripting,
      L"Initializing scripting..." );

    mEngine->getConsole()->printf( Console::srcScripting,
      L"Using V8 %S", v8::V8::GetVersion() );

    v8::V8::InitializeICU();
    v8::V8::SetCaptureStackTraceForUncaughtExceptions( true, 10 );

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

  void Scripting::initialize()
  {
    v8::HandleScope handleScope( mIsolate );

    // Set up global object template
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

    // Initialize native classes in the global namespace
    JS::Vector3::initialize( global );
    JS::Quaternion::initialize( global );

    // Create our one and only global context
    v8::Handle<v8::Context> context = v8::Context::New( mIsolate,
      nullptr, global );

    // Switch it in place
    mContext.Reset( mIsolate, context );

    JS::Console::initialize( context );
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

  DataStreamPtr Scripting::openScriptFile( const Ogre::String& filename )
  {
    DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(
      filename, cJSResourceGroup );
    return stream;
  }

  void Scripting::test( const wstring& filename )
  {
    Script script( filename, this );
    if ( script.compile() )
      script.execute();
  }

}