// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GEOGRID_INTERSECTION_HH
#define DUNE_GEOGRID_INTERSECTION_HH

#include <dune/grid/geometrygrid/declaration.hh>
#include <dune/grid/geometrygrid/entitypointer.hh>
#include <dune/grid/geometrygrid/cornerstorage.hh>

namespace Dune
{

  namespace GeoGrid
  {

    // Intersection
    // ------------

    template< class Grid, class HostIntersection >
    class Intersection
    {
      typedef typename HostIntersection::Geometry HostGeometry;
      typedef typename HostIntersection::LocalGeometry HostLocalGeometry;

      typedef typename remove_const< Grid >::type::Traits Traits;

    public:
      typedef typename Traits::ctype ctype;

      static const int dimension = Traits::dimension;
      static const int dimensionworld = Traits::dimensionworld;

      typedef typename Traits::template Codim< 0 >::Entity Entity;
      typedef typename Traits::template Codim< 0 >::EntityPointer EntityPointer;
      typedef typename Traits::template Codim< 1 >::Geometry Geometry;
      typedef typename Traits::template Codim< 1 >::LocalGeometry LocalGeometry;

      typedef typename Traits::template Codim< 0 >::Geometry ElementGeometry;

    private:
      typedef GeoGrid::IntersectionCoordVector< Grid > CoordVector;

      typedef typename Traits::template Codim< 0 >::EntityImpl EntityImpl;

      typedef typename Traits::template Codim< 1 >::GeometryImpl GeometryImpl;
      typedef typename Traits::template Codim< 0 >::GeometryImpl ElementGeometryImpl;

    public:

      Intersection()
      {}

      explicit Intersection ( const HostIntersection &hostIntersection, const ElementGeometryImpl &insideGeo )
        : hostIntersection_( hostIntersection )
        , insideGeo_ ( insideGeo )
        , geo_( grid() )
      {}

      explicit Intersection ( HostIntersection&& hostIntersection, const ElementGeometryImpl &insideGeo )
        : hostIntersection_( std::move( hostIntersection ) )
        , insideGeo_ ( insideGeo )
        , geo_( grid() )
      {}

      bool equals ( const Intersection &other) const
      {
        return hostIntersection_ == other.hostIntersection_;
      }

      operator bool () const { return bool( hostIntersection_ ); }

      Entity inside () const
      {
        return EntityImpl( insideGeo_, hostIntersection().inside() );
      }

      Entity outside () const
      {
        return EntityImpl( grid(), hostIntersection().outside() );
      }

      bool boundary () const { return hostIntersection().boundary(); }

      bool conforming () const { return hostIntersection().conforming(); }

      bool neighbor () const { return hostIntersection().neighbor(); }

      int boundaryId () const { return hostIntersection().boundaryId(); }

      size_t boundarySegmentIndex () const
      {
        return hostIntersection().boundarySegmentIndex();
      }

      LocalGeometry geometryInInside () const
      {
        return hostIntersection().geometryInInside();
      }

      LocalGeometry geometryInOutside () const
      {
        return hostIntersection().geometryInOutside();
      }

      Geometry geometry () const
      {
        if( !geo_ )
        {
          CoordVector coords( insideGeo_, geometryInInside() );
          geo_ = GeometryImpl( grid(), type(), coords );
        }
        return Geometry( geo_ );
      }

      GeometryType type () const { return hostIntersection().type(); }

      int indexInInside () const
      {
        return hostIntersection().indexInInside();
      }

      int indexInOutside () const
      {
        return hostIntersection().indexInOutside();
      }

      FieldVector< ctype, dimensionworld >
      integrationOuterNormal ( const FieldVector< ctype, dimension-1 > &local ) const
      {
        const ReferenceElement< ctype, dimension > &refElement
          = ReferenceElements< ctype, dimension>::general( insideGeo_.type() );

        FieldVector< ctype, dimension > x( geometryInInside().global( local ) );
        const typename ElementGeometryImpl::JacobianInverseTransposed &jit = insideGeo_.jacobianInverseTransposed( x );
        const FieldVector< ctype, dimension > &refNormal = refElement.integrationOuterNormal( indexInInside() );

        FieldVector< ctype, dimensionworld > normal;
        jit.mv( refNormal, normal );
        normal *= jit.detInv();
        //normal *= insideGeo_.integrationElement( x );
        return normal;
      }

      FieldVector< ctype, dimensionworld >
      outerNormal ( const FieldVector< ctype, dimension-1 > &local ) const
      {
        const ReferenceElement< ctype, dimension > &refElement
          = ReferenceElements< ctype, dimension>::general( insideGeo_.type() );

        FieldVector< ctype, dimension > x( geometryInInside().global( local ) );
        const typename ElementGeometryImpl::JacobianInverseTransposed &jit = insideGeo_.jacobianInverseTransposed( x );
        const FieldVector< ctype, dimension > &refNormal = refElement.integrationOuterNormal( indexInInside() );

        FieldVector< ctype, dimensionworld > normal;
        jit.mv( refNormal, normal );
        return normal;
      }

      FieldVector< ctype, dimensionworld >
      unitOuterNormal ( const FieldVector< ctype, dimension-1 > &local ) const
      {
        FieldVector< ctype, dimensionworld > normal = outerNormal( local );
        normal *= (ctype( 1 ) / normal.two_norm());
        return normal;
      }

      FieldVector< ctype, dimensionworld > centerUnitOuterNormal () const
      {
        const ReferenceElement< ctype, dimension-1 > &refFace
          = ReferenceElements< ctype, dimension-1 >::general( type() );
        return unitOuterNormal( refFace.position( 0, 0 ) );
      }

      const HostIntersection &hostIntersection () const
      {
        return hostIntersection_;
      }

      const Grid &grid () const { return insideGeo_.grid(); }

    private:
      HostIntersection hostIntersection_;
      ElementGeometryImpl insideGeo_;
      mutable GeometryImpl geo_;
    };

  } // namespace GeoGrid

} // namespace Dune

#endif // #ifndef DUNE_GEOGRID_INTERSECTION_HH
