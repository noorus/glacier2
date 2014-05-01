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

}