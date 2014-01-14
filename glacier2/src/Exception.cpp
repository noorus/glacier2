#include "StdAfx.h"
#include "Utilities.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  Exception::Exception( const wstring& description, Type type ):
  mDescription( description ), mType( type )
  {
    handleAdditional();
  }

  Exception::Exception( const wstring& description, const wstring& source, Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    handleAdditional();
  }

  Exception::Exception( const wstring& description, const wstring& source, NTSTATUS ntstatus, Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    handleAdditional();
    // TODO handle NTSTATUS
  }

  Exception::Exception( const wstring& description, const wstring& source, FMOD_RESULT result, Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    handleAdditional();
    // TODO handle FMOD_RESULT
  }

  void Exception::handleAdditional()
  {
    if ( mType == WinAPI )
    {
      WinAPIError error;
      LPWSTR message = nullptr;
      error.code = GetLastError();
      FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error.code, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPWSTR)&message, 0, NULL );
      error.description = message;
      LocalFree( message );
      mAdditional = error;
    }
  }

  const wstring& Exception::getFullDescription() const
  {
    if ( mFullDescription.empty() )
    {
      wstringstream stream;
      stream << mDescription;
      if ( !mSource.empty() )
        stream << L"\r\nIn function " << mSource;
      if ( mType == WinAPI )
      {
        const WinAPIError& error = boost::get<WinAPIError>( mAdditional );
        stream << L"\r\nWinAPI return code " << std::hex << error.code << L":\r\n" << error.description;
      }
      mFullDescription = stream.str();
    }
    return mFullDescription;
  }

  const char* Exception::what() const
  {
    return Utilities::wideToUtf8( getFullDescription() ).c_str();
  }

}