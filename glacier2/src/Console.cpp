#include "StdAfx.h"
#include "Console.h"

namespace Glacier {

  ConCmdBase::ConCmdBase( const wstring& name, const wstring& description ):
  mName( name ), mDescription( description ), mRegistered( false )
  {
    // Name and description are required
    assert( !mName.empty() && !mDescription.empty() );
  }

  const wstring& ConCmdBase::getName()
  {
    return mName;
  }

  const wstring& ConCmdBase::getDescription()
  {
    return mDescription;
  }

  const bool ConCmdBase::isRegistered()
  {
    return mRegistered;
  }

  ConCmd::ConCmd( const wstring& name, const wstring& description,
  ConCmd::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    // Callback is required for a command
    assert( mCallback );
  }

  bool ConCmd::isCommand()
  {
    return true;
  }

  ConVar::ConVar( const wstring& name, const wstring& description,
  int defaultValue, ConVar::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    setValue( defaultValue );
    mDefaultValue = mValue;
  }

  ConVar::ConVar( const wstring& name, const wstring& description,
  double defaultValue, ConVar::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    setValue( defaultValue );
    mDefaultValue = mValue;
  }

  ConVar::ConVar( const wstring& name, const wstring& description,
  const wstring& defaultValue, ConVar::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    setValue( defaultValue );
    mDefaultValue = mValue;
  }

  bool ConVar::isCommand()
  {
    return false;
  }

  int ConVar::getInt()
  {
    return mValue.i;
  }

  double ConVar::getFloat()
  {
    return mValue.f;
  }

  const wstring& ConVar::getString()
  {
    return mValue.str;
  }

  void ConVar::setValue( int value )
  {
    Value oldValue = mValue;
    mValue.i = value;
    mValue.f = (double)value;
    wchar_t strtmp[32];
    swprintf_s( strtmp, 32, L"%i", value );
    mValue.str = strtmp;
    if ( !mRegistered || !mCallback || mCallback( this, oldValue ) )
      return;
    mValue = oldValue;
  }

  void ConVar::setValue( double value )
  {
    Value oldValue = mValue;
    mValue.i = (int)value;
    mValue.f = value;
    wchar_t strtmp[32];
    // TODO: not sure if this is correct
    swprintf_s( strtmp, 32, L"%f", value );
    mValue.str = strtmp;
    if ( !mRegistered || !mCallback || mCallback( this, oldValue ) )
      return;
    mValue = oldValue;
  }

  void ConVar::setValue( const wstring& value )
  {
    Value oldValue = mValue;
    mValue.i = _wtoi( value.c_str() );
    mValue.f = _wtof( value.c_str() );
    mValue.str = value;
    if ( !mRegistered || !mCallback || mCallback( this, oldValue ) )
      return;
    mValue = oldValue;
  }

  Console::Console()
  {
    //
  }
}