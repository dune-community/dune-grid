// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GEOGRID_ENTITYPOINTER_HH
#define DUNE_GEOGRID_ENTITYPOINTER_HH

#include <dune/grid/common/grid.hh>

#include <dune/grid/geogrid/capabilities.hh>
#include <dune/grid/geogrid/storage.hh>

namespace Dune
{

  // External Forward Declarations
  // -----------------------------

  template< int, int, class >
  class GeometryGridEntity;

  template< class HostGrid, class CoordFunction >
  struct GeometryGridExportParams;

  template< class HostGrid, class CordFunction >
  class GeometryGrid;



  // Internal Forward Declarations
  // -----------------------------

  template< int codim, class Grid >
  struct GeometryGridEntityPointerTraits;

  template< class Traits, bool fake = Traits :: fake >
  class GeometryGridEntityPointer;



  // GeometryGridEntityPointerTraits
  // -------------------------------

  template< int codim, class Grid >
  struct GeometryGridEntityPointerTraits;

  /** \cond */
  template< int codim, class Grid >
  struct GeometryGridEntityPointerTraits< codim, const Grid >
    : public GeometryGridEntityPointerTraits< codim, Grid >
  {};
  /** \endcond */

  template< int codim, class HostGrid, class CoordFunction >
  struct GeometryGridEntityPointerTraits
  < codim, GeometryGrid< HostGrid, CoordFunction > >
    : public GeometryGridExportParams< HostGrid, CoordFunction >
  {
    typedef Dune :: GeometryGrid< HostGrid, CoordFunction > Grid;

    static const bool fake = !Capabilities :: hasHostEntity< Grid, codim > :: v;

    typedef typename HostGrid :: ctype ctype;

    static const int dimension = HostGrid :: dimension;
    static const int codimension = codim;

    typedef Dune :: Entity< codimension, dimension, const Grid, GeometryGridEntity >
    Entity;

    typedef typename HostGrid :: template Codim< codim > :: Entity HostEntity;
    typedef typename HostGrid :: template Codim< codim > :: EntityPointer
    HostEntityPointer;
    typedef HostEntityPointer HostEntityIterator;

    typedef typename HostGrid :: template Codim< 0 > :: Entity HostElement;
    typedef typename HostGrid :: template Codim< 0 > :: EntityPointer
    HostElementPointer;
    typedef HostElementPointer HostElementIterator;
  };



  // GeometryGridEntityPointer (real)
  // --------------------------------

  template< class Traits >
  class GeometryGridEntityPointer< Traits, false >
  {
    typedef GeometryGridEntityPointer< Traits, false > This;

    typedef typename Traits :: Grid Grid;

    typedef GeometryGridEntityPointerTraits< Traits :: codimension, const Grid >
    BaseTraits;
    friend class GeometryGridEntityPointer< BaseTraits, false >;

  public:
    static const int dimension = Traits :: dimension;
    static const int codimension = Traits :: codimension;

    typedef typename Traits :: Entity Entity;

    static const bool fake = Traits :: fake;

    typedef GeometryGridEntityPointer< BaseTraits, fake > Base;
    typedef GeometryGridEntityPointer< BaseTraits, fake > base;

  private:
    typedef GeometryGridEntityWrapper< Entity > EntityWrapper;
    typedef GeometryGridStorage< EntityWrapper > EntityStorage;

    const Grid *grid_;
    mutable EntityWrapper *entity_;

  protected:
    typedef typename Traits :: HostEntityPointer HostEntityPointer;
    typedef typename Traits :: HostEntityIterator HostEntityIterator;
    typedef typename Traits :: HostElement HostElement;

    HostEntityIterator hostEntityIterator_;

  public:
    GeometryGridEntityPointer ( const Grid &grid,
                                const HostEntityIterator &hostEntityIterator )
      : grid_( &grid ),
        entity_( 0 ),
        hostEntityIterator_( hostEntityIterator )
    {}

    GeometryGridEntityPointer ( const Grid &grid,
                                const HostElement &hostElement,
                                int subEntity )
      : grid_( &grid ),
        entity_( 0 ),
        hostEntityIterator_( hostElement.template entity< codimension >( subEntity ) )
    {}

    GeometryGridEntityPointer ( const This &other )
      : grid_( other.grid_ ),
        entity_( 0 ),
        hostEntityIterator_( other.hostEntityIterator_ )
    {}

    template< class T >
    explicit GeometryGridEntityPointer ( const GeometryGridEntityPointer< T, fake > &other )
      : grid_( other.grid_ ),
        entity_( 0 ),
        hostEntityIterator_( other.hostEntityIterator_ )
    {}

    ~GeometryGridEntityPointer ()
    {
      EntityStorage :: free( entity_ );
    }

    This &operator= ( const This &other )
    {
      grid_ = other.grid_;
      hostEntityIterator_ = other.hostEntityIterator_;
      update();
      return *this;
    }

    operator const Base & () const
    {
      return reinterpret_cast< const Base & >( *this );
    }

    operator Base & ()
    {
      return reinterpret_cast< Base & >( *this );
    }

    template< class T >
    bool equals ( const GeometryGridEntityPointer< T, fake > &other ) const
    {
      return (hostEntityPointer() == other.hostEntityPointer());
    }

    Entity &dereference () const
    {
      if( entity_ == 0 )
      {
        entity_ = EntityStorage :: alloc();
        entity_->initialize( grid(), *hostEntityPointer() );
      }
      return *entity_;
    }

    int level () const
    {
      return hostEntityPointer().level();
    }

    const HostEntityPointer &hostEntityPointer () const
    {
      return hostEntityIterator_;
    }

    const Grid &grid () const
    {
      return *grid_;
    }

  protected:
    void update ()
    {
      EntityStorage :: free( entity_ );
      entity_ = 0;
    }
  };



  // GeometryGridEntityPointer (fake)
  // --------------------------------

  template< class Traits >
  class GeometryGridEntityPointer< Traits, true >
  {
    typedef GeometryGridEntityPointer< Traits, true > This;

    typedef typename Traits :: Grid Grid;

    typedef GeometryGridEntityPointerTraits< Traits :: codimension, const Grid >
    BaseTraits;
    friend class GeometryGridEntityPointer< BaseTraits, true >;

  public:
    static const int dimension = Traits :: dimension;
    static const int codimension = Traits :: codimension;

    typedef typename Traits :: Entity Entity;

    static const bool fake = Traits :: fake;

    typedef GeometryGridEntityPointer< BaseTraits, fake > Base;
    typedef GeometryGridEntityPointer< BaseTraits, fake > base;

  private:
    typedef GeometryGridEntityWrapper< Entity > EntityWrapper;
    typedef GeometryGridStorage< EntityWrapper > EntityStorage;

    const Grid *grid_;
    mutable EntityWrapper *entity_;

  protected:
    typedef typename Traits :: HostEntityPointer HostEntityPointer;
    typedef typename Traits :: HostElementPointer HostElementPointer;
    typedef typename Traits :: HostElementIterator HostElementIterator;
    typedef typename Traits :: HostElement HostElement;

    int subEntity_;
    HostElementIterator hostElementIterator_;

  public:
    GeometryGridEntityPointer ( const Grid &grid,
                                const HostElementIterator &hostElementIterator,
                                int subEntity )
      : grid_( &grid ),
        entity_( 0 ),
        subEntity_( subEntity ),
        hostElementIterator_( hostElementIterator )
    {}

    GeometryGridEntityPointer ( const Grid &grid,
                                const HostElement &hostElement,
                                int subEntity )
      : grid_( &grid ),
        entity_( 0 ),
        subEntity_( subEntity ),
        hostElementIterator_( hostElement.template entity< 0 >( 0 ) )
    {}

    GeometryGridEntityPointer ( const This &other )
      : grid_( other.grid_ ),
        entity_( 0 ),
        subEntity_( other.subEntity_ ),
        hostElementIterator_( other.hostElementIterator_ )
    {}

    template< class T >
    explicit GeometryGridEntityPointer ( const GeometryGridEntityPointer< T, fake > &other )
      : grid_( other.grid_ ),
        entity_( 0 ),
        subEntity_( other.subEntity_ ),
        hostElementIterator_( other.hostElementIterator_ )
    {}

    ~GeometryGridEntityPointer ()
    {
      EntityStorage :: free( entity_ );
    }

    This &operator= ( const This &other )
    {
      grid_ = other.grid_;
      subEntity_ = other.subEntity_;
      hostElementIterator_ = other.hostElementIterator_;
      update();
      return *this;
    }

    operator const Base & () const
    {
      return reinterpret_cast< const Base & >( *this );
    }

    operator Base & ()
    {
      return reinterpret_cast< Base & >( *this );
    }

    template< class T >
    bool equals ( const GeometryGridEntityPointer< T, fake > &other ) const
    {
      const int thisSub = subEntity_;
      const int otherSub = other.subEntity_;

      if( (thisSub < 0) || (otherSub < 0) )
        return (thisSub * otherSub >= 0);

      const int lvl = level();
      if( lvl != other.level() )
        return false;

      const typename Traits :: HostGrid :: Traits :: LevelIndexSet &indexSet
        = grid().hostGrid().levelIndexSet( lvl );

      const HostElement &thisElement = *hostElementPointer();
      assert( indexSet.contains( thisElement ) );
      const HostElement &otherElement = *(other.hostElementPointer());
      assert( indexSet.contains( otherElement ) );

      const int thisIndex
        = indexSet.template subIndex< codimension >( thisElement, thisSub );
      const int otherIndex
        = indexSet.template subIndex< codimension >( otherElement, otherSub );
      return thisIndex == otherIndex;
    }

    Entity &dereference () const
    {
      if( entity_ == 0 )
      {
        entity_ = EntityStorage :: alloc();
        entity_->initialize( grid(), *hostElementPointer(), subEntity_ );
      }
      return *entity_;
    }

    int level () const
    {
      return hostElementPointer().level();
    }

    const HostEntityPointer &hostEntityPointer () const
    {
      DUNE_THROW( NotImplemented, "HostGrid has no entities of codimension "
                  << codimension << "." );
    }

    const Grid &grid () const
    {
      return *grid_;
    }

  protected:
    void update ()
    {
      EntityStorage :: free( entity_ );
      entity_ = 0;
    }

    const HostElementPointer &hostElementPointer () const
    {
      return hostElementIterator_;
    }
  };

}

#endif
