#pragma once
#include <v8.h>

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

    static Local<v8::String> newStr( const wstring& str )
    {
      return v8::String::NewFromTwoByte( Isolate::GetCurrent(), (uint16_t*)str.c_str() );
    }

    static Local<v8::String> newStr( const Ogre::String& str )
    {
      return v8::String::NewFromUtf8( Isolate::GetCurrent(), str.c_str() );
    }

    static Local<v8::String> newStr( const char* str )
    {
      return v8::String::NewFromUtf8( Isolate::GetCurrent(), str );
    }

    class ObjectWrapper {
    private:
      Persistent<v8::Object> mJSHandle; //!< Internal v8 object handle
      static void weakCallback( const WeakCallbackData<v8::Object, ObjectWrapper>& data );
    protected:
      Isolate* mJSIsolate; //!< Isolation
      int mJSReferences; //!< Reference counter
      inline void wrap( Handle<v8::Object> handle )
      {
        assert( persistent().IsEmpty() );
        assert( handle->InternalFieldCount() > 0 );
        handle->SetAlignedPointerInInternalField( 0, this );
        persistent().Reset( v8::Isolate::GetCurrent(), handle );
        makeWeak();
      }
      inline void makeWeak()
      {
        persistent().SetWeak( this, weakCallback );
        persistent().MarkIndependent();
      }
      virtual void ref();
      virtual void unref();
    public:
      ObjectWrapper();
      virtual ~ObjectWrapper();
      inline Local<v8::Object> handle()
      {
        return handle( v8::Isolate::GetCurrent() );
      }
      inline Local<v8::Object> handle( v8::Isolate* isolate )
      {
        return Local<v8::Object>::New( isolate, persistent() );
      }
      inline Persistent<v8::Object>& persistent()
      {
        return mJSHandle;
      }
      template <class T>
      static inline T* unwrap( Handle<v8::Object> handle )
      {
        assert( !handle.IsEmpty() );
        assert( handle->InternalFieldCount() > 0 );
        auto ptr = handle->GetAlignedPointerFromInternalField( 0 );
        auto wrapper = static_cast<ObjectWrapper*>( ptr );
        return static_cast<T*>( wrapper );
      }
    };

    class Vector3: public Ogre::Vector3, public ObjectWrapper {
    private:
      static Eternal<FunctionTemplate> constructor;
    protected:
      explicit Vector3( const Ogre::Vector3& source );
      ~Vector3();
      static void create( const FunctionCallbackInfo<v8::Value>& args );
      static Vector3* extractVector3Argument(
        const FunctionCallbackInfo<v8::Value>& args );
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
      static void jsLength( const FunctionCallbackInfo<v8::Value>& args );
      static void jsSquaredLength( const FunctionCallbackInfo<v8::Value>& args );
      static void jsDistance( const FunctionCallbackInfo<v8::Value>& args );
      static void jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args );
      static void jsDotProduct( const FunctionCallbackInfo<v8::Value>& args );
      static void jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args );
      static void jsNormalise( const FunctionCallbackInfo<v8::Value>& args );
      static void jsCrossProduct( const FunctionCallbackInfo<v8::Value>& args );
    public:
      static void initialize( Handle<ObjectTemplate> exports );
    };

  }

}