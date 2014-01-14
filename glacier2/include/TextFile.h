#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class TextFile: boost::noncopyable {
  protected:
    HANDLE mFile;
  public:
    TextFile( const wstring& filename );
    void write( const wstring& str );
    ~TextFile();
  };

}