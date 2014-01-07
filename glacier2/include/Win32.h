#pragma once
#include "Utilities.h"

namespace Glacier {

  namespace Win32 {

#   define max(a,b) (((a) > (b)) ? (a) : (b))
#   define min(a,b) (((a) < (b)) ? (a) : (b))
#   include <gdiplus.h>

    class Win32: public Singleton<Win32> {
    friend class Singleton<Win32>;
    protected:
      Gdiplus::GdiplusStartupInput mGDIPlusStartup;
      ULONG_PTR mGDIPlusToken;
      HMODULE mRichEdit;
      Win32();
    public:
      void initialize();
      void drawConsole( Gdiplus::Graphics& gfx, RECT area, const wstring& title, const wstring& subtitle );
      void drawNiceBar( Gdiplus::Graphics& gfx, RECT area );
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
      WNDPROC pfnWndProc;
      HINSTANCE hInstance;
      LPVOID lpUserdata;
      HWND mHandle;
      virtual void registerClass( const wstring& name, const HICON icon,
        const HICON smallIcon, const HCURSOR cursor );
      virtual void unregisterClass();
      virtual void destroy();
    public:
      explicit Window( const HINSTANCE hInstance, const WNDPROC lpfnWndProc, Settings& mSettings, LPVOID lpUserdata = NULL );
      ~Window();
      virtual void create( POINT ptPosition, POINT ptSize, const wstring& caption );
      virtual void center();
      virtual void resizeClient( POINT ptClientSize );
      virtual void setVisible( bool visible );
      virtual void setSizable( bool sizable );
      virtual void setEnabled( bool enabled );
      virtual HWND getHandle() { return mHandle; }
      virtual HINSTANCE getInstance() { return hInstance; }
    };

  }

}