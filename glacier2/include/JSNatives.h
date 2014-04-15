#pragma once
#include <v8.h>
#include "JSObjectWrapper.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    using v8::Isolate;
    using v8::HandleScope;
    using v8::Local;
    using v8::Handle;
    using v8::Persistent;
    using v8::Eternal;
    using v8::PropertyCallbackInfo;
    using v8::ObjectTemplate;
    using v8::FunctionTemplate;
    using v8::FunctionCallbackInfo;
    using v8::WeakCallbackData;

    class Vector3: public Ogre::Vector3, public ObjectWrapper {
    private:
      static Eternal<FunctionTemplate> constructor;
    protected:
      explicit Vector3( const Ogre::Vector3& source );
      ~Vector3();
      static void create( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetX( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetX( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsGetY( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetY( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsGetZ( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetZ( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );
      static void jsEquals( const FunctionCallbackInfo<v8::Value>& args );
      static void jsAdd( const FunctionCallbackInfo<v8::Value>& args );
      static void jsSubtract( const FunctionCallbackInfo<v8::Value>& args );
      static void jsMultiply( const FunctionCallbackInfo<v8::Value>& args );
      static void jsLength( const FunctionCallbackInfo<v8::Value>& args );
      static void jsSquaredLength( const FunctionCallbackInfo<v8::Value>& args );
      static void jsDistance( const FunctionCallbackInfo<v8::Value>& args );
      static void jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args );
      static void jsDotProduct( const FunctionCallbackInfo<v8::Value>& args );
      static void jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args );
      static void jsNormalise( const FunctionCallbackInfo<v8::Value>& args );
      static void jsCrossProduct( const FunctionCallbackInfo<v8::Value>& args );
      static void jsMidPoint( const FunctionCallbackInfo<v8::Value>& args );
      static void jsMakeFloor( const FunctionCallbackInfo<v8::Value>& args );
      static void jsMakeCeil( const FunctionCallbackInfo<v8::Value>& args );
      static void jsPerpendicular( const FunctionCallbackInfo<v8::Value>& args );
      static void jsRandomDeviant( const FunctionCallbackInfo<v8::Value>& args );
      static void jsAngleBetween( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetRotationTo( const FunctionCallbackInfo<v8::Value>& args );
      static void jsIsZeroLength( const FunctionCallbackInfo<v8::Value>& args );
      static void jsNormalisedCopy( const FunctionCallbackInfo<v8::Value>& args );
    public:
      static void initialize( Handle<ObjectTemplate> exports );
      static Local<v8::Object> newFrom( const Ogre::Vector3& vector );
    };

    class Quaternion: public Ogre::Quaternion, public ObjectWrapper {
    private:
      static Eternal<FunctionTemplate> constructor;
    protected:
      explicit Quaternion( const Ogre::Quaternion& source );
      ~Quaternion();
      static void create( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetW( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetW( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsGetX( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetX( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsGetY( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetY( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsGetZ( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );
      static void jsSetZ( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );
      static void jsMultiply( const FunctionCallbackInfo<v8::Value>& args );
      static void jsDot( const FunctionCallbackInfo<v8::Value>& args );
      static void jsNorm( const FunctionCallbackInfo<v8::Value>& args );
      static void jsNormalise( const FunctionCallbackInfo<v8::Value>& args );
      static void jsInverse( const FunctionCallbackInfo<v8::Value>& args );
      static void jsUnitInverse( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetRoll( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetPitch( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetYaw( const FunctionCallbackInfo<v8::Value>& args );
    public:
      static void initialize( Handle<ObjectTemplate> exports );
      static Local<v8::Object> newFrom( const Ogre::Quaternion& qtn );
    };

  }

}