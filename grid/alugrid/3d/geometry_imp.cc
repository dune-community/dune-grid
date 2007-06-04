// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#include "grid.hh"
#include "mappings.hh"

namespace Dune {
  // --Geometry

  //- Tetra specialization
  template<int mydim, int cdim>
  inline ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> >::
  ALU3dGridGeometry()
    : coord_()
      , Jinv_ ()
      , volume_(-1.0)
      , A_()
      , AT_x_()
      , localCoord_()
      , globalCoord_()
      , tmpV_()
      , tmpU_()
      , myGeomType_(GeometryType::simplex,mydim)
      , builtinverse_ (false) , builtA_ (false)
      , builtDetDF_ (false)
  {}

  //   B U I L T G E O M   - - -
  template<int mydim, int cdim>
  inline void ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> >::
  calcElMatrix () const
  {
    if(!builtA_)
    {
      // creat Matrix A (=Df)               INDIZES: row/col
      // Mapping: R^dim -> R^3,  F(x) = A x + p_0
      // columns:    p_1 - p_0  |  p_2 - p_0  |  p_3 - p_0

      for (int i=0; i<mydim; ++i) {
        for(int j=0; j<cdim; ++j)
          A_[j][i] = coord_[i+1][j] - coord_[0][j];
      }
      builtA_ = true;
    }
  }

  //dim = dimworld = 3
  template<int mydim, int cdim>
  inline void ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::
  buildJacobianInverseTransposed() const
  {
    if(!builtinverse_)
    {
      calcElMatrix();

      // DetDf = integrationElement, invert transposed Matrix
      detDF_ = std::abs( FMatrixHelp::invertMatrix_retTransposed(A_,Jinv_) );
      builtinverse_ = builtDetDF_ = true;
    }
  }

  template<> //dim = 2 , dimworld = 3
  inline void ALU3dGridGeometry<2,3, const ALU3dGrid<3,3,tetra> > :: buildJacobianInverseTransposed() const
  {
    if(!builtinverse_)
    {
      // calc A and stores it in A_
      calcElMatrix();

      // calc ret = A^T*A
      FMatrixHelp::multTransposedMatrix(A_,AT_A_);

      // calc Jinv_ = (A^T*A)^-1
      FMatrixHelp::invertMatrix_retTransposed(AT_A_,Jinv_);

      enum { dim = 3 };

      tmpV_ = coord_[1] - coord_[0];
      tmpU_ = coord_[2] - coord_[1];

      // calculate scaled outer normal
      for(int i=0; i<dim; i++)
      {
        globalCoord_[i] = (  tmpU_[(i+1)%dim] * tmpV_[(i+2)%dim]
                             - tmpU_[(i+2)%dim] * tmpV_[(i+1)%dim] );
      }

      detDF_ = std::abs ( globalCoord_.two_norm() );

      //std::cout << det << " calced| detDF " << detDF_ << "\n";

      builtinverse_ = builtDetDF_ = true;
    }
  }

  template<> //dim = 1 , dimworld = 3
  inline void ALU3dGridGeometry<1,3, const ALU3dGrid<3,3,tetra> > :: buildJacobianInverseTransposed() const
  {
    if(!builtinverse_)
    {
      // calc A and stores it in A_
      calcElMatrix();

      // calc ret = A^T*A
      FMatrixHelp::multTransposedMatrix(A_,AT_A_);

      // calc Jinv_ = (A^T*A)^-1
      FMatrixHelp::invertMatrix_retTransposed(AT_A_,Jinv_);

      enum { dim = 3 };
      //derr << "WARNING: ALU3dGridGeometry::buildJacobianInverseTransposed not tested yet! " << __LINE__ <<"\n";
      // create vectors of face
      globalCoord_ = coord_[1] - coord_[0];
      detDF_ = std::abs ( globalCoord_.two_norm() );
      builtinverse_ = builtDetDF_ = true;
    }
  }

  template<> //dim = 1 , dimworld = 3
  inline void ALU3dGridGeometry<0,3, const ALU3dGrid<3,3,tetra> > :: buildJacobianInverseTransposed() const
  {
    if(!builtinverse_)
    {
      enum { dim = 3 };
      detDF_ = 1.0;
      builtinverse_ = builtDetDF_ = true;
    }
  }

  // built Geometry
  template <int mydim, int cdim>
  template <class GeometryType>
  inline bool
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, tetra> >::
  buildGeomInFather(const GeometryType &fatherGeom , const GeometryType & myGeom)
  {
    // reset flags, because mappings need to be calculated again
    builtinverse_ = builtA_ = false;

    // compute the local coordinates in father refelem
    for(int i=0; i < myGeom.corners() ; ++i)
    {
      coord_[i] = fatherGeom.local( myGeom[i] );
      // to avoid rounding errors
      for(int j=0; j<cdim; ++j)
        if ( coord_[i][j] < 1e-16) coord_[i][j] = 0.0;
    }

    calculateDeterminant();
    return true;
  }

  template<int mydim, int cdim>
  inline void ALU3dGridGeometry<mydim,cdim, const ALU3dGrid<3, 3, tetra> > ::
  copyCoordVec(const alu3d_ctype (& point)[cdim] ,
               FieldVector<alu3d_ctype,cdim> & coord ) const
  {
    assert( cdim == 3 );
    coord[0] = point[0];
    coord[1] = point[1];
    coord[2] = point[2];
  }

  template <>
  inline bool ALU3dGridGeometry<3,3, const ALU3dGrid<3,3,tetra> > ::
  buildGeom(const IMPLElementType & item)
  {
    enum { dim = 3 };
    enum { dimworld = 3};

    builtinverse_ = builtA_ = false;

#ifndef NDEBUG
    builtDetDF_ = true ;

    // if this assertion is thrown, use ElementTopo::dune2aluVertex instead
    // of number when calling myvertex
    for(int i=0; i<4; ++i)
      assert( ElementTopo::dune2aluVertex(i) == i );

#endif
    // get volume and calc elDet
    volume_ = item.volume();
    detDF_  = 6.0 * volume_;

    copyCoordVec(item.myvertex(0)->Point(), coord_[0]);
    copyCoordVec(item.myvertex(1)->Point(), coord_[1]);
    copyCoordVec(item.myvertex(2)->Point(), coord_[2]);
    copyCoordVec(item.myvertex(3)->Point(), coord_[3]);
    return true;
  }

  template <>
  inline bool ALU3dGridGeometry<2,3, const ALU3dGrid<3,3,tetra> > ::
  buildGeom(const ALU3DSPACE HFaceType & item, int twist, int)
  {
    enum { dim = 2 };
    enum { dimworld = 3};

    builtinverse_ = builtA_ = builtDetDF_ = false;

    for (int i=0; i<(dim+1); i++)
    {
      // Transform Dune index to ALU index and apply twist
      int localALUIndex = FaceTopo::dune2aluVertex(i);
      int rotatedALUIndex = FaceTopo::twist(localALUIndex, twist);

      const double (&p)[3] =
        static_cast<const GEOFaceType &>(item).myvertex(rotatedALUIndex)->Point();
      for (int j=0; j<dimworld; j++)
      {
        coord_[i][j] = p[j];
      }
    }

    //buildJacobianInverseTransposed();
    return true;
  }

  template <>
  inline bool ALU3dGridGeometry<2, 3, const ALU3dGrid<3,3,tetra> > ::
  buildGeom(const FaceCoordinatesType& coords) {
    enum { dim = 2 };
    enum { dimworld = 3};

    builtinverse_ = builtA_ = builtDetDF_ = false;

    for (int i = 0; i < (dim+1); ++i) {
      coord_[i] = coords[i];
    }

    //buildJacobianInverseTransposed();
    return true;
  }

  template <> // for edges
  inline bool ALU3dGridGeometry<1,3, const ALU3dGrid<3,3,tetra> > ::
  buildGeom(const ALU3DSPACE HEdgeType & item, int twist, int)
  {
    enum { dim = 1 };
    enum { dimworld = 3};

    builtinverse_ = builtA_ = builtDetDF_ = false;

    for (int i=0; i<(dim+1); i++)
    {
      const double (&p)[3] =
        static_cast<const GEOEdgeType &> (item).myvertex((i+twist)%2)->Point();
      for (int j=0; j<dimworld; j++)
      {
        coord_[i][j] = p[j];
      }
    }

    buildJacobianInverseTransposed();
    return true;
  }

  template <> // for Vertices ,i.e. Points (note that twist is a dummy parameter here, needed for consistency)
  inline bool ALU3dGridGeometry<0,3, const ALU3dGrid<3,3,tetra> > ::
  buildGeom(const ALU3DSPACE VertexType & item, int, int)
  {
    enum { dim = 0 };
    enum { dimworld = 3};

    builtinverse_ = builtA_ = builtDetDF_ = false;

    const double (&p)[3] = static_cast<const GEOVertexType &> (item).Point();
    for (int j=0; j<dimworld; j++) coord_[0][j] = p[j];

    buildJacobianInverseTransposed();
    return true;
  }


  /* Comment in for adaptation to new GeometryType */
  template <int mydim, int cdim>
  inline const GeometryType &
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::type () const {
    return myGeomType_;
  }

  template <int mydim, int cdim>
  inline const GeometryType &
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, hexa> > ::type () const {
    return myGeomType_;
  }

  template<int mydim, int cdim>
  inline int ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::corners () const
  {
    return dimbary;
  }

  template<int mydim, int cdim>
  inline const FieldVector<alu3d_ctype, cdim>&
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > :: operator[] (int i) const
  {
    assert((i>=0) && (i < mydim+1));
    return coord_[i];
  }

  //   G L O B A L   - - -

  // dim = 1,2,3 dimworld = 3
  template<int mydim, int cdim>
  inline FieldVector<alu3d_ctype, cdim>
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> >::
  global(const FieldVector<alu3d_ctype, mydim>& local) const
  {
    calcElMatrix();

    globalCoord_ = coord_[0];
    // multiply with transposed because AT is also transposed
    A_.umv(local,globalCoord_);
    return globalCoord_;
  }

  template<> // dim = dimworld = 3
  inline FieldVector<alu3d_ctype, 3>
  ALU3dGridGeometry<3,3,const ALU3dGrid<3,3,tetra> > ::
  local(const FieldVector<alu3d_ctype, 3>& global) const
  {
    if (!builtinverse_) buildJacobianInverseTransposed();

    globalCoord_  = global;
    globalCoord_ -= coord_[0];

    localCoord_ = 0.0;
    // multiply with transposed because Jinv_ is already transposed
    Jinv_.umtv(globalCoord_,localCoord_);
    return localCoord_;
  }

  template<int mydim, int cdim> // dim = dimworld = 3
  inline FieldVector<alu3d_ctype, mydim>
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3,3,tetra> > ::
  local(const FieldVector<alu3d_ctype, cdim>& global) const
  {
    if(!builtinverse_)
      buildJacobianInverseTransposed();

    globalCoord_  = global;
    globalCoord_ -= coord_[0];

    AT_x_ = 0.0;
    A_.umtv(globalCoord_,AT_x_);

    localCoord_ = 0.0;
    //multipy with transposed, because Jinv_ ist transposed
    Jinv_.umtv(AT_x_,localCoord_);
    return localCoord_;
  }

  template<int mydim, int cdim>
  inline bool ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::
  checkInside(const FieldVector<alu3d_ctype, mydim>& local) const
  {
    alu3d_ctype sum = 0.0;

    for(int i=0; i<mydim; i++)
    {
      sum += local[i];
      if(local[i] < 0.0)
      {
        if(std::abs(local[i]) > ALUnumericEpsilon)
        {
          return false;
        }
      }
    }

    if( sum > 1.0 )
    {
      if(sum > (1.0 + ALUnumericEpsilon))
        return false;
    }
    return true;
  }

  // for elements this is already calculated
  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<3,3,const ALU3dGrid<3, 3, tetra> > ::
  integrationElement (const FieldVector<alu3d_ctype, 3>& local) const
  {
    assert( builtDetDF_ );
    return detDF_;
  }

  template<int mydim, int cdim>
  inline void
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::
  calculateDeterminant() const
  {
    calcElMatrix();

    // only can do this for mydim == dim, otherwise not defined
    if(mydim == cdim)
    {
      detDF_ = A_.determinant();
    }
    else
    {
      // buildJacobianInverseTransposed() also calculates determinant
      buildJacobianInverseTransposed();
    }
    assert(detDF_ > 0.0);

    enum { factor = Factorial<mydim>::factorial };
    volume_ = detDF_ / factor ;

    builtDetDF_ = true;
  }

  template<int mydim, int cdim>
  inline alu3d_ctype
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::integrationElement (const FieldVector<alu3d_ctype, mydim>& local) const
  {
    if(builtDetDF_)
      return detDF_;

    calculateDeterminant() ;
    return detDF_;
  }

  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<3,3,const ALU3dGrid<3, 3, tetra> > ::volume () const
  {
    assert( builtDetDF_ );
    return volume_;
  }

  template<int mydim, int cdim>
  inline alu3d_ctype
  ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> > ::volume () const
  {
    if( builtDetDF_ )
      return volume_;

    calculateDeterminant() ;
    return volume_;
  }


  //  J A C O B I A N _ I N V E R S E  - - -

  template<> // dim = dimworld = 3
  inline const FieldMatrix<alu3d_ctype,3,3> &
  ALU3dGridGeometry<3,3, const ALU3dGrid<3,3,tetra> >::
  jacobianInverseTransposed (const FieldVector<alu3d_ctype, 3>& local) const
  {
    if (!builtinverse_) buildJacobianInverseTransposed();
    return Jinv_;
  }

  template<> // dim = dimworld = 3
  inline const FieldMatrix<alu3d_ctype,2,2> &
  ALU3dGridGeometry<2,3, const ALU3dGrid<3,3,tetra> >::
  jacobianInverseTransposed (const FieldVector<alu3d_ctype, 2>& local) const
  {
    if (!builtinverse_) buildJacobianInverseTransposed();
    return Jinv_;
  }

  // print the ElementInformation
  template<int mydim, int cdim>
  inline void ALU3dGridGeometry<mydim,cdim,const ALU3dGrid<3, 3, tetra> >::print (std::ostream& ss) const
  {
    ss << "ALU3dGridGeometry<" << mydim << "," << cdim << ", tetra> = {\n";
    for(int i=0; i<corners(); i++)
    {
      ss << " corner " << i << " ";
      ss << "{" << ((*this)[i]) << "}"; ss << std::endl;
    }
    ss << "} \n";
  }

  //- Hexahedron specialization
  template <int mydim, int cdim>
  inline ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  ALU3dGridGeometry() :
    coord_(0.0),
    myGeomType_(GeometryType::cube,mydim),
    triMap_(0),
    biMap_(0),
    localBaryCenter_(0.5)
  {}

  template <>
  inline ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  ALU3dGridGeometry() :
    coord_(0.0),
    myGeomType_(GeometryType::cube,3),
    triMap_(0),
    biMap_(0),
    localBaryCenter_(0.5),
    volume_(-1.0)
  {}

  template <>
  inline ALU3dGridGeometry<2, 3, const ALU3dGrid<3, 3, hexa> >::
  ALU3dGridGeometry()
    : coord_(0.0),
      myGeomType_(GeometryType::cube,2),
      triMap_(0),
      biMap_(0),
      localBaryCenter_(0.5)
  {}

  template <>
  inline ALU3dGridGeometry<2, 2, const ALU3dGrid<3, 3, hexa> >::
  ALU3dGridGeometry()
    : coord_(0.0),
      myGeomType_(GeometryType::cube,2),
      triMap_(0),
      biMap_(0),
      localBaryCenter_(0.5)
  {}


  template <int mydim, int cdim>
  inline ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  ~ALU3dGridGeometry() {
    delete triMap_;
    delete biMap_;
  }

  template <int mydim, int cdim>
  inline int
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::corners() const {
    return Power_m_p<2,mydim>::power;
  }

  template <int mydim, int cdim>
  inline const FieldVector<alu3d_ctype, cdim>&
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  operator[] (int i) const {
    assert((i >= 0) && (i < corners()));
    return coord_[i];
  }

  template <>
  inline FieldVector<alu3d_ctype, 3>
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  global (const FieldVector<alu3d_ctype, 3>& local) const {
    assert(triMap_);
    triMap_->map2world(local, tmp2_);
    return tmp2_;
  }

  template <>
  inline FieldVector<alu3d_ctype, 3>
  ALU3dGridGeometry<2, 3, const ALU3dGrid<3, 3, hexa> >::
  global (const FieldVector<alu3d_ctype, 2>& local) const {
    assert(biMap_);
    biMap_->map2world(local, tmp2_);
    return tmp2_;
  }

  template <>
  inline FieldVector<alu3d_ctype, 3>
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  local (const FieldVector<alu3d_ctype, 3>& global) const {
    assert(triMap_);
    triMap_->world2map(global, tmp2_);
    return tmp2_;
  }

  template <>
  inline FieldVector<alu3d_ctype, 2>
  ALU3dGridGeometry<2, 3, const ALU3dGrid<3, 3, hexa> >::
  local (const FieldVector<alu3d_ctype, 3>& global) const {
    assert( biMap_ );
    biMap_->world2map(global, tmp1_);
    return tmp1_;
  }

  template <int mydim, int cdim>
  inline bool
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  checkInside(const FieldVector<alu3d_ctype, mydim>& local) const {
    bool result = true;
    for (int i = 0; i < mydim; i++ ) {
      result &= (local[i] >= - ALUnumericEpsilon &&
                 local[i] <= 1.0 +  ALUnumericEpsilon);
    }
    return result;
  }

  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  integrationElement (const FieldVector<alu3d_ctype, 3>& local) const {
    assert(triMap_);
    return triMap_->det(local);
  }

  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<2, 3, const ALU3dGrid<3, 3, hexa> >::
  integrationElement (const FieldVector<alu3d_ctype, 2>& local) const {
    assert(biMap_);
    biMap_->normal(local, tmp2_);
    return tmp2_.two_norm();
  }

  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<1, 3, const ALU3dGrid<3, 3, hexa> >::
  integrationElement (const FieldVector<alu3d_ctype, 1>& local) const {
    return (this->operator[] (0) - this->operator[] (1)).two_norm();
  }

  template<>
  inline alu3d_ctype
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  volume() const
  {
    return volume_;
  }

  template<int mydim, int cdim>
  inline alu3d_ctype
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  volume () const
  {
    return integrationElement(localBaryCenter_);
  }


  template <>
  inline const FieldMatrix<alu3d_ctype, 3, 3>&
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  jacobianInverseTransposed (const FieldVector<alu3d_ctype, 3>& local) const {
    assert(triMap_);
    jInv_ = triMap_->jacobianInverse(local);
    return jInv_;
  }

  template <>
  inline const FieldMatrix<alu3d_ctype, 2, 2>&
  ALU3dGridGeometry<2, 3, const ALU3dGrid<3, 3, hexa> >::
  jacobianInverseTransposed (const FieldVector<alu3d_ctype, 2>& local) const {
    assert( biMap_ );
#ifndef NDEBUG
    static bool called = false;
    if(!called)
    {
      dwarn << "WARNING: ALU3dGridGeometry<2,3>::jacobianInverseTransposed: method not tested yet! \n";
      called = true;
    }
#endif
    jInv_ = biMap_->jacobianInverse(local);
    return jInv_;
  }

  template <int mydim, int cdim>
  inline void
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  print (std::ostream& ss) const {
    ss << "ALU3dGridGeometry<" << mydim << "," << cdim << ", hexa> = {\n";
    for(int i=0; i<corners(); i++)
    {
      ss << " corner " << i << " ";
      ss << "{" << ((*this)[i]) << "}"; ss << std::endl;
    }
    ss << "} \n";
  }


  template <int mydim, int cdim>
  inline void
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  buildMapping()
  {
    assert( mydim == 3 );
    assert( cdim  == 3 );
    if(triMap_) delete triMap_;
    triMap_ = new TrilinearMapping(coord_[0], coord_[1], coord_[2], coord_[3],
                                   coord_[4], coord_[5], coord_[6], coord_[7]);
  }

  // built Geometry
  template <int mydim, int cdim>
  template <class GeometryType>
  inline bool
  ALU3dGridGeometry<mydim, cdim, const ALU3dGrid<3, 3, hexa> >::
  buildGeomInFather(const GeometryType &fatherGeom , const GeometryType & myGeom)
  {
    // compute the local coordinates in father refelem
    for(int i=0; i < myGeom.corners() ; ++i)
    {
      coord_[i] = fatherGeom.local( myGeom[i] );
      // to avoid rounding errors
      for(int j=0; j<cdim; ++j)
        if ( coord_[i][j] < 1e-16) coord_[i][j] = 0.0;
    }

    // delete old mapping and creats new mapping
    buildMapping();
    return true;
  }

  template <>
  inline bool
  ALU3dGridGeometry<3, 3, const ALU3dGrid<3, 3, hexa> >::
  buildGeom(const IMPLElementType& item) {
    enum { dim = 3 };
    enum { dimworld = 3 };

    volume_ = item.volume();
    for (int i = 0; i < corners(); ++i) {
      const double (&p)[3] =
        item.myvertex(ElementTopo::dune2aluVertex(i))->Point();

      coord_[i][0] = p[0];
      coord_[i][1] = p[1];
      coord_[i][2] = p[2];
    }

    // delete old mapping and creats new mapping
    buildMapping();
    return true;
  }

  template <>
  inline bool
  ALU3dGridGeometry<2,3, const ALU3dGrid<3, 3, hexa> > ::
  buildGeom(const ALU3DSPACE HFaceType & item, int twist, int duneFace ) {
    enum { dim = 2 };
    enum { dimworld = 3 };

    const GEOFaceType& face = static_cast<const GEOFaceType&> (item);

    //assert( duneFace >= 0 && duneFace < 6 );
    if(duneFace < 0 ) duneFace = 0;
    // for all vertices of this face
    for (int i = 0; i < 4; ++i)
    {
      // Transform Dune index to ALU index and apply twist
      int localALUIndex = ElementTopo::dune2aluFaceVertex(duneFace,i);
      int rotatedALUIndex = FaceTopo::twist(localALUIndex, twist);

      const double (&p)[3] =
        face.myvertex(rotatedALUIndex)->Point();
      for (int j = 0; j < dimworld; ++j)
      {
        coord_[i][j] = p[j];
      }
    }

    delete biMap_;
    biMap_ = new BilinearSurfaceMapping(coord_[0], coord_[1],
                                        coord_[2], coord_[3]);

    return true;
  }

  template <>
  inline bool
  ALU3dGridGeometry<2,3, const ALU3dGrid<3, 3, hexa> > ::
  buildGeom(const FaceCoordinatesType& coords) {
    enum { dim = 2 };
    enum { dimworld = 3 };

    for (int i = 0; i < 4; ++i) {
      coord_[i] = coords[i];
    }

    delete biMap_;
    biMap_ = new BilinearSurfaceMapping(coord_[0], coord_[1],
                                        coord_[2], coord_[3]);

    return true;
  }

  template <> // for edges
  inline bool
  ALU3dGridGeometry<1,3, const ALU3dGrid<3, 3, hexa> >::
  buildGeom(const ALU3DSPACE HEdgeType & item, int twist, int) {
    enum { dim = 1 };
    enum { dimworld = 3 };

    for (int i = 0; i < 2; ++i) {
      const double (&p)[3] =
        static_cast<const GEOEdgeType &> (item).myvertex((i+twist)%2)->Point();
      for (int j = 0; j < dimworld; ++j) {
        coord_[i][j] = p[j];
      }
    }

    return true;
  }

  template <> // for Vertices ,i.e. Points
  inline bool
  ALU3dGridGeometry<0,3, const ALU3dGrid<3,3,hexa> >::
  buildGeom(const ALU3DSPACE VertexType & item, int twist, int) {
    enum { dim = 0 };
    enum { dimworld = 3};

    const double (&p)[3] = static_cast<const GEOVertexType &> (item).Point();
    for (int j=0; j<dimworld; j++) coord_[0][j] = p[j];

    return true;
  }


} // end namespace Dune
