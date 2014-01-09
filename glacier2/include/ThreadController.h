#pragma once
#include "Exception.h"

namespace Glacier {

  class ThreadController: boost::noncopyable {
  protected:
    HANDLE mThread;
    DWORD mThreadID;
    volatile HANDLE mRunEvent;
    volatile HANDLE mStopEvent;
    virtual void onStart() = 0;
    virtual void onStep() = 0;
    virtual void onPreStop() = 0;
    virtual void onStop() = 0;
    static DWORD WINAPI threadProc( void* argument );
  public:
    ThreadController();
    virtual void start();
    virtual void stop();
    virtual ~ThreadController();
  };

}