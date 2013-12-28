#pragma once

namespace Glacier {

  struct WinAPIError {
  public:
    DWORD code;
    wstring description;
  };

  class Exception: public std::exception {
  public:
    enum Type {
      Generic = 0,
      WinAPI,
      NT
    };
  private:
    Exception() {}
  protected:
    Type mType;
    wstring mDescription;
    wstring mSource;
    mutable wstring mFullDescription;
    boost::variant<WinAPIError> mAdditional;
    void handleAdditional();
  public:
    Exception( const wstring& description, Type type = Generic );
    Exception( const wstring& description, const wstring& source, Type type = Generic );
    Exception( const wstring& description, const wstring& source, NTSTATUS ntstatus, Type type = Generic );
    virtual const std::wstring& getFullDescription() const;
    virtual const char* what() const throw();
  };

#if defined(ENGINE_EXCEPT) || defined(ENGINE_EXCEPT_W32)
# error ENGINE_EXCEPT* macro already defined!
#else
# define ENGINE_EXCEPT(description) {throw Glacier::Exception(description,__FUNCTIONW__,Glacier::Exception::Generic);}
# define ENGINE_EXCEPT_W32(description) {throw Glacier::Exception(description,__FUNCTIONW__,Glacier::Exception::WinAPI);}
# define ENGINE_EXCEPT_NT(r,description) {throw Glacier::Exception(description,__FUNCTIONW__,r,Glacier::Exception::NT);}
// ENGINE_EXCEPT_FMOD
// ENGINE_EXCEPT_SEH
#endif

}