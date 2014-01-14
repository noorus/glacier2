#include "StdAfx.h"
#include "ThreadController.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ThreadController::ThreadController():
  mThread( NULL ), mThreadID( 0 ), mRunEvent( NULL ), mStopEvent( NULL )
  {
    mRunEvent  = CreateEventW( NULL, TRUE, FALSE, NULL );
    mStopEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
    if ( !mRunEvent || !mStopEvent )
      ENGINE_EXCEPT_W32( L"Could not create control events" );
  }

  void ThreadController::start()
  {
    stop();

    mThread = CreateThread( NULL, NULL, threadProc, this, CREATE_SUSPENDED, &mThreadID );
    if ( !mThread )
      ENGINE_EXCEPT_W32( L"Could not create thread" );

    if ( ResumeThread( mThread ) == (DWORD)-1 )
      ENGINE_EXCEPT_W32( L"Could not resume thread" );

    HANDLE events[2];
    events[0] = mRunEvent;
    events[1] = mThread;

    DWORD wait = WaitForMultipleObjects( 2, events, FALSE, INFINITE );

    switch ( wait )
    {
      case WAIT_OBJECT_0 + 0:
        // Thread running OK
      break;
      case WAIT_OBJECT_0 + 1:
        ENGINE_EXCEPT( L"Thread failed to start" );
      break;
      case WAIT_FAILED:
        ENGINE_EXCEPT_W32( L"Wait for thread start failed" );
      break;
    }
  }

  DWORD WINAPI ThreadController::threadProc( void* argument )
  {
    auto controller = (ThreadController*)argument;
    try
    {
      controller->onStart();
      SetEvent( controller->mRunEvent );
      while ( WaitForSingleObject( controller->mStopEvent, 0 ) != WAIT_OBJECT_0 )
      {
        controller->onStep();
      }
      controller->onStop();
    }
    catch ( ... )
    {
      controller->onStop();
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  void ThreadController::stop()
  {
    if ( mStopEvent )
    {
      onPreStop();
      SetEvent( mStopEvent );
      WaitForSingleObject( mThread, INFINITE );
    }
    ResetEvent( mRunEvent );
    ResetEvent( mStopEvent );
  }

  ThreadController::~ThreadController()
  {
    stop();
    if ( mRunEvent )
      CloseHandle( mRunEvent );
    if ( mStopEvent )
      CloseHandle( mStopEvent );
  }

}