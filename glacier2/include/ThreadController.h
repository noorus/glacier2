#pragma once

namespace Glacier {

  class ThreadController: public boost::noncopyable {
  protected:
    HANDLE mThread;
    DWORD mThreadID;
    volatile HANDLE mRunEvent;
    volatile HANDLE mStopEvent;
    static DWORD WINAPI threadProc( void* argument );
  public:
    ThreadController();
    virtual void start() = 0;
    virtual void step() = 0;
    virtual void stop() = 0;
    virtual ~ThreadController();
  };

}