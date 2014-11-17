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
    Console* Console::instance( nullptr );

    Console::Console( Glacier::Console* console ): mConsole( console ),
    ObjectWrapper( Wrapped_Console )
    {
      //
    }

    void Console::initialize( Glacier::Console* console_, Handle<v8::Context> context )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );
      v8::Context::Scope contextScope( context );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate );

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_SET( tpl, "print", jsPrint );
      JS_TEMPLATE_SET( tpl, "getVariable", jsGetVariable );
      JS_TEMPLATE_SET( tpl, "setVariable", jsSetVariable );
      JS_TEMPLATE_SET( tpl, "execute", jsExecute );

      instance = new Console( console_ );
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      instance->wrap( object );

      context->Global()->Set( Util::allocString( className.c_str() ), object );
      instance->ref();
    }

    void Console::shutdown()
    {
      if ( instance )
        instance->unref();
      SAFE_DELETE( instance );
    }

    //! \verbatim
    //! Console.print( ... )
    //! \endverbatim
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

    //! \verbatim
    //! String Console.getVariable( String variable )
    //! \endverbatim
    void Console::jsGetVariable( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      HandleScope handleScope( isolate );

      Console* console = unwrap( args.Holder() );

      if ( args.Length() != 1 || !args[0]->IsString() )
      {
        Util::throwException( isolate,
          L"Syntax error: String Console.getVariable( String variable )" );
        return;
      }

      v8::String::Value variableName( args[0] );

      ConVar* variable = console->getConsole()->getVariable(
        (const wchar_t*)*variableName );

      if ( !variable )
      {
        Util::throwException( isolate,
          L"Console.getVariable: No such console variable \"%s\"",
          (const wchar_t*)*variableName );
        return;
      }

      args.GetReturnValue().Set( Util::allocString( variable->getString(), isolate ) );
    }

    //! \verbatim
    //! Console.setVariable( String variable, Mixed value )
    //! \endverbatim
    void Console::jsSetVariable( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      HandleScope handleScope( isolate );

      Console* console = unwrap( args.Holder() );

      if ( args.Length() != 2 || !args[0]->IsString() )
      {
        Util::throwException( isolate,
          L"Syntax error: Console.setVariable( String variable, String value )" );
        return;
      }

      v8::String::Value variableName( args[0] );
      v8::String::Value variableValue( args[1] );

      ConVar* variable = console->getConsole()->getVariable(
        (const wchar_t*)*variableName );

      if ( !variable )
      {
        Util::throwException( isolate,
          L"Console.setVariable: No such console variable \"%s\"",
          (const wchar_t*)*variableName );
        return;
      }

      variable->setValue( (const wchar_t*)*variableValue );
    }

    //! \verbatim
    //! Console.execute( String commandLine )
    //! \endverbatim
    void Console::jsExecute( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      HandleScope handleScope( isolate );

      Console* console = unwrap( args.Holder() );

      if ( args.Length() != 1 || !args[0]->IsString() )
      {
        Util::throwException( isolate,
          L"Syntax error: Console.execute( String commandLine )" );
        return;
      }

      v8::String::Value commandLine( args[0] );

      console->getConsole()->execute( (const wchar_t*)*commandLine );
    }

    Glacier::Console* Console::getConsole()
    {
      return mConsole;
    }

  }

}