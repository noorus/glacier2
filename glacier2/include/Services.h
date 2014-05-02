#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Audio {
  public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void restart() = 0;
  };

  class NullAudio: public Audio {
  public:
    virtual void initialize() { /* Nothing */ }
    virtual void shutdown() { /* Nothing */ }
    virtual void restart() { /* Nothing */ }
  };

  class Memory {
  public:
    virtual void* alloc( size_t size, size_t alignment = 0Ui64 ) = 0;
    virtual void* realloc( void* location, size_t size, size_t alignment = 0Ui64 ) = 0;
    virtual void free( void* location ) = 0;
  };

}