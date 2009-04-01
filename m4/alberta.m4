# $Id: alberta.m4 5156 2008-04-14 09:28:06Z christi $
# searches for alberta-headers and libs

AC_DEFUN([DUNE_PATH_ALBERTA],[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_PROG_F77])
  AC_REQUIRE([AC_PATH_XTRA])
  AC_REQUIRE([DUNE_PATH_OPENGL])
  AC_REQUIRE([DUNE_ALBERTA_DIMENSION])

  AC_ARG_WITH(alberta,
    AC_HELP_STRING([--with-alberta=PATH],[directory where ALBERTA (ALBERTA
    version 2.0 or higher) is installed]))

  # do not use alberta debug lib 
  with_alberta_debug=0

  # store old values
  ac_save_LDFLAGS="$LDFLAGS"
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LIBS="$LIBS"
  # LIBS=""

  ## do nothing if no --with-alberta was supplied
  if test x$with_alberta != x && test x$with_alberta != xno ; then

    # initialize to some default value
    ALBERTAROOT="/usr/local/alberta"
    # is --with-alberta=PATH used?
    if test -d $with_alberta ; then
      ALBERTAROOT=`cd $with_alberta && pwd`
    elif test "x$with_alberta" != "xyes" ; then
      AC_MSG_WARN([ALBERTA directory '$with_alberta' does not exist])
    fi


    ALBERTA_LIB_PATH="$ALBERTAROOT/lib"
    # take both include paths 
    ALBERTA_INCLUDE_PATH="$ALBERTAROOT/include/alberta"
    ALBERTA_VERSION="2.0"

    # set variables so that tests can use them
    REM_CPPFLAGS=$CPPFLAGS

    LDFLAGS="$LDFLAGS -L$ALBERTA_LIB_PATH"
    ALBERTADIM="-DDIM_OF_WORLD=$with_alberta_dim"
    CPPFLAGS="$CPPFLAGS $ALBERTADIM -DEL_INDEX=0 -I$ALBERTA_INCLUDE_PATH"

    # check for header
    AC_CHECK_HEADER([alberta.h], 
       [ALBERTA_CPPFLAGS="-I$ALBERTA_INCLUDE_PATH -DENABLE_ALBERTA"
      HAVE_ALBERTA="1"],
      AC_MSG_WARN([alberta.h not found in $ALBERTA_INCLUDE_PATH]))

    if test "x$HAVE_ALBERTA" = "x1" ; then
      AC_CHECK_MEMBER([struct el_info.wall_bound],[ALBERTA_VERSION="2.1"],[],[#include <alberta.h>])
    fi

    CPPFLAGS="$REM_CPPFLAGS -I$ALBERTA_INCLUDE_PATH"
    REM_CPPFLAGS=

    # TODO: check if static flag exists 
    # link_static_flag defines the flag for the linker to link only static
    # didnt work, with $link_static_flag, so quick hack here

    # check for libalberta_util...
    if test "x$HAVE_ALBERTA" = "x1" ; then
      AC_CHECK_LIB(alberta_util,[alberta_calloc],
        [ALBERTA_LIBS="-lalberta_util"
         ALBERTA_LDFLAGS="-L$ALBERTA_LIB_PATH"
         LIBS="$LIBS $ALBERTA_LIBS"],
        [HAVE_ALBERTA="0"
         AC_MSG_WARN(-lalberta_util not found!)])
    fi

    # check for ALBERTA grid library...
    if test "x$HAVE_ALBERTA" = "x1" ; then
      # construct libname
      # define varaible lib name depending on problem and world dim, to change
      # afterwards easily 
      variablealbertalibname="alberta_$``(``ALBERTA_DIM``)``d"
      alberta_1d_libname="alberta_1d"
      alberta_2d_libname="alberta_2d"
      alberta_3d_libname="alberta_3d"

      # we do not check libraries for ALBERTA 2.1 (linking would require libtool)
      if test "$ALBERTA_VERSION" == "2.1" ; then
        AC_MSG_WARN([ALBERTA $ALBERTA_VERSION found -- Skipping check for ALBERTA grid libraries])
      else
        AC_CHECK_LIB($alberta_1d_libname,[mesh_traverse], [],
          [HAVE_ALBERTA="0"
           AC_MSG_WARN(-l$alberta_1d_libname not found!)])
        AC_CHECK_LIB($alberta_2d_libname,[mesh_traverse], [],
          [HAVE_ALBERTA="0"
           AC_MSG_WARN(-l$alberta_2d_libname not found!)])
        AC_CHECK_LIB($alberta_3d_libname,[mesh_traverse], [],
          [HAVE_ALBERTA="0"
           AC_MSG_WARN(-l$alberta_3d_libname not found!)])
      fi
      ALBERTA_LIBS="-l$variablealbertalibname $ALBERTA_LIBS $ALBERTA_EXTRA"
      ALBERTA_1DLIBS="-l$alberta_1d_libname $ALBERTA_LIBS $ALBERTA_EXTRA"
      ALBERTA_2DLIBS="-l$alberta_2d_libname $ALBERTA_LIBS $ALBERTA_EXTRA"
      ALBERTA_3DLIBS="-l$alberta_3d_libname $ALBERTA_LIBS $ALBERTA_EXTRA"
    fi

  fi  # end of alberta check (--without wasn't set)

  # survived all tests?
  if test "x$HAVE_ALBERTA" = "x1" ; then
    AC_SUBST(ALBERTA_LIBS, $ALBERTA_LIBS)
    AC_SUBST(ALBERTA_1DLIBS, $ALBERTA_1DLIBS)
    AC_SUBST(ALBERTA_2DLIBS, $ALBERTA_2DLIBS)
    AC_SUBST(ALBERTA_3DLIBS, $ALBERTA_3DLIBS)
    AC_SUBST(ALBERTA_LDFLAGS, $ALBERTA_LDFLAGS)
    AC_SUBST(ALBERTA_CPPFLAGS, $ALBERTA_CPPFLAGS)
    AC_DEFINE(HAVE_ALBERTA, ENABLE_ALBERTA,
      [This is only true if alberta-library was found by configure 
       _and_ if the application uses the ALBERTA_CPPFLAGS])

    if test "$ALBERTA_VERSION" = "2.0" ; then
      AC_DEFINE([DUNE_ALBERTA_VERSION], [0x200], [Alberta version found by configure])
    elif test "$ALBERTA_VERSION" = "2.1" ; then
      AC_DEFINE([DUNE_ALBERTA_VERSION], [0x201], [Alberta version found by configure])
    else
      AC_MSG_ERROR([Internal Inconsistency: Invalid Alberta version reported: $ALBERTA_VERSION.])
    fi

    # add to global list
    DUNE_PKG_LDFLAGS="$DUNE_PKG_LDFLAGS $ALBERTA_LDFLAGS"
    DUNE_PKG_LIBS="$DUNE_PKG_LIBS $ALBERTA_LIBS"
    DUNE_PKG_CPPFLAGS="$DUNE_PKG_CPPFLAGS $ALBERTA_CPPFLAGS"

    # set variable for summary
    with_alberta="yes (Version $ALBERTA_VERSION)"
  else
    AC_SUBST(ALBERTA_LIBS, "")
    AC_SUBST(ALBERTA_1DLIBS, "")
    AC_SUBST(ALBERTA_2DLIBS, "")
    AC_SUBST(ALBERTA_3DLIBS, "")
    AC_SUBST(ALBERTA_LDFLAGS, "")
    AC_SUBST(ALBERTA_CPPFLAGS, "")

    # set variable for summary
    with_alberta="no"
  fi
    
  # also tell automake
  AM_CONDITIONAL(ALBERTA, test x$HAVE_ALBERTA = x1)

  # reset old values
  LIBS="$ac_save_LIBS"
  CPPFLAGS="$ac_save_CPPFLAGS"
  LDFLAGS="$ac_save_LDFLAGS"

  DUNE_ADD_SUMMARY_ENTRY([ALBERTA],[$with_alberta])
])

# asks for alberta-dim to pass on to Alberta
AC_DEFUN([DUNE_ALBERTA_DIMENSION],[
  AC_REQUIRE([DUNE_GRID_DIMENSION])

  AC_ARG_WITH(alberta_dim,
              AC_HELP_STRING([--with-alberta-dim=1|2|3],
                [dimension of world enclosing the ALBERTA grid (default=world-dim if delivered, otherwise 2)]),,
              with_alberta_dim=0)

  if test x$with_alberta_dim != x0 ; then
    variablealbertadim=$with_alberta_dim
  else
    with_alberta_dim=2
    if test x$with_world_dim != x0 || test x$with_grid_dim != x0 ; then
      variablealbertadim='$(WORLDDIM)'
    else
      variablealbertadim=2
    fi
  fi

  AC_SUBST(ALBERTA_DIM, $variablealbertadim ) 
  #AC_DEFINE_UNQUOTED(ALBERTA_DIM, $with_alberta_dim, [World dimension of ALBERTA grid])
])
