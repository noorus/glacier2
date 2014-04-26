#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// warning C4244: '=' : conversion from double to float, possible loss of data
#pragma warning( disable: 4244 )

namespace Glacier {

  namespace JS {

    string Vector3::className( "Vector3" );

    Vector3::Vector3( const Ogre::Vector3& source ):
    Ogre::Vector3( source ), ObjectWrapper( Wrapped_Vector3 )
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

      tpl->SetClassName( Util::allocString( className.c_str() ) );
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
      JS_TEMPLATE_SET( tpl, "randomDeviant", jsRandomDeviant );
      JS_TEMPLATE_SET( tpl, "angleBetween", jsAngleBetween );
      JS_TEMPLATE_SET( tpl, "getRotationTo", jsGetRotationTo );
      JS_TEMPLATE_SET( tpl, "isZeroLength", jsIsZeroLength );
      JS_TEMPLATE_SET( tpl, "normalisedCopy", jsNormalisedCopy );

      exports->Set( isolate, className.c_str(), tpl );
      constructor.Set( isolate, tpl );
    }

    //! New JS::Vector3 from Ogre::Vector3
    Local<v8::Object> Vector3::newFrom( const Ogre::Vector3& vector )
    {
      Local<v8::Function> constFunc = constructor.Get(
        Isolate::GetCurrent() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      Vector3* ret = unwrap( object );
      // Ugly this way, but the Ogre::Vector3 assignment operator silently fails.
      ret->x = vector.x;
      ret->y = vector.y;
      ret->z = vector.z;
      return object;
    }

    //! \verbatim
    //! Vector3()
    //! Vector3( Real scalar )
    //! Vector3( Real x, Real y, Real z )
    //! Vector3([Real x, Real y, Real z])
    //! Vector3({Real x, Real y, Real z})
    //! \endverbatim
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

    //! \verbatim
    //! Real Vector3.x
    //! \endverbatim
    void Vector3::jsGetX( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->x );
    }

    //! \verbatim
    //! Vector3.x
    //! \endverbatim
    void Vector3::jsSetX( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      ptr->x = value->NumberValue();
    }

    //! \verbatim
    //! Real Vector3.y
    //! \endverbatim
    void Vector3::jsGetY( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->y );
    }

    //! \verbatim
    //! Vector3.y
    //! \endverbatim
    void Vector3::jsSetY( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      ptr->y = value->NumberValue();
    }

    //! \verbatim
    //! Real Vector3.z
    //! \endverbatim
    void Vector3::jsGetZ( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->z );
    }

    //! \verbatim
    //! Vector3.z
    //! \endverbatim
    void Vector3::jsSetZ( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Vector3* ptr = unwrap( info.This() );
      ptr->z = value->NumberValue();
    }

    //! \verbatim
    //! String Vector3.toString
    //! \endverbatim
    void Vector3::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "Vector3[%f,%f,%f]", ptr->x, ptr->y, ptr->z );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

    //! \verbatim
    //! bool Vector3.equals( Vector3 )
    //! \endverbatim
    void Vector3::jsEquals( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( (Ogre::Vector3)*ptr == (Ogre::Vector3)*other );
    }

    //! \verbatim
    //! Vector3 Vector3.add( Vector3 )
    //! \endverbatim
    void Vector3::jsAdd( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr + (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Vector3 Vector3.subtract( Vector3 )
    //! \endverbatim
    void Vector3::jsSubtract( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr - (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Vector3 Vector3.multiply( Vector3 )
    //! \endverbatim
    void Vector3::jsMultiply( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Vector3)*ptr * (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Real Vector3.length()
    //! \endverbatim
    void Vector3::jsLength( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->length() );
    }

    //! \verbatim
    //! Real Vector3.squaredLength()
    //! \endverbatim
    void Vector3::jsSquaredLength( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->squaredLength() );
    }

    //! \verbatim
    //! Real Vector3.distance( Vector3 other )
    //! \endverbatim
    void Vector3::jsDistance( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->distance( *other ) );
    }

    //! \verbatim
    //! Real Vector3.squaredDistance( Vector3 other )
    //! \endverbatim
    void Vector3::jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->squaredDistance( *other ) );
    }

    //! \verbatim
    //! Real Vector3.dotProduct( Vector3 )
    //! \endverbatim
    void Vector3::jsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->dotProduct( *other ) );
    }

    //! \verbatim
    //! Real Vector3.absDotProduct( Vector3 other )
    //! \endverbatim
    void Vector3::jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->absDotProduct( *other ) );
    }

    //! \verbatim
    //! Real Vector3.normalise()
    //! \endverbatim
    void Vector3::jsNormalise( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->normalise() );
    }

    //! \verbatim
    //! Vector3 Vector3.crossProduct( Vector3 other )
    //! \endverbatim
    void Vector3::jsCrossProduct( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = ptr->crossProduct( *other );
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Vector3 Vector3.midPoint( Vector3 other )
    //! \endverbatim
    void Vector3::jsMidPoint( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = ptr->midPoint( *other );
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Vector3.makeFloor( Vector3 cmp )
    //! \endverbatim
    void Vector3::jsMakeFloor( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* cmp = Util::extractVector3( 0, args );
      if ( !cmp )
        return;
      ptr->makeFloor( *cmp );
    }

    //! \verbatim
    //! Vector3.makeCeil( Vector3 cmp )
    //! \endverbatim
    void Vector3::jsMakeCeil( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* cmp = Util::extractVector3( 0, args );
      if ( !cmp )
        return;
      ptr->makeCeil( *cmp );
    }

    //! \verbatim
    //! Vector3 Vector3.perpendicular()
    //! \endverbatim
    void Vector3::jsPerpendicular( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Ogre::Vector3 result = ptr->perpendicular();
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Vector3 Vector3.randomDeviant( Radian angle, Vector3 up )
    //! \endverbatim
    void Vector3::jsRandomDeviant( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      if ( args.Length() != 2 || !args[0]->IsNumber() )
      {
        args.GetIsolate()->ThrowException(
          v8::String::NewFromUtf8( args.GetIsolate(),
          "Expected arguments Radian angle, Vector3 up" ) );
        return;
      }
      Ogre::Radian angle( args[0]->NumberValue() );
      Vector3* up = Util::extractVector3( 1, args );
      if ( !up )
        return;
      Ogre::Vector3 result = ptr->randomDeviant( angle, *up );
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Radian Vector3.angleBetween( Vector3 )
    //! \endverbatim
    void Vector3::jsAngleBetween( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Radian result = ptr->angleBetween( *other );
      args.GetReturnValue().Set( result.valueRadians() );
    }

    //! \verbatim
    //! Quaternion Vector3.getRotationTo( Vector3 )
    //! \endverbatim
    void Vector3::jsGetRotationTo( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Quaternion result = ptr->getRotationTo( *other );
      args.GetReturnValue().Set( Quaternion::newFrom( result ) );
    }

    //! \verbatim
    //! bool Vector3.isZeroLength()
    //! \endverbatim
    void Vector3::jsIsZeroLength( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->isZeroLength() );
    }

    //! \verbatim
    //! Vector3 Vector3.normalisedCopy()
    //! \endverbatim
    void Vector3::jsNormalisedCopy( const FunctionCallbackInfo<v8::Value>& args )
    {
      Vector3* ptr = unwrap( args.Holder() );
      Ogre::Vector3 result = ptr->normalisedCopy();
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

  }

}