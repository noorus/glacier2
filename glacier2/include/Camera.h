#pragma once
#include "StdAfx.h"
#include "Types.h"
#include "Console.h"
#include "Director.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

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
  public:
    explicit Camera( SceneManager* scene, const Ogre::String& name,
      const Vector3& position, const Radian& fovy );
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
    SceneNode* mTarget;
    Vector3 mDirection;
    Vector3 mMovement;
    Quaternion mRotation;
    Real mZoomVelocity;
    Real mSensitivity;
    Radian mAngle;
    Real mDistance;
    Real mZoom;
    bool mReverseAxes;
  public:
    explicit GameCamera( SceneManager* scene,
      const Ogre::String& name, SceneNode* target );
    virtual void applyMovement( const Vector3& movement );
    virtual void update( const GameTime delta );
    virtual const Vector3& getDirection() const throw() { return mDirection; }
    virtual Real getSensitivity() { return mSensitivity; }
    virtual void setSensitivity( const Real sensitivity );
    virtual bool hasAxisReversion() { return mReverseAxes; }
    virtual void setAxisReversion( const bool reverse );
    ~GameCamera();
  };

  //! @}

}