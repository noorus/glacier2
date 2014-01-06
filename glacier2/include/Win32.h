#pragma once
#include "Utilities.h"

namespace Glacier {

  class Win32: public Singleton<Win32> {
  friend class Singleton<Win32>;
  protected:
    HMODULE mRichEdit;
    Win32();
  public:
    void initialize();
    void shutdown();
  };

}