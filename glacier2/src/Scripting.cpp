#include "StdAfx.h"
#include "Scripting.h"
#include "Script.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Scripting::Scripting( Engine* engine ): EngineComponent( engine ),
  mIsolate( nullptr )
  {
    mEngine->getConsole()->printf( Console::srcScripting,
      L"Initializing scripting..." );

    mEngine->getConsole()->printf( Console::srcScripting,
      L"Using V8 %S", v8::V8::GetVersion() );

    v8::V8::InitializeICU();

    mIsolate = v8::Isolate::GetCurrent();
  }

  Scripting::~Scripting()
  {
    v8::V8::Dispose();
  }

  void Scripting::test()
  {
    Script s( mIsolate );
    if ( s.compile( L"print('hello world');" ) )
      s.execute();
  }

}