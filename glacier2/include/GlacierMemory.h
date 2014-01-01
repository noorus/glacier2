#pragma once
#include "Utilities.h"

namespace Glacier {

  class Memory: public Singleton<Memory> {
  friend class Singleton<Memory>;
  private:
    Memory();
    ~Memory();
  protected:
    nedalloc::nedpool* mPool;
  public:
    void* alloc( size_t size, size_t alignment = 0Ui64 );
    void* realloc( void* location, size_t size, size_t alignment = 0Ui64 );
    void free( void* location );
  };

}