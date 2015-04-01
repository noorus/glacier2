//
//  This source file is a part of HDRlib,
//  an addon for OGRE, the Object-oriented Graphics Rendering Engine.
//  Original author: Christian Luksch
//  Forked and modified in 2014 by noorus/sldt@github
//  Licensed under the GNU Lesser General Public License v3
//  See: https://github.com/sldt/hdrlib
//

#include "HDRCompositor.h"
#include "Ogre.h"

namespace HDRlib {

  using namespace Ogre;

  const String cCompositorName = "HDR";

  HDRCompositor::Glare::Glare():
  type( Glare_None ), strength( 1.0f ), passes( 2 )
  {
  }

  HDRCompositor::Star::Star():
  type( Star_None ), strength( 1.0f ), passes( 2 )
  {
  }

  HDRCompositor::HDRCompositor( RenderWindow* window, Camera* camera ):
  mWindow( window ), mCamera( camera ), mScales( 7 ),
  mTonemapper( Tonemapper_None ), mTechnique( nullptr ), mAutokeying( true ),
  mLumAdaption( true ), mKey( 0.2f ), mAdaptationScale( 1.0f ),
  mBrightPassOffset( 1.0f ), mBrightPassThreshold( 3.0f ),
  mE( 0.05f ), mPhi( 8.0f ), mEnabled( false )
  {
    mCompositor.setNull();
    mFrameTime = 1.0f / 60.0f;
    mViewWidth = (float)mWindow->getWidth();
    mViewHeight = (float)mWindow->getHeight();
  }

  HDRCompositor::~HDRCompositor()
  {
  }

  void HDRCompositor::release()
  {
    if ( !mCompositor.isNull() )
    {
      auto viewport = mWindow->getViewport( 0 );
      CompositorManager::getSingleton().removeCompositor(
        viewport, cCompositorName );
      mCompositor->removeAllTechniques();
      CompositorManager::getSingleton().remove( cCompositorName );
    }
  }

  void HDRCompositor::enable( const bool enable_ )
  {
    auto viewport = mWindow->getViewport( 0 );
    CompositorManager::getSingleton().setCompositorEnabled(
      viewport, cCompositorName, enable_ );
    mEnabled = enable_;
  }

  void HDRCompositor::notifyMaterialSetup( uint32 passID, MaterialPtr &material )
  {
    if ( passID == MID_DOWN || passID == MID_LUMSCALE2 )
    {
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "rcpScreenSize" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 0, 1.0f / mWindow->getWidth() );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 1, 1.0f / mWindow->getHeight() );
    }
    else if ( passID == MID_LUMSCALE4 )
    {
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "rcpScreenSize" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 0, 2.0f / mWindow->getWidth() );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 1, 2.0f / mWindow->getHeight() );
    }
    else if ( passID == MID_LUMSCALE8 )
    {
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "rcpScreenSize" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 0, 4.0f / mWindow->getWidth() );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition.physicalIndex + 1, 4.0f / mWindow->getHeight() );
    }
    else if ( passID == MID_GAUSSBLUR )
    {
      float gs[4*13];
      buildGaussGlare( gs, 2.5f, 1.0f, 4.0f );
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "gaussTable[0]" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstants( definition.physicalIndex, gs, 13 * 4 );
    }
    else if ( passID == MID_STARH )
    {
      float gs[4*13];
      buildGaussStarH( gs, 5.0f, 3.0f );
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "gaussTable[0]" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstants( definition.physicalIndex, gs, 13 * 4 );
    }
    else if ( passID == MID_STARV )
    {
      float gs[4*13];
      buildGaussStarV( gs, 5.0f, 3.0f );
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "gaussTable[0]" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstants( definition.physicalIndex, gs, 13 * 4 );
    }
    else if ( passID == MID_SCALE )
    {
      float gs[4*13];
      int scale = passID - MID_SCALE;
      int scaleSize = ( 1 << ( ( scale + 1 ) / 2 ) );
      float s = ( ( 1.0f / 2.0f / sqrt( 2.0f ) ) * (float)pow( 1.6, scale + 1 ) );
      buildGaussGlare( gs, s, 1.0f, (float)scaleSize );
      auto definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->getConstantDefinition( "gaussTable[0]" );
      material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstants( definition.physicalIndex, gs, 13 * 4 );
    }
  }

  void HDRCompositor::notifyMaterialRender( uint32 passID, MaterialPtr &material )
  {
    const GpuConstantDefinition* definition;
    switch ( passID )
    {
      case MID_KEY:
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "Key" );
        if ( definition )
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mKey );
      break;
      case MID_BRIGHT:
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "BRIGHT_PASS_THRESHOLD" );
        if ( definition )
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mBrightPassThreshold );
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "BRIGHT_PASS_OFFSET" );
        if ( definition )
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mBrightPassOffset );
      break;
      case MID_ADAPT:
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "dTime" );
        if ( definition )
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mFrameTime );
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "AdaptationScale" );
        if ( definition )
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mAdaptationScale );
      break;
      case MID_FINAL:
        if ( mTonemapper != Tonemapper_None && material->getTechnique(0)->getPass(0)->hasFragmentProgram() )					
        {
          definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "GlareStrength" );
          if ( definition )
          {
            if ( mGlare.type != Glare_None )
              material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mGlare.strength );
            else
              material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, 0.0f);
          }
          definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "StarStrength" );
          if ( definition )
          {
            if ( mStar.type != Star_None )
              material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mStar.strength );
            else
              material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, 0.0f );
          }
        }
      break;
      case MID_BUILDLOCAL:
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "e" );
        if ( definition )
        {
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mE );
        }
        definition = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition( "phi" );
        if ( definition )
        {
          material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_writeRawConstant( definition->physicalIndex, mPhi );
        }
      break;
    }
  }

  void HDRCompositor::notifyViewportSize( int width, int height )
  {
    mViewWidth = (float)width;
    mViewHeight = (float)height;
  }

  void HDRCompositor::create()
  {
    release();

    mCompositor = CompositorManager::getSingleton().create(
      cCompositorName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
    mTechnique = mCompositor->createTechnique();

    createTextureDefinition( "Scene", 0, 0, PF_FLOAT16_RGB );

    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_PREVIOUS );
    targetPass->setOutputName( "Scene" );

    renderDownsample();
    calculateLuminance();
    calculateKey();

    if ( mGlare.type > Glare_None || mStar.type > Star_None )
    {
      brightPass();
      if ( mGlare.type > Glare_None )
        buildGlare();
      if ( mStar.type > Star_None )
        buildStar();
    }

    if ( mTonemapper == Tonemapper_ReinhardLocal )
      buildScales();

    finalRendering();

    auto viewport = mWindow->getViewport( 0 );

    auto instance = CompositorManager::getSingleton().addCompositor( 
      viewport, cCompositorName );
    if ( instance )
      instance->addListener( this );

    notifyViewportSize( viewport->getActualWidth(),
      viewport->getActualHeight() );

    enable( mEnabled );
  }

  void HDRCompositor::createTextureDefinition( const String& name,
  const unsigned int width, const unsigned int height, const PixelFormat format )
  {
    auto textureDefinition = mTechnique->createTextureDefinition( name );

    textureDefinition->width = width;
    textureDefinition->height = height;
    textureDefinition->formatList.push_back( format );
  }

  void HDRCompositor::brightPass()
  {
    createTextureDefinition( "Bright",
      mWindow->getWidth() / 4,
      mWindow->getHeight() / 4,
      PF_A8R8G8B8 );

    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Bright" ); 

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "BrightPass" );
    pass->setInput( 0, "scenedown4" );
    pass->setInput( 1, "Luminance" );
    pass->setInput( 2, "Key" );

    pass->setIdentifier( MID_BRIGHT );
  }

  void HDRCompositor::renderDownsample()
  {
    createTextureDefinition(
      "scenedown4",
      mWindow->getWidth() / 4,
      mWindow->getHeight() / 4,
      PF_FLOAT16_RGB );

    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "scenedown4" ); 

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Down4" );
    pass->setInput( 0, "Scene" );

    pass->setIdentifier( MID_DOWN );
  }

  void HDRCompositor::calculateLuminance()
  {
    createTextureDefinition( "Lum64", 64, 64, PF_FLOAT16_R );
    createTextureDefinition( "Lum16", 16, 16, PF_FLOAT16_R );
    createTextureDefinition( "Lum4", 4, 4, PF_FLOAT16_R );
    createTextureDefinition( "Luminance", 1, 1, PF_FLOAT16_R );

    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lum64" );

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "LuminanceInitial" );
    pass->setInput( 0, "scenedown4" );

    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lum16" );

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Luminance1" );
    pass->setInput( 0, "Lum64" );

    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lum4" );

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Luminance2" );
    pass->setInput( 0, "Lum64" );

    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Luminance" );

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "LuminanceFinalAvg" );
    pass->setInput( 0, "Lum4" );

    if ( mLumAdaption )
    {
      createTextureDefinition( "AdaptedLumLast", 1, 1, PF_FLOAT16_R );

      targetPass = mTechnique->createTargetPass();
      targetPass->setInputMode( CompositionTargetPass::IM_NONE );
      targetPass->setOutputName( "AdaptedLumLast" );

      pass = targetPass->createPass();
      pass->setType( CompositionPass::PT_RENDERQUAD );
      pass->setMaterialName( "CopyHDR" );
      pass->setInput( 0, "AdaptedLum" );

      createTextureDefinition( "AdaptedLum", 1, 1, PF_FLOAT16_R );

      targetPass = mTechnique->createTargetPass();
      targetPass->setInputMode( CompositionTargetPass::IM_NONE );
      targetPass->setOutputName( "AdaptedLum" );

      pass = targetPass->createPass();
      pass->setType( CompositionPass::PT_RENDERQUAD );
      pass->setMaterialName( "AdaptLuminance" );
      pass->setInput( 0, "Luminance" );
      pass->setInput( 1, "AdaptedLumLast" );
      pass->setIdentifier( MID_ADAPT );
    }
  }

  void HDRCompositor::buildScales()
  {
    createTextureDefinition( "Lumscale1",
      mWindow->getWidth(), mWindow->getHeight(),
      PF_FLOAT16_RGB );
    createTextureDefinition( "Lumscale2",
      mWindow->getWidth() / 2, mWindow->getHeight() / 2,
      PF_FLOAT16_RGB );
    createTextureDefinition( "Lumscale4",
      mWindow->getWidth() / 4, mWindow->getHeight() / 4,
      PF_FLOAT16_RGB );
    createTextureDefinition( "Lumscale8",
      mWindow->getWidth() / 8, mWindow->getHeight() / 8,
      PF_FLOAT16_RGB );

    // Luminance scale 1
    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lumscale1" ); 

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "LuminanceScale" );
    pass->setInput( 0, "Scene" );
    if ( mLumAdaption )
      pass->setInput( 1, "AdaptedLum" );
    else
      pass->setInput( 1, "Luminance" );
    pass->setInput( 2, "Key" );

    pass->setIdentifier( MID_LUMSCALE1 );

    // Luminance scale 2
    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lumscale2" ); 

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Down2" );
    pass->setInput( 0, "Lumscale1" );

    pass->setIdentifier( MID_LUMSCALE2 );

    // Luminance scale 4
    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lumscale4" ); 

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Down2" );
    pass->setInput( 0, "Lumscale2" );

    pass->setIdentifier( MID_LUMSCALE4 );

    // Luminance scale 8
    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Lumscale8" ); 

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "Down2" );
    pass->setInput( 0, "Lumscale4" );

    pass->setIdentifier( MID_LUMSCALE8 );

    float s = 1.0f / 2.0f / sqrt( 2.0f ) ;

    for ( int i = 0; i < mScales; i++ )
    {
      String strScale = "scale";
      strScale += StringConverter::toString( i + 1 );

      int scaleSize = ( 1 << ( ( i + 1 ) / 2 ) );

      String strLumScale = "Lumscale";
      strLumScale += StringConverter::toString( scaleSize );

      createTextureDefinition( strScale,
        mWindow->getWidth() / scaleSize,
        mWindow->getHeight() / scaleSize,
        PF_FLOAT16_RGB );

      targetPass = mTechnique->createTargetPass();
      targetPass->setInputMode( CompositionTargetPass::IM_NONE );
      targetPass->setOutputName( strScale );

      pass = targetPass->createPass();
      pass->setType( CompositionPass::PT_RENDERQUAD );
      pass->setMaterialName( "GaussBlur" );
      pass->setInput( 0, strLumScale );
      pass->setIdentifier( MID_SCALE + i );
    }

    createTextureDefinition( "LocalAdaptMap",
      mWindow->getWidth(), mWindow->getHeight(),
      PF_FLOAT16_RGB );

    targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "LocalAdaptMap" ); 

    pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setMaterialName( "BuildLocalAdaptMap" );
    pass->setInput( 0, "Key" );
    pass->setInput( 1, "scale1" );
    pass->setInput( 2, "scale2" );
    pass->setInput( 3, "scale3" );
    pass->setInput( 4, "scale4" );
    pass->setInput( 5, "scale5" );
    pass->setInput( 6, "scale6" );
    pass->setInput( 7, "scale7" );

    pass->setIdentifier( MID_BUILDLOCAL );
  }

  void HDRCompositor::buildGlare()
  {
    if ( mGlare.type == Glare_Blur )
    {
      String strSrc = "Bright";
      for ( int i = 0; i < mGlare.passes; i++ )
      {
        String strDst = "Glare";

        if ( i < mGlare.passes - 1 )
          strDst += StringConverter::toString( i );

        createTextureDefinition( strDst,
          mWindow->getWidth() / 4,
          mWindow->getHeight() / 4,
          PF_A8R8G8B8 );

        auto targetPass = mTechnique->createTargetPass();
        targetPass->setInputMode( CompositionTargetPass::IM_NONE );
        targetPass->setOutputName( strDst );

        auto pass = targetPass->createPass();
        pass->setType( CompositionPass::PT_RENDERQUAD );
        pass->setMaterialName( "GaussBlur" );
        pass->setInput( 0, strSrc );
        pass->setIdentifier( MID_GAUSSBLUR );

        strSrc = strDst;
      }
    }
  }

  void HDRCompositor::calculateKey()
  {
    createTextureDefinition( "Key", 1, 1, PF_FLOAT16_RGB );

    auto targetPass = mTechnique->createTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );
    targetPass->setOutputName( "Key" );

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    if ( mAutokeying )
    {
      pass->setMaterialName( "AutoKey" );
      pass->setInput( 0, "Luminance" );
    }
    else
    {
      pass->setIdentifier( MID_KEY );
      pass->setMaterialName( "ManualKey" );
    }
  }

  void HDRCompositor::finalRendering()
  {
    auto targetPass = mTechnique->getOutputTargetPass();
    targetPass->setInputMode( CompositionTargetPass::IM_NONE );

    auto pass = targetPass->createPass();
    pass->setType( CompositionPass::PT_RENDERQUAD );
    pass->setInput( 0, "Scene" );

    switch ( mTonemapper )
    {
      case Tonemapper_None: pass->setMaterialName( "CompositorCopyback" ); break;
      case Tonemapper_Linear: pass->setMaterialName( "HDRFinalLinear" ); break;
      case Tonemapper_Reinhard: pass->setMaterialName( "HDRFinalReinh" ); break;
      case Tonemapper_ReinhardMod: pass->setMaterialName( "HDRFinalReinhMod" ); break;
      case Tonemapper_Log: pass->setMaterialName( "HDRFinalLog" ); break;
      case Tonemapper_AdaptLog: pass->setMaterialName( "HDRFinalAdaptiveLog" ); break;
      case Tonemapper_ReinhardLocal: pass->setMaterialName( "HDRFinalReinhLocal" ); break;
    }
 
    if ( mTonemapper != Tonemapper_None )
    {
      if ( mTonemapper == Tonemapper_ReinhardLocal )
      {
        pass->setInput( 0, "Lumscale1" );
        pass->setInput( 1, "Key" );
        if ( mGlare.type != Glare_None )
          pass->setInput( 2, "Glare" );
        if ( mStar.type != Star_None )
          pass->setInput( 3, "Star" );
        pass->setInput( 4, "LocalAdaptMap" );
      }
      else
      {
        if ( mLumAdaption )
          pass->setInput( 1, "AdaptedLum" );
        else
          pass->setInput( 1, "Luminance" );
        pass->setInput( 2, "Key" );
      }
    }

    if ( mTonemapper != Tonemapper_ReinhardLocal )
    {
      if ( mGlare.type != Glare_None )
        pass->setInput( 3, "Glare" );
      if ( mStar.type != Star_None )
        pass->setInput( 4, "Star" );
    }

    pass->setIdentifier( MID_FINAL );
  }

  void HDRCompositor::buildGaussGlare(
  float* out, float rho, float strength, float scale )
  {
    float tu = scale / mViewWidth;
    float tv = scale / mViewHeight;

    float totalWeight = 0.0f;
    int index = 0;

    double prae = 1 / ( 2 * 3.14159265 * ( rho * rho ) );

    for ( int x = -2; x <= 2; x++ )
    {
      for ( int y = -2; y <= 2; y++ )
      {
        // Exclude pixels with a block distance greater than 2. This will
        // create a kernel which approximates a 5x5 kernel using only 13
        // sample points instead of 25; this is necessary since 2.0 shaders
        // only support 16 texture grabs.
        if ( abs( x ) + abs( y ) > 2 )
          continue;

        // Get the unscaled Gaussian intensity for this offset
        out[index*4+0] = x * tu;
        out[index*4+1] = y * tv;
        out[index*4+2] = (float)( prae * pow( 2.7182818028, -0.5 * ( ( x * x * scale * scale + y * y * scale * scale ) / ( rho * rho ) ) ) );
        totalWeight += out[index * 4 + 2];
        out[index*4+3] = 0.f;

        index++;
      }
    }

    // Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    for ( int i = 0; i < index; i++ )
      out[i*4+2] /= totalWeight / strength;
  }

  void HDRCompositor::buildStar()
  {
    if ( mStar.type == Star_Plus )
    {
      String strDstH, strDstV;
      String strSrcH = "Bright";
      String strSrcV = "Bright";

      for ( int i = 0; i < mStar.passes; i++ )
      {
        String strDstH = "StarH";
        String strDstV = "StarV";

        if ( i < mStar.passes - 1 )
        {
          strDstH += StringConverter::toString( i );
          strDstV += StringConverter::toString( i );
        }

        createTextureDefinition( strDstH,
          mWindow->getWidth() / 4,
          mWindow->getHeight() / 4,
          PF_A8R8G8B8 );

        auto targetPass = mTechnique->createTargetPass();
        targetPass->setInputMode( CompositionTargetPass::IM_NONE );
        targetPass->setOutputName( strDstH );

        auto pass = targetPass->createPass();
        pass->setType( CompositionPass::PT_RENDERQUAD );
        pass->setMaterialName( "GaussBlur" );
        pass->setInput( 0, strSrcH );
        pass->setIdentifier( MID_STARH );

        strSrcH = strDstH;

        createTextureDefinition( strDstV,
          mWindow->getWidth() / 4,
          mWindow->getHeight() / 4,
          PF_A8R8G8B8 );

        targetPass = mTechnique->createTargetPass();
        targetPass->setInputMode( CompositionTargetPass::IM_NONE );
        targetPass->setOutputName( strDstV );

        pass = targetPass->createPass();
        pass->setType( CompositionPass::PT_RENDERQUAD );
        pass->setMaterialName( "GaussBlur" );
        pass->setInput( 0, strSrcV );
        pass->setIdentifier( MID_STARV );

        strSrcV = strDstV;
      }

      createTextureDefinition( "Star",
        mWindow->getWidth() / 4,
        mWindow->getHeight() / 4,
        PF_A8R8G8B8 );

      //merge horizontal and vertical blur
      auto targetPass = mTechnique->createTargetPass();
      targetPass->setInputMode( CompositionTargetPass::IM_NONE );
      targetPass->setOutputName( "Star" );

      auto pass = targetPass->createPass();
      pass->setType( CompositionPass::PT_RENDERQUAD );
      pass->setMaterialName( "Merge2" );
      pass->setInput( 0, "StarH" );
      pass->setInput( 1, "StarV" );
    }
  }

  void HDRCompositor::buildGaussStarH( float* out, float rho, float strength )
  {
    float tu = 4.0f / mViewWidth;

    float totalWeight = 0.0f;
    int index = 0;

    double prae = 1 / ( 2 * 3.14159265 * ( rho * rho ) );

    for ( int x = -6; x <= 6; x++ )
    {
      out[index * 4 + 0] = x * tu;
      out[index * 4 + 1] = 0;
      out[index * 4 + 2] = (float)( prae * pow( 2.7182818028, -0.5 * ( ( x * x ) / ( rho * rho ) ) ) );
      totalWeight += out[index * 4 + 2];
      out[index * 4 + 3] = 0.f;

      index++;
    }

    for ( int i = 0; i < index; i++ )
      out[i * 4 + 2] /= totalWeight / strength;
  }

  void HDRCompositor::buildGaussStarV( float* out, float rho, float strength )
  {
    float tv = 4.0f / mViewHeight;

    float totalWeight = 0.0f;
    int index = 0;

    double prae = 1 / ( 2 * 3.14159265 * ( rho * rho ) );

    for ( int y = -6; y <= 6; y++ )
    {
      out[index * 4 + 0] = 0;
      out[index * 4 + 1] = y * tv;
      out[index * 4 + 2] = (float)( prae * pow( 2.7182818028, -0.5 * ( ( y * y ) / ( rho * rho ) ) ) );
      totalWeight += out[index * 4 + 2];
      out[index * 4 + 3] = 0.f;

      index++;
    }

    for ( int i = 0; i < index; i++ )
      out[i * 4 + 2] /= totalWeight / strength;
  }

}