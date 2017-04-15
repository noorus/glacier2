#include "StdAfx.h"
#include "ServiceLocator.h"
#include "Console.h"
#include "Utilities.h"
#include "Engine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const size_t cMaterialCountReserve = 128;

  ENGINE_DECLARE_CONCMD( mat_reload,
    L"Reload one or more materials by name.", Materials::callbackReload );

  Materials::Materials()
  {
    materials_.reserve( cMaterialCountReserve );
  }

  Materials::~Materials()
  {
    //
  }

  size_t Materials::add( const string& datablock, PxMaterial* physmat /*= nullptr */ )
  {
    auto resolved = Ogre::Root::getSingleton().getHlmsManager()->getDatablock( datablock );
    return add( resolved, physmat );
  }

  size_t Materials::add( HlmsDatablock* datablock, PxMaterial* physmat /*= nullptr */ )
  {
    materials_.emplace_back( ( datablock ? datablock : defaultBlock_ ), ( physmat ? physmat : defaultPhys_ ) );
    return ( materials_.size() - 1 );
  }

  void Materials::callbackReload( Console* console, ConCmd* command, StringVector& arguments )
  {
    if ( arguments.size() < 2 )
      return;

    vector<string> datablocks;
    for ( size_t i = 1; i < arguments.size(); i++ )
    {
      auto name = Utilities::wideToUtf8( arguments[i] );
      datablocks.push_back( name );
    }
    reloadHlmsResources( Ogre::Root::getSingletonPtr(), datablocks );
  }

  void Materials::reload( const size_t index )
  {
    auto mat = materials_[index];
    // mat.datablock_->
  }

  size_t failNo_ = 0;

  struct FailedReloadInfoMultiElem
  {
    FailedReloadInfoMultiElem( Ogre::Hlms* hlms, std::string const& resourceName, Ogre::String const& resourceGroup,
      Ogre::HlmsDatablock* placeholderDatablock, Ogre::String const& placeholderDatablockName )
      : hlms_( hlms )
      , resourceName_( resourceName )
      , resourceGroup_( resourceGroup )
      , placeholderDatablock_( placeholderDatablock )
      , placeholderDatablockName_( placeholderDatablockName )
      , datablockNew_( nullptr )
    {}

    Ogre::Hlms* hlms() const { return hlms_; }
    std::string const& resourceName() const { return resourceName_; }
    Ogre::String const& resourceGroup() const { return resourceGroup_; }
    Ogre::HlmsDatablock* placeholderDatablock() const { return placeholderDatablock_; }
    Ogre::String const& placeholderDatablockName() const { return placeholderDatablockName_; }

  public:
    Ogre::HlmsDatablock* datablockNew_; // 2do: hack

  private:
    Ogre::Hlms* hlms_;
    std::string resourceName_;
    Ogre::String resourceGroup_;
    Ogre::HlmsDatablock* placeholderDatablock_;
    Ogre::String placeholderDatablockName_;
  };
  struct FailedReloadInfoMulti
  {
    FailedReloadInfoMulti( Ogre::String const& filename, std::vector<FailedReloadInfoMultiElem> elems )
      : filename_( filename )
      , elems_( elems )
    {}

    Ogre::String const& filename() const { return filename_; }
    std::vector<FailedReloadInfoMultiElem>& elems() { return elems_; }

  private:
    Ogre::String filename_;
    std::vector<FailedReloadInfoMultiElem> elems_;
  };
  std::vector<FailedReloadInfoMulti> failedMultiReloadInfos_;

  struct DatablockReloadInfo
  {
    DatablockReloadInfo( std::string const& resourceName, Ogre::Hlms* hlms, Ogre::HlmsDatablock* datablock, Ogre::String filename, Ogre::String resourceGroup )
      : resourceName_( resourceName )
      , hlms_( hlms )
      , datablock_( datablock )
      , filename_( filename )
      , resourceGroup_( resourceGroup )
      , datablockNew_( nullptr )
    {}

    std::string const& resourceName() const { return resourceName_; }

    Ogre::Hlms* hlms() const { return hlms_; }
    Ogre::HlmsDatablock* datablock() const { return datablock_; }

    Ogre::String const& filename() const { return filename_; }
    Ogre::String const& resourceGroup() const { return resourceGroup_; }

    Ogre::vector<Ogre::Renderable*>::type& lrlist() { return lrlist_; }

  public:
    Ogre::HlmsDatablock* datablockNew_; // 2do: hack

  private:
    std::string resourceName_;

    Ogre::Hlms* hlms_;
    Ogre::HlmsDatablock* datablock_;

    Ogre::String filename_;
    Ogre::String resourceGroup_;

    Ogre::vector<Ogre::Renderable*>::type lrlist_;
  };

  void reloadHlmsResources( Ogre::Root* root, vector<string> resourceNames )
  {
    auto resourceNamesIt = resourceNames.begin();
    while ( resourceNamesIt != resourceNames.end() )
    {
      gEngine->getConsole()->printf( Console::srcGfx, L"Reloading HLMS resource %S", resourceNamesIt->c_str() );

      auto findFailIt = std::find_if( failedMultiReloadInfos_.begin(), failedMultiReloadInfos_.end(), [&]( FailedReloadInfoMulti& ri ) {
        return std::any_of( ri.elems().begin(), ri.elems().end(), [&]( FailedReloadInfoMultiElem const& elem ) {
          return elem.resourceName() == *resourceNamesIt;
        } );
      } );
      bool found = ( findFailIt != failedMultiReloadInfos_.end() );
      if ( found )
      {
        FailedReloadInfoMulti& frim = *findFailIt;

        for ( auto elemIt = frim.elems().begin(); elemIt != frim.elems().end(); ++elemIt )
        {
          auto findResourceNameIt = std::find( resourceNames.begin(), resourceNames.end(), elemIt->resourceName() );
          if ( findResourceNameIt != resourceNames.end() )
            resourceNames.erase( findResourceNameIt );
        }

        try
        {
          frim.elems().begin()->hlms()->getHlmsManager()->loadMaterials( frim.filename(), frim.elems().begin()->resourceGroup() );

          bool ok = true;
          for ( auto elemIt = frim.elems().begin(); elemIt != frim.elems().end(); ++elemIt )
          {
            try
            {
              elemIt->datablockNew_ = elemIt->hlms()->getDatablock( elemIt->resourceName() );
            }
            catch ( Ogre::InvalidParametersException const& )
            {
              ok = false;
            }
          }
          if ( ok )
          {
            for ( auto elemIt = frim.elems().begin(); elemIt != frim.elems().end(); ++elemIt )
            {
              Ogre::vector<Ogre::Renderable*>::type lrlist = elemIt->placeholderDatablock()->getLinkedRenderables();
              for ( auto it = lrlist.begin(); it != lrlist.end(); ++it )
                ( *it )->setDatablock( elemIt->datablockNew_ );
              elemIt->hlms()->destroyDatablock( elemIt->placeholderDatablockName() );
            }

            failedMultiReloadInfos_.erase( findFailIt );
          }
          else {}
        }
        catch ( Ogre::InvalidParametersException const& )
        {
        }

        resourceNamesIt = resourceNames.begin();
      }
      else
        ++resourceNamesIt;
    }

    const std::array<Ogre::HlmsTypes, 7> searchHlms = { Ogre::HLMS_PBS, Ogre::HLMS_TOON, Ogre::HLMS_UNLIT,
      Ogre::HLMS_USER0, Ogre::HLMS_USER1, Ogre::HLMS_USER2, Ogre::HLMS_USER3 };

    std::vector<DatablockReloadInfo> datablocksToReload;
    datablocksToReload.reserve( resourceNames.size() );
    for ( auto const& resourceName : resourceNames )
    {
      Ogre::Hlms* hlms = nullptr;
      Ogre::HlmsDatablock* datablockToReload = nullptr;
      for ( auto searchHlmsIt = searchHlms.begin(); searchHlmsIt != searchHlms.end() && datablockToReload == nullptr; ++searchHlmsIt )
      {
        hlms = root->getHlmsManager()->getHlms( *searchHlmsIt );
        if ( hlms )
          datablockToReload = hlms->getDatablock( resourceName );
      }
      if ( datablockToReload == nullptr || datablockToReload == hlms->getDefaultDatablock() )
        continue;

      const Ogre::String *filenameTmp, *resourceGroupTmp;
      datablockToReload->getFilenameAndResourceGroup( &filenameTmp, &resourceGroupTmp );
      if ( !filenameTmp || !resourceGroupTmp || filenameTmp->empty() || resourceGroupTmp->empty() )
        continue;

      datablocksToReload.push_back( DatablockReloadInfo( resourceName, hlms, datablockToReload, *filenameTmp, *resourceGroupTmp ) );
    }

    std::sort( datablocksToReload.begin(), datablocksToReload.end(), []( DatablockReloadInfo const& a, DatablockReloadInfo const& b ) {
      return a.filename() < b.filename();
    } );

    auto curGroupIt = datablocksToReload.begin();
    for ( auto it = curGroupIt; it != datablocksToReload.end(); ++it )
    {
      auto& datablockToReload = *it;

      datablockToReload.lrlist() = datablockToReload.datablock()->getLinkedRenderables();
      for ( auto it = datablockToReload.lrlist().begin(); it != datablockToReload.lrlist().end(); ++it )
        ( *it )->_setNullDatablock();

      datablockToReload.hlms()->destroyDatablock( datablockToReload.resourceName() );

      if ( it->filename() != curGroupIt->filename() || std::next( it ) == datablocksToReload.end() )
      {
        auto endIt = ( std::next( it ) == datablocksToReload.end() ) ? datablocksToReload.end() : it;

        bool loadOk = false;
        try
        {
          curGroupIt->hlms()->getHlmsManager()->loadMaterials( curGroupIt->filename(), curGroupIt->resourceGroup() );
          loadOk = true;
        }
        catch ( Ogre::InvalidParametersException const& )
        {
          std::vector<FailedReloadInfoMultiElem> elems;
          for ( auto reloadIt = curGroupIt; reloadIt != endIt; ++reloadIt )
          {
            Ogre::String datablockName = "FailPlaceholder" + Ogre::StringConverter::toString( ++failNo_ );
            Ogre::HlmsDatablock* datablockNew = static_cast<Ogre::HlmsPbsDatablock*>(
              reloadIt->hlms()->createDatablock( datablockName,
                datablockName,
                Ogre::HlmsMacroblock(),
                Ogre::HlmsBlendblock(),
                Ogre::HlmsParamVec() ) );

            elems.push_back( FailedReloadInfoMultiElem( reloadIt->hlms(), reloadIt->resourceName(), reloadIt->resourceGroup(),
              datablockNew, datablockName ) );

            for ( auto it = reloadIt->lrlist().begin(); it != reloadIt->lrlist().end(); ++it )
              ( *it )->setDatablock( datablockNew );
          }

          failedMultiReloadInfos_.push_back( FailedReloadInfoMulti( curGroupIt->filename(), elems ) );
        }

        if ( loadOk )
        {
          bool reloadOk = true;
          for ( auto reloadIt = curGroupIt; reloadIt != endIt; ++reloadIt )
          {
            try
            {
              reloadIt->datablockNew_ = curGroupIt->hlms()->getDatablock( reloadIt->resourceName() );
            }
            catch ( Ogre::InvalidParametersException const& )
            {
              reloadOk = false;
            }
          }

          if ( reloadOk )
            for ( auto reloadIt = curGroupIt; reloadIt != endIt; ++reloadIt )
            {
              for ( auto setIt = reloadIt->lrlist().begin(); setIt != reloadIt->lrlist().end(); ++setIt )
                ( *setIt )->setDatablock( reloadIt->datablockNew_ );
            }
        }
        else {}

        curGroupIt = it;
      }
      else {}
    }
  }

}