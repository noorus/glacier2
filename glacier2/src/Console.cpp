#include "StdAfx.h"
#include "Console.h"
#include "Exception.h"
#include "Utilities.h"
#include "TextFile.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

#pragma warning( disable: 4073 )
#pragma init_seg( lib )

namespace Glacier {

  // Console constants ========================================================

  const long     cMaxConsoleLine    = 4096;
  const wchar_t* cConsoleEcho       = L"> %s";
  const wchar_t* cConsolePrint      = L"[%02d:%02d:%02d] [%s] %s\r\n";
  const wchar_t* cConsoleErrorPrint = L"[%02d:%02d:%02d] [%s] ERROR: %s\r\n";
  const wchar_t* cConsoleVarOut     = L"%s is \"%s\"";
  const wchar_t* cConsoleUnknown    = L"Unknown command \"%s\"";
  COLORREF       cConsoleErrorColor = RGB(255,0,0);

  // ConBase class ============================================================

  ConBase::ConBase( const wstring& name, const wstring& description ):
  mName( name ), mDescription( description ), mRegistered( false )
  {
    // Name and description are required
    assert( !mName.empty() && !mDescription.empty() );

    auto it = std::find( Console::mPrecreated.begin(), Console::mPrecreated.end(), this );
    if ( it != Console::mPrecreated.end() )
      ENGINE_EXCEPT( L"CVAR was declared more than once" );

    Console::mPrecreated.push_back( this );
  }

  const wstring& ConBase::getName()
  {
    return mName;
  }

  const wstring& ConBase::getDescription()
  {
    return mDescription;
  }

  const bool ConBase::isRegistered()
  {
    return mRegistered;
  }

  void ConBase::onRegister()
  {
    mRegistered = true;
  }

  // ConCmd class =============================================================

  ConCmd::ConCmd( const wstring& name, const wstring& description,
  ConCmd::Callback callback ):
  ConBase( name, description ), mCallback( callback )
  {
    // Callback is required for a command
    assert( mCallback );
  }

  void ConCmd::call( Console* console, StringVector& arguments )
  {
    mCallback( console, this, arguments );
  }

  bool ConCmd::isCommand()
  {
    return true;
  }

  // ConVar class =============================================================

  ConVar::ConVar( const wstring& name, const wstring& description,
  int defaultValue, ConVar::Callback callback ):
  ConBase( name, description ), mCallback( callback )
  {
    setValue( defaultValue );
    mDefaultValue = mValue;
  }

  ConVar::ConVar( const wstring& name, const wstring& description,
  float defaultValue, ConVar::Callback callback ):
  ConBase( name, description ), mCallback( callback )
  {
    setValue( defaultValue );
    mDefaultValue = mValue;
  }

  ConVar::ConVar( const wstring& name, const wstring& description,
  const wstring& defaultValue, ConVar::Callback callback ):
  ConBase( name, description ), mCallback( callback )
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

  float ConVar::getFloat()
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
    mValue.f = (float)value;
    wchar_t strtmp[32];
    swprintf_s( strtmp, 32, L"%i", value );
    mValue.str = strtmp;
    if ( !mRegistered || !mCallback || mCallback( this, oldValue ) )
      return;
    mValue = oldValue;
  }

  void ConVar::setValue( float value )
  {
    Value oldValue = mValue;
    mValue.i = (int)value;
    mValue.f = value;
    wchar_t strtmp[32];
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
    mValue.f = (float)_wtof( value.c_str() );
    mValue.str = value;
    if ( !mRegistered || !mCallback || mCallback( this, oldValue ) )
      return;
    mValue = oldValue;
  }

  void ConVar::forceValue( int value )
  {
    mValue.i = value;
    mValue.f = (float)value;
    wchar_t strtmp[32];
    swprintf_s( strtmp, 32, L"%i", value );
    mValue.str = strtmp;
  }

  void ConVar::forceValue( float value )
  {
    mValue.i = (int)value;
    mValue.f = value;
    wchar_t strtmp[32];
    swprintf_s( strtmp, 32, L"%f", value );
    mValue.str = strtmp;
  }

  void ConVar::forceValue( const wstring& value )
  {
    mValue.i = _wtoi( value.c_str() );
    mValue.f = (float)_wtof( value.c_str() );
    mValue.str = value;
  }

  // Console class ============================================================

  ConBaseList Console::mPrecreated;

  Console::Console( Engine* engine ): EngineComponent( engine ),
  mOutFile( nullptr ), mCmdList( nullptr ), mCmdHelp( nullptr ),
  mCmdFind( nullptr ), mCmdExec( nullptr )
  {
    InitializeSRWLock( &mLock );
    InitializeSRWLock( &mBufferLock );

    // Register console sources
    registerSource( L"engine", RGB(60,64,76) );
    registerSource( L"gfx", RGB(79,115,44) );
    registerSource( L"sound", RGB(181,80,10) );
    registerSource( L"physics", RGB(78,29,153) );
    registerSource( L"scripts", RGB(34,70,197) );
    registerSource( L"input", RGB(219,38,122) );
    registerSource( L"game", RGB(4,127,77) );

    // Create core commands
    mCmdList = new ConCmd( L"list", L"List all cvars.", callbackList );
    mCmdFind = new ConCmd( L"find", L"Perform a cvar lookup using regexp.", callbackFind );
    mCmdExec = new ConCmd( L"exec", L"Execute a configuration file.", callbackExec );
    mCmdHelp = new ConCmd( L"help", L"Get help on a variable/command.", callbackHelp );

    // Register everything in the precreated list
    for ( ConBase* var : mPrecreated )
      registerVariable( var );

    mPrecreated.clear();

    mCommands.sort( Console::cmpSortCmds );

    mOutFile = new TextFile( L"console.log" );
  }

  void Console::componentPreUpdate( GameTime time )
  {
    processBuffered();
  }

  Console::~Console()
  {
    SAFE_DELETE( mOutFile );
    SAFE_DELETE( mCmdList );
    SAFE_DELETE( mCmdFind );
    SAFE_DELETE( mCmdExec );
    SAFE_DELETE( mCmdHelp );
  }

  StringVector Console::tokenize( const wstring& str )
  {
    // OPTIMIZE:LOW This implementation is naïve, but hardly critical so far
    bool quoted = false;
    bool escaped = false;
    wstring buffer;
    StringVector v;
    for ( size_t i = 0; i < str.length(); i++ )
    {
      if ( str[i] == BACKSLASH ) {
        if ( escaped )
          buffer.append( str.substr( i, 1 ) );
        escaped = !escaped;
      } else if ( str[i] == SPACE ) {
        if ( !quoted ) {
          if ( !buffer.empty() ) {
            v.push_back( buffer );
            buffer.clear();
          }
        } else
          buffer.append( str.substr( i, 1 ) );
        escaped = false;
      } else if ( str[i] == QUOTE ) {
        if ( escaped ) {
          buffer.append( str.substr( i, 1 ) );
          escaped = false;
        } else {
          if ( quoted ) {
            if ( !buffer.empty() ) {
              v.push_back( buffer );
              buffer.clear();
            }
          }
          quoted = !quoted;
        }
      } else {
        buffer.append( str.substr( i, 1 ) );
        escaped = false;
      }
    }
    if ( !buffer.empty() )
      v.push_back( buffer );
    return v;
  }

  void Console::describe( ConBase* base )
  {
    printf( srcEngine,
      L"%s: (%s) - %s",
      base->getName().c_str(),
      base->isCommand() ? L"command" : L"variable",
      base->getDescription().c_str() );
  }

  void Console::callbackList( Console* console, ConCmd* command, StringVector& arguments )
  {
    for ( ConBase* base : console->mCommands )
      console->describe( base );
  }

  void Console::callbackFind( Console* console, ConCmd* command, StringVector& arguments )
  {
    if ( arguments.size() != 2 ) {
      console->printf( srcEngine, L"Format: %s <regexp>", arguments[0].c_str() );
      return;
    }

    const std::tr1::wregex pattern( arguments[1] );

    for ( ConBase* base : console->mCommands )
      if ( std::tr1::regex_match( base->getName(), pattern ) )
        console->describe( base );
  }

  void Console::callbackExec( Console* console, ConCmd* command, StringVector& arguments )
  {
    if ( arguments.size() != 2 ) {
      console->printf( srcEngine, L"Format: %s <filename>", arguments[0].c_str() );
      return;
    }

    console->executeFile( arguments[1] );
  }

  void Console::callbackHelp( Console* console, ConCmd* command, StringVector& arguments )
  {
    if ( arguments.size() != 2 ) {
      console->printf( srcEngine, L"Format: %s <variable/command>", arguments[0].c_str() );
      return;
    }

    for ( ConBase* base : console->mCommands )
      if ( boost::iequals( base->getName(), arguments[1] ) ) {
        console->describe( base );
        return;
      }

    console->errorPrintf( srcEngine, cConsoleUnknown, arguments[1].c_str() );
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

  bool Console::cmpSortCmds( ConBase* a, ConBase* b )
  {
    if ( _wcsicmp( a->getName().c_str(), b->getName().c_str() ) <= 0 )
      return true;
    return false;
  }

  void Console::registerVariable( ConBase* var )
  {
    ScopedSRWLock lock( &mLock );

    auto it = std::find( mCommands.begin(), mCommands.end(), var );
    if ( it != mCommands.end() || var->isRegistered() )
      ENGINE_EXCEPT( L"CVAR has already been registered" );

    var->onRegister();
    mCommands.push_back( var );
  }

  void Console::printf( Source source_, const wchar_t* line, ... )
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
    swprintf_s( out, cMaxConsoleLine, cConsolePrint,
      time.wHour, time.wMinute, time.wSecond, source.name.c_str(),
      buffer );

    mLines.push_back( out );

    for ( ConsoleListener* listener : mListeners )
      listener->onAddLine( source.color, out );

    if ( mOutFile )
      mOutFile->write( out );
  }

  void Console::addListener( ConsoleListener* listener )
  {
    ScopedSRWLock lock( &mLock );
    
    mListeners.push_back( listener ); // Not checking for dupes
  }

  void Console::removeListener( ConsoleListener* listener )
  {
    ScopedSRWLock lock( &mLock );

    mListeners.remove( listener );
  }

  void Console::errorPrintf( Source source_, const wchar_t* line, ... )
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
    swprintf_s( out, cMaxConsoleLine, cConsoleErrorPrint,
      time.wHour, time.wMinute, time.wSecond, source.name.c_str(),
      buffer );

    mLines.push_back( out );

    for ( ConsoleListener* listener : mListeners )
      listener->onAddLine( cConsoleErrorColor, out );

    if ( mOutFile )
      mOutFile->write( out );
  }

  void Console::autoComplete( const wstring& line, ConBaseList& matches )
  {
    // ScopedSRWLock lock( &mLock, false );

    matches.clear();
    wstring trimmed = boost::trim_copy( line );
    for ( ConBase* base : mCommands )
    {
      wstring comparison = base->getName().substr( 0, trimmed.length() );
      if ( !_wcsicmp( trimmed.c_str(), comparison.c_str() ) )
        matches.push_back( base );
    }
  }

  ConVar* Console::getVariable( const wstring& name )
  {
    for ( ConBase* base : mCommands )
    {
      if ( !base->isRegistered() )
        continue;

      if ( boost::iequals( base->getName(), name ) )
        return base->isCommand() ? nullptr : (ConVar*)base;
    }

    return nullptr;
  }

  ConCmd* Console::getCommand( const wstring& name )
  {
    for ( ConBase* base : mCommands )
    {
      if ( !base->isRegistered() )
        continue;

      if ( boost::iequals( base->getName(), name ) )
        return base->isCommand() ? (ConCmd*)base : nullptr;
    }

    return nullptr;
  }

  void Console::execute( wstring commandLine, const bool echo )
  {
    boost::trim( commandLine );
    if ( commandLine.empty() )
      return;

    StringVector arguments = tokenize( commandLine );
    if ( arguments.empty() )
      return;

    if ( echo )
      printf( srcEngine, cConsoleEcho, commandLine.c_str() );

    ScopedSRWLock lock( &mLock );

    wstring command = arguments[0];
    for ( ConBase* base : mCommands )
    {
      if ( !base->isRegistered() )
        continue;
      if ( boost::iequals( base->getName(), command ) )
      {
        if ( base->isCommand() )
        {
          ConCmd* cmd = static_cast<ConCmd*>( base );
          lock.unlock();
          cmd->call( this, arguments );
        }
        else
        {
          ConVar* var = static_cast<ConVar*>( base );
          if ( arguments.size() > 1 )
            var->setValue( arguments[1] );
          else
          {
            lock.unlock();
            printf( srcEngine, cConsoleVarOut,
              var->getName().c_str(),
              var->getString().c_str() );
          }
        }
        return;
      }
    }

    lock.unlock();

    printf( srcEngine, cConsoleUnknown, command.c_str() );
  }

  void Console::executeBuffered( const wstring& commandLine )
  {
    ScopedSRWLock lock( &mBufferLock );

    mCommandBuffer.push( commandLine );
  }

  void Console::processBuffered()
  {
    ScopedSRWLock lock( &mBufferLock );

    while ( !mCommandBuffer.empty() )
    {
      execute( mCommandBuffer.front() );
      mCommandBuffer.pop();
    }
  }

  void Console::executeFile( const wstring& filename )
  {
    try
    {
      DataStreamPtr stream = Ogre::Root::getSingleton().openFileStream(
        UTFString( filename ), "User" );

      printf( srcEngine, L"Executing %s", filename.c_str() );

      Ogre::String fullString = stream->getAsString();
      const char* content = fullString.c_str();

      stream->close();

      size_t i = 0;

      const BYTE utf8BOM[3] = { 0xEF, 0xBB, 0xBF };
      if ( !memcmp( content, utf8BOM, 3 ) )
        i = 3;

      std::string line;

      for ( i; i < fullString.length(); i++ )
      {
        if ( content[i] != NULL && content[i] != LF && content[i] != CR )
        {
          line.append( 1, content[i] );
        }
        else if ( content[i] == LF )
        {
          if ( !line.empty() )
            execute( Utilities::utf8ToWide( line ), false );

          line.clear();
        }
        else if ( content[i] == NULL )
        {
          break;
        }
      }

      if ( !line.empty() )
        execute( Utilities::utf8ToWide( line ), false );
    }
    catch ( Ogre::FileNotFoundException& e )
    {
      printf( srcEngine, L"Unable to execute %s, file not found",
        filename.c_str() );
    }
  }

}