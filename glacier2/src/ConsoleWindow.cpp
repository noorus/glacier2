#include "StdAfx.h"
#include "ConsoleWindow.h"
#include "Win32.h"
#include "Exception.h"
#include "Engine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  // ConsoleWindow constants ==================================================

  const wchar_t* cConsoleWindowClass  = L"gcr2_console";
  const wchar_t* cConsoleWindowTitle  = L"glacier² » console";
  const COLORREF cConsoleWindowBackground = RGB( 255, 255, 255 );
  const COLORREF cConsoleWindowForeground = RGB( 10, 13, 20 );
  const wchar_t* cConsoleWindowFont   = L"Trebuchet MS";
  const char*    cConsoleThreadName   = "Gcr2 Console Thread";

  // ConsoleWindowThread class ================================================

  ConsoleWindowThread::ConsoleWindowThread( HINSTANCE instance,
  Console* console ): mWindow( nullptr ), mInstance( instance ),
  mConsole( console ), mThread( NULL ), mThreadID( 0 ), mRunEvent( NULL ),
  mStopEvent( NULL )
  {
    mRunEvent  = CreateEventW( NULL, TRUE, FALSE, NULL );
    mStopEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
    if ( !mRunEvent || !mStopEvent )
      ENGINE_EXCEPT_W32( L"Could not create control events" );
  }

  void ConsoleWindowThread::start()
  {
    stop();

    mThread = CreateThread( NULL, NULL, threadProc, this,
      CREATE_SUSPENDED, &mThreadID );

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
      Utilities::debugSetThreadName( mThreadID, cConsoleThreadName );
      break;
    case WAIT_OBJECT_0 + 1:
      ENGINE_EXCEPT( L"Console window thread failed to start" );
      break;
    case WAIT_FAILED:
      ENGINE_EXCEPT_W32( L"Wait for console window thread start failed" );
      break;
    }
  }

  DWORD WINAPI ConsoleWindowThread::threadProc( void* argument )
  {
    auto me = (ConsoleWindowThread*)argument;
    try
    {
      me->mWindow = new ConsoleWindow( me->mInstance, me->mConsole );
      SetEvent( me->mRunEvent );
      while ( WaitForSingleObject( me->mStopEvent, 0 ) != WAIT_OBJECT_0 )
      {
        if ( !me->mWindow->step() )
          break;
      }
      SAFE_DELETE( me->mWindow );
    }
    catch ( ... )
    {
      SAFE_DELETE( me->mWindow );
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  void ConsoleWindowThread::stop()
  {
    if ( mStopEvent )
    {
      SetEvent( mStopEvent );
      if ( mWindow )
        mWindow->kill();
      WaitForSingleObject( mThread, INFINITE );
    }
    ResetEvent( mRunEvent );
    ResetEvent( mStopEvent );
  }

  ConsoleWindowThread::~ConsoleWindowThread()
  {
    stop();
    if ( mRunEvent )
      CloseHandle( mRunEvent );
    if ( mStopEvent )
      CloseHandle( mStopEvent );
  }

  // ConsoleWindow class ======================================================

  void ConsoleWindow::Autocomplete::reset()
  {
    matches.clear();
    base.clear();
    suggestion = nullptr;
  }

  void ConsoleWindow::History::reset()
  {
    stack.clear();
    browsing = false;
    position = 0;
  }

  ConsoleWindow::ConsoleWindow( HINSTANCE instance, Console* console ):
  mConsole( console ), mWindow( nullptr )
  {
    mConsole->addListener( this );
    mWindow = new Win32::Window( instance, wndProc,
      Win32::Window::Settings::toolWindow( 420, 280,
      cConsoleWindowClass, cConsoleWindowTitle ),
      this );
    POINT pos = { 0, 0 };
    POINT size = { 420, 280 };
    mWindow->create( pos, size, cConsoleWindowTitle );
    mWindow->setVisible( true );
  }

  ConsoleWindow::~ConsoleWindow()
  {
    mConsole->removeListener( this );
    SAFE_DELETE( mWindow );
  }

  void ConsoleWindow::kill()
  {
    if ( mWindow )
      SendMessageW( mWindow->getHandle(), WM_CLOSE, NULL, NULL );
  }

  void ConsoleWindow::onAddLine( COLORREF color, const wstring& line )
  {
    print( color, line );
  }

  void ConsoleWindow::print( COLORREF color, const wstring& line )
  {
    CHARRANGE range = { -1, -1 };
    SendMessage( mLog, EM_EXSETSEL, 0, (LPARAM)&range );
    CHARFORMAT2W format;
    format.cbSize = sizeof( CHARFORMAT2W );
    format.dwEffects = NULL;
    format.dwMask = CFM_COLOR | CFM_EFFECTS;
    format.crTextColor = color;
    SendMessage( mLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format );
    SETTEXTEX textex = { ST_SELECTION, 1200 };
    SendMessage( mLog, EM_SETTEXTEX, (WPARAM)&textex, (LPARAM)line.c_str() );
  }

  bool ConsoleWindow::step()
  {
    MSG msg;
    BOOL ret = GetMessageW( &msg, NULL, 0, 0 );
    if ( ret == -1 )
      ENGINE_EXCEPT_W32( L"GetMessageW returned -1" )
    else if ( ret == 0 )
      return false;
    else {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
      return true;
    }
  }

  void ConsoleWindow::clearCmdline()
  {
    SETTEXTEX textex = { ST_DEFAULT, 1200 };
    SendMessageW( mCmdline, EM_SETEVENTMASK, NULL, ENM_NONE );
    SendMessageW( mCmdline, EM_SETTEXTEX, (WPARAM)&textex, (LPARAM)NULL );
    SendMessageW( mCmdline, EM_SETEVENTMASK, NULL, ENM_CHANGE );
  }

  void ConsoleWindow::setCmdline( const wstring& line )
  {
    SETTEXTEX textex = { ST_DEFAULT, 1200 };
    SendMessageW( mCmdline, EM_SETEVENTMASK, NULL, ENM_NONE );
    SendMessageW( mCmdline, EM_SETTEXTEX, (WPARAM)&textex, (LPARAM)line.c_str() );
    SendMessageW( mCmdline, EM_SETEVENTMASK, NULL, ENM_CHANGE );
  }

  LRESULT CALLBACK ConsoleWindow::cmdLineProc( HWND wnd, UINT msg,
  WPARAM wParam, LPARAM lParam )
  {
    auto window = (ConsoleWindow*)GetWindowLongPtrW( wnd, GWLP_USERDATA );
    auto console = window->mConsole;
    wstring strtemp;
    wstring backline;
    CHARRANGE range = { 0, -1 };
    if ( msg == WM_CHAR && wParam == TAB )
      return 0;
    else if ( msg == WM_KEYDOWN )
    {
      switch ( wParam )
      {
        case VK_TAB:
          strtemp = Utilities::getWindowText( window->mCmdline );
          if ( strtemp.length() < 1 )
          {
            window->mAutocomplete.suggestion = nullptr;
            break;
          }
          if ( window->mAutocomplete.suggestion )
            console->autoComplete( window->mAutocomplete.base, window->mAutocomplete.matches );
          else
          {
            window->mAutocomplete.base = strtemp;
            console->autoComplete( strtemp, window->mAutocomplete.matches );
          }
          if ( window->mAutocomplete.matches.empty() )
          {
            window->mAutocomplete.suggestion = nullptr;
            break;
          }
          if ( window->mAutocomplete.suggestion )
          {
            ConBaseList::iterator itCmd = window->mAutocomplete.matches.begin();
            while ( itCmd != window->mAutocomplete.matches.end() )
            {
              if ( (*itCmd) == window->mAutocomplete.suggestion )
              {
                ++itCmd;
                if ( itCmd == window->mAutocomplete.matches.end() )
                  break;
                window->mAutocomplete.suggestion = (*itCmd);
                window->setCmdline( window->mAutocomplete.suggestion->getName() );
                SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
                return 0;
              }
              ++itCmd;
            }
          }
          window->mAutocomplete.suggestion = window->mAutocomplete.matches.front();
          window->setCmdline( window->mAutocomplete.suggestion->getName() );
          SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          return 0;
        break;
        case VK_UP:
          if ( window->mHistory.stack.empty() )
            break;
          if ( window->mHistory.browsing )
          {
            if ( window->mHistory.position <= 0 )
            {
              window->mHistory.position = 0;
              SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
              break;
            }
            window->mHistory.position--;
            wstring sBackLine = window->mHistory.stack[window->mHistory.position];
            window->setCmdline( sBackLine.c_str() );
            SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          }
          else
          {
            window->mHistory.browsing = true;
            window->mHistory.position = window->mHistory.stack.size() - 1;
            backline = window->mHistory.stack.back();
            strtemp = Utilities::getWindowText( window->mCmdline );
            window->mHistory.stack.push_back( strtemp );
            window->setCmdline( backline );
            SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          }
          return 0;
        break;
        case VK_DOWN:
          if ( window->mHistory.stack.empty() || !window->mHistory.browsing )
            break;
          window->mHistory.position++;
          if ( window->mHistory.position >= window->mHistory.stack.size() )
          {
            window->mHistory.position = window->mHistory.stack.size() - 1;
            SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
            window->mHistory.browsing = false;
            break;
          }
          backline = window->mHistory.stack[window->mHistory.position];
          window->setCmdline( backline );
          SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          if ( window->mHistory.position >= window->mHistory.stack.size() - 1 )
          {
            window->mHistory.stack.pop_back();
            window->mHistory.position = window->mHistory.stack.size() - 1;
            window->mHistory.browsing = false;
          }
          return 0;
        break;
        case VK_RETURN:
          if ( window->mHistory.browsing )
          {
            window->mHistory.stack.pop_back();
            window->mHistory.position = window->mHistory.stack.size() - 1;
            window->mHistory.browsing = false;
          }
          window->mAutocomplete.reset();
          strtemp = Utilities::getWindowText( window->mCmdline );
          if ( !strtemp.empty() )
          {
            if ( console )
              console->executeBuffered( strtemp );
            window->mHistory.stack.push_back( strtemp );
            window->mHistory.position = window->mHistory.stack.size() - 1;
            window->clearCmdline();
            range.cpMin = -1;
            SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          }
          return 0;
        break;
      }
    }
    return CallWindowProcW( window->mCmdlineProc, wnd, msg, wParam, lParam );
  }

  void ConsoleWindow::paint( HWND wnd, HDC dc )
  {
    RECT area;
    GetClientRect( wnd, &area );

    Win32::Gdiplus::Graphics gfx( dc );

    Win32::Win32::instance().drawConsole(
      gfx, area,
      gEngine->getVersion().title,
      gEngine->getVersion().subtitle );
  }

  LRESULT CALLBACK ConsoleWindow::wndProc( HWND wnd, UINT msg,
  WPARAM wParam, LPARAM lParam )
  {
    auto window = (ConsoleWindow*)GetWindowLongPtrW( wnd, GWLP_USERDATA );
    LPCREATESTRUCTW cs;
    PAINTSTRUCT paintdata;
    CHARRANGE range = { -1, -1 };
    SETTEXTEX textex = { ST_DEFAULT, 1200 };
    CHARFORMAT2W format;
    RECT rect;
    HDC dc;
    switch ( msg )
    {
      case WM_COMMAND:
        switch ( HIWORD( wParam ) )
        {
          case EN_CHANGE:
            if ( (HANDLE)lParam != window->mCmdline )
              break;
            window->mAutocomplete.reset();
          break;
        }
      break;
      case WM_EXITSIZEMOVE:
        InvalidateRect( wnd, NULL, FALSE );
        return 0;
      break;
      case WM_ERASEBKGND:
        return 1;
      break;
      case WM_CREATE:
        // Get/set ownerships
        cs = (LPCREATESTRUCTW)lParam;
        window = (ConsoleWindow*)cs->lpCreateParams;
        SetWindowLongPtrW( wnd, GWLP_USERDATA, (LONG_PTR)window );

        // Get window size
        GetClientRect( wnd, &rect );

        // Create the log box
        window->mLog = CreateWindowExW( WS_EX_LEFT, L"RichEdit50W", L"",
          WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
          0, 30, rect.right, rect.bottom - 18 - 32, wnd, NULL,
          window->mWindow->getInstance(), (void*)window );

        if ( !window->mLog )
          ENGINE_EXCEPT( L"Could not create RichEdit5 log window control" );

        // Create the command line
        window->mCmdline = CreateWindowExW( WS_EX_LEFT | WS_EX_STATICEDGE,
          L"RichEdit50W", L"", WS_VISIBLE | WS_CHILD | ES_LEFT,
          0, rect.bottom - 18, rect.right, 18, wnd, NULL,
          window->mWindow->getInstance(), (void*)window );

        if ( !window->mCmdline )
          ENGINE_EXCEPT( L"Could not create RichEdit5 command line control" );

        // Set the command line proc
        SetWindowLongPtrW( window->mCmdline, GWLP_USERDATA, (LONG_PTR)window );
        window->mCmdlineProc = (WNDPROC)SetWindowLongPtrW(
          window->mCmdline, GWLP_WNDPROC, (LONG_PTR)(WNDPROC)cmdLineProc );

        // Setup
        SendMessageW( window->mLog, EM_LIMITTEXT, -1, 0 );
        SendMessageW( window->mCmdline, EM_LIMITTEXT, 100, 0 );
        SendMessageW( window->mLog, EM_AUTOURLDETECT, TRUE, 0 );
        SendMessageW( window->mLog, EM_SETEVENTMASK, NULL, ENM_SELCHANGE | ENM_LINK );
        SendMessageW( window->mLog, EM_SETOPTIONS, ECOOP_OR,
          ECO_AUTOVSCROLL | ECO_NOHIDESEL | ECO_SAVESEL | ECO_SELECTIONBAR );

        // Create character format
        format.cbSize = sizeof( CHARFORMAT2W );
        format.dwMask = CFM_SIZE | CFM_OFFSET | CFM_EFFECTS | CFM_COLOR | CFM_BACKCOLOR | CFM_CHARSET | CFM_UNDERLINETYPE | CFM_FACE;
        format.yHeight = 160;
        format.yOffset = NULL;
        format.dwEffects = NULL;
        format.crTextColor = cConsoleWindowForeground;
        format.crBackColor = cConsoleWindowBackground;
        format.bCharSet = DEFAULT_CHARSET;
        format.bUnderlineType = CFU_UNDERLINENONE;
        wcscpy_s( format.szFaceName, LF_FACESIZE, cConsoleWindowFont );

        // Set character format
        SendMessageW( window->mLog, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format );
        SendMessageW( window->mCmdline, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format );

        // Set margins
        SendMessageW( window->mLog, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );
        SendMessageW( window->mCmdline, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );

        // Reset selection
        SendMessageW( window->mLog, EM_EXSETSEL, 0, (LPARAM)&range );
        SendMessageW( window->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );

        // Setup codepage (UCS-2) & text
        SendMessageW( window->mLog, EM_SETTEXTEX, (WPARAM)&textex, NULL );
        SendMessageW( window->mCmdline, EM_SETTEXTEX, (WPARAM)&textex, NULL );

        // Enable change notifications for the cmdline
        SendMessageW( window->mCmdline, EM_SETEVENTMASK, NULL, ENM_CHANGE );
        return 0;
      break;
      case WM_PAINT:
        dc = BeginPaint( wnd, &paintdata );
        window->paint( wnd, dc );
        EndPaint( wnd, &paintdata );
        return 0;
      break;
      case WM_SIZE:
        MoveWindow( window->mLog, 0, 32, LOWORD(lParam), HIWORD(lParam) - 18 - 32, TRUE );
        MoveWindow( window->mCmdline, 0, HIWORD(lParam) - 16, LOWORD(lParam), 18, TRUE );
        return 0;
      break;
      case WM_DESTROY:
        PostQuitMessage( EXIT_SUCCESS );
        return 0;
      break;
    }
    return DefWindowProcW( wnd, msg, wParam, lParam );
  }

}