#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Script {
  protected:
    v8::Isolate* mIsolate;
    v8::Persistent<v8::Context> mContext;
    v8::Persistent<v8::Script> mScript;
    inline v8::Local<v8::String> allocString( const wstring& str );
  public:
    Script( v8::Isolate* isolation );
    bool compile( const wstring& source );
    bool execute();
    ~Script();
  };

}