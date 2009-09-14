// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTA_INTERSECTION_HH
#define DUNE_ALBERTA_INTERSECTION_HH

#include <dune/common/smallobject.hh>

#include <dune/grid/common/intersection.hh>

#include <dune/grid/albertagrid/transformation.hh>
#include <dune/grid/albertagrid/agmemory.hh>
#include <dune/grid/albertagrid/elementinfo.hh>
#include <dune/grid/albertagrid/geometry.hh>

#define ALBERTA_CACHED_LOCAL_INTERSECTION_GEOMETRIES 1

namespace Dune
{

  // External Forward Declarations
  // -----------------------------

  template< int codim, int dim, class GridImp >
  class AlbertaGridEntity;



  // AlbertaGridLeafIntersection
  // ---------------------------

  /*!
     Mesh entities of codimension 0 ("elements") allow to visit all neighbors, where
     a neighbor is an entity of codimension 0 which has a common entity of codimension 1
     These neighbors are accessed via a IntersectionIterator. This allows the implementation of
     non-matching meshes. The number of neigbors may be different from the number of faces
     of an element!
   */
  template< class GridImp >
  class AlbertaGridLeafIntersection
    : public SmallObject
  {
    typedef AlbertaGridLeafIntersection< GridImp > This;

    friend class AlbertaGridEntity< 0, GridImp::dimension, GridImp >;

  public:
    //! define type used for coordinates in grid module
    typedef typename GridImp::ctype ctype;

    //! know your own dimension
    static const int dimension = GridImp::dimension;
    //! know your own dimension of world
    static const int dimensionworld = GridImp::dimensionworld;

    //! return unit outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    typedef FieldVector< ctype, GridImp::dimensionworld > NormalVector;
    typedef FieldVector< ctype, GridImp::dimension-1 > LocalCoordType;

    typedef This ImplementationType;

    typedef typename GridImp::template Codim<0>::Entity Entity;
    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;

    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;

    typedef Alberta::ElementInfo< dimension > ElementInfo;

  private:
    typedef AlbertaGridEntity< 0, dimension, GridImp > EntityImp;
    typedef AlbertaGridGeometry< dimension-1, dimensionworld, GridImp > GeometryImp;
    typedef AlbertaGridGeometry< dimension-1, dimension, GridImp > LocalGeometryImp;

    struct GlobalCoordReader;
    struct LocalCoordReader;

  public:
    AlbertaGridLeafIntersection ( const EntityImp &entity, const int n );

    //! The copy constructor
    AlbertaGridLeafIntersection ( const This &other );

    This &operator= ( const This &other )
    {
      assign( other );
      return *this;
    }

    //! equality
    bool equals ( const This &other ) const;

    //! increment
    void next ();

    //! access neighbor
    EntityPointer outside () const;

    //! access element where IntersectionIterator started
    EntityPointer inside () const;

    //! assignment operator, implemented because default does not the right thing
    void assign ( const This &other );

    //! return true if intersection is with boundary.
    bool boundary () const;

    //! return true if across the edge an neighbor on this level exists
    bool neighbor () const;

    //! return information about the Boundary
    int boundaryId () const;

    //! return true if intersection is conform.
    bool conforming () const;

    AlbertaTransformation transformation () const;

    //! intersection of codimension 1 of this neighbor with element where
    //! iteration started.
    //! Here returned element is in LOCAL coordinates of the element
    //! where iteration started.
    const LocalGeometry &geometryInInside () const;

    /*! intersection of codimension 1 of this neighbor with element where iteration started.
       Here returned element is in LOCAL coordinates of neighbor
     */
    const LocalGeometry &geometryInOutside () const;

    /*! intersection of codimension 1 of this neighbor with element where iteration started.
       Here returned element is in GLOBAL coordinates of the element where iteration started.
     */
    const Geometry &geometry () const;

    GeometryType type () const;

    //! local index of codim 1 entity in self where intersection is contained in
    int indexInInside () const;

    //! local index of codim 1 entity in neighbor where intersection is contained in
    int indexInOutside () const;

    //! twist of the face seen from the inner element
    int twistInSelf () const;

    //! twist of the face seen from the outer element
    int twistInNeighbor () const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVector outerNormal ( const LocalCoordType &local ) const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVector integrationOuterNormal ( const LocalCoordType &local ) const;

    const NormalVector unitOuterNormal ( const LocalCoordType &local ) const;

  private:
    //! setup the virtual neighbor
    void setupVirtEn () const;

    ////////////////////////////////////////////////
    // private member variables
    ////////////////////////////////////////////////

    //! know the grid were im coming from
    const GridImp &grid_;

    //! count on which neighbor we are lookin' at
    mutable int neighborCount_;

    ElementInfo elementInfo_;

    // the objects holding the real implementations
#if not ALBERTA_CACHED_LOCAL_INTERSECTION_GEOMETRIES
    mutable MakeableInterfaceObject< LocalGeometry > fakeNeighObj_;
    mutable MakeableInterfaceObject< LocalGeometry > fakeSelfObj_;
#endif
    mutable MakeableInterfaceObject< Geometry > neighGlobObj_;

    //! ElementInfo to store the information of the neighbor if needed
    mutable ElementInfo neighborInfo_;
  };

}

#endif // #ifndef DUNE_ALBERTA_INTERSECTION_HH
