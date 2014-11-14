#pragma once
#include "EngineComponent.h"
#include "Script.h"
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
  class Scripting: public EngineComponent {
  friend class Script;
  protected:
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> mContext;
    v8::Isolate* getIsolate();
    v8::Platform* mPlatform;
    v8::Persistent<v8::Context>& getContext();
    ScriptList mScripts;
    void initialize();
    void shutdown();
    void clearScripts();
    DataStreamPtr openScriptFile( const Ogre::String& filename );
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    Script* getScript( const wstring& filename );
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    void simpleExecute( const wstring& filename );
  };

  //! @}

  //! @}

}