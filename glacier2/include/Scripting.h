#pragma once
#include "EngineComponent.h"
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
    v8::Isolate* getIsolation();
    v8::Persistent<v8::Context>& getContext();
    void initialize();
    void shutdown();
    DataStreamPtr openScriptFile( const Ogre::String& filename );
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    void test( const wstring& filename );
  };

  //! @}

  //! @}

}