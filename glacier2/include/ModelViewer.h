#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class ModelViewer {
  private:
    Ogre::Overlay* mOverlay;
    Ogre::OverlayElement* mNamesText;
    Ogre::OverlayElement* mValuesText;
    Ogre::Entity* mEntity;
    PCZSceneNode* mModelNode;
  public:
    ModelViewer( wstring meshName );
    void update();
    ~ModelViewer();
  };

}