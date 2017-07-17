#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class Graphics;
  class GameCamera;
  class HDR;

  //! \class Environment
  class Environment: boost::noncopyable {
  protected:
    SceneManager* scene_;
    ColourValue upperHemisphere_;
    ColourValue lowerHemisphere_;
    Real ambientScale_;
    Ogre::Light* sunLight_;
    SceneNode* sunNode_;
    bool running_;
  public:
    Environment( Graphics* gfx );
    void setUpperHemisphere( const ColourValue& value );
    void setLowerHemisphere( const ColourValue& value );
    void setHemisphereScale( const Real scale );
    void update( const GameTime delta );
    void initialize();
    void shutdown();
    void reset();
    ~Environment();
  };

  //! @}

}