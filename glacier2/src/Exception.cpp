#include "StdAfx.h"
#include "Utilities.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const wchar_t* cNtDLL = L"ntdll.dll";

  Exception::Exception( const string& description, const Type type ):
  mDescription( description ), mType( type )
  {
    handleAdditional();
  }

  Exception::Exception( const string& description, const string& source, const Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    handleAdditional();
  }

  Exception::Exception( const string& description, const string& source, NTSTATUS ntstatus, const Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    if ( mType == Type::NT )
    {
      WinAPIError error;
      LPWSTR message = nullptr;
      error.code = ntstatus;
      FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandleW( cNtDLL ),
        ntstatus,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPWSTR)&message, 0, NULL );
      error.description = Utilities::wideToUtf8( message );
      LocalFree( message );
      mAdditional = error;
    }
  }

  Exception::Exception( const string& description, const string& source, FMOD_RESULT result, const Type type ):
  mDescription( description ), mSource( source ), mType( type )
  {
    if ( mType == Type::FMOD )
    {
      FMODError error;
      error.code = result;
      error.description = FMOD_ErrorString( result );
      mAdditional = error;
    }
  }

  void Exception::handleAdditional()
  {
    if ( mType == Type::WinAPI )
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
      error.description = Utilities::wideToUtf8( message );
      LocalFree( message );
      mAdditional = error;
    }
  }

  const string& Exception::getFullDescription() const
  {
    // TODO Much prettier formatting!
    if ( mFullDescription.empty() )
    {
      stringstream stream;
      stream << mDescription;
      if ( !mSource.empty() )
        stream << "\r\nIn function " << mSource;
      if ( mType == Type::WinAPI )
      {
        const WinAPIError& error = boost::get<WinAPIError>( mAdditional );
        stream << "\r\nWinAPI return code " << std::hex << error.code << ":\r\n" << error.description;
      }
      else if ( mType == Type::NT )
      {
        const WinAPIError& error = boost::get<WinAPIError>( mAdditional );
        stream << "\r\nNT return code " << std::hex << error.code << ":\r\n" << error.description;
      }
      else if ( mType == Type::FMOD )
      {
        const FMODError& error = boost::get<FMODError>( mAdditional );
        stream << "\r\nFMOD error code " << std::hex << error.code << ":\r\n" << error.description;
      }
      mFullDescription = stream.str();
    }
    return mFullDescription;
  }

  const char* Exception::what() const
  {
    return getFullDescription().c_str();
  }

}