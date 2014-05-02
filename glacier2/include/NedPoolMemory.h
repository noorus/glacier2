#pragma once
#include "Utilities.h"
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class NedPoolMemory: public Memory {
  protected:
    glacier_nedalloc::nedpool* mPool;
  public:
    NedPoolMemory();
    ~NedPoolMemory();
    virtual void* alloc( size_t size, size_t alignment = 0Ui64 );
    virtual void* realloc( void* location, size_t size, size_t alignment = 0Ui64 );
    virtual void free( void* location );
  };

}