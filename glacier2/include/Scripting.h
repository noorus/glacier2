#pragma once
#include "EngineComponent.h"

namespace Glacier {

  class Scripting: public EngineComponent {
  public:
    Scripting( Engine* engine );
    virtual ~Scripting();
    void test();
  };

}