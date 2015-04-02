#include "StdAfx.h"
#include "ModelViewerState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ModelViewer.h"
#include "ServiceLocator.h"
#include "Exception.h"
#include "Win32.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const string cModelViewerTitle( "glacier² » modelviewer" );

  ModelViewerState::ModelViewerState(): State( L"ModelViewer" ) {}

  void ModelViewerState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    Locator::getGraphics().setRenderWindowTitle( cModelViewerTitle );

//    mDirector = new Director( &Locator::getGraphics() );

    /*wstring fileName;
    if ( Win32::Win32::instance().fileOpenDialog(
    L"Ogre Mesh Files (*.mesh)", L"*.mesh", fileName ) )
    {
      mViewer = new ModelViewer( fileName );
    }*/

    mViewer = new ModelViewer( L"rotta_01.mesh" );
  }

  void ModelViewerState::pause( GameTime time )
  {
    State::pause( time );
  }

  void ModelViewerState::resume( GameTime time )
  {
    State::resume( time );
  }

  void ModelViewerState::update( GameTime tick, GameTime time )
  {
    if ( mViewer )
      mViewer->update();

//     mDirector->getCamera()->applyMovement(
//       gEngine->getActionManager()->getCameraController()->getMovement()
//     );
//
//     mDirector->getCamera()->update( (float)tick );
  }

  void ModelViewerState::draw( GameTime delta, GameTime time )
  {
    //
  }

  void ModelViewerState::shutdown( GameTime time )
  {
    SAFE_DELETE( mViewer );
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}