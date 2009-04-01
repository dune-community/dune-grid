// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTA_DGFPARSER_HH
#define DUNE_ALBERTA_DGFPARSER_HH

#include <dune/grid/albertagrid.hh>
#include <dune/grid/albertagrid/gridfactory.hh>

#include <dune/grid/io/file/dgfparser/dgfparser.hh>

#if HAVE_ALBERTA

namespace Dune
{

  // MacroGrid::Impl (for AlbertaGrid)
  // ---------------------------------

  template< int dim, int dimworld >
  class MacroGrid::Impl< AlbertaGrid< dim, dimworld > >
  {
    typedef MPIHelper::MPICommunicator MPICommunicatorType;

    typedef AlbertaGrid< dim, dimworld > Grid;
    typedef Dune::GridFactory< Grid > GridFactory;

    static const int dimension = Grid::dimension;

  public:
    static Grid *
    generate( MacroGrid &macroGrid,
              const std::string &filename,
              MPICommunicatorType MPICOMM = MPIHelper::getCommunicator() );
  };


  template< int dim, int dimworld >
  inline typename MacroGrid::Impl< AlbertaGrid< dim, dimworld > >::Grid *
  MacroGrid::Impl< AlbertaGrid< dim, dimworld > >
  ::generate( MacroGrid &macroGrid, const std::string &filename, MPICommunicatorType )
  {
    macroGrid.element = Simplex;
    macroGrid.dimgrid = dim;
    macroGrid.dimw = dimworld;

    std::ifstream file( filename.c_str() );
    if( !macroGrid.readDuneGrid( file, dim, dimworld ) )
      return new AlbertaGrid< dim, dimworld >( filename.c_str() );

    macroGrid.setOrientation( 0, 1 );
    macroGrid.setRefinement( 0, 1, -1, -1 );

    dgf::GridParameterBlock parameter( file );
    std::string gridName = parameter.name( "AlbertaGrid" );

    GridFactory factory;
    for( int n = 0; n < macroGrid.nofvtx; ++n )
    {
      typename GridFactory::WorldVector coord;
      for( int i = 0; i < dimworld; ++i )
        coord[ i ] = macroGrid.vtx[ n ][ i ];
      factory.insertVertex( coord );
    }

    GeometryType type( GeometryType::simplex, dimension );
    std::vector< unsigned int > elementId( dimension+1 );
    for( int n = 0; n < macroGrid.nofelements; ++n )
    {
      // This is a nasty hack: The tetrahedrons generated by make6 are not
      // directly useable by ALBERTA. On every second tetrahedron we have to
      // switch the last 2 vertices (otherwise ALBERTA causes a segmentation
      // fault during refinement).
      if( (dimension == 3) && macroGrid.cube2simplex && (n % 2 == 0) )
      {
        const int flip[ 4 ] = { 0, 1, 3, 2 };
        for( int i = 0; i <= dimension; ++i )
          elementId[ i ] = macroGrid.elements[ n ][ flip[ i ] ];
      }
      else
      {
        for( int i = 0; i <= dimension; ++i )
          elementId[ i ] = macroGrid.elements[ n ][ i ];
      }

      factory.insertElement( type, elementId );

      // look for bounaries and insert them
      for( int face = 0; face <= dimension; ++face )
      {
        typedef typename MacroGrid::facemap_t::key_type Key;
        typedef typename MacroGrid::facemap_t::iterator Iterator;

        const Key key( elementId, dimension, face+1 );
        const Iterator it = macroGrid.facemap.find( key );
        if( it != macroGrid.facemap.end() )
          factory.insertBoundary( n, face, it->second );
      }
    }

    if( GridFactory::supportPeriodicity )
    {
      typedef dgf::PeriodicFaceTransformationBlock::AffineTransformation Transformation;
      dgf::PeriodicFaceTransformationBlock block( file, dimworld );
      const int size = block.numTransformations();
      for( int k = 0; k < size; ++k )
      {
        const Transformation &trafo = block.transformation( k );

        typename GridFactory::WorldMatrix matrix;
        for( int i = 0; i < dimworld; ++i )
          for( int j = 0; j < dimworld; ++j )
            matrix[ i ][ j ] = trafo.matrix( i, j );

        typename GridFactory::WorldVector shift;
        for( int i = 0; i < dimworld; ++i )
          shift[ i ] = trafo.shift[ i ];

        factory.insertFaceTransformation( matrix, shift );
      }
    }

    return factory.createGrid( gridName, parameter.markLongestEdge() );
  }



  template< int dim, int dimworld >
  struct DGFGridInfo< AlbertaGrid< dim, dimworld > >
  {
    static int refineStepsForHalf ()
    {
      return dim;
    }

    static double refineWeight ()
    {
      return 0.5;
    }
  };

}

#endif // #if HAVE_ALBERTA

#endif
