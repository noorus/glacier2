#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"
#include "Exception.h"
#include "JSNatives.h"
#include "JSUtil.h"
#include "ServiceLocator.h"
#include "Win32.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const Ogre::String cJSResourceGroup = "JavaScript";

  Scripting::Scripting( Engine* engine ): EngineComponent( engine ),
  mIsolate( nullptr )
  {
    mEngine->getConsole()->printf( Console::srcScripting,
      L"Initializing V8 v%S", v8::V8::GetVersion() );

    v8::V8::InitializeICU();

    auto dir = UTFString( Win32::Win32::instance().getCurrentDirectory() );
    dir.append( "\\" );

    v8::V8::InitializeExternalStartupData( dir.asUTF8_c_str() );

    mPlatform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform( mPlatform );

    v8::V8::Initialize();

    v8::Isolate::CreateParams params;
    params.array_buffer_allocator = this;

    mIsolate = v8::Isolate::New( params );
    if ( !mIsolate )
      ENGINE_EXCEPT( "Creating v8 default Isolate failed" );

    mIsolate->Enter();

    v8::V8::SetCaptureStackTraceForUncaughtExceptions(
      true, 5, v8::StackTrace::kDetailed );

    initialize();
  }

  void* Scripting::Allocate( size_t length )
  {
    return Locator::getMemory().alloc( Memory::Sector_Generic, length );
  }

  void* Scripting::AllocateUninitialized( size_t length )
  {
    return Locator::getMemory().alloc( Memory::Sector_Generic, length );
  }

  void Scripting::Free( void* data, size_t length )
  {
    Locator::getMemory().free( Memory::Sector_Generic, data );
  }

  v8::Isolate* Scripting::getIsolate()
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
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New( mIsolate );

    // Initialize native classes in the global namespace
    JS::Vector3::initialize( global );
    JS::Quaternion::initialize( global );

    // Create our one and only global context
    v8::Handle<v8::Context> context = v8::Context::New( mIsolate,
      nullptr, global );

    // Switch it in place
    mContext.Reset( mIsolate, context );

    // Initialize native objects in the global namespace
    JS::Console::initialize( mEngine->getConsole(), context );
    JS::Colors::initialize( context );
  }

  void Scripting::clearScripts()
  {
    for ( auto script : mScripts )
      delete script;

    mScripts.clear();
  }

  void Scripting::shutdown()
  {
    clearScripts();
    JS::Colors::shutdown();
    JS::Console::shutdown();
    mContext.Reset();
  }

  Script* Scripting::getScript( const wstring& filename )
  {
    for ( auto script : mScripts )
      if ( boost::iequals( script->getName(), filename ) )
        return script;

    return nullptr;
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
    if ( mIsolate )
    {
      mIsolate->Exit();
      mIsolate->Dispose();
    }
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    if ( mPlatform )
      delete mPlatform;
  }

  DataStreamPtr Scripting::openScriptFile( const Ogre::String& filename )
  {
    DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(
      filename, cJSResourceGroup );
    return stream;
  }

  void Scripting::simpleExecute( const wstring& filename )
  {
    Script* script = getScript( filename );

    if ( !script )
    {
      script = new Script( *this, filename );
      mScripts.push_back( script );
    }

    if ( !script->isSimple() )
      ENGINE_EXCEPT( "Script is registered as non-simple" );

    script->compile();
    script->execute();
  }

}