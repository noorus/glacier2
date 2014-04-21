#pragma once
#include "StdAfx.h"
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \class Camera
  //! Main camera class, PCZ-compatible and always bound to a SceneNode.
  class Camera: boost::noncopyable {
  protected:
    PCZone* mZone;            //!< My home zone
    PCZSceneManager* mScene;  //!< The scene
    PCZCamera* mCamera;       //!< My camera
    PCZSceneNode* mNode;      //!< My SceneNode
    bool mReverseAxes;        //!< true to reverse axes
  public:
    //! Constructor.
    //! \param [in,out] scene    The scene to add ourselves to.
    //! \param  name             The name.
    //! \param  position         The position.
    //! \param  fovy             The FOVy.
    //! \param [in,out] homeZone The camera's home zone.
    explicit Camera( PCZSceneManager* scene,
      const Ogre::String& name, const Vector3& position,
      const Radian& fovy, PCZone* homeZone );

    //! Look at.
    //! \param  position The position.
    virtual void lookAt( const Vector3& position );

    //! Gets the internal camera object.
    //! \return null if it fails, else the camera.
    virtual PCZCamera* getCamera() { return mCamera; }

    //! Gets the scene node.
    //! \return null if it fails, else the SceneNode.
    virtual PCZSceneNode* getNode() { return mNode; }
    virtual const Radian& getFOVy();
    virtual void setFOVy( const Radian& fovy );

    //! Gets my world orientation.
    //! \return Orientation.
    virtual const Quaternion& getWorldOrientation() {
      return mNode->_getDerivedOrientation(); }

    //! Gets my world position.
    //! \return Position.
    virtual const Vector3& getWorldPosition() {
      return mNode->_getDerivedPosition(); }

    //! Query if this object has axis reversion.
    //! \return true if axis reversion, false if not.
    virtual bool hasAxisReversion() { return mReverseAxes; }

    //! Sets axis reversion.
    //! \param  reverse Whether to reverse axes or not.
    virtual void setAxisReversion( const bool reverse );

    //! Cast viewport ray.
    //! \param  position    The position.
    //! \param [in,out] ray The ray.
    virtual void castViewportRay( const Vector2& position, Ray& ray );

    //! Destructor.
    ~Camera();
  };

  //! @}

}