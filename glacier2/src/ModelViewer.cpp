#include "StdAfx.h"
#include "ModelViewer.h"
#include "ServiceLocator.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ModelViewer::ModelViewer( wstring meshName ):
  mEntity( nullptr ), mModelNode( nullptr )
  {
    PCZSceneManager* scm = Locator::getGraphics().getScene();

    PCZSceneNode* root = (PCZSceneNode*)scm->getRootSceneNode();

    mModelNode = (PCZSceneNode*)root->createChildSceneNode();
    scm->addPCZSceneNode( mModelNode, scm->getDefaultZone() );

    size_t nameCut = meshName.find_last_of( L"/\\" );
    meshName = meshName.substr( nameCut + 1 );

    mEntity = scm->createEntity( Ogre::UTFString( meshName ) );
    mEntity->setVisible( true );
    mModelNode->attachObject( mEntity );
    mModelNode->setPosition( 0, 0, 0 );
    mModelNode->setOrientation( Quaternion::IDENTITY );

    mOverlay = Ogre::OverlayManager::getSingleton().getByName(
      "ModelViewer/Stats" );
    mNamesText = Ogre::OverlayManager::getSingleton().getOverlayElement(
      "ModelViewer/Stats/Names" );
    mValuesText = Ogre::OverlayManager::getSingleton().getOverlayElement(
      "ModelViewer/Stats/Values" );

    mOverlay->show();
  }

  void ModelViewer::update()
  {
    mNamesText->setCaption( "LoD Level:\r\n" );

    static wchar_t values[64];
    swprintf_s( values, 64,
      L"%d/%d\r\n",
      mEntity->getCurrentLodIndex(),
      mEntity->getNumManualLodLevels() );

    mValuesText->setCaption( Ogre::UTFString( values ) );
  }

  ModelViewer::~ModelViewer()
  {
    mOverlay->hide();

    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );
    if ( mModelNode )
      Locator::getGraphics().getScene()->destroySceneNode( mModelNode );
  }

}