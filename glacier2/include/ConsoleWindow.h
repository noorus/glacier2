#pragma once
#include "Win32.h"
#include "Console.h"
#include "ThreadController.h"

namespace Glacier {

  class ConsoleWindow: protected ConsoleListener {
  protected:
    Win32::Window* mWindow;   //!< My window
    Console* mConsole;        //!< Console I'm hooked up to
    HWND mLog;                //!< The log box
    HWND mCmdline;            //!< The command line
    WNDPROC mCmdlineProc;     //!< Original wndproc for the command line
    struct History {
      StringVector stack;     //!< Command history stack
      bool browsing;          //!< Is the user browsing through the history?
      size_t position;        //!< Position of currently located command
      History() { reset(); }
      void reset();
    } mHistory;
    struct Autocomplete {
      ConBaseList matches;    //!< Autocomplete matches vector
      ConBase* suggestion;    //!< Autocomplete last suggestion
      wstring base;           //!< Search string for autocomplete
      Autocomplete() { reset(); }
      void reset();
    } mAutocomplete;
  protected:
    void clearCmdline();
    void setCmdline( const wstring& line );
    void paint( HWND window, HDC dc );
    static LRESULT CALLBACK cmdLineProc( HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK wndProc( HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam );
    void onAddLine( COLORREF color, const wstring& line );
  public:
    ConsoleWindow( HINSTANCE instance, Console* console );
    ~ConsoleWindow();
    void kill();
    void print( COLORREF color, const wstring& line );
    void step();
  };

  class ConsoleWindowThread {
  protected:
    HANDLE mThread;
    DWORD mThreadID;
    HINSTANCE mInstance;
    Console* mConsole;
    ConsoleWindow* mWindow;
    volatile HANDLE mRunEvent;
    volatile HANDLE mStopEvent;
    static DWORD WINAPI threadProc( void* argument );
  public:
    ConsoleWindowThread( HINSTANCE instance, Console* console );
    virtual void start();
    virtual void stop();
    ~ConsoleWindowThread();
  };

}