#include "StdAfx.h"
#include "ModelViewer.h"
#include "ServiceLocator.h"
#include "Graphics.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  ModelViewer::ModelViewer( wstring meshName ):
  mEntity( nullptr ), mModelNode( nullptr ), mFloor( nullptr ),
  mFloorNode( nullptr )
  {
    size_t nameCut = meshName.find_last_of( L"/\\" );
    meshName = meshName.substr( nameCut + 1 );

    PCZSceneManager* scm = Locator::getGraphics().getScene();

    PCZSceneNode* root = (PCZSceneNode*)scm->getRootSceneNode();

    mFloorNode = (PCZSceneNode*)root->createChildSceneNode();
    scm->addPCZSceneNode( mFloorNode, scm->getDefaultZone() );

    mModelNode = (PCZSceneNode*)root->createChildSceneNode();
    scm->addPCZSceneNode( mModelNode, scm->getDefaultZone() );

    Ogre::PCZSceneNode* skynode = (PCZSceneNode*)root->createChildSceneNode( "sky" );
    scm->addPCZSceneNode( skynode, scm->getDefaultZone() );
    Ogre::Entity* sky = scm->createEntity( "sky", "sphere.mesh" );
    sky->setMaterialName( "HDRSky" );
    sky->setCastShadows( false );
    skynode->attachObject( sky );
    skynode->setPosition( 0.0f, 0.0f, 0.0f );
    skynode->setOrientation( Quaternion::IDENTITY );
    skynode->scale( Vector3( 10.0f, 10.0f, 10.0f ) );

    Ogre::Plane floorPlane( Vector3::UNIT_Y, 0.0f );

    Ogre::MeshManager::getSingleton().createPlane( "ModelViewer/FloorMesh",
      ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, floorPlane,
      1.0f, 1.0f, 8, 8, true, 1, 1, 1, Vector3::UNIT_Z );

    mFloor = scm->createEntity( "ModelViewer/Floor", "ModelViewer/FloorMesh" );
    mFloor->setMaterialName( "ModelViewer/Floor" );
    mFloor->setCastShadows( false );
    mFloorNode->attachObject( mFloor );
    mFloorNode->setPosition( 0.0f, 0.0f, 0.0f );
    mFloorNode->setOrientation( Quaternion::IDENTITY );

    mEntity = scm->createEntity( Ogre::UTFString( meshName ) );
    mEntity->setVisible( true );
    mEntity->setCastShadows( true );
    mModelNode->attachObject( mEntity );
    mModelNode->setPosition( 0.0f, 0.0f, 0.0f );
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
    mNamesText->setCaption( "LoD:\r\nAnims:\r\nSubEnts:" );

    static wchar_t values[64];
    swprintf_s( values, 64,
      L"%d/%d\r\n%d\r\n%d",
      mEntity->getCurrentLodIndex(),
      mEntity->getNumManualLodLevels(),
      mEntity->getMesh()->getNumAnimations(),
      mEntity->getNumSubEntities() );

    mValuesText->setCaption( Ogre::UTFString( values ) );
  }

  ModelViewer::~ModelViewer()
  {
    mOverlay->hide();

    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );
    if ( mModelNode )
      Locator::getGraphics().getScene()->destroySceneNode( mModelNode );
    if ( mFloor )
      Locator::getGraphics().getScene()->destroyEntity( mFloor );
    if ( mFloorNode )
      Locator::getGraphics().getScene()->destroySceneNode( mFloorNode );
  }

}