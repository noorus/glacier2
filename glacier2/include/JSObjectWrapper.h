#pragma once
#include <v8.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Scripting
  //! @{

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

    enum WrappedType {
      Wrapped_Console = 0,
      Wrapped_Vector3,
      Wrapped_Quaternion
    };

    //! \class ObjectWrapper
    //! A wrapper for easy creation of JavaScript-exported objects.
    template <typename T>
    class ObjectWrapper {
    protected:
      //! My JavaScript-exported constructor function template.
      static Eternal<FunctionTemplate> constructor;
      //! My JavaScript-exported class name
      static string className;
    private:
      //! Internal v8 object handle
      Persistent<v8::Object> mJSHandle;
      //! Internal type for checking before casts
      WrappedType mWrappedType;
      //! Internal callback for when the object is made weak.
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
      ObjectWrapper() {}
    protected:
      Isolate* mJSIsolate; //!< Isolation
      int mJSReferences; //!< Reference counter
      //! Wraps the given handle.
      inline void wrap( Handle<v8::Object> handle )
      {
        assert( persistent().IsEmpty() );
        assert( handle->InternalFieldCount() > 0 );
        handle->SetAlignedPointerInInternalField( 0, this );
        persistent().Reset( Isolate::GetCurrent(), handle );
        makeWeak();
      }
      //! Makes myself weak.
      inline void makeWeak()
      {
        persistent().SetWeak( this, weakCallback );
        persistent().MarkIndependent();
      }
    public:
      //! Default constructor.
      explicit ObjectWrapper( const WrappedType type ):
      mJSReferences( 0 ), mWrappedType( type )
      {
        mJSIsolate = v8::Isolate::GetCurrent();
      }
      //! Destructor.
      virtual ~ObjectWrapper()
      {
        if ( !persistent().IsEmpty() )
        {
          // assert( persistent().IsNearDeath() );
          persistent().ClearWeak();
          persistent().Reset();
        }
      }
      //! Gets the handle.
      inline Local<v8::Object> handle()
      {
        return handle( Isolate::GetCurrent() );
      }
      //! Returns a local handle to self.
      inline Local<v8::Object> handle( v8::Isolate* isolate )
      {
        return Local<v8::Object>::New( isolate, persistent() );
      }
      //! Gets the persistent.
      inline Persistent<v8::Object>& persistent()
      {
        return mJSHandle;
      }
      //! Increases my references.
      virtual void ref()
      {
        assert( !persistent().IsEmpty() );
        persistent().ClearWeak();
        mJSReferences++;
      }
      //! Decreses my references.
      virtual void unref()
      {
        assert( !persistent().IsEmpty() );
        assert( !persistent().IsWeak() );
        assert( mJSReferences > 0 );
        if ( --mJSReferences == 0 )
          makeWeak();
      }
      //! Gets the wrapped type identifier.
      inline const WrappedType getWrappedType()
      {
        return mWrappedType;
      }
      //! Check whether a JavaScript object is of wanted wrapped type.
      static inline bool isWrappedType( Handle<v8::Object> handle, const WrappedType type )
      {
        if ( handle->InternalFieldCount() < 1 )
          return false;
        auto ptr = handle->GetAlignedPointerFromInternalField( 0 );
        auto wrapper = static_cast<ObjectWrapper*>( ptr );
        return ( wrapper->getWrappedType() == type );
      }
      //! Unwraps the given handle.
      static inline T* unwrap( Handle<v8::Object> handle )
      {
        assert( !handle.IsEmpty() );
        assert( handle->InternalFieldCount() > 0 );
        auto ptr = handle->GetAlignedPointerFromInternalField( 0 );
        auto wrapper = static_cast<ObjectWrapper*>( ptr );
        return static_cast<T*>( wrapper );
      }
    };

    template <typename T> Eternal<v8::FunctionTemplate> ObjectWrapper<T>::constructor;

  }

  //! @}

  //! @}

}