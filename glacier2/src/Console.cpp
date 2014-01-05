#include "StdAfx.h"
#include "Console.h"
#include "Exception.h"
#include "Utilities.h"
#include "TextFile.h"

namespace Glacier {

  const unsigned long cMaxConsoleLine = 4096;

  // ConCmdBase ===============================================================

  ConCmdBase::ConCmdBase( const wstring& name, const wstring& description ):
  mName( name ), mDescription( description ), mRegistered( false )
  {
    // Name and description are required
    assert( !mName.empty() && !mDescription.empty() );

    auto it = std::find( Console::mPrecreated.begin(), Console::mPrecreated.end(), this );
    if ( it != Console::mPrecreated.end() )
      ENGINE_EXCEPT( L"CVAR was declared more than once" );

    Console::mPrecreated.push_back( this );
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

  void ConCmdBase::onRegister()
  {
    mRegistered = true;
  }

  // ConCmd ===================================================================

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

  // ConVar ===================================================================

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

  // Console ==================================================================

  Console::Console(): mOutFile( nullptr )
  {
    InitializeSRWLock( &mLock );

    registerSource( L"engine", RGB(60,64,76) );
    registerSource( L"gfx", RGB(79,115,44) );
    registerSource( L"sound", RGB(181,80,10) );
    registerSource( L"physics", RGB(78,29,153) );
    registerSource( L"scripts", RGB(34,70,197) );
    registerSource( L"input", RGB(219,38,122) );
    registerSource( L"game", RGB(4,127,77) );

    for ( ConCmdBase* var : mPrecreated )
      registerVariable( var );

    mPrecreated.clear();

    mCommands.sort( Console::cmpSortCmds );

    mOutFile = new TextFile( L"console.log" );
  }

  Console::~Console()
  {
    SAFE_DELETE( mOutFile );
  }

  Console::Source Console::registerSource( const wstring& name, COLORREF color )
  {
    ScopedSRWLock lock( &mLock );
    ConsoleSource tmp = { name, color };
    auto index = (Console::Source)mSources.size();
    mSources[index] = tmp;
    return index;
  }

  void Console::unregisterSource( Source source )
  {
    ScopedSRWLock lock( &mLock );
    mSources.erase( source );
  }

  bool Console::cmpSortCmds( ConCmdBase* x, ConCmdBase* y )
  {
    if ( _wcsicmp( x->getName().c_str(), y->getName().c_str() ) <= 0 )
      return true;
    return false;
  }

  void Console::registerVariable( ConCmdBase* var )
  {
    ScopedSRWLock lock( &mLock );

    auto it = std::find( mCommands.begin(), mCommands.end(), var );
    if ( it != mCommands.end() || var->isRegistered() )
      ENGINE_EXCEPT( L"CVAR has already been registered" );

    var->onRegister();
    mCommands.push_back( var );
  }

  void Console::printf( Source source_, LPCWSTR line, ... )
  {
    ScopedSRWLock lock( &mLock );

    va_list va_alist;
    WCHAR buffer[cMaxConsoleLine];
    va_start( va_alist, line );
    _vsnwprintf_s( buffer, cMaxConsoleLine, line, va_alist );
    va_end( va_alist );

    ConsoleSource source = mSources[source_];

    WCHAR out[cMaxConsoleLine];
    SYSTEMTIME time;
    GetLocalTime( &time );
    swprintf_s( out, cMaxConsoleLine, L"[%02d:%02d:%02d] [%s] %s\r\n",
      time.wHour, time.wMinute, time.wSecond, source.name.c_str(),
      buffer );

    mLines.push_back( out );

    if ( mOutFile )
      mOutFile->write( out );
  }

}