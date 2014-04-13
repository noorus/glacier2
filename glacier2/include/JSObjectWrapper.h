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

    class ObjectWrapper {
    private:
      Persistent<v8::Object> mJSHandle; //!< Internal v8 object handle
      static void weakCallback( const WeakCallbackData<v8::Object, ObjectWrapper>& data )
      {
        v8::Isolate* isolate = data.GetIsolate();
        v8::HandleScope scope( isolate );
        ObjectWrapper* wrapper = data.GetParameter();
        assert( wrapper->mJSReferences == 0 );
        assert( wrapper->mJSHandle.IsNearDeath() );
        assert( data.GetValue() == v8::Local<v8::Object>::New( isolate, wrapper->mJSHandle ) );
        wrapper->mJSHandle.Reset();
        delete wrapper;
      }
    protected:
      Isolate* mJSIsolate; //!< Isolation
      int mJSReferences; //!< Reference counter
      inline void wrap( Handle<v8::Object> handle )
      {
        assert( persistent().IsEmpty() );
        assert( handle->InternalFieldCount() > 0 );
        handle->SetAlignedPointerInInternalField( 0, this );
        persistent().Reset( Isolate::GetCurrent(), handle );
        makeWeak();
      }
      inline void makeWeak()
      {
        persistent().SetWeak( this, weakCallback );
        persistent().MarkIndependent();
      }
      virtual void ref()
      {
        assert( !persistent().IsEmpty() );
        persistent().ClearWeak();
        mJSReferences++;
      }
      virtual void unref()
      {
        assert( !persistent().IsEmpty() );
        assert( !persistent().IsWeak() );
        assert( mJSReferences > 0 );
        if ( --mJSReferences == 0 )
          makeWeak();
      }
    public:
      ObjectWrapper(): mJSReferences( 0 )
      {
        mJSIsolate = v8::Isolate::GetCurrent();
      }
      virtual ~ObjectWrapper()
      {
        if ( !persistent().IsEmpty() )
        {
          assert( persistent().IsNearDeath() );
          persistent().ClearWeak();
          persistent().Reset();
        }
      }
      inline Local<v8::Object> handle()
      {
        return handle( Isolate::GetCurrent() );
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

  }

}