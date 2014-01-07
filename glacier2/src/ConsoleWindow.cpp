#include "StdAfx.h"
#include "ConsoleWindow.h"
#include "Win32.h"
#include "Exception.h"
#include "Engine.h"

#define GLACIER_RICHEDIT_CONTROL L"RichEdit50W"
#define GLACIER_CONSOLE_FONT L"Trebuchet MS"
#define GLACIER_GFX_RENDER_CLASS "GCR_RENDER"
#define GLACIER_CONSOLE_CLASS L"GCR_CONSOLE"
#define GLACIER_EDITOR_TOOLWINDOW_CLASS L"GCR_ED_TOOLBOX"
#define GLACIER_UI_FONT L"Segoe UI"
#define GLACIER_CONCOLOR_FOREGROUND RGB(10,13,20)
#define GLACIER_CONCOLOR_BACKGROUND RGB(255,255,255)
#define GLACIER_CONCOLOR_ERROR RGB(255,0,0)

namespace Glacier {

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
    ucrLog.cpMin = -1;
    ucrLog.cpMax = -1;
    SendMessage( hCtrlLog, EM_EXSETSEL, 0, (LPARAM)&ucrLog );
    ucfLog.dwMask = CFM_COLOR;
    ucfLog.crTextColor = color;
    SendMessage( hCtrlLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&ucfLog );
    ustLog.codepage = 1200;
    ustLog.flags = ST_SELECTION;
    SendMessage( hCtrlLog, EM_SETTEXTEX, (WPARAM)&ustLog, (LPARAM)line.c_str() );
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
    SendMessageW( hCtrlCmdLine, EM_SETEVENTMASK, NULL, ENM_NONE );
    ustLog.codepage = 1200;
    ustLog.flags = ST_DEFAULT;
    SendMessageW( hCtrlCmdLine, EM_SETTEXTEX, (WPARAM)&ustLog, (LPARAM)NULL );
    SendMessageW( hCtrlCmdLine, EM_SETEVENTMASK, NULL, ENM_CHANGE );
  }

  void ConsoleWindow::setCmdline( const wstring& sLine )
  {
    SendMessageW( hCtrlCmdLine, EM_SETEVENTMASK, NULL, ENM_NONE );
    ustLog.codepage = 1200;
    ustLog.flags = ST_DEFAULT;
    SendMessageW( hCtrlCmdLine, EM_SETTEXTEX, (WPARAM)&ustLog, (LPARAM)sLine.c_str() );
    SendMessageW( hCtrlCmdLine, EM_SETEVENTMASK, NULL, ENM_CHANGE );
  }

  LRESULT CALLBACK ConsoleWindow::cmdLineProc( HWND hWnd, UINT uiMessage,
  WPARAM wParam, LPARAM lParam )
  {
    ConsoleWindow* pW = (ConsoleWindow*)GetWindowLongPtrW( hWnd, GWLP_USERDATA );
    Console* pC = pW->mConsole;
    DWORD dwLength;
    LPWSTR pwsLine;
    wstring sBackLine;
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
            dwLength = (DWORD)SendMessageW( pW->hCtrlCmdLine, WM_GETTEXTLENGTH, NULL, NULL );
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
              GetWindowTextW( pW->hCtrlCmdLine, pwsLine, dwLength );
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
                  pW->ucrLog.cpMin = 0;
                  pW->ucrLog.cpMax = -1;
                  SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
                  return 0;
                }
                ++itCmd;
              }
            }
            pW->mAutocomplete.suggestion = pW->mAutocomplete.matches.front();
            pW->setCmdline( pW->mAutocomplete.suggestion->getName() );
            pW->ucrLog.cpMin = 0;
            pW->ucrLog.cpMax = -1;
            SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
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
            pW->ucrLog.cpMin = 0;
            pW->ucrLog.cpMax = -1;
            SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
            break;
          }
          pW->mHistory.position--;
          wstring sBackLine = pW->mHistory.stack[pW->mHistory.position];
          pW->setCmdline( sBackLine.c_str() );
          pW->ucrLog.cpMin = 0;
          pW->ucrLog.cpMax = -1;
          SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
        }
        else
        {
          pW->mHistory.browsing = true;
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          sBackLine = pW->mHistory.stack.back();
          // get current
          dwLength = (DWORD)SendMessageW( pW->hCtrlCmdLine, WM_GETTEXTLENGTH, NULL, NULL );
          if ( dwLength > 0 )
          {
            dwLength++;
            pwsLine = new WCHAR[dwLength];
            GetWindowTextW( pW->hCtrlCmdLine, pwsLine, dwLength );
            pW->mHistory.stack.push_back( pwsLine );
            delete pwsLine;
          }
          else
          {
            pW->mHistory.stack.push_back( L"" );
          }
          pW->setCmdline( sBackLine );
          pW->ucrLog.cpMin = 0;
          pW->ucrLog.cpMax = -1;
          SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
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
          pW->ucrLog.cpMin = 0;
          pW->ucrLog.cpMax = -1;
          SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
          pW->ucrLog.cpMin = -1;
          pW->mHistory.browsing = false;
          break;
        }
        sBackLine = pW->mHistory.stack[pW->mHistory.position];
        pW->setCmdline( sBackLine );
        pW->ucrLog.cpMin = 0;
        pW->ucrLog.cpMax = -1;
        SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
        pW->ucrLog.cpMin = -1;
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
        dwLength = (DWORD)SendMessageW( pW->hCtrlCmdLine, WM_GETTEXTLENGTH, NULL, NULL );
        if ( dwLength > 0 )
        {
          dwLength++;
          pwsLine = new WCHAR[dwLength];
          GetWindowTextW( pW->hCtrlCmdLine, pwsLine, dwLength );
          pC = pW->mConsole;
          if ( pC ) {
            pC->executeBuffered( pwsLine );
          }
          pW->mHistory.stack.push_back( pwsLine );
          pW->mHistory.position = pW->mHistory.stack.size() - 1;
          pW->clearCmdline();
          pW->ucrLog.cpMin = -1;
          pW->ucrLog.cpMax = -1;
          SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0, (LPARAM)&pW->ucrLog );
          delete pwsLine;
        }
        return 0;
        break;
      }
      break;
    }
    return CallWindowProcW( pW->lpfnOrigCmdLineProc, hWnd, uiMessage, wParam, lParam );
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

  LRESULT CALLBACK ConsoleWindow::wndProc( HWND hWnd, UINT uiMessage,
  WPARAM wParam, LPARAM lParam )
  {
    ConsoleWindow* pW = (ConsoleWindow*)GetWindowLongPtrW( hWnd, GWLP_USERDATA );
    RECT rect;
    HDC dc;
    LPCREATESTRUCTW cs;
    PAINTSTRUCT psDlg;
    switch ( uiMessage )
    {
      case WM_COMMAND:
        switch ( HIWORD( wParam ) )
        {
          case EN_CHANGE:
            if ( (HANDLE)lParam != pW->hCtrlCmdLine )
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
      case WM_CREATE:
        // Get/set ownerships
        cs = (LPCREATESTRUCTW)lParam;
        SetWindowLongPtrW( hWnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams );
        pW = (ConsoleWindow*)cs->lpCreateParams;

        // Get window size
        GetClientRect( hWnd, &rect );

        // Create the log box
        pW->hCtrlLog = CreateWindowExW( WS_EX_LEFT,
          GLACIER_RICHEDIT_CONTROL, L"",
          WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
          0, 30, rect.right, rect.bottom-18-32, hWnd, NULL, pW->mWindow->getInstance(), (LPVOID)pW );

        if ( !pW->hCtrlLog )
          ENGINE_EXCEPT( L"Could not create the RichEdit5 log control" );

        // Create the command line
        pW->hCtrlCmdLine = CreateWindowExW( WS_EX_LEFT | WS_EX_STATICEDGE,
          GLACIER_RICHEDIT_CONTROL, L"", WS_VISIBLE | WS_CHILD | ES_LEFT,
          0, rect.bottom-18, rect.right, 18, hWnd, NULL, pW->mWindow->getInstance(), (LPVOID)pW );

        if ( !pW->hCtrlCmdLine )
          ENGINE_EXCEPT( L"Could not create the RichEdit5 command line control" );

        // Set the command line proc
        SetWindowLongPtrW( pW->hCtrlCmdLine, GWLP_USERDATA, (LONG)pW );
        pW->lpfnOrigCmdLineProc = (WNDPROC)SetWindowLongPtrW(
          pW->hCtrlCmdLine, GWLP_WNDPROC,
          (LONG_PTR)(WNDPROC)cmdLineProc );

        // Setup
        SendMessageW( pW->hCtrlLog, EM_LIMITTEXT, -1, 0 );
        SendMessageW( pW->hCtrlCmdLine, EM_LIMITTEXT, 100, 0 );
        SendMessageW( pW->hCtrlLog, EM_AUTOURLDETECT, TRUE, 0 );
        SendMessageW( pW->hCtrlLog, EM_SETEVENTMASK, NULL, ENM_SELCHANGE | ENM_LINK );
        SendMessageW( pW->hCtrlLog, EM_SETOPTIONS, ECOOP_OR,
          ECO_AUTOVSCROLL | ECO_NOHIDESEL | ECO_SAVESEL | ECO_SELECTIONBAR );

        // Create character format
        pW->ucfLog.cbSize = sizeof( CHARFORMAT2W );
        pW->ucfLog.dwMask = CFM_SIZE | CFM_OFFSET | CFM_EFFECTS | CFM_COLOR | CFM_BACKCOLOR | CFM_CHARSET | CFM_UNDERLINETYPE | CFM_FACE;
        pW->ucfLog.yHeight = 160;
        pW->ucfLog.yOffset = NULL;
        pW->ucfLog.dwEffects = NULL;
        pW->ucfLog.crTextColor = GLACIER_CONCOLOR_FOREGROUND;
        pW->ucfLog.crBackColor = GLACIER_CONCOLOR_BACKGROUND;
        pW->ucfLog.bCharSet = DEFAULT_CHARSET;
        pW->ucfLog.bUnderlineType = CFU_UNDERLINENONE;
        wcscpy_s( pW->ucfLog.szFaceName, LF_FACESIZE, GLACIER_CONSOLE_FONT );

        // Set character format
        SendMessageW( pW->hCtrlLog, EM_SETCHARFORMAT,
          SCF_ALL, (LPARAM)&pW->ucfLog );
        SendMessageW( pW->hCtrlCmdLine, EM_SETCHARFORMAT,
          SCF_ALL, (LPARAM)&pW->ucfLog );
        SendMessageW( pW->hCtrlLog, EM_SETMARGINS,
          EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );
        SendMessageW( pW->hCtrlCmdLine, EM_SETMARGINS,
          EC_LEFTMARGIN | EC_RIGHTMARGIN | EC_USEFONTINFO, NULL );

        // Reset selection
        pW->ucrLog.cpMin = -1;
        pW->ucrLog.cpMax = -1;
        SendMessageW( pW->hCtrlLog, EM_EXSETSEL, 0,
          (LPARAM)&pW->ucrLog );
        SendMessageW( pW->hCtrlCmdLine, EM_EXSETSEL, 0,
          (LPARAM)&pW->ucrLog );

        // Setup codepage (UCS-2) & text
        pW->ustLog.codepage = 1200;
        pW->ustLog.flags = ST_DEFAULT;
        SendMessageW( pW->hCtrlLog, EM_SETTEXTEX, (WPARAM)&pW->ustLog, NULL );
        SendMessageW( pW->hCtrlCmdLine, EM_SETTEXTEX, (WPARAM)&pW->ustLog, NULL );

        // Enable change notifications for the cmdline
        SendMessageW( pW->hCtrlCmdLine, EM_SETEVENTMASK, NULL, ENM_CHANGE );
        return 0;
      break;
      case WM_PAINT:
        dc = BeginPaint( hWnd, &psDlg );
        pW->paint( hWnd, dc );
        EndPaint( hWnd, &psDlg );
        return 0;
      break;
      case WM_SIZE:
        MoveWindow( pW->hCtrlLog, 0, 32, LOWORD(lParam), HIWORD(lParam) - 18 - 32, TRUE );
        MoveWindow( pW->hCtrlCmdLine, 0, HIWORD(lParam) - 16, LOWORD(lParam), 18, TRUE );
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