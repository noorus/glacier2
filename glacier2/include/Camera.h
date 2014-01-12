#pragma once
#include "Types.h"

namespace Glacier {

  class Camera: boost::noncopyable {
  protected:
    PCZone* mZone;
    PCZSceneManager* mScene;
    PCZCamera* mCamera;
    PCZSceneNode* mNode;
    bool mReverseAxes;
  public:
    explicit Camera( PCZSceneManager* scene,
      const Ogre::String& name, const Vector3& position,
      const Radian& fovy, PCZone* homeZone );
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
    ~Camera();
  };

}