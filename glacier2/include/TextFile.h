#pragma once

namespace Glacier {

  class TextFile: public boost::noncopyable {
  protected:
    HANDLE mFile;
  public:
    TextFile( const wstring& filename );
    void write( const wstring& str );
    ~TextFile();
  };

}