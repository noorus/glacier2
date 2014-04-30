#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    string Console::className( "Console" );

    Console::Console( Glacier::Console* console ): mConsole( console ),
    ObjectWrapper( Wrapped_Console )
    {
      //
    }

    void Console::initialize( Handle<v8::Context> context )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );
      v8::Context::Scope contextScope( context );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate );

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_SET( tpl, "print", jsPrint );

      Console* console = new Console( gEngine->getConsole() );
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      console->wrap( object );

      context->Global()->Set( Util::allocString( className.c_str() ), object );
    }

    void Console::jsPrint( const FunctionCallbackInfo<v8::Value>& args )
    {
      Console* ptr = unwrap( args.Holder() );
      HandleScope handleScope( args.GetIsolate() );

      wstring msg;
      for ( int i = 0; i < args.Length(); i++ )
      {
        if ( i > 0 )
          msg.append( L" " );
        v8::String::Value str( args[i] );
        if ( *str )
          msg.append( (const wchar_t*)*str );
      }

      ptr->getConsole()->printf( Glacier::Console::srcScripting, msg.c_str() );
    }

    Glacier::Console* Console::getConsole()
    {
      return mConsole;
    }

  }

}