#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// warning C4244: '=' : conversion from double to float, possible loss of data
#pragma warning(disable: 4244)

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

      tpl->SetClassName( Util::allocString( cJSVector3Class ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_ACCESSOR( tpl, "x", jsGetX, jsSetX );
      JS_TEMPLATE_ACCESSOR( tpl, "y", jsGetY, jsSetY );
      JS_TEMPLATE_ACCESSOR( tpl, "z", jsGetZ, jsSetZ );

      JS_TEMPLATE_SET( tpl, "toString", jsToString );
      JS_TEMPLATE_SET( tpl, "equals", jsEquals );
      JS_TEMPLATE_SET( tpl, "add", jsAdd );
      JS_TEMPLATE_SET( tpl, "subtract", jsSubtract );
      JS_TEMPLATE_SET( tpl, "multiply", jsMultiply );
      JS_TEMPLATE_SET( tpl, "length", jsLength );
      JS_TEMPLATE_SET( tpl, "squaredLength", jsSquaredLength );
      JS_TEMPLATE_SET( tpl, "distance", jsDistance );
      JS_TEMPLATE_SET( tpl, "squaredDistance", jsSquaredDistance );
      JS_TEMPLATE_SET( tpl, "dotProduct", jsDotProduct );
      JS_TEMPLATE_SET( tpl, "absDotProduct", jsAbsDotProduct );
      JS_TEMPLATE_SET( tpl, "normalise", jsNormalise );
      JS_TEMPLATE_SET( tpl, "crossProduct", jsCrossProduct );
      JS_TEMPLATE_SET( tpl, "midPoint", jsMidPoint );
      JS_TEMPLATE_SET( tpl, "makeFloor", jsMakeFloor );
      JS_TEMPLATE_SET( tpl, "makeCeil", jsMakeCeil );
      JS_TEMPLATE_SET( tpl, "perpendicular", jsPerpendicular );

      exports->Set( isolate, cJSVector3Class, tpl );
      constructor.Set( isolate, tpl );
    }

    //! New JS::Vector3 from Ogre::Vector3
    Local<v8::Object> Vector3::newFrom( const Ogre::Vector3& vector )
    {
      Local<v8::Function> constFunc = constructor.Get(
        Isolate::GetCurrent() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      auto ret = unwrap<Vector3>( object );
      ret->x = vector.x;
      ret->y = vector.y;
      ret->z = vector.z;
      return object;
    }

    //! Vector3()
    //! Vector3( Real scalar )
    //! Vector3( Real x, Real y, Real z )
    //! Vector3([Real x, Real y, Real z])
    //! Vector3({Real x, Real y, Real z})
    void Vector3::create( const FunctionCallbackInfo<v8::Value>& args )
    {
      HandleScope handleScope( args.GetIsolate() );
      if ( !args.IsConstructCall() )
      {
        args.GetIsolate()->ThrowException(
          Util::allocString( "Function called as non-constructor" ) );
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
        else if ( args[0]->IsObject() )
        {
          v8::Local<v8::Value> val = args[0]->ToObject()->Get( Util::allocString( "x" ) );
          if ( val->IsNumber() )
            vec.x = val->NumberValue();
          val = args[0]->ToObject()->Get( Util::allocString( "y" ) );
          if ( val->IsNumber() )
            vec.y = val->NumberValue();
          val = args[0]->ToObject()->Get( Util::allocString( "z" ) );
          if ( val->IsNumber() )
            vec.z = val->NumberValue();
        }
      }
      Vector3* object = new Vector3( vec );
      object->wrap( args.This() );
      args.GetReturnValue().Set( args.This() );
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

    //! Vector3.toString
    void Vector3::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "[%f,%f,%f]", ptr->x, ptr->y, ptr->z );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

    //! bool Vector3.equals( Vector3 )
    void Vector3::jsEquals( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( (Ogre::Vector3)*ptr == (Ogre::Vector3)*other );
    }

    //! Vector3 Vector3.add( Vector3 )
    void Vector3::jsAdd( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr + (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! Vector3 Vector3.subtract( Vector3 )
    void Vector3::jsSubtract( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr - (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! Vector3 Vector3.multiply( Vector3 )
    void Vector3::jsMultiply( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr * (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
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
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->distance( *other ) );
    }

    //! Real Vector3.squaredDistance( Vector3 other )
    void Vector3::jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->squaredDistance( *other ) );
    }

    //! Real Vector3.dotProduct( Vector3 )
    void Vector3::jsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->dotProduct( *other ) );
    }

    //! Real Vector3.absDotProduct( Vector3 other )
    void Vector3::jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->absDotProduct( *other ) );
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
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = ptr->crossProduct( *other );
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! Vector3 Vector3.midPoint( Vector3 other )
    void Vector3::jsMidPoint( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = ptr->midPoint( *other );
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! Vector3.makeFloor( Vector3 cmp )
    void Vector3::jsMakeFloor( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* cmp = Util::extractVector3( 0, args );
      if ( !cmp )
        return;
      ptr->makeFloor( *cmp );
    }

    //! Vector3.makeCeil( Vector3 cmp )
    void Vector3::jsMakeCeil( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Vector3* cmp = Util::extractVector3( 0, args );
      if ( !cmp )
        return;
      ptr->makeCeil( *cmp );
    }

    //! Vector3 Vector3.perpendicular()
    void Vector3::jsPerpendicular( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap<Vector3>( args.Holder() );
      Ogre::Vector3 result = ptr->perpendicular();
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

  }

}