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
  void getMeshInformation( const Ogre::v1::MeshPtr mesh,
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
      auto submesh = mesh->getSubMesh( i );
      // We only need to add the shared vertices once
      if ( submesh->useSharedVertices )
      {
        if ( !added_shared )
        {
          vertex_count += mesh->sharedVertexData[Ogre::VpNormal]->vertexCount;
          added_shared = true;
        }
      }
      else
      {
        vertex_count += submesh->vertexData[Ogre::VpNormal]->vertexCount;
      }
      // Add the indices
      index_count += submesh->indexData[Ogre::VpNormal]->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i )
    {
      auto submesh = mesh->getSubMesh( i );

      auto vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData[Ogre::VpNormal] : submesh->vertexData[Ogre::VpNormal];

      if ( ( !submesh->useSharedVertices ) || ( submesh->useSharedVertices && !added_shared ) )
      {
        if ( submesh->useSharedVertices )
        {
          added_shared = true;
          shared_offset = current_offset;
        }

        const Ogre::v1::VertexElement* posElem =
          vertex_data->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );

        Ogre::v1::HardwareVertexBufferSharedPtr vbuf =
          vertex_data->vertexBufferBinding->getBuffer( posElem->getSource() );

        unsigned char* vertex =
          static_cast<unsigned char*>( vbuf->lock( Ogre::v1::HardwareBuffer::HBL_READ_ONLY ) );

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

      Ogre::v1::IndexData* index_data = submesh->indexData[0];
      size_t numTris = index_data->indexCount / 3;
      Ogre::v1::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

      bool use32bitindexes = ( ibuf->getType() == Ogre::v1::HardwareIndexBuffer::IT_32BIT );

      unsigned long* pLong = static_cast<unsigned long*>( ibuf->lock( Ogre::v1::HardwareBuffer::HBL_READ_ONLY ) );
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
  }

  /*
  Get the mesh information for the given mesh in v2 Ogre3D format. This is a really useful function that can be used by many
  different systems. e.g. physics mesh, navmesh, occlusion geometry etc...

  Original Code - Code found on this forum link: http://www.ogre3d.org/wiki/index.php/RetrieveVertexData
  Most Code courtesy of al2950( thanks m8 :)), but then edited by Jayce Young & Hannah Young at Aurasoft UK (Skyline Game Engine)
  to work with Items in the scene.
  */
  void getMesh2Information( const Ogre::MeshPtr mesh,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, Ogre::uint32* &indices,
    const Ogre::Vector3 &position, const Ogre::Quaternion &orient,
    const Ogre::Vector3 &scale )
  {
    //First, we compute the total number of vertices and indices and init the buffers.
    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    Ogre::Mesh::SubMeshVec::const_iterator subMeshIterator = mesh->getSubMeshes().begin();

    while ( subMeshIterator != mesh->getSubMeshes().end() )
    {
      Ogre::SubMesh *subMesh = *subMeshIterator;
      numVertices += subMesh->mVao[0][0]->getVertexBuffers()[0]->getNumElements();
      numIndices += subMesh->mVao[0][0]->getIndexBuffer()->getNumElements();

      subMeshIterator++;
    }

    vertices = new Ogre::Vector3[numVertices];
    indices = new Ogre::uint32[numIndices];

    vertex_count = numVertices;
    index_count = numIndices;

    unsigned int addedVertices = 0;
    unsigned int addedIndices = 0;

    unsigned int index_offset = 0;
    unsigned int subMeshOffset = 0;

    // Read Submeshes
    subMeshIterator = mesh->getSubMeshes().begin();
    while ( subMeshIterator != mesh->getSubMeshes().end() )
    {
      Ogre::SubMesh *subMesh = *subMeshIterator;
      Ogre::VertexArrayObjectArray vaos = subMesh->mVao[0];

      if ( !vaos.empty() )
      {
        //Get the first LOD level 
        Ogre::VertexArrayObject *vao = vaos[0];
        bool indices32 = ( vao->getIndexBuffer()->getIndexType() == Ogre::IndexBufferPacked::IT_32BIT );

        const Ogre::VertexBufferPackedVec &vertexBuffers = vao->getVertexBuffers();
        Ogre::IndexBufferPacked *indexBuffer = vao->getIndexBuffer();

        //request async read from buffer 
        Ogre::VertexArrayObject::ReadRequestsArray requests;
        requests.push_back( Ogre::VertexArrayObject::ReadRequests( Ogre::VES_POSITION ) );

        vao->readRequests( requests );
        vao->mapAsyncTickets( requests );
        unsigned int subMeshVerticiesNum = requests[0].vertexBuffer->getNumElements();
        if ( requests[0].type == Ogre::VET_HALF4 )
        {
          for ( size_t i = 0; i < subMeshVerticiesNum; ++i )
          {
            const Ogre::uint16* pos = reinterpret_cast<const Ogre::uint16*>( requests[0].data );
            Ogre::Vector3 vec;
            vec.x = Ogre::Bitwise::halfToFloat( pos[0] );
            vec.y = Ogre::Bitwise::halfToFloat( pos[1] );
            vec.z = Ogre::Bitwise::halfToFloat( pos[2] );
            requests[0].data += requests[0].vertexBuffer->getBytesPerElement();
            vertices[i + subMeshOffset] = ( orient * ( vec * scale ) ) + position;
          }
        }
        else if ( requests[0].type == Ogre::VET_FLOAT3 )
        {
          for ( size_t i = 0; i < subMeshVerticiesNum; ++i )
          {
            const float* pos = reinterpret_cast<const float*>( requests[0].data );
            Ogre::Vector3 vec;
            vec.x = *pos++;
            vec.y = *pos++;
            vec.z = *pos++;
            requests[0].data += requests[0].vertexBuffer->getBytesPerElement();
            vertices[i + subMeshOffset] = ( orient * ( vec * scale ) ) + position;
          }
        }
        else
        {
          ENGINE_EXCEPT( "Unknown vertex buffer format" );
        }
        subMeshOffset += subMeshVerticiesNum;
        vao->unmapAsyncTickets( requests );

        ////Read index data
        if ( indexBuffer )
        {
          Ogre::AsyncTicketPtr asyncTicket = indexBuffer->readRequest( 0, indexBuffer->getNumElements() );

          unsigned int *pIndices = 0;
          if ( indices32 )
          {
            pIndices = (unsigned*)( asyncTicket->map() );
          }
          else
          {
            unsigned short *pShortIndices = (unsigned short*)( asyncTicket->map() );
            pIndices = new unsigned int[indexBuffer->getNumElements()];
            for ( size_t k = 0; k < indexBuffer->getNumElements(); k++ ) pIndices[k] = static_cast<unsigned int>( pShortIndices[k] );
          }
          unsigned int bufferIndex = 0;

          for ( size_t i = addedIndices; i < addedIndices + indexBuffer->getNumElements(); i++ )
          {
            indices[i] = pIndices[bufferIndex] + index_offset;
            bufferIndex++;
          }
          addedIndices += indexBuffer->getNumElements();

          if ( !indices32 ) delete[] pIndices;

          asyncTicket->unmap();
        }
        index_offset += vertexBuffers[0]->getNumElements();
      }
      subMeshIterator++;
    }
  }

  /*void getMesh2Information( const Ogre::MeshPtr mesh,
    size_t &vertex_count, Ogre::Vector3* &vertices,
    size_t &index_count, unsigned long* &indices,
    const Ogre::Vector3 &position, const Ogre::Quaternion &orient,
    const Ogre::Vector3 &scale )
  {
    /*            Ogre::VertexBufferPacked *partialVertexBuffer = mPartialMesh->getSubMesh( 0 )->
                    mVao[Ogre::VpNormal][0]->getVertexBuffers()[0];
    for ( auto submesh : mesh->getSubMeshes() )
    {
      for ( auto vao : submesh->mVao[Ogre::VpNormal] )
      {
        for ( auto vec : vao->getVertexBuffers() )
        {
          auto elems = vec->getVertexElements();
        }
      }
    }
  }*/

  // From http://www.ogre3d.org/tikiwiki/RetrieveVertexData
  // This code has been extracted from the OgreODE project by monster.
  // Note that this code assumes sizeof(long) == sizeof(uint32_t), which is not true on AMD64 Linux.
  void getManualMeshInformation( const Ogre::v1::ManualObject *manual,
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
      Ogre::v1::ManualObject::ManualObjectSection* section = manual->getSection( (unsigned int)i );
      Ogre::v1::RenderOperation* renderOp = section->getRenderOperation();

      std::vector<Ogre::Vector3> pushVertices;
      // Collect the vertices
      {
        const Ogre::v1::VertexElement * vertexElement = renderOp->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        Ogre::v1::HardwareVertexBufferSharedPtr vertexBuffer = renderOp->vertexData->vertexBufferBinding->getBuffer( vertexElement->getSource() );

        char* verticesBuffer = (char*)vertexBuffer->lock( Ogre::v1::HardwareBuffer::HBL_READ_ONLY );
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
          Ogre::v1::HardwareIndexBufferSharedPtr indexBuffer = renderOp->indexData->indexBuffer;

          if ( indexBuffer.isNull() || renderOp->operationType != Ogre::OperationType::OT_TRIANGLE_LIST )
            continue;

          returnVertices.reserve( returnVertices.size() + pushVertices.size() );
          returnVertices.insert( returnVertices.end(), pushVertices.begin(), pushVertices.end() );

          unsigned int * pLong = (unsigned int*)indexBuffer->lock( Ogre::v1::HardwareBuffer::HBL_READ_ONLY );
          unsigned short * pShort = (unsigned short*)pLong;

          returnIndices.reserve( returnIndices.size() + renderOp->indexData->indexCount );

          for ( size_t j = 0; j < renderOp->indexData->indexCount; j++ )
          {
            unsigned long index;

            if ( indexBuffer->getType() == Ogre::v1::HardwareIndexBuffer::IT_32BIT )
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