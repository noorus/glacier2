#include "StdAfx.h"
#include "Graphics.h"
#include "Console.h"
#include "Exception.h"
#include "Engine.h"
#include "MeshHelpers.h"

namespace MeshHelpers {

  // From http://www.ogre3d.org/tikiwiki/RetrieveVertexData
  // This code has been extracted from the OgreODE project by monster.
  // Note that this code assumes sizeof(long) == sizeof(uint32_t), which is not true on AMD64 Linux.
  void getMeshInformation( const Ogre::MeshPtr mesh,
  size_t &vertex_count, Ogre::Vector3* &vertices,
  size_t &index_count, unsigned long* &indices,
  const Ogre::Vector3 &position, const Ogre::Quaternion &orient,
  const Ogre::Vector3 &scale )
  {
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i )
    {
      Ogre::SubMesh* submesh = mesh->getSubMesh( i );
      // We only need to add the shared vertices once
      if ( submesh->useSharedVertices )
      {
        if ( !added_shared )
        {
          vertex_count += mesh->sharedVertexData->vertexCount;
          added_shared = true;
        }
      }
      else
      {
        vertex_count += submesh->vertexData->vertexCount;
      }
      // Add the indices
      index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i )
    {
      Ogre::SubMesh* submesh = mesh->getSubMesh( i );

      Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

      if ( ( !submesh->useSharedVertices ) || ( submesh->useSharedVertices && !added_shared ) )
      {
        if ( submesh->useSharedVertices )
        {
          added_shared = true;
          shared_offset = current_offset;
        }

        const Ogre::VertexElement* posElem =
          vertex_data->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );

        Ogre::HardwareVertexBufferSharedPtr vbuf =
          vertex_data->vertexBufferBinding->getBuffer( posElem->getSource() );

        unsigned char* vertex =
          static_cast<unsigned char*>( vbuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );

        float* pReal;

        for ( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize() )
        {
          posElem->baseVertexPointerToElement( vertex, &pReal );
          Ogre::Vector3 pt( pReal[0], pReal[1], pReal[2] );
          vertices[current_offset + j] = ( orient * ( pt * scale ) ) + position;
        }

        vbuf->unlock();
        next_offset += vertex_data->vertexCount;
      }

      Ogre::IndexData* index_data = submesh->indexData;
      size_t numTris = index_data->indexCount / 3;
      Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

      bool use32bitindexes = ( ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT );

      unsigned long* pLong = static_cast<unsigned long*>( ibuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
      unsigned short* pShort = reinterpret_cast<unsigned short*>( pLong );

      size_t offset = ( submesh->useSharedVertices ) ? shared_offset : current_offset;

      if ( use32bitindexes )
      {
        for ( size_t k = 0; k < numTris * 3; ++k )
        {
          indices[index_offset++] = pLong[k] + static_cast<unsigned long>( offset );
        }
      }
      else
      {
        for ( size_t k = 0; k < numTris * 3; ++k )
        {
          indices[index_offset++] = static_cast<unsigned long>( pShort[k] ) +
            static_cast<unsigned long>( offset );
        }
      }

      ibuf->unlock();
      current_offset = next_offset;
    }
  };

  // From http://www.ogre3d.org/tikiwiki/RetrieveVertexData
  // This code has been extracted from the OgreODE project by monster.
  // Note that this code assumes sizeof(long) == sizeof(uint32_t), which is not true on AMD64 Linux.
  void getManualMeshInformation( const Ogre::ManualObject *manual,
  size_t &vertex_count, Ogre::Vector3* &vertices,
  size_t &index_count, unsigned long* &indices,
  const Ogre::Vector3 &position, const Ogre::Quaternion &orient,
  const Ogre::Vector3 &scale )
  {
    std::vector<Ogre::Vector3> returnVertices;
    std::vector<unsigned long> returnIndices;
    unsigned long thisSectionStart = 0;
    for ( size_t i = 0; i < manual->getNumSections(); i++ )
    {
      Ogre::ManualObject::ManualObjectSection* section = manual->getSection( (unsigned int)i );
      Ogre::RenderOperation* renderOp = section->getRenderOperation();

      std::vector<Ogre::Vector3> pushVertices;
      // Collect the vertices
      {
        const Ogre::VertexElement * vertexElement = renderOp->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        Ogre::HardwareVertexBufferSharedPtr vertexBuffer = renderOp->vertexData->vertexBufferBinding->getBuffer( vertexElement->getSource() );

        char* verticesBuffer = (char*)vertexBuffer->lock( Ogre::HardwareBuffer::HBL_READ_ONLY );
        float* positionArrayHolder;

        thisSectionStart = (unsigned long)pushVertices.size();

        pushVertices.reserve( renderOp->vertexData->vertexCount );

        for ( unsigned int j = 0; j < renderOp->vertexData->vertexCount; j++ )
        {
          vertexElement->baseVertexPointerToElement( verticesBuffer + j * vertexBuffer->getVertexSize(), &positionArrayHolder );
          Ogre::Vector3 vertexPos = Ogre::Vector3( positionArrayHolder[0],
            positionArrayHolder[1],
            positionArrayHolder[2] );

          vertexPos = ( orient * ( vertexPos * scale ) ) + position;

          pushVertices.push_back( vertexPos );
        }

        vertexBuffer->unlock();
      }
      // Collect the indices
      {
        if ( renderOp->useIndexes )
        {
          Ogre::HardwareIndexBufferSharedPtr indexBuffer = renderOp->indexData->indexBuffer;

          if ( indexBuffer.isNull() || renderOp->operationType != Ogre::RenderOperation::OT_TRIANGLE_LIST )
            continue;

          returnVertices.reserve( returnVertices.size() + pushVertices.size() );
          returnVertices.insert( returnVertices.end(), pushVertices.begin(), pushVertices.end() );

          unsigned int * pLong = (unsigned int*)indexBuffer->lock( Ogre::HardwareBuffer::HBL_READ_ONLY );
          unsigned short * pShort = (unsigned short*)pLong;

          returnIndices.reserve( returnIndices.size() + renderOp->indexData->indexCount );

          for ( size_t j = 0; j < renderOp->indexData->indexCount; j++ )
          {
            unsigned long index;

            if ( indexBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT )
              index = (unsigned long)pLong[j] + thisSectionStart;
            else
              index = (unsigned long)pShort[j] + thisSectionStart;

            returnIndices.push_back( index );
          }

          indexBuffer->unlock();
        }
      }
    }

    vertex_count = returnVertices.size();
    vertices = new Ogre::Vector3[vertex_count];
    for ( unsigned long i = 0; i < vertex_count; i++ )
      vertices[i] = returnVertices[i];

    index_count = returnIndices.size();
    indices = new unsigned long[index_count];
    for ( unsigned long i = 0; i < index_count; i++ )
      indices[i] = returnIndices[i];
  }

}