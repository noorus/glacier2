#pragma once
#include "StdAfx.h"
#include "Types.h"

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
    bool mReverseAxes;
    CameraModifier* mModifier;
  public:
    explicit Camera( PCZSceneManager* scene,
      const Ogre::String& name, const Vector3& position,
      const Radian& fovy, PCZone* homeZone, bool reverseAxes );
    virtual void lookAt( const Vector3& position );
    virtual PCZCamera* getCamera() { return mCamera; }
    virtual PCZSceneNode* getNode() { return mNode; }
    virtual const Radian& getFOVy();
    virtual void setFOVy( const Radian& fovy );
    virtual const Quaternion& getWorldOrientation() {
      return mNode->_getDerivedOrientation(); }
    virtual const Vector3& getWorldPosition() {
      return mNode->_getDerivedPosition(); }
    virtual bool hasAxisReversion() { return mReverseAxes; }
    virtual void setAxisReversion( const bool reverse );
    virtual void castViewportRay( const Vector2& position, Ray& ray );
    virtual CameraModifier* getModifier() { return mModifier; }
    virtual void setModifier( CameraModifier* modifier ) { mModifier = modifier; }
    void resetModifier() { setModifier( nullptr ); }
    ~Camera();
  };

  class ArcballCamera: public Camera {
  friend class CameraModifier;
  protected:
    const PCZSceneNode* pgeTarget;
    Vector3 mOffset;
    Vector3 mDirection;
    Vector3 mMovement;
    Quaternion mRotation;
    Real mZoomVelocity;
    Quaternion mClampRotation;
    Real mClampRotProgress;
    bool mClampRotating;
    Real mZoomAcceleration;
    Real mZoomDeceleration;
    Real mRotDeceleration;
    Real mSensitivity;
    Real mMinDistance;
    Real mMaxDistance;
    Radian mClampTop;
    Radian mClampBottom;
  public:
    explicit ArcballCamera( PCZSceneManager* scene,
      const Ogre::String& name, const PCZSceneNode* target,
      const Vector3& offset, const Radian& fovy, PCZone* homeZone,
      bool reverseAxes, Real sensitivity, Real minDistance,
      Real maxDistance, Real rotationDeceleration = 20.0f,
      Real zoomAcceleration = 150.0f, Real zoomDeceleration = 15.0f );
    virtual void applyMovement( const Vector3& movement );
    virtual void update( const float delta );
    virtual const Vector3& getDirection() const throw() { return mDirection; }
    virtual Real getSensitivity() { return mSensitivity; }
    virtual Real getMinDistance() { return mMinDistance; }
    virtual Real getMaxDistance() { return mMaxDistance; }
    virtual Radian getClampTop() { return mClampTop; }
    virtual Radian getClampBottom() { return mClampBottom; }
    virtual void setSensitivity( const Real sensitivity );
    virtual void setMinDistance( const Real minDistance );
    virtual void setMaxDistance( const Real maxDistance );
    virtual void setClampTop( const Radian clampTop );
    virtual void setClampBottom( const Radian clampBottom );
    ~ArcballCamera();
  };

  //! @}

}