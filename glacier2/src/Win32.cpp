#include "StdAfx.h"
#include "Win32.h"
#include "Exception.h"

namespace Glacier {

  Win32::Win32(): mRichEdit( NULL )
  {
  }

  void Win32::initialize()
  {
    INITCOMMONCONTROLSEX ctrls;
    ctrls.dwSize = sizeof( INITCOMMONCONTROLSEX );
    ctrls.dwICC = ICC_STANDARD_CLASSES | ICC_NATIVEFNTCTL_CLASS;

    if ( !InitCommonControlsEx( &ctrls ) )
      ENGINE_EXCEPT_W32( L"Couldn't initialize common controls, missing manifest?" );

    mRichEdit = LoadLibraryW( L"msftedit.dll" );
    if ( !mRichEdit )
      ENGINE_EXCEPT_W32( L"Couldn't load RichEdit module" );
  }

  void Win32::shutdown()
  {
    if ( mRichEdit )
      FreeLibrary( mRichEdit );
    mRichEdit = NULL;
  }

}