#pragma once
#include "EngineComponent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Scripting: public EngineComponent {
  protected:
    v8::Isolate* mIsolate;
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    void test();
  };

}