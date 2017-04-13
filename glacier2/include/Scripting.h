#pragma once
#include "EngineComponent.h"
#include "Script.h"
#include "Console.h"
#include <v8.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Scripting
  //! @{

  //! \class Scripting
  //! Main engine component responsible of running all JS scripting.
  //! \sa EngineComponent
  class Scripting: public EngineComponent, public v8::ArrayBuffer::Allocator {
  friend class Script;
  protected:
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> mContext;
    v8::Platform* mPlatform;
    ScriptList mScripts;
    void initialize();
    void shutdown();
    void clearScripts();
    DataStreamPtr openScriptFile( const Ogre::String& filename );
    virtual void* Allocate( size_t length );
    virtual void* AllocateUninitialized( size_t length );
    virtual void Free( void* data, size_t length );
  public:
    // Callbacks
    static void callbackJSExecute( Console* console,
      ConCmd* command, StringVector& arguments );
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    v8::Persistent<v8::Context>& getContext();
    v8::Isolate* getIsolate();
    Script* getScript( const wstring& filename );
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    void simpleExecute( const wstring& filename );
  };

  //! @}

  //! @}

}