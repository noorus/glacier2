#pragma once
#include "EngineComponent.h"
#include <v8.h>

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  class Scripting: public EngineComponent {
  friend class Script;
  protected:
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> mContext;
    v8::Local<v8::String> allocString( const wstring& str );
    v8::Local<v8::String> allocString( const Ogre::String& str );
    v8::Isolate* getIsolation();
    v8::Persistent<v8::Context>& getContext();
    void initialize();
    void shutdown();
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    static void registerResources( ResourceGroupManager& manager );
    static void unregisterResources( ResourceGroupManager& manager );
    void test( const Ogre::String& filename );
  };

}