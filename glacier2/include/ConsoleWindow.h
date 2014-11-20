#pragma once
#include "Win32.h"
#include "Console.h"
#include "ThreadController.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Console
  //! @{

  //! External console window.
  //! \sa ConsoleListener
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
    //! Clears the command line.
    void clearCmdline();
    //! Sets the command line.
    void setCmdline( const wstring& line );
    //! Paints the console window.
    void paint( HWND window, HDC dc );
    //! Command line procedure.
    static LRESULT CALLBACK cmdLineProc( HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam );
    //! Window procedure.
    static LRESULT CALLBACK wndProc( HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam );
    //! Executes the add line action.
    void onAddLine( COLORREF color, const wstring& line );
  public:
    //! Constructor.
    ConsoleWindow( HINSTANCE instance, Console* console );
    //! Destructor.
    ~ConsoleWindow();
    //! Kills the console window.
    void kill();
    //! Prints a line.
    void print( COLORREF color, const wstring& line );
    //! Steps this ConsoleWindow forward.
    //! \return true if it succeeds, false if it fails.
    bool step();
  };

  //! \class ConsoleWindowThread
  //! A console window thread.
  class ConsoleWindowThread {
  protected:
    HANDLE mThread;             //!< Handle of the thread
    DWORD mThreadID;            //!< Identifier for the thread
    HINSTANCE mInstance;        //!< The owning instance handle
    Console* mConsole;          //!< The console
    ConsoleWindow* mWindow;     //!< The console window
    volatile HANDLE mRunEvent;  //!< My run event
    volatile HANDLE mStopEvent; //!< My stop event
    static DWORD WINAPI threadProc( void* argument );
  public:
    //! Constructor.
    ConsoleWindowThread( HINSTANCE instance, Console* console );
    //! Starts this ConsoleWindowThread.
    virtual void start();
    //! Stops this ConsoleWindowThread.
    virtual void stop();
    //! Destructor.
    ~ConsoleWindowThread();
  };

  //! @}

  //! @}

}