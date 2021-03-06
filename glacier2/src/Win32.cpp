#include "StdAfx.h"
#include "Win32.h"
#include "Exception.h"
#include "../glacier2_resource.h"
#include "ServiceLocator.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

#pragma warning( push )

// warning C4244: 'argument': conversion from 'LONG' to 'REAL', possible loss of data
#pragma warning( disable: 4244 )

namespace Glacier {

  namespace Win32 {

    using namespace Gdiplus;

    // Win32 constants ========================================================

    const wchar_t* cErrorDialogTitle  = L"glacier² » error!";
    const wchar_t* cRichEditDLL       = L"msftedit.dll";
    const wchar_t* cWin32UIFont       = L"Segoe UI";

    // Win32 singleton ========================================================

    Win32::Win32(): mRichEdit( NULL ), mGDIPlusStartup( nullptr ),
    mGDIPlusToken( 0 )
    {
    }

    void Win32::prepareProcess()
    {
      // Never show those useless, stupid Windows "solution" dialogs
      SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );

      // Up our generic process scheduling priority
      SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );

      // Tell MMCSS to prioritise this thread as a game
      DWORD index = 0;
      AvSetMmThreadCharacteristicsW( L"Games", &index );
    }

    const unsigned long cPrivileges = 1;

    void Win32::adjustPrivileges()
    {
      HANDLE process = GetCurrentProcess();
      HANDLE token;

      if ( !OpenProcessToken( process, TOKEN_ADJUST_PRIVILEGES, &token ) )
        return;

      LPCWSTR wantedPrivileges[cPrivileges] = {
        SE_LOCK_MEMORY_NAME
      };

      auto privileges = (PTOKEN_PRIVILEGES)malloc(
        FIELD_OFFSET( TOKEN_PRIVILEGES, Privileges[cPrivileges] ) );

      if ( privileges )
      {
        privileges->PrivilegeCount = cPrivileges;
        for ( int i = 0; i < cPrivileges; i++ )
        {
          if ( !LookupPrivilegeValueW( NULL, wantedPrivileges[i], &privileges->Privileges[i].Luid ) )
            return;
          privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
        }
        AdjustTokenPrivileges( token, FALSE, privileges, 0, NULL, NULL );
        free( privileges );
      }

      CloseHandle( token );
    }

    void Win32::initialize()
    {
      INITCOMMONCONTROLSEX ctrls;
      ctrls.dwSize = sizeof( INITCOMMONCONTROLSEX );
      ctrls.dwICC = ICC_STANDARD_CLASSES | ICC_NATIVEFNTCTL_CLASS;

      if ( !InitCommonControlsEx( &ctrls ) )
        ENGINE_EXCEPT_WINAPI( "Couldn't initialize common controls, missing manifest?" );

      mRichEdit = LoadLibraryW( cRichEditDLL );
      if ( !mRichEdit )
        ENGINE_EXCEPT_WINAPI( "Couldn't load RichEdit module" );

      Gdiplus::GdiplusStartup( &mGDIPlusToken, &mGDIPlusStartup, NULL );
    }

    wstring Win32::getCurrentDirectory()
    {
      wchar_t currentDirectory[MAX_PATH];
      if ( !GetCurrentDirectoryW( MAX_PATH, currentDirectory ) )
        ENGINE_EXCEPT_WINAPI( "Couldn't fetch current directory" );

      return wstring( currentDirectory );
    }

    bool Win32::windowHasFocus( const HWND window )
    {
      return ( GetFocus() == window );
    }

    bool Win32::fileOpenDialog( const wstring& filterName, const wstring& filterExt, wstring& returnValue )
    {
      IFileDialog* fileDialog;
      if ( CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &fileDialog ) ) < 0 )
        ENGINE_EXCEPT( "CoCreateInstance failed" );

      COMDLG_FILTERSPEC filterSpecs[] = {
        { filterName.c_str(), filterExt.c_str() },
        { L"All Files (*.*)", L"*.*" } };

      fileDialog->SetFileTypes( ARRAYSIZE( filterSpecs ), filterSpecs );

      wstring browseDirectory = getCurrentDirectory();
      browseDirectory.append( L"\\data\\bootload" );

      IShellItem* shellItem = NULL;
      SHCreateItemFromParsingName( browseDirectory.c_str(), NULL, IID_PPV_ARGS( &shellItem ) );
      if ( shellItem ) {
        fileDialog->AddPlace( shellItem, FDAP_BOTTOM );
        fileDialog->SetFolder( shellItem );
      }
      fileDialog->SetOptions( FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM );

      if ( fileDialog->Show( Locator::getGraphics().getRenderWindowHandle() ) != S_OK )
        return false;

      fileDialog->GetResult( &shellItem );

      wchar_t* returned;
      shellItem->GetDisplayName( SIGDN_FILESYSPATH, &returned );
      returnValue = returned;
      CoTaskMemFree( returned );

      return true;
    }

    void Win32::handleMessagesFor( HWND window )
    {
      MSG  msg;

      while( PeekMessage( &msg, window, 0U, 0U, PM_REMOVE ) )
      {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
      }
    }

    bool Win32::getCursorPosition( const HWND window, POINT& position )
    {
      if ( !GetCursorPos( &position ) )
        return false;
      if ( !ScreenToClient( window, &position ) )
        return false;

      return true;
    }

    void Win32::drawErrorDialog( Gdiplus::Graphics& gfx, RECT area, const ErrorDialog::Context& ctx )
    {
      Gdiplus::SolidBrush gpbrText( Color( 255, 255, 255, 255 ) );

      RECT bar = { area.left, area.top, area.right, area.top + 32 };
      drawNiceBar( gfx, bar );

      Gdiplus::FontFamily fontFamily( cWin32UIFont );
      Gdiplus::Font gpfntTitle( &fontFamily, 11, FontStyleRegular, UnitPixel );
      Gdiplus::Font gpfntSubtitle( &fontFamily, 9, FontStyleRegular, UnitPixel );
      Gdiplus::RectF rectTitle( area.left + 8.0f, 0.0f, area.right - 16.0f, 20.0f );
      Gdiplus::RectF rectSubtitle( area.left + 9.0f, 15.0f, area.right - 16.0f, 20.0f );

      Gdiplus::StringFormat format;
      format.SetAlignment( StringAlignmentNear );
      format.SetLineAlignment( StringAlignmentCenter );

      Gdiplus::PointF ptTitle( area.left + 8.0f, 12.0f );
      Gdiplus::PointF ptSubtitle( area.left + 9.0f, 22.0f );

      Gdiplus::RectF clip( area.left, area.top, area.right - area.left - 8.0f, area.bottom - area.top );

      gfx.SetClip( clip );

      gfx.DrawString( ctx.title.c_str(), -1, &gpfntTitle, ptTitle, &format, &gpbrText );
      gfx.DrawString( ctx.subtitle.c_str(), -1, &gpfntSubtitle, ptSubtitle, &format, &gpbrText );

      gfx.ResetClip();
    }

    void Win32::drawConsole( Gdiplus::Graphics& gfx, RECT area, const wstring& title, const wstring& subtitle )
    {
      Gdiplus::SolidBrush gpbrBlack( Color( 255, 0, 0, 0 ) );
      Gdiplus::SolidBrush headerBrush( Color( 255, 22, 16, 14 ) );
      Gdiplus::SolidBrush bgBrush( Color( 255, 240, 240, 240 ) );
      Gdiplus::SolidBrush gpbrText( Color( 255, 255, 255, 255 ) );

      gfx.FillRectangle( &bgBrush, area.left, area.top, area.right - area.left, area.bottom - area.top );
      gfx.FillRectangle( &headerBrush, area.left, area.top, area.right - area.left, 32 );

      RECT bar = { area.left, area.top, area.right, area.top + 32 };
      drawNiceBar( gfx, bar );

      Gdiplus::FontFamily fontFamily( cWin32UIFont );
      Gdiplus::Font gpfntTitle( &fontFamily, 11, FontStyleRegular, UnitPixel );
      Gdiplus::Font gpfntSubtitle( &fontFamily, 9, FontStyleRegular, UnitPixel );
      Gdiplus::RectF rectTitle( area.left + 8.0f, 0.0f, area.right - 16.0f, 20.0f );
      Gdiplus::RectF rectSubtitle( area.left + 9.0f, 15.0f, area.right - 16.0f, 20.0f );

      Gdiplus::StringFormat format;
      format.SetAlignment( StringAlignmentNear );
      format.SetLineAlignment( StringAlignmentCenter );

      Gdiplus::PointF ptTitle( area.left + 8.0f, 12.0f );
      Gdiplus::PointF ptSubtitle( area.left + 9.0f, 22.0f );

      Gdiplus::RectF clip( area.left, area.top, area.right - area.left - 8.0f, area.bottom - area.top );

      gfx.SetClip( clip );

      gfx.DrawString( title.c_str(), -1, &gpfntTitle, ptTitle, &format, &gpbrText );
      gfx.DrawString( subtitle.c_str(), -1, &gpfntSubtitle, ptSubtitle, &format, &gpbrText );

      gfx.ResetClip();
    }

    void Win32::drawNiceBar( Gdiplus::Graphics& gfx, RECT area )
    {
      Gdiplus::SolidBrush gpbrWhite( Color( 255, 255, 255, 255 ) );
      Gdiplus::SolidBrush gpbrVistaGray( Color( 255, 240, 240, 240 ) );
      Gdiplus::SolidBrush gpbrGray( Color( 150, 108, 114, 122 ) );
      Gdiplus::SolidBrush gpbrBlack( Color( 255, 0, 0, 0 ) );
      Gdiplus::SolidBrush gpbrKewl( Color( 255, 166, 24, 94 ) );
      Gdiplus::SolidBrush gpbrOpWhite( Color( 200, 255, 255, 255 ) );

      POINT ptBarSize = { area.right - area.left, area.bottom - area.top };

      Gdiplus::SolidBrush gpbrBorder( Gdiplus::Color( 255, 183, 211, 217 ) );
      Gdiplus::SolidBrush gpbrBackground( Gdiplus::Color( 255, 22, 103, 119 ) );
      Gdiplus::SolidBrush gpbrShadow( Gdiplus::Color( 255, 13, 58, 66 ) );
      Gdiplus::SolidBrush gpbrHighlight( Gdiplus::Color( 255, 255, 255, 255 ) );

      gfx.FillRectangle( &gpbrHighlight, area.left, area.top, ptBarSize.x, ptBarSize.y+2 );
      gfx.FillRectangle( &gpbrShadow, area.left, area.top, ptBarSize.x, ptBarSize.y + 1 );
      gfx.FillRectangle( &gpbrBorder, area.left, area.top, ptBarSize.x, ptBarSize.y );

      area.top++;
      area.left++;
      area.right++;
      area.bottom++;

      ptBarSize.x -= 2;
      ptBarSize.y -= 2;

      LinearGradientBrush gpbrTopGradient(
        Gdiplus::Point( 0, area.top ),
        Gdiplus::Point( 0, area.bottom / 2 ),
        Gdiplus::Color( 255, 138, 184, 193 ),
        Gdiplus::Color( 255, 74, 145, 159 )
      );

      gfx.FillRectangle( &gpbrBackground, area.left, area.top, ptBarSize.x, ptBarSize.y );
      gfx.FillRectangle( &gpbrTopGradient, area.left, area.top, ptBarSize.x, ptBarSize.y / 2 );

      area.top += ptBarSize.y / 2;
      ptBarSize.y = ptBarSize.y / 2;

      LinearGradientBrush gpbrBottomGradientTop(
        Gdiplus::Point( 0, area.top - 1),
        Gdiplus::Point( 0, area.bottom - (ptBarSize.y / 2) ),
        Gdiplus::Color( 255, 22, 103, 119 ),
        Gdiplus::Color( 255, 43, 117, 131 )
      );

      gfx.FillRectangle( &gpbrBottomGradientTop, area.left, area.top, ptBarSize.x, (ptBarSize.y / 2) );

      LinearGradientBrush gpbrBottomGradientBottom(
        Gdiplus::Point( 0, area.top + (ptBarSize.y / 2) - 1),
        Gdiplus::Point( 0, area.bottom ),
        Gdiplus::Color( 215, 43, 117, 131 ),
        Gdiplus::Color( 255, 105, 180, 188 )
      );

      gfx.FillRectangle( &gpbrBottomGradientBottom, area.left, area.top + (ptBarSize.y / 2), ptBarSize.x, ptBarSize.y / 2 );
    }

    void Win32::shutdown()
    {
      if ( mGDIPlusToken )
        Gdiplus::GdiplusShutdown( mGDIPlusToken );

      if ( mRichEdit )
        FreeLibrary( mRichEdit );

      mRichEdit = NULL;
    }

    // ErrorDialog class ======================================================

    ErrorDialog::ErrorDialog( const Context& context ):
    mContext( context )
    {
      DialogBoxParamW( mContext.instance,
        MAKEINTRESOURCEW( IDD_ERROR ),
        0, dlgProc, (LPARAM)this );
    }

    void ErrorDialog::paint( HWND dlg )
    {
      RECT area;
      PAINTSTRUCT paintStruct;
      HDC dc = BeginPaint( dlg, &paintStruct );
      GetClientRect( dlg, &area );
      Gdiplus::Graphics gfx( dc );
      Win32::instance().drawErrorDialog( gfx, area, mContext );
      EndPaint( dlg, &paintStruct );
    }

    void ErrorDialog::report()
    {
      // TODO
    }

    INT_PTR CALLBACK ErrorDialog::dlgProc( HWND dlg, UINT msg,
    WPARAM wParam, LPARAM lParam )
    {
      auto dialog = (ErrorDialog*)GetWindowLongPtrW( dlg, GWLP_USERDATA );
      switch ( msg )
      {
        case WM_INITDIALOG:
          dialog = (ErrorDialog*)lParam;
          SetWindowLongPtrW( dlg, GWLP_USERDATA, (LONG_PTR)dialog );
          SetWindowTextW( dlg, cErrorDialogTitle );
          SetDlgItemTextW( dlg, IDC_ERROR_BODY, dialog->mContext.body.c_str() );
        break;
        case WM_PAINT:
          dialog->paint( dlg );
          return 0;
        break;
        case WM_SYSCOMMAND:
          switch ( wParam ) {
            case SC_CLOSE:
              EndDialog( dlg, FALSE );
              return 1;
            break;
          }
        break;
        case WM_COMMAND:
          switch ( wParam ) {
            case IDC_ERROR_REPORT:
              dialog->report();
            break;
            case IDC_ERROR_EXIT:
              EndDialog( dlg, FALSE );
              return 1;
            break;
          }
        break;
      }
      return 0;
    }

    // Window class ===========================================================

    Window::Window( const HINSTANCE instance, const WNDPROC wndProc,
    Settings& settings, LPVOID userData ): mClass( NULL ), mHandle( NULL ),
    mSettings( settings ), mWndProc( wndProc ), mInstance( instance ),
    mUserData( userData )
    {
      registerClass( mSettings.sClassName, mSettings.hIcon,
        mSettings.hSmallIcon, mSettings.hCursor );
    }

    Window::~Window()
    {
      destroy();
      unregisterClass();
    }

    void Window::registerClass( const wstring& name, const HICON icon,
    const HICON smallIcon, const HCURSOR cursor )
    {
      WNDCLASSEXW wndClass = { 0 };

      wndClass.cbSize        = sizeof( WNDCLASSEXW );
      wndClass.style         = 0;
      wndClass.lpfnWndProc   = mWndProc;
      wndClass.cbClsExtra    = NULL;
      wndClass.cbWndExtra    = NULL;
      wndClass.hInstance     = mInstance;
      wndClass.hIcon         = icon;
      wndClass.hCursor       = cursor;
      wndClass.hbrBackground = NULL;
      wndClass.lpszMenuName  = NULL;
      wndClass.lpszClassName = name.c_str();
      wndClass.hIconSm       = smallIcon;

      mClass = RegisterClassExW( &wndClass );

      if ( !mClass )
        ENGINE_EXCEPT_WINAPI( "Could not register window class" );
    }

    void Window::resizeClient( POINT clientSize )
    {
      RECT client;
      if ( !GetClientRect( mHandle, &client ) )
        return;

      RECT window;
      if ( !GetWindowRect( mHandle, &window ) )
        return;

      POINT diff = {
        ( window.right - window.left ) - client.right,
        ( window.bottom - window.top ) - client.bottom
      };

      MoveWindow(
        mHandle, window.left, window.top,
        clientSize.x + diff.x, clientSize.y + diff.y,
        IsWindowVisible( mHandle )
      );
    }

    void Window::center()
    {
      RECT area;
      if ( !SystemParametersInfoW( SPI_GETWORKAREA, 0, &area, SPIF_UPDATEINIFILE ) )
        return;

      RECT wndrect;
      if ( !GetWindowRect( mHandle, &wndrect ) )
        return;

      POINT sized = { ( wndrect.right - wndrect.left ) / 2, ( wndrect.bottom - wndrect.top ) / 2 };

      SetWindowPos( mHandle, HWND_TOP,
        area.left + ( ( ( area.right - area.left ) / 2 ) - sized.x ),
        area.top + ( ( ( area.bottom - area.top ) / 2 ) - sized.y ),
        NULL, NULL, SWP_NOSIZE );
    }

    void Window::create( POINT position, POINT size, const wstring& caption )
    {
      DWORD style = WS_CLIPCHILDREN;
      DWORD exStyle = NULL;

      if ( mSettings.bToolWindow )
        exStyle = WS_EX_TOOLWINDOW | WS_EX_APPWINDOW;

      if ( mSettings.bBorder )
      {
        style |= WS_OVERLAPPEDWINDOW | WS_POPUP;
        mSettings.bSizable ? style |= WS_SIZEBOX : style &= ~WS_SIZEBOX;
        mSettings.bMaximizable ? style |= WS_MAXIMIZEBOX : style &= ~WS_MAXIMIZEBOX;
      }

      mSettings.bDisabled ? style |= WS_DISABLED : style &= ~WS_DISABLED;

      mHandle = CreateWindowExW( exStyle, (LPCWSTR)mClass, //-V542
        caption.c_str(), style, position.x, position.y,
        size.x, size.y, NULL, NULL, mInstance, mUserData );

      if ( !mHandle )
        ENGINE_EXCEPT_WINAPI( "Could not create window" );

      resizeClient( size );

      if ( mSettings.bCentered )
        center();

      UpdateWindow( mHandle );
    }

    void Window::destroy()
    {
      if ( mHandle )
        DestroyWindow( mHandle );
    }

    void Window::setVisible( bool visible )
    {
      ShowWindow( mHandle, visible ? SW_SHOW : SW_HIDE );
      UpdateWindow( mHandle );
    }

    void Window::setSizable( bool sizable )
    {
      LONG_PTR style = GetWindowLongPtrW( mHandle, GWL_STYLE );
      sizable ? style |= WS_SIZEBOX : style &= ~WS_SIZEBOX;
      SetWindowLongPtrW( mHandle, GWL_STYLE, style );
      UpdateWindow( mHandle );
    }

    void Window::setEnabled( bool enabled )
    {
      EnableWindow( mHandle, enabled );
    }

    void Window::unregisterClass()
    {
      if ( mClass )
        ::UnregisterClassW( (LPCWSTR)mClass, mInstance ); //-V542
    }

  }

}

#pragma warning( pop )