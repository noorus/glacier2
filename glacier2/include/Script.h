#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Scripting;

  class Script {
  protected:
    Scripting* mHost;
    v8::Persistent<v8::Script> mScript;
  public:
    Script( Scripting* host );
    bool compile( const Ogre::String& source );
    bool execute();
    ~Script();
  };

}