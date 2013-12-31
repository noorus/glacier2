#include "StdAfx.h"
#include "Console.h"

namespace Glacier {

  ConCmdBase::ConCmdBase( const wstring& name, const wstring& description ):
  mName( name ), mDescription( description )
  {
    //
  }

  const wstring& ConCmdBase::getName()
  {
    return mName;
  }

  const wstring& ConCmdBase::getDescription()
  {
    return mDescription;
  }

  ConCmd::ConCmd( const wstring& name, const wstring& description, ConCmd::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    // Callback is required for a command
    assert( mCallback );
  }

  bool ConCmd::isCommand()
  {
    return true;
  }

  ConVar::ConVar( const wstring& name, const wstring& description, ConVar::Callback callback ):
  ConCmdBase( name, description ), mCallback( callback )
  {
    //
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

  Console::Console()
  {
    //
  }
}