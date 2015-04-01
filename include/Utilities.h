#pragma once
#include <windows.h>
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \class Singleton
  //! Simple template for a Meyers singleton.
  template <class T>
  class Singleton: boost::noncopyable {
  public:
    static T& instance()
    {
      static T _instance;
      return _instance;
    }
  };

  namespace Platform {

    //! \class RWLock
    //! Reader-writer lock class for easy portability.
    class RWLock: boost::noncopyable {
    protected:
      SRWLOCK mLock;
    public:
      RWLock() { InitializeSRWLock( &mLock ); }
      void lock() { AcquireSRWLockExclusive( &mLock ); }
      void unlock() { ReleaseSRWLockExclusive( &mLock ); }
      void lockShared() { AcquireSRWLockShared( &mLock ); }
      void unlockShared() { ReleaseSRWLockShared( &mLock ); }
    };

    //! \class Event
    //! Waitable, thread-safe event signaling class.
    class Event: boost::noncopyable {
    protected:
      HANDLE mEvent;
    public:
      Event()
      {
        mEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
      }
      void set() { SetEvent( mEvent ); }
      void reset() { ResetEvent( mEvent ); }
      bool waitFor( size_t milliseconds = 0 )
      {
        DWORD wait = ( milliseconds == 0 ? INFINITE : (DWORD)milliseconds );
        auto result = WaitForSingleObject( mEvent, wait );
        if ( result == WAIT_OBJECT_0 )
          return true;
        else if ( result == WAIT_TIMEOUT )
          return false;
        else
          ENGINE_EXCEPT_WINAPI( "Wait for event failed" );
      }
    };

  }

  //! \class ScopedRWLock
  //! Automation for scoped acquisition and release of an RWLock.
  class ScopedRWLock: boost::noncopyable {
  protected:
    Platform::RWLock* mLock;    //!< The lock
    bool mExclusive;  //!< Whether we're acquired in exclusive mode
    bool mLocked;     //!< Whether we're still locked
  public:
    //! Constructor.
    //! \param  lock      The lock to acquire.
    //! \param  exclusive (Optional) true to acquire in exclusive mode, false for shared.
    ScopedRWLock( Platform::RWLock* lock, bool exclusive = true ):
    mLock( lock ), mExclusive( exclusive ), mLocked( true )
    {
      mExclusive ? mLock->lock() : mLock->lockShared();
    }
    //! Call directly if you want to unlock before object leaves scope.
    void unlock()
    {
      if ( mLocked )
        mExclusive ? mLock->unlock() : mLock->unlockShared();
      mLocked = false;
    }
    //! Destructor.
    ~ScopedRWLock()
    {
      unlock();
    }
  };

  //! \class SafeWaitableQueue
  //! A thread-safe queue object with a waitable semaphore handle.
  //! Thanks to Jim Beveridge's example for the semaphore approach.
  template <typename T>
  class SafeWaitableQueue: boost::noncopyable {
  protected:
    SRWLOCK mLock;        //!< The lock
    HANDLE mSemaphore;    //!< Handle of the semaphore
    std::list<T> mQueue;  //!< The queue
    bool mOverflow;       //!< Whether we're overflowing
  public:
    //! Constructor.
    //! \param  maxCount Maximum number of waitables.
    SafeWaitableQueue( long maxCount ): mSemaphore( NULL ),
    mOverflow( false )
    {
      InitializeSRWLock( &mLock );
      mSemaphore = CreateSemaphoreW( NULL, 0, maxCount, NULL );
    }
    //! Destructor.
    ~SafeWaitableQueue()
    {
      if ( mSemaphore )
        CloseHandle( mSemaphore );
    }
    //! Gets the handle.
    //! \return The handle.
    HANDLE getHandle()
    {
      return mSemaphore;
    }
    //! Query if this object is overflowed.
    //! \return true if overflowed, false if not.
    bool isOverflowed()
    {
      return mOverflow;
    }
    //! Pushes an object onto this stack.
    //! \param element The element to push.
    //! \return true if it succeeds, false if it fails.
    bool push( T& element )
    {
      ScopedRWLock lock( &mLock );
      mQueue.push_back( element );
      if ( !ReleaseSemaphore( mSemaphore, 1, NULL ) )
      {
        mQueue.pop_back();
        if ( GetLastError() == ERROR_TOO_MANY_POSTS )
          mOverflow = true;
        return false;
      }
      return true;
    }
    //! Removes and returns the top-of-stack object.
    //! \param element The element to pop.
    //! \return true if it succeeds, false if it fails.
    bool pop( T& element )
    {
      ScopedRWLock lock( &mLock );
      if ( mQueue.empty() )
      {
        while ( WaitForSingleObject( mSemaphore, 0 ) != WAIT_TIMEOUT )
          1;
        return false;
      }
      element = mQueue.front();
      mQueue.pop_front();
      return true;
    }
    //! Clears this object to its blank/initial state.
    void clear()
    {
      ScopedRWLock lock( &mLock );
      for ( size_t i = 0; i < mQueue.size(); i++ )
        WaitForSingleObject( mSemaphore, 0 );
      mQueue.clear();
      mOverflow = false;
    }
  };

  namespace Utilities
  {
    //! Signal the debugger to give a thread a name
    inline void debugSetThreadName( DWORD threadID, const std::string& threadName )
    {
#ifdef _DEBUG
#pragma pack( push, 8 )
      struct threadNamingStruct
      {
        DWORD type;
        LPCSTR name;
        DWORD threadID;
        DWORD flags;
      } nameSignalStruct;
#pragma pack( pop )
      nameSignalStruct.type = 0x1000;
      nameSignalStruct.name = threadName.c_str();
      nameSignalStruct.threadID = threadID;
      nameSignalStruct.flags = 0;
      __try
      {
        RaiseException( 0x406D1388, 0,
          sizeof( nameSignalStruct ) / sizeof( ULONG_PTR ),
          (const ULONG_PTR*)&nameSignalStruct );
      }
      __except( EXCEPTION_EXECUTE_HANDLER ) {}
#endif
    }

    //! UTF-8 to wide string conversion.
    inline wstring utf8ToWide( const string& in ) throw()
    {
      int length = MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, nullptr, 0 );
      if ( length == 0 )
        return wstring();
      vector<wchar_t> conversion( length );
      MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, &conversion[0], length );
      return wstring( &conversion[0] );
    }

    //! Wide string to UTF-8 conversion.
    inline string wideToUtf8( const wstring& in ) throw()
    {
      int length = WideCharToMultiByte( CP_UTF8, 0, in.c_str(), -1, nullptr, 0, 0, FALSE );
      if ( length == 0 )
        return string();
      vector<char> conversion( length );
      WideCharToMultiByte( CP_UTF8, 0, in.c_str(), -1, &conversion[0], length, 0, FALSE );
      return string( &conversion[0] );
    }

    //! Calculate 64-bit Hamming weight.
    inline int hammingWeight64( uint64_t x ) throw()
    {
      x = ( x & 0x5555555555555555ULL ) + ( ( x >> 1 ) & 0x5555555555555555ULL );
      x = ( x & 0x3333333333333333ULL ) + ( ( x >> 2 ) & 0x3333333333333333ULL );
      x = ( x + ( x >> 4 ) ) & 0x0F0F0F0F0F0F0F0FULL;
      x = ( x + ( x >> 8 ) );
      x = ( x + ( x >> 16 ) );
      x = ( x + ( x >> 32 ) );
      return x & 0xFF;
    }

    //! Gets window text.
    inline wstring getWindowText( HWND wnd ) throw()
    {
      wstring str;
      int length = GetWindowTextLengthW( wnd );
      if ( !length )
        return str;
      str.resize( length + 1, '\0' );
      GetWindowTextW( wnd, &str[0], length + 1 );
      str.resize( length );
      return str;
    }

  }

  //! @}

}