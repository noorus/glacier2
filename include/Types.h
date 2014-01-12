#pragma once

namespace Glacier {

  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;

  using std::vector;

  typedef std::string string;
  typedef std::wstring wstring;
  typedef std::stringstream stringstream;
  typedef std::wstringstream wstringstream;

  typedef double GameTime;

  typedef boost::char_separator<wchar_t> CharSeparator;
  typedef boost::tokenizer<CharSeparator, wstring::const_iterator, wstring> StringTokenizer;
  typedef std::vector<uint8_t> ByteVector;
  typedef std::vector<wstring> StringVector;
  typedef std::vector<void*> VoidVector;
  typedef std::list<wstring> StringList;
  typedef std::queue<wstring> StringQueue;

  enum CharacterConstants
  {
    TAB = 9,
    LF = 10,
    CR = 13,
    SPACE = 32,
    QUOTE = 34,
    COLON = 58,
    SEMICOLON = 59,
    BACKSLASH = 92
  };

  using Ogre::PCZSceneManager;
  using Ogre::PCZone;
  using Ogre::PCZCamera;
  using Ogre::PCZSceneNode;
  using Ogre::ResourceManager;
  using Ogre::ResourceGroupManager;
  using Ogre::ResourceGroupListener;
  using Ogre::SceneManager;
  using Ogre::SceneNode;
  using Ogre::ColourValue;
  using Ogre::Font;
  using Ogre::Viewport;
  using Ogre::Quaternion;
  using Ogre::Vector2;
  using Ogre::Vector3;
  using Ogre::Vector4;
  using Ogre::Matrix3;
  using Ogre::Matrix4;
  using Ogre::Real;
  using Ogre::Radian;
  using Ogre::Degree;
  using Ogre::Ray;
  using Ogre::AxisAlignedBox;
  using Ogre::Sphere;

  using Ogre::Math;

}