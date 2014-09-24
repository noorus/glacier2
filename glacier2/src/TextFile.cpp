#include "StdAfx.h"
#include "TextFile.h"
#include "Exception.h"
#include "Utilities.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  TextFile::TextFile( const wstring& filename ): mFile( INVALID_HANDLE_VALUE )
  {
    mFile = CreateFileW( filename.c_str(), GENERIC_WRITE,
      FILE_SHARE_READ, nullptr, OPEN_ALWAYS,
      FILE_ATTRIBUTE_NORMAL, 0 );

    if ( mFile == INVALID_HANDLE_VALUE )
      ENGINE_EXCEPT_WINAPI( L"Couldn't create text file" );

    DWORD position = SetFilePointer( mFile, 0, nullptr, FILE_END );
    if ( position == 0 )
    {
      DWORD written;
      const BYTE UTF8BOM[3] = { 0xEF, 0xBB, 0xBF };
      if ( !WriteFile( mFile, UTF8BOM, 3, &written, nullptr ) )
        ENGINE_EXCEPT_WINAPI( L"Couldn't write BOM" );
    }
  }

  void TextFile::write( const wstring& str )
  {
    DWORD written;
    string out_utf8 = Utilities::wideToUtf8( str );
    WriteFile( mFile, out_utf8.c_str(), (DWORD)out_utf8.size(), &written, nullptr );
  }

  TextFile::~TextFile()
  {
    if ( mFile != INVALID_HANDLE_VALUE )
      CloseHandle( mFile );
  }

}