#pragma once

namespace Glacier {

  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;

  using std::vector;

  typedef std::string string;
  typedef std::wstring wstring;

  typedef boost::char_separator<wchar_t> CharSeparator;
  typedef boost::tokenizer<CharSeparator, wstring::const_iterator, std::wstring> StringTokenizer;
  typedef std::vector<uint8_t> ByteVector;
  typedef std::vector<std::wstring> StringVector;
  typedef std::vector<void*> VoidVector;
  typedef std::list<std::wstring> StringList;
  typedef std::queue<std::wstring> StringQueue;

}