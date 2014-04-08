#include "StdAfx.h"
#include "JSNatives.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    const char* cJSVector3Class = "Vector3";
    Eternal<v8::FunctionTemplate> Vector3::constructor;

    Vector3::Vector3( const Ogre::Vector3& source ): Ogre::Vector3( source )
    {
      //
    }

    Vector3::~Vector3()
    {
      //
    }

    void Vector3::initialize( Handle<ObjectTemplate> exports )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( Vector3::create );

      tpl->SetClassName( newStr( cJSVector3Class ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      tpl->PrototypeTemplate()->SetAccessor( newStr( "x" ),
        Vector3::jsGetX, Vector3::jsSetX );

      tpl->PrototypeTemplate()->SetAccessor( newStr( "y" ),
        Vector3::jsGetY, Vector3::jsSetY );

      tpl->PrototypeTemplate()->SetAccessor( newStr( "z" ),
        Vector3::jsGetZ, Vector3::jsSetZ );

      tpl->PrototypeTemplate()->Set( newStr( "length" ),
        FunctionTemplate::New( jsLength ) );

      tpl->PrototypeTemplate()->Set( newStr( "squaredLength" ),
        FunctionTemplate::New( jsSquaredLength ) );

      tpl->PrototypeTemplate()->Set( newStr( "distance" ),
        FunctionTemplate::New( jsDistance ) );

      tpl->PrototypeTemplate()->Set( newStr( "squaredDistance" ),
        FunctionTemplate::New( jsSquaredDistance ) );

      tpl->PrototypeTemplate()->Set( newStr( "dotProduct" ),
        FunctionTemplate::New( jsDotProduct ) );

      tpl->PrototypeTemplate()->Set( newStr( "absDotProduct" ),
        FunctionTemplate::New( jsAbsDotProduct ) );

      tpl->PrototypeTemplate()->Set( newStr( "normalise" ),
        FunctionTemplate::New( jsNormalise ) );

      tpl->PrototypeTemplate()->Set( newStr( "crossProduct" ),
        FunctionTemplate::New( jsCrossProduct ) );

      constructor.Set( isolate, tpl );

      exports->Set( isolate, cJSVector3Class, tpl );
    }

    //! Vector3()
    //! Vector3( Real scalar )
    //! Vector3( Real x, Real y, Real z )
    //! Vector3([Real x, Real y, Real z])
    void Vector3::create( const FunctionCallbackInfo<v8::Value>& args )
    {
      HandleScope handleScope( args.GetIsolate() );
      if ( !args.IsConstructCall() )
      {
        args.GetIsolate()->ThrowException(
          v8::String::NewFromUtf8( args.GetIsolate(),
          "Function called as non-constructor" ) );
        return;
      }
      Ogre::Vector3 vec( Ogre::Vector3::ZERO );
      if ( args.Length() == 3 )
      {
        vec.x = args[0]->NumberValue();
        vec.y = args[1]->NumberValue();
        vec.z = args[2]->NumberValue();
      }
      else if ( args.Length() == 1 )
      {
        if ( args[0]->IsNumber() )
        {
          vec.x = vec.y = vec.z = args[0]->NumberValue();
        }
        else if ( args[0]->IsArray() )
        {
          v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( args[0] );
          if ( arr->Length() == 3 )
          {
            vec.x = arr->Get( 0 )->NumberValue();
            vec.y = arr->Get( 1 )->NumberValue();
            vec.z = arr->Get( 2 )->NumberValue();
          }
        }
      }
      Vector3* object = new Vector3( vec );
      object->wrap( args.This() );
      args.GetReturnValue().Set( args.This() );
    }

    //! Expect and extract a Vector3 object as argument[0],
    //! throw JS exception and return null on failure
    Vector3* Vector3::extractVector3Argument(
    const FunctionCallbackInfo<v8::Value>& args )
    {
      if ( args.Length() != 1 || !args[0]->IsObject() )
      {
        args.GetIsolate()->ThrowException(
          v8::String::NewFromUtf8( args.GetIsolate(),
          "Argument must be a Vector3" ) );
        return nullptr;
      }
      // TODO actually verify object type..
      Vector3* that = unwrap<Vector3>( args[0]->ToObject() );
      return that;
    }

    //! Real Vector3.x getter
    void Vector3::jsGetX( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      info.GetReturnValue().Set( ptr->x );
    }

    //! Real Vector3.x setter
    void Vector3::jsSetX( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      ptr->x = value->NumberValue();
    }

    //! Real Vector3.y getter
    void Vector3::jsGetY( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      info.GetReturnValue().Set( ptr->y );
    }

    //! Real Vector3.y setter
    void Vector3::jsSetY( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      ptr->y = value->NumberValue();
    }

    //! Real Vector3.z getter
    void Vector3::jsGetZ( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      info.GetReturnValue().Set( ptr->z );
    }

    //! Real Vector3.z setter
    void Vector3::jsSetZ( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      auto ptr = unwrap<Vector3>( info.This() );
      ptr->z = value->NumberValue();
    }

    //! Real Vector3.length()
    void Vector3::jsLength( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      args.GetReturnValue().Set( ptr->length() );
    }

    //! Real Vector3.squaredLength()
    void Vector3::jsSquaredLength( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      args.GetReturnValue().Set( ptr->squaredLength() );
    }

    //! Real Vector3.distance( Vector3 other )
    void Vector3::jsDistance( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      auto that = extractVector3Argument( args );
      if ( !that )
        return;
      args.GetReturnValue().Set( ptr->distance( *that ) );
    }

    //! Real Vector3.squaredDistance( Vector3 other )
    void Vector3::jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      auto that = extractVector3Argument( args );
      if ( !that )
        return;
      args.GetReturnValue().Set( ptr->squaredDistance( *that ) );
    }

    //! Real Vector3.dotProduct( Vector3 )
    void Vector3::jsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      auto that = extractVector3Argument( args );
      if ( !that )
        return;
      args.GetReturnValue().Set( ptr->dotProduct( *that ) );
    }

    //! Real Vector3.absDotProduct( Vector3 other )
    void Vector3::jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      auto that = extractVector3Argument( args );
      if ( !that )
        return;
      args.GetReturnValue().Set( ptr->absDotProduct( *that ) );
    }

    //! Real Vector3.normalise()
    void Vector3::jsNormalise( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      args.GetReturnValue().Set( ptr->normalise() );
    }

    //! Vector3 Vector3.crossProduct( Vector3 other )
    void Vector3::jsCrossProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      auto that = extractVector3Argument( args );
      if ( !that )
        return;
      Ogre::Vector3 result = ptr->crossProduct( *that );
      Local<v8::Function> constFunc = constructor.Get( args.GetIsolate() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      auto ret = unwrap<Vector3>( object );
      ret->swap( result );
      args.GetReturnValue().Set( object );
    }
  }

}