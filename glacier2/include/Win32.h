#pragma once
#include "Utilities.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Win32 {

#   define max(a,b) (((a) > (b)) ? (a) : (b))
#   define min(a,b) (((a) < (b)) ? (a) : (b))
#   include <gdiplus.h>

    class WindowProcDetour: boost::noncopyable {
    public:
      HWND mWindow;
      WNDPROC mRealProc;
      explicit WindowProcDetour( HWND window, WNDPROC newProc ): mWindow( window )
      {
        mRealProc = reinterpret_cast<WNDPROC>( GetWindowLongPtrW( mWindow, GWLP_WNDPROC ) );
        SetWindowLongPtrW( mWindow, GWLP_WNDPROC, reinterpret_cast<DWORD_PTR>( newProc ) );
      }
      ~WindowProcDetour()
      {
        SetWindowLongPtrW( mWindow, GWLP_WNDPROC, reinterpret_cast<DWORD_PTR>( mRealProc ) );
      }
    };

    class ErrorDialog: boost::noncopyable {
    public:
      struct Context {
        HINSTANCE instance;
        wstring title;
        wstring subtitle;
        wstring body;
        explicit Context( HINSTANCE instance_ ): instance( instance_ ) {}
      };
    protected:
      Context mContext;
      void paint( HWND dlg );
      void report();
      static INT_PTR CALLBACK dlgProc( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );
    public:
      ErrorDialog( const Context& context );
    };

    class Win32: public Singleton<Win32> {
    friend class Singleton<Win32>;
    protected:
      Gdiplus::GdiplusStartupInput mGDIPlusStartup;
      ULONG_PTR mGDIPlusToken;
      HMODULE mRichEdit;
      Win32();
    public:
      void prepareProcess();
      void initialize();
      wstring getCurrentDirectory();
      bool fileOpenDialog( const wstring& filterName, const wstring& filterExt, wstring& returnValue );
      void handleMessagesFor( HWND window );
      void drawErrorDialog( Gdiplus::Graphics& gfx, RECT area, const ErrorDialog::Context& ctx );
      void drawConsole( Gdiplus::Graphics& gfx, RECT area, const wstring& title, const wstring& subtitle );
      void drawNiceBar( Gdiplus::Graphics& gfx, RECT area );
      bool getCursorPosition( const HWND window, POINT& position );
      void shutdown();
    };

    class Window {
    public:
      struct Settings {
        wstring sClassName;
        HICON   hIcon;
        HICON   hSmallIcon;
        HCURSOR hCursor;
        POINT   ptPosition;
        POINT   ptSize;
        bool    bBorder;
        bool    bSizable;
        bool    bCentered;
        bool    bMaximizable;
        bool    bDisabled;
        bool    bToolWindow;
        wstring sWindowName;
        Settings(): hIcon( NULL ), hSmallIcon( NULL ), hCursor( NULL ),
          bBorder( false ), bSizable( false ), bCentered( false ),
          bMaximizable( false ), bDisabled( false ), bToolWindow( false ) {}
        static Settings toolWindow( long w, long h,
        const wstring& cls, const wstring& title )
        {
          Settings set;
          set.bBorder = true;
          set.bSizable = true;
          set.bCentered = true;
          set.bToolWindow = true;
          set.ptSize.x = w;
          set.ptSize.y = h;
          set.sClassName = cls;
          set.sWindowName = title;
          return set;
        }
      } mSettings;
    protected:
      ATOM mClass;
      WNDPROC mWndProc;
      HINSTANCE mInstance;
      LPVOID mUserData;
      HWND mHandle;
      virtual void registerClass( const wstring& name, const HICON icon,
        const HICON smallIcon, const HCURSOR cursor );
      virtual void unregisterClass();
      virtual void destroy();
    public:
      explicit Window( const HINSTANCE instance, const WNDPROC wndProc, Settings& settings, LPVOID userData = NULL );
      ~Window();
      virtual void create( POINT position, POINT size, const wstring& caption );
      virtual void center();
      virtual void resizeClient( POINT clientSize );
      virtual void setVisible( bool visible );
      virtual void setSizable( bool sizable );
      virtual void setEnabled( bool enabled );
      virtual HWND getHandle() { return mHandle; }
      virtual HINSTANCE getInstance() { return mInstance; }
    };

#undef min
#undef max

  }

}