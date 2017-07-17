#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class GameCamera;
  class HDR;

  struct Sun {
    Real power_;
    ColourValue diffuse_;
    ColourValue specular_;
    Vector3 direction_;
  };

  //! \class Environment
  class Environment: boost::noncopyable {
  protected:
    SceneManager* scene_;
    ColourValue upperHemisphere_;
    ColourValue lowerHemisphere_;
    Real ambientScale_;
    Ogre::Light* sunLight_;
    SceneNode* sunNode_;
    Sun sun_;
    bool running_;
  public:
    Environment( Graphics* gfx );
    void setUpperHemisphere( const ColourValue& value );
    void setLowerHemisphere( const ColourValue& value );
    void setHemisphereScale( const Real scale );
    void setSunPower( Real power );
    void setSunDiffuse( const ColourValue& color );
    void setSunSpecular( const ColourValue& color );
    void setSunDirection( const Vector3& dir );
    void setSun( Real power, const ColourValue& diffuse, const ColourValue& specular, const Vector3& direction );
    void update( const GameTime delta );
    void initialize();
    void shutdown();
    void reset();
    ~Environment();
  };

  //! @}

}