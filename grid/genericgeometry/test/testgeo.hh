// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
namespace Dune
{

  template <int d1,int d2> class YaspGrid;
  template <int d1,int d2> class AlbertaGrid;
  template< int dim, int dimworld > class ALUSimplexGrid;
  template< int dim, int dimworld, ALU3dGridElementType elType > class ALU3dGrid;
  template <class ct,int d1,int d2,class CCOMM> class ParallelSimplexGrid;
  template <int dim> class UGGrid;



  // Topology
  // --------

  template <class Grid>
  struct Topology;

  template< int d >
  struct Topology< YaspGrid< d, d > >
  {
    enum { basicType = GeometryType :: cube };
    enum { dimension = d };
    enum {correctJacobian=false};

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef typename Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: template map< 1 >( duneFN );
    }
  };

  template <int d>
  struct Topology<UGGrid<d> >
  {
    enum { basicType = GeometryType :: cube };
    enum { dimension = d };
    enum {correctJacobian=false};

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef typename Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: template map< 1 >( duneFN );
    }
  };

  template <int d>
  struct Topology<AlbertaGrid<d,d> >
  {
    enum { basicType = GeometryType :: simplex };
    enum { dimension = d };
    enum {correctJacobian=false};

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef typename Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: template map< 1 >( duneFN );
    }
  };

  template<>
  struct Topology< ALU3dGrid< 3, 3, tetra > >
  {
    enum { basicType = GeometryType :: simplex };
    enum { dimension = 3 };
    enum {correctJacobian=false};

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: map< 1 >( duneFN );
    }
  };

  template<>
  struct Topology< ALUSimplexGrid< 3, 3 > >
  {
    enum { basicType = GeometryType :: simplex };
    enum { dimension = 3 };
    enum {correctJacobian=false};

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: map< 1 >( duneFN );
    }
  };

  template< class ct, int dim, int dimworld, class CComm >
  struct Topology< ParallelSimplexGrid< ct, dim, dimworld, CComm > >
  {
    enum { basicType = GeometryType :: simplex };
    enum { dimension = dim };
    enum { correctJacobian = true };

    typedef GenericGeometry :: Convert< (GeometryType :: BasicType) basicType, dimension > Convert;
    typedef typename Convert :: type Type;
    static int faceNr( int duneFN )
    {
      return Convert :: template map< 1 >( duneFN );
    }
  };


  // GeometryTraits
  // --------------

  namespace GenericGeometry
  {

    template< class Grid >
    struct GeometryTraits
      : public DefaultGeometryTraits
        < typename Grid :: ctype, Grid :: dimension, Grid :: dimensionworld >
    {
      enum { hybrid = true };
      enum { duneType = Topology< Grid > :: basicType };
    };

  }

}
