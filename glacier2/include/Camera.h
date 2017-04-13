#pragma once
#include "StdAfx.h"
#include "Types.h"
#include "Console.h"
#include "Director.h"
#include "GlacierCustomPass.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Gorilla {
  class Screen;
}

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  class CameraModifier {
  friend class Camera;
  public:
    virtual void updatePosition( Camera* pCamera, const Vector3& vecTarget,
      Vector3& vecOffset, Vector3& vecDest ) = 0;
  };

  class Camera: boost::noncopyable {
  protected:
    SceneManager* mScene;
    Ogre::Camera* mCamera;
    SceneNode* mNode;
    CameraModifier* mModifier;
    GlacierCustomPassInformation* mCustomPass;
  public:
    explicit Camera( SceneManager* scene, const Ogre::String& name,
      const Vector3& position, const Radian& fovy, Gorilla::Screen* hud = nullptr );
    virtual void lookAt( const Vector3& position );
    virtual Ogre::Camera* getCamera() { return mCamera; }
    virtual SceneNode* getNode() { return mNode; }
    virtual const Radian& getFOVy();
    virtual void setFOVy( const Radian& fovy );
    virtual const Quaternion getOrientation() {
      return mNode->_getDerivedOrientation(); }
    virtual const Vector3 getPosition() {
      return mNode->_getDerivedPosition(); }
    virtual void castViewportRay( const Vector2& position, Ray& ray );
    virtual CameraModifier* getModifier() { return mModifier; }
    virtual void setModifier( CameraModifier* modifier ) { mModifier = modifier; }
    void resetModifier() { setModifier( nullptr ); }
    virtual ~Camera();
  };

  class GameCamera: public Camera {
  friend class CameraModifier;
  protected:
    SceneNode* anchor_;
    Vector3 direction_;
    Vector3 rotationInput_;
    Quaternion rotation_;
    Real zoomVelocity_;
    Real mSensitivity;
    Radian angle_;
    Real distance_;
    Real zoom_;
    Vector2 edgeScrollInput_;
    Vector2 roamInput_;
    Real window_;
    bool mReverseAxes;
  public:
    explicit GameCamera( SceneManager* scene, const Ogre::String& name, Gorilla::Screen* hud = nullptr );
    void applyRotation( const Vector3& movement );
    void applyEdgeScrolling( const Vector2& capped );
    void applyRoaming( const Vector2& input );
    virtual void update( const GameTime delta );
    virtual const Vector3& getDirection() const throw() { return direction_; }
    virtual Real getSensitivity() { return mSensitivity; }
    virtual void setSensitivity( const Real sensitivity );
    virtual bool hasAxisReversion() { return mReverseAxes; }
    virtual void setAxisReversion( const bool reverse );
    ~GameCamera();
  };

  //! @}

}