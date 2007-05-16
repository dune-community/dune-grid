// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_UG_WRAPPER_2D_HH
#define DUNE_UG_WRAPPER_2D_HH

/** \file
 * \brief Encapsulates some UG macros and functions
 */

/** \todo Here only to provide the constant DBL_EPSILON.  There's maybe a better way? */
#include "float.h"

namespace Dune {

  /** \brief Encapsulates a few UG methods and macros
   *
   * This class provides a wrapper to several methods and macros from
   * UG.  There are two reasons for doing this.  First, we don't want
   * to call UG macros directly from DUNE, because they pollute the
   * namespace and therefore we undefine them all.  Secondly,  UG methods
   * appear in the namespaces UG::D2 and UG::D3, but we need the dimension
   * as a template parameter.
   */
  template<int dim>
  class UG_NS {};

  template<>
  class UG_NS<2> {
  public:

    // //////////////////////////////////////////////
    //   Types exported by UG
    // //////////////////////////////////////////////

    typedef UG::D2::RefinementRule RefinementRule;

    typedef UG::D2::CoeffProcPtr CoeffProcPtr;

    typedef UG::D2::UserProcPtr UserProcPtr;

#ifndef UG_LGMDOMAIN
    typedef UG::D2::BndSegFuncPtr BndSegFuncPtr;
#endif

    /** \todo This type becomes obsolete as soon as UG implements faces and edges */
    typedef UG::D2::vector Vector;

    typedef UG::D2::multigrid MultiGrid;

    typedef UG::D2::grid Grid;

    typedef UG::D2::edge Edge;

    typedef UG::D2::node Node;

    typedef UG::D2::element Element;

    typedef UG::D2::vertex Vertex;

    /** \brief Types of the subentities parametrized by the codimension.  Gets specialized below */
    template <int codim>
    class Entity;

    // //////////////////////////////////////////////
    //   Constants exported by UG
    // //////////////////////////////////////////////

    enum {GM_REFINE_NOT_CLOSED = UG::D2::GM_REFINE_NOT_CLOSED};

    enum {GM_COPY_ALL = UG::D2::GM_COPY_ALL};

    enum {GM_REFINE_TRULY_LOCAL = UG::D2::GM_REFINE_TRULY_LOCAL};

    enum {GM_REFINE_PARALLEL = UG::D2::GM_REFINE_PARALLEL};

    enum {GM_REFINE_NOHEAPTEST = UG::D2::GM_REFINE_NOHEAPTEST};

    /** \brief Control word entries */
    enum {NEWEL_CE      = UG::D2::NEWEL_CE,
          COARSEN_CE    = UG::D2::COARSEN_CE,
          ECLASS_CE     = UG::D2::ECLASS_CE,
          MARK_CE       = UG::D2::MARK_CE};

    /** \brief Refinement rules */
    enum {NO_REFINEMENT = UG::D2::NO_REFINEMENT,
          RED           = UG::D2::RED,
          COARSE        = UG::D2::COARSE};

    enum {RED_CLASS = UG::D2::RED_CLASS};

    enum {GM_OK = UG::D2::GM_OK};

    enum {MAX_SONS = UG::D2::MAX_SONS};

    /** \brief The PFIRSTNODE macro which returns the first node in a
     * grid even in a parallel setting.
     */
    static UG_NS<2>::Node* PFirstNode(const UG_NS<2>::Grid* grid) {
      using UG::PrioHGhost;
      using UG::PrioVGhost;
      using UG::PrioVHGhost;
      using UG::PrioMaster;
      using UG::PrioBorder;
      using UG::ELEMENT_LIST;
      using UG::NODE_LIST;
      return PFIRSTNODE(grid);
    }

    /** \brief The FIRSTNODE macro which returns the first node in a
     * grid even in a parallel setting.
     */
    static UG_NS<2>::Node* FirstNode(UG_NS<2>::Grid* grid) {
      using UG::PrioHGhost;
      using UG::PrioVGhost;
      using UG::PrioVHGhost;
      using UG::PrioMaster;
      using UG::PrioBorder;
      using UG::ELEMENT_LIST;
      using UG::NODE_LIST;
      return FIRSTNODE(grid);
    }

    /** \brief The PFIRSTELEMENT macro which returns the first element in a
     * grid even in a parallel setting.
     */
    static UG_NS<2>::Element* PFirstElement(const UG_NS<2>::Grid* grid) {
      using UG::PrioHGhost;
      using UG::PrioVGhost;
      using UG::PrioVHGhost;
      using UG::PrioMaster;
      using UG::PrioBorder;
      using UG::ELEMENT_LIST;
      using UG::NODE_LIST;
      return PFIRSTELEMENT(grid);
    }

    /** \brief The FIRSTELEMENT macro which returns the first element in a
     * grid even in a parallel setting.
     */
    static UG_NS<2>::Element* FirstElement(UG_NS<2>::Grid* grid) {
      using UG::PrioHGhost;
      using UG::PrioVGhost;
      using UG::PrioVHGhost;
      using UG::PrioMaster;
      using UG::PrioBorder;
      using UG::ELEMENT_LIST;
      return FIRSTELEMENT(grid);
    }

    /** \brief Returns pointers to the coordinate arrays of an UG element */
    static void Corner_Coordinates(const UG_NS<2>::Element* theElement, double* x[]) {
      using UG::D2::NODE;
      using UG::D2::TRIANGLE;
      using UG::D2::QUADRILATERAL;
      using UG::D2::n_offset;
      using UG::UINT;
      int n;    // Dummy variable just to please the macro
      CORNER_COORDINATES(theElement, n, x);
    }

    static int GlobalToLocal(int n, const double** cornerCoords,
                             const double* EvalPoint, double* localCoord) {
      return UG::D2::UG_GlobalToLocal(n, cornerCoords, EvalPoint, localCoord);
    }

    static int myLevel (const UG_NS<2>::Element* theElement) {
      using UG::UINT;
      return LEVEL(theElement);
    }

    static int myLevel (const UG_NS<2>::Node* theNode) {
      using UG::UINT;
      return LEVEL(theNode);
    }

    //! return true if element has an exact copy on the next level
    static bool hasCopy (const UG_NS<2>::Element* theElement) {
      using UG::D2::ELEMENT;
      using UG::D2::control_entries;
      using UG::UINT;
      using UG::D2::REFINECLASS_CE;
      using UG::D2::YELLOW_CLASS;
      return REFINECLASS(theElement) == YELLOW_CLASS;
    }

    //! return true if element has an exact copy on the next level
    static bool isRegular (const UG_NS<2>::Element* theElement) {
      using UG::D2::ELEMENT;
      using UG::D2::control_entries;
      using UG::UINT;
      return ECLASS(theElement) == RED_CLASS;
    }

    //! \todo Please doc me!
    static int Sides_Of_Elem(const UG_NS<2>::Element* theElement) {
      using UG::D2::element_descriptors;
      using UG::UINT;
      return SIDES_OF_ELEM(theElement);
    }

    //! Encapsulates the NBELEM macro
    static UG_NS<2>::Element* NbElem(const UG_NS<2>::Element* theElement, int nb) {
      using UG::D2::ELEMENT;
      using UG::D2::nb_offset;
      using UG::UINT;
      return NBELEM(theElement, nb);
    }

    //! Returns true if the i-th side of the element is on the domain boundary
    static bool Side_On_Bnd(const UG_NS<2>::Element* theElement, int i) {
      using UG::D2::BNDS;
      using UG::D2::BEOBJ;
      using UG::D2::side_offset;
      using UG::UINT;
      using UG::D2::GM_OBJECTS;
      return OBJT(theElement)==BEOBJ && SIDE_ON_BND(theElement, i);
    }

    //! \todo Please doc me!
    static int Edges_Of_Elem(const UG_NS<2>::Element* theElement) {
      using UG::D2::element_descriptors;
      using UG::UINT;
      return EDGES_OF_ELEM(theElement);
    }

    //! \todo Please doc me!
    static int Corners_Of_Elem(const UG_NS<2>::Element* theElement) {
      using UG::D2::element_descriptors;
      using UG::UINT;
      return CORNERS_OF_ELEM(theElement);
    }

    //! \todo Please doc me!
    // Dummy implementation for vertices
    static int Corners_Of_Elem(const UG_NS<2>::Node* theNode) {
      return 1;
    }

    //! \todo Please doc me!
    static int Corners_Of_Side(const UG_NS<2>::Element* theElement, int side) {
      using UG::D2::element_descriptors;
      using UG::UINT;
      return CORNERS_OF_SIDE(theElement, side);
    }

    //! \todo Please doc me!
    static int Corner_Of_Side(const UG_NS<2>::Element* theElement, int side, int corner) {
      using UG::D2::element_descriptors;
      using UG::UINT;
      return CORNER_OF_SIDE(theElement, side, corner);
    }

    static int nSons(const UG::D2::element* element) {
      return UG::D2::ReadCW(element, UG::D2::NSONS_CE);
    }

    static int GetSons(const UG::D2::element* element, UG::D2::element* sonList[MAX_SONS]) {
      return UG::D2::GetSons(element, sonList);
    }

    /** \todo Remove the const casts */
    static int GetNodeContext(const UG::D2::element* element, const UG::D2::node** context) {
      return UG::D2::GetNodeContext(element, const_cast<UG::D2::node**>(context));
    }

    //! Encapsulates the GRID_ATTR macro
    static int Grid_Attr(const UG_NS<2>::Grid* grid) {
      return GRID_ATTR(grid);
    }

    static int MarkForRefinement(UG::D2::element* element, int rule, int data) {
      return UG::D2::MarkForRefinement(element, (UG::D2::RefinementRule)rule, data);
    }

    //! Encapsulates the TAG macro
    static unsigned int Tag(const UG_NS<2>::Element* theElement) {
      using UG::UINT;
      return TAG(theElement);
    }

    //! Doesn't ever get called, but needs to be there to calm the compiler
    static unsigned int Tag(const UG_NS<2>::Node* theNode) {
      DUNE_THROW(GridError, "Called method Tag() for a vertex.  This should never happen!");
      return 0;
    }

    //! get corner in local coordinates, corner number in UG's numbering system
    template<class T>
    static void  getCornerLocal (const UG_NS<2>::Element* theElement, int corner, FieldVector<T, 2>& local)
    {
      using UG::D2::element_descriptors;
      using UG::UINT;
      local[0] = LOCAL_COORD_OF_TAG(TAG(theElement),corner)[0];
      local[1] = LOCAL_COORD_OF_TAG(TAG(theElement),corner)[1];
    }

    //! Next element in the UG element lists
    static UG_NS<2>::Element* succ(const UG_NS<2>::Element* theElement) {
      return theElement->ge.succ;
    }

    //! Next element in the UG nodes lists
    static UG_NS<2>::Node* succ(const UG_NS<2>::Node* theNode) {
      return theNode->succ;
    }

    //! Calm the compiler
    static void* succ(const void* theWhatever) {
      DUNE_THROW(NotImplemented, "No successor available for this kind of object");
      return 0;
    }

    //! Return true if the element is a leaf element
    static bool isLeaf(const UG_NS<2>::Element* theElement) {
      return UG::D2::EstimateHere(theElement);
    }

    //! Return true if the node is a leaf node
    static bool isLeaf(const UG_NS<2>::Node* theNode) {
#ifndef ModelP
      return !theNode->son;
#else
      DUNE_THROW(NotImplemented, "isLeaf for nodes in a parallel grid");
#endif
    }

    // /////////////////////////////////////////////
    //   Level indices
    // /////////////////////////////////////////////

    //! Gets the level index of a UG element
    static int& levelIndex(UG_NS<2>::Element* theElement) {
      return theElement->ge.levelIndex;
    }

    //! Gets the level index of a UG element
    static const int& levelIndex(const UG_NS<2>::Element* theElement) {
      return theElement->ge.levelIndex;
    }

    //! Gets the level index of a UG sidevector
    static int& levelIndex(Vector* theVector) {
      DUNE_THROW(GridError, "levelIndex in side vector only in 3D!");
      return reinterpret_cast<int&>(theVector->index);
    }

    //! Gets the level index of a UG sidevector
    static const int& levelIndex(const Vector* theVector) {
      DUNE_THROW(GridError, "levelIndex in side vector only in 3D!");
      return reinterpret_cast<const int&>(theVector->index);
    }

    //! Gets the level index of a UG edge
    static int& levelIndex(UG_NS<2>::Edge* theEdge) {
      return theEdge->levelIndex;
    }

    //! Gets the level index of a UG edge
    static const int& levelIndex(const UG_NS<2>::Edge* theEdge) {
      return theEdge->levelIndex;
    }

    //! Gets the level index of a UG node
    static int& levelIndex(UG_NS<2>::Node* theNode) {
      return theNode->levelIndex;
    }

    //! Gets the level index of a UG node
    static const int& levelIndex(const UG_NS<2>::Node* theNode) {
      return theNode->levelIndex;
    }

    // /////////////////////////////////////////////
    //   Leaf indices
    // /////////////////////////////////////////////

    //! Gets the leaf index of a UG element
    static int& leafIndex(UG_NS<2>::Element* theElement) {
      return theElement->ge.leafIndex;
    }

    //! Gets the leaf index of a UG element
    static const int& leafIndex(const UG_NS<2>::Element* theElement) {
      return theElement->ge.leafIndex;
    }

    //! Gets the leaf index of a UG sidevector
    static int& leafIndex(Vector* theVector) {
      return reinterpret_cast<int &>(theVector->skip);
    }

    //! Gets the leaf index of a UG sidevector
    static const int& leafIndex(const Vector* theVector) {
      return reinterpret_cast<const int &>(theVector->skip);
    }

    //! Gets the leaf index of a UG edge
    static int& leafIndex(UG_NS<2>::Edge* theEdge) {
      return theEdge->leafIndex;
    }

    //! Gets the leaf index of a UG edge
    static const int& leafIndex(const UG_NS<2>::Edge* theEdge) {
      return theEdge->leafIndex;
    }

    //! Gets the leaf index of a UG node
    static int& leafIndex(UG_NS<2>::Node* theNode) {
      return theNode->myvertex->iv.leafIndex;
    }

    //! Gets the leaf index of a UG node
    static const int& leafIndex(const UG_NS<2>::Node* theNode) {
      return theNode->myvertex->iv.leafIndex;
    }

    // /////////////////////////////////////////////
    //   IDs
    // /////////////////////////////////////////////

    //! Gets the index of a UG element
    static unsigned int id(const UG_NS<2>::Element* theElement) {
      return theElement->ge.id;
    }

    //! Gets the index of a UG node
    static unsigned int id(const UG_NS<2>::Node* theNode) {
      return theNode->myvertex->iv.id | 0x80000000;
    }

    //! \todo Please doc me!
    static void Local_To_Global(int n, double** y,
                                const FieldVector<double, 2>& local,
                                FieldVector<double, 2>& global) {
      LOCAL_TO_GLOBAL(n,y,local,global);
    }

    /**
     * \param n Number of corners of the element
     * \param x Coordinates of the corners of the element
     * \param local Local evaluation point
     *
     * \return The return type is int because the macro INVERSE_TRANSFORMATION
     *  return 1 on failure.
     */
    static int Transformation(int n, double** x,
                              const FieldVector<double, 2>& local, FieldMatrix<double,2,2>& mat) {
      using UG::D2::DOUBLE_VECTOR;
      using UG::DOUBLE;
      double det;
#ifndef SMALL_D
      const double SMALL_D = DBL_EPSILON*10;
#endif
      INVERSE_TRANSFORMATION(n, x, local, mat, det);
      return 0;
    }

    /** \brief Compute the integration element on an element face
        \param nc Number of corners of the element face
        \param co_global Coordinates of the corners of the face
        \param ip_local Local coordinates where integration element is to be evaluated
        \todo Currently, this method is not actually called
     */
    static double SurfaceElement(int nc,
                                 double co_global[MAX_CORNERS_OF_ELEM][2],
                                 double ip_local[2]) {
      double result;
      if (UG::D2::SurfaceElement(2, nc, co_global, ip_local, &result))
        DUNE_THROW(GridError, "UG::D2::SurfaceElement returned error code!");
      return result;
    }

    //! Returns the i-th corner of a UG element
    static UG_NS<2>::Node* Corner(const UG_NS<2>::Element* theElement, int i) {
      using UG::D2::NODE;
      using UG::D2::n_offset;
      using UG::UINT;
      return CORNER(theElement, i);
    }

    //! get edge from node i to node j (in UG's numbering !
    static UG_NS<2>::Edge* GetEdge (UG_NS<2>::Node* nodei, UG_NS<2>::Node* nodej) {
      return UG::D2::GetEdge(nodei,nodej);
    }

    //! access side vector from element (this is just a dummy to compile code also in 2d)
    static Vector* SideVector (const UG_NS<2>::Element* theElement, int i)
    {
      DUNE_THROW(GridError, "side vector only in 3D!");
      return NULL;
    }

    /** \brief Return a pointer to the father of the given element */
    static UG_NS<2>::Element* EFather(const UG_NS<2>::Element* theElement) {
      using UG::D2::ELEMENT;
      using UG::D2::father_offset;
      using UG::UINT;
      return EFATHER(theElement);
    }

    //! get father element of vertex
    static UG_NS<2>::Element* NFather(UG_NS<2>::Node* theNode) {
      return theNode->myvertex->iv.father;
    }

    //! get father node of vertex
    static UG_NS<2>::Node* NodeNodeFather(UG_NS<2>::Node* theNode) {
      using UG::D2::NDOBJ;
      using UG::D2::GM_OBJECTS;
      using UG::UINT;
      if (theNode->father==0)
        return 0;         // no father at all
      if (OBJT(theNode->father)==NDOBJ)
        return (UG::D2::node*) theNode->father;
      else
        return 0;         // may be edge or element
    }

    static unsigned int ReadCW(void* obj, int ce) {
      return UG::D2::ReadCW(obj, ce);
    }

    static void WriteCW(void* obj, int ce, int n) {
      UG::D2::WriteCW(obj, ce, n);
    }

    //! \todo Please doc me!
    static int InitUg(int* argcp, char*** argvp) {
      return UG::D2::InitUg(argcp, argvp);
    }

    static void ExitUg() {
      UG::D2::ExitUg();
    }

    static void DisposeMultiGrid(UG::D2::multigrid* mg) {
      UG::D2::DisposeMultiGrid(mg);
    }

#ifndef UG_LGMDOMAIN
    //! \todo Please doc me!
    static void* CreateBoundaryValueProblem(const char* BVPname,
                                            int numOfCoeffFunc,
                                            UG::D2::CoeffProcPtr coeffs[],
                                            int numOfUserFct,
                                            UG::D2::UserProcPtr userfct[]) {
      return UG::D2::CreateBoundaryValueProblem(BVPname, 0, numOfCoeffFunc, coeffs,
                                                numOfUserFct, userfct);
    }
#endif

    static void* BVP_GetByName(const char* bvpName) {
      return UG::D2::BVP_GetByName(bvpName);
    }

    static void Set_Current_BVP(void** thisBVP) {
      UG::D2::Set_Current_BVP(thisBVP);
    }

    //! \todo Please doc me!
    static UG_NS<2>::MultiGrid* GetMultigrid(const char* name) {
      return UG::D2::GetMultigrid(name);
    }

    static int LBCommand(int argc, const char** argv) {
      /** \todo Can we remove the cast? */
      return UG::D2::LBCommand(argc, (char**)argv);
    }

    static int ConfigureCommand(int argc, const char** argv) {
      /** \todo Kann man ConfigureCommand so �ndern da� man auch ohne den cast auskommt? */
      return UG::D2::ConfigureCommand(argc, (char**)argv);
    }

    static int NewCommand(int argc, char** argv) {
      return UG::D2::NewCommand(argc, argv);
    }

    static int CreateFormatCmd(int argc, char** argv) {
      return UG::D2::CreateFormatCmd(argc, argv);
    }

#ifndef UG_LGMDOMAIN
    static void* CreateDomain(const char* name, const double* midPoint, double radius,
                              int segments, int corners, int convex) {
      return UG::D2::CreateDomain(name, midPoint, radius, segments, corners, convex);
    }
#endif

    static void* InsertInnerNode(UG::D2::grid* grid, const double* pos) {
      return UG::D2::InsertInnerNode(grid, pos);
    }

#ifndef UG_LGMDOMAIN
    static void* CreateBoundarySegment(const char *name, int left, int right,
                                       int index, int res,
                                       int *point,
                                       const double *alpha, const double *beta,
                                       UG::D2::BndSegFuncPtr boundarySegmentFunction,
                                       void *userData) {
      return UG::D2::CreateBoundarySegment(name,            // internal name of the boundary segment
                                           left,                    //  id of left subdomain
                                           right,                    //  id of right subdomain
                                           index,                // Index of the segment
                                           UG::D2::NON_PERIODIC,   // I don't know what this means
                                           res,                    // Resolution, only for the UG graphics
                                           point,
                                           alpha,
                                           beta,
                                           boundarySegmentFunction,
                                           userData);
    }
#endif

  };

  template <>
  class UG_NS<2>::Entity<0>
  {
  public:
    typedef UG::D2::element T;
  };

  template <>
  class UG_NS<2>::Entity<1>
  {
  public:
    typedef UG::D2::edge T;
  };

  template <>
  class UG_NS<2>::Entity<2>
  {
  public:
    typedef UG::D2::node T;
  };


} // namespace Dune

#endif
