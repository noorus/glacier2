#include "StdAfx.h"
#include "ConsoleWindow.h"
#include "Win32.h"
#include "Exception.h"
#include "Engine.h"

namespace Glacier {

  const wchar_t* cConsoleWindowFont = L"Trebuchet MS";
  const COLORREF cConsoleWindowBackground = RGB( 255, 255, 255 );
  const COLORREF cConsoleWindowForeground = RGB( 10, 13, 20 );

  ConsoleWindow::ConsoleWindow( HINSTANCE instance, Console* console ):
  mConsole( console ), mWindow( nullptr )
  {
    mConsole->addListener( this );
    mWindow = new Win32::Window( instance, wndProc,
      Win32::Window::Settings::toolWindow( 320, 240, L"gcr_console", L"console" ),
      this );
    POINT pos = { 0, 0 };
    POINT size = { 400, 260 };
    mWindow->create( pos, size, L"console" );
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
    format.dwMask = CFM_COLOR;
    format.crTextColor = color;
    SendMessage( mLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format );
    SETTEXTEX textex = { ST_SELECTION | ST_UNICODE, 1200 };
    SendMessage( mLog, EM_SETTEXTEX, (WPARAM)&textex, (LPARAM)line.c_str() );
  }

  bool ConsoleWindow::step()
  {
    MSG msgWnd;
    bool bRet = ( GetMessageW( &msgWnd, NULL, 0, 0 ) != FALSE );
    if ( bRet )
    {
      TranslateMessage( &msgWnd );
      DispatchMessage( &msgWnd );
    }
    return bRet;
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

  LRESULT CALLBACK ConsoleWindow::cmdLineProc( HWND hWnd, UINT uiMessage,
  WPARAM wParam, LPARAM lParam )
  {
    auto pW = (ConsoleWindow*)GetWindowLongPtrW( hWnd, GWLP_USERDATA );
    Console* pC = pW->mConsole;
    DWORD dwLength;
    LPWSTR pwsLine;
    wstring sBackLine;
    CHARRANGE range = { 0, -1 };
    switch ( uiMessage )
    {
      case WM_CHAR:
        if ( wParam == TAB )
          return 0;
      break;
      case WM_KEYDOWN:
        switch ( wParam )
        {
          case VK_TAB:
            dwLength = (DWORD)SendMessageW( pW->mCmdline, WM_GETTEXTLENGTH, NULL, NULL );
            if ( dwLength < 1 )
            {
              pW->mAutocomplete.suggestion = nullptr;
              break;
            }
            dwLength++;
            if ( pW->mAutocomplete.suggestion )
            {
              pC->autoComplete( pW->mAutocomplete.base, pW->mAutocomplete.matches );
            }
            else
            {
              pwsLine = new WCHAR[dwLength];
              GetWindowTextW( pW->mCmdline, pwsLine, dwLength );
              pW->mAutocomplete.base = pwsLine;
              pC->autoComplete( pwsLine, pW->mAutocomplete.matches );
              delete pwsLine;
            }
            if ( pW->mAutocomplete.matches.empty() )
            {
              pW->mAutocomplete.suggestion = nullptr;
              break;
            }
            if ( pW->mAutocomplete.suggestion )
            {
              ConBaseList::iterator itCmd = pW->mAutocomplete.matches.begin();
              while ( itCmd != pW->mAutocomplete.matches.end() )
              {
                if ( (*itCmd) == pW->mAutocomplete.suggestion )
                {
                  ++itCmd;
                  if ( itCmd == pW->mAutocomplete.matches.end() )
                    break;
                  pW->mAutocomplete.suggestion = (*itCmd);
                  pW->setCmdline( pW->mAutocomplete.suggestion->getName() );
                  SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
                  return 0;
                }
                ++itCmd;
              }
            }
            pW->mAutocomplete.suggestion = pW->mAutocomplete.matches.front();
            pW->setCmdline( pW->mAutocomplete.suggestion->getName() );
            SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
            return 0;
        break;
      case VK_UP:
        if ( pW->mHistory.stack.empty() )
          break;
        if ( pW->mHistory.browsing )
        {
          if ( pW->mHistory.position <= 0 )
          {
            pW->mHistory.position = 0;
            SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
            break;
          }
          pW->mHistory.position--;
          wstring sBackLine = pW->mHistory.stack[pW->mHistory.position];
          pW->setCmdline( sBackLine.c_str() );
          SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
        }
        else
        {
          pW->mHistory.browsing = true;
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          sBackLine = pW->mHistory.stack.back();
          // get current
          dwLength = (DWORD)SendMessageW( pW->mCmdline, WM_GETTEXTLENGTH, NULL, NULL );
          if ( dwLength > 0 )
          {
            dwLength++;
            pwsLine = new WCHAR[dwLength];
            GetWindowTextW( pW->mCmdline, pwsLine, dwLength );
            pW->mHistory.stack.push_back( pwsLine );
            delete pwsLine;
          }
          else
          {
            pW->mHistory.stack.push_back( L"" );
          }
          pW->setCmdline( sBackLine );
          SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
        }
        return 0;
        break;
      case VK_DOWN:
        if ( pW->mHistory.stack.empty() || !pW->mHistory.browsing )
          break;
        pW->mHistory.position++;
        if ( pW->mHistory.position >= pW->mHistory.stack.size() )
        {
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          pW->mHistory.browsing = false;
          break;
        }
        sBackLine = pW->mHistory.stack[pW->mHistory.position];
        pW->setCmdline( sBackLine );
        SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
        if ( pW->mHistory.position >= pW->mHistory.stack.size() - 1 )
        {
          pW->mHistory.stack.pop_back();
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          pW->mHistory.browsing = false;
        }
        return 0;
        break;
      case VK_RETURN:
        if ( pW->mHistory.browsing )
        {
          pW->mHistory.stack.pop_back();
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          pW->mHistory.browsing = false;
        }
        pW->mAutocomplete.reset();
        dwLength = (DWORD)SendMessageW( pW->mCmdline, WM_GETTEXTLENGTH, NULL, NULL );
        if ( dwLength > 0 )
        {
          dwLength++;
          pwsLine = new WCHAR[dwLength];
          GetWindowTextW( pW->mCmdline, pwsLine, dwLength );
          pC = pW->mConsole;
          if ( pC ) {
            pC->executeBuffered( pwsLine );
          }
          pW->mHistory.stack.push_back( pwsLine );
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          pW->clearCmdline();
          range.cpMin = -1;
          SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );
          delete pwsLine;
        }
        return 0;
        break;
      }
      break;
    }
    return CallWindowProcW( pW->lpfnOrigCmdLineProc, hWnd, uiMessage, wParam, lParam );
  }

  void ConsoleWindow::paint( HWND window, HDC dc )
  {
    RECT area;
    GetClientRect( window, &area );

    Win32::Gdiplus::Graphics gfx( dc );

    Win32::Win32::instance().drawConsole(
      gfx, area,
      gEngine->getVersion().title,
      gEngine->getVersion().subtitle );
  }

  LRESULT CALLBACK ConsoleWindow::wndProc( HWND hWnd, UINT uiMessage,
  WPARAM wParam, LPARAM lParam )
  {
    auto pW = (ConsoleWindow*)GetWindowLongPtrW( hWnd, GWLP_USERDATA );
    LPCREATESTRUCTW cs;
    PAINTSTRUCT paintdata;
    CHARRANGE range = { -1, -1 };
    SETTEXTEX textex = { ST_DEFAULT, 1200 };
    CHARFORMAT2W format;
    RECT rect;
    HDC dc;
    switch ( uiMessage )
    {
      case WM_COMMAND:
        switch ( HIWORD( wParam ) )
        {
          case EN_CHANGE:
            if ( (HANDLE)lParam != pW->mCmdline )
              break;
            pW->mAutocomplete.reset();
          break;
        }
      break;
      case WM_EXITSIZEMOVE:
        InvalidateRect( hWnd, NULL, FALSE );
        return 0;
      break;
      case WM_ERASEBKGND:
        return 1;
      break;
      case WM_CREATE:
        // Get/set ownerships
        cs = (LPCREATESTRUCTW)lParam;
        SetWindowLongPtrW( hWnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams );
        pW = (ConsoleWindow*)cs->lpCreateParams;

        // Get window size
        GetClientRect( hWnd, &rect );

        // Create the log box
        pW->mLog = CreateWindowExW( WS_EX_LEFT, L"RichEdit50W", L"",
          WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
          0, 30, rect.right, rect.bottom-18-32, hWnd, NULL, pW->mWindow->getInstance(), (LPVOID)pW );

        if ( !pW->mLog )
          ENGINE_EXCEPT( L"Could not create RichEdit5 log window control" );

        // Create the command line
        pW->mCmdline = CreateWindowExW( WS_EX_LEFT | WS_EX_STATICEDGE,
          L"RichEdit50W", L"", WS_VISIBLE | WS_CHILD | ES_LEFT,
          0, rect.bottom-18, rect.right, 18, hWnd, NULL, pW->mWindow->getInstance(), (LPVOID)pW );

        if ( !pW->mCmdline )
          ENGINE_EXCEPT( L"Could not create RichEdit5 command line control" );

        // Set the command line proc
        SetWindowLongPtrW( pW->mCmdline, GWLP_USERDATA, (LONG)pW );
        pW->lpfnOrigCmdLineProc = (WNDPROC)SetWindowLongPtrW(
          pW->mCmdline, GWLP_WNDPROC,
          (LONG_PTR)(WNDPROC)cmdLineProc );

        // Setup
        SendMessageW( pW->mLog, EM_LIMITTEXT, -1, 0 );
        SendMessageW( pW->mCmdline, EM_LIMITTEXT, 100, 0 );
        SendMessageW( pW->mLog, EM_AUTOURLDETECT, TRUE, 0 );
        SendMessageW( pW->mLog, EM_SETEVENTMASK, NULL, ENM_SELCHANGE | ENM_LINK );
        SendMessageW( pW->mLog, EM_SETOPTIONS, ECOOP_OR,
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
        SendMessageW( pW->mLog, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format );
        SendMessageW( pW->mCmdline, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format );

        // Set margins
        SendMessageW( pW->mLog, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );
        SendMessageW( pW->mCmdline, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );

        // Reset selection
        SendMessageW( pW->mLog, EM_EXSETSEL, 0, (LPARAM)&range );
        SendMessageW( pW->mCmdline, EM_EXSETSEL, 0, (LPARAM)&range );

        // Setup codepage (UCS-2) & text
        SendMessageW( pW->mLog, EM_SETTEXTEX, (WPARAM)&textex, NULL );
        SendMessageW( pW->mCmdline, EM_SETTEXTEX, (WPARAM)&textex, NULL );

        // Enable change notifications for the cmdline
        SendMessageW( pW->mCmdline, EM_SETEVENTMASK, NULL, ENM_CHANGE );
        return 0;
      break;
      case WM_PAINT:
        dc = BeginPaint( hWnd, &paintdata );
        pW->paint( hWnd, dc );
        EndPaint( hWnd, &paintdata );
        return 0;
      break;
      case WM_SIZE:
        MoveWindow( pW->mLog, 0, 32, LOWORD(lParam), HIWORD(lParam) - 18 - 32, TRUE );
        MoveWindow( pW->mCmdline, 0, HIWORD(lParam) - 16, LOWORD(lParam), 18, TRUE );
        return 0;
      break;
      case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
      break;
    }
    return DefWindowProcW( hWnd, uiMessage, wParam, lParam );
  }

}