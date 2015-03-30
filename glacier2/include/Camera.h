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
    PCZone* mZone;
    PCZSceneManager* mScene;
    PCZCamera* mCamera;
    PCZSceneNode* mNode;
    CameraModifier* mModifier;
  public:
    explicit Camera( PCZSceneManager* scene, const Ogre::String& name,
      const Vector3& position, const Radian& fovy, PCZone* homeZone );
    virtual void lookAt( const Vector3& position );
    virtual PCZCamera* getCamera() { return mCamera; }
    virtual PCZSceneNode* getNode() { return mNode; }
    virtual const Radian& getFOVy();
    virtual void setFOVy( const Radian& fovy );
    virtual const Quaternion& getOrientation() {
      return mNode->_getDerivedOrientation(); }
    virtual const Vector3& getPosition() {
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
    const PCZSceneNode* mTarget;
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
    explicit GameCamera( PCZSceneManager* scene,
      const Ogre::String& name, const PCZSceneNode* target,
      PCZone* homeZone );
    virtual void applyMovement( const Vector3& movement );
    virtual void update( const GameTime delta );
    virtual const Vector3& getDirection() const throw() { return mDirection; }
    virtual Real getSensitivity() { return mSensitivity; }
    virtual void setSensitivity( const Real sensitivity );
    virtual bool hasAxisReversion() { return mReverseAxes; }
    virtual void setAxisReversion( const bool reverse );
    virtual void updateWindow();
    ~GameCamera();
  };

  //! @}

}