#pragma once

namespace glacier {

  //! \class Singleton
  //! Simple template for a Meyers singleton.
  template <class T>
  class Singleton: boost::noncopyable {
  public:
    static inline T& instance()
    {
      static T _instance;
      return _instance;
    }
  };

  //! \class ScopedSRWLock
  //! Automation for scoped acquisition and release of an SRWLOCK.
  //! \warning Lock must be initialized in advance!
  class ScopedSRWLock: boost::noncopyable {
  protected:
    PSRWLOCK mLock;
  public:
    ScopedSRWLock( PSRWLOCK lock ): mLock( lock )
    {
      AcquireSRWLockExclusive( mLock );
    }
    void unlock()
    {
      ReleaseSRWLockExclusive( mLock );
    }
    ~ScopedSRWLock()
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
    SRWLOCK mLock;
    HANDLE mSemaphore;
    std::list<T> mQueue;
    bool mOverflow;
  public:
    SafeWaitableQueue( long maxCount ): mSemaphore( NULL ),
    mOverflow( false )
    {
      InitializeSRWLock( &mLock );
      mSemaphore = CreateSemaphoreW( NULL, 0, maxCount, NULL );
    }
    ~SafeWaitableQueue()
    {
      if ( mSemaphore )
        CloseHandle( mSemaphore );
    }
    HANDLE getHandle()
    {
      return mSemaphore;
    }
    bool isOverflowed()
    {
      return mOverflow;
    }
    bool push( T& element )
    {
      ScopedSRWLock lock( &mLock );
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
    bool pop( T& element )
    {
      ScopedSRWLock lock( &mLock );
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
    void clear()
    {
      ScopedSRWLock lock( &mLock );
      for ( size_t i = 0; i < mQueue.size(); i++ )
        WaitForSingleObject( mSemaphore, 0 );
      mQueue.clear();
      mOverflow = false;
    }
  };

  namespace Utilities
  {
    inline static wstring utf8ToWide( const string& in ) throw()
    {
      int length = MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, nullptr, 0 );
      if ( length == 0 )
        return wstring();
      vector<wchar_t> conversion( length );
      MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, &conversion[0], length );
      return wstring( &conversion[0] );
    }

    inline static string wideToUtf8( const wstring& in ) throw()
    {
      int length = WideCharToMultiByte( CP_UTF8, 0, in.c_str(), -1, nullptr, 0, 0, FALSE );
      if ( length == 0 )
        return string();
      vector<char> conversion( length );
      WideCharToMultiByte( CP_UTF8, 0, in.c_str(), -1, &conversion[0], length, 0, FALSE );
      return string( &conversion[0] );
    }
  }

}