/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* (c) Copyright 1995 by                                                     */
/*     SINTEF, Oslo, Norway                                                  */
/*     All rights reserved. See the copyright.h for more details.            */
/*                                                                           */
/*****************************************************************************/

#include "copyright.h"

/*
 *
 * $Id: s2503.c,v 1.7 1995-06-28 11:06:50 jka Exp $
 *
 */


#define S2503

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void s2503(SISLSurf *surf, int ider, double derive[], double normal[],
	   double *meancurvature, int *jstat)
#else
 void s2503(surf, ider, derive, normal, meancurvature, jstat)
      SISLSurf *surf;
      int ider;
      double derive[],
      double normal[],
      double *meancurvature;
      int *jstat;
#endif
/*
***************************************************************************
*
***************************************************************************
*  PURPOSE      :  To compute the mean curvature H(u,v) of a surface for given
*                  values (u,v). This is a lower level routine, used
*                  for evaluation of many H(u,v)'s.
*
*  INPUT        :
*          surf     - Pointer to the surface to evaluate.
*          ider     - Only implemented for ider=0 (derivative order).
*       derive      - Array containing derivatives from routine s1421().
*                     Size = idim*6.
*       normal      - Array containing the normal from routine s1421().
*                     Size = 3.
*  OUTPUT       :
*    meancurvature  - Mean curvature of the surface in (u,v) =
*        jstat      - Status messages
*
*                         = 0 : Ok.
*                         < 0 : Error.
*
*  METHOD       :  The mean curvature is given by
*
*                      H(x,y) = 0.5((1+hx^2)hyy-2hxhyhxy+(1+hy^2)hxx)/
*                                      ((1+hx^2+hy^2)^3/2),
*
*                  if the surface (h(x,y)) is 1D, and
*
*                      H(u,v) = 0.5(eG-2fF+gE)/(EG-F*F),
*
*                  if the surface (X(u,v)) is 3D. The variables E,F,G,e,f and g
*                  are the coefficients of the first and second fundamental form.
*                  They are given by: e = <N,Xuu>, f = <N,Xuv>, g = <N,Xvv>,
*                  E = <Xu,Xu>, F = <Xu,Xv> and G = <Xv,Xv>. The routine will
*                  test if the surface is degenerate (not regular) or close to
*                  degenerate.
*
*  REFERENCES   :  Differential Geometry of Curves and Surfaces,
*                    (Manfredo P. Do Carmo, Prentice Hall,
*                      ISBN: 0-13-212589-7).
*
*  CALLS        :
*
*  LIMITATIONS  :
*                (i) If the surface is degenerated (not regular) at the point
*                    (u,v), it makes no sense to speak about the mean curvature
*                    H(u,v).
*               (ii) If the surface is closed to degenerate, the mean curvature
*                    H(u,v) can be numerical unstable.
*              (iii) If the surface is Cr the curvature calculated is C(r-2).
*                    To get the correct behavior use the sided evaluator s1422
*		     instead of s1421.
*               (iv) The dimension of the space in which the surface lies must
*                    be 1,2 or 3, if not, jstat = -105 is returned.
*
*
* WRITTEN BY :  Geir Westgaard, SINTEF, Oslo, Norway.             Date: 1995-1
* CORRECTED BY :  Ulf J Krystad, SINTEF, Oslo, Norway.             Date: 1995-1
*                 Removed knot navigators + some clean up.
* CORRECTED BY :  Johannes Kaasa, SINTEF, Oslo, Norway.           Date: 1995-06
*                 Error in explicit curvature.
******************************************************************************
*/
{
  double a,b;           /* Temporary variables.                            */
  double hx,hy,
    hxx,hyy,hxy;        /* The derivatives of the 1D surface, h(x,y).      */
  double E,F,G;         /* The coefficents of the first fundamental form,
			   that is, E = <Xu,Xu>, F = <Xu,Xv>  and
			   G = <Xv,Xv>.                                    */
  double e,f,g;         /* The coefficents of the second fundamental form,
			   that is, e = <N,Xuu>, f = <N,Xuv> and
			   g = <N,Xvv>.                                   */


  if (ider != 0) goto err178;

  if (surf->idim == 1) /* 1D surface */
  {
    hx  = derive[1];
    hy  = derive[2];
    hxx = derive[3];
    hxy = derive[4];
    hyy = derive[5];

    b = (1+hx*hx+hy*hy);
    a = sqrt(b*b*b);

    b = (1.0 + hx*hx)*hyy - 2.0*hx*hy*hxy + (1.0 + hy*hy)*hxx;

    *meancurvature = 0.5*b/a;
  }

  else if (surf->idim == 2) /* 2D surface */
  {
    /* The surface lies in a plane => H(u,v) = 0 */

    *meancurvature = 0.0;
  }
  else if (surf->idim == 3) /* 3D surface */
  {
    /* E = <Xu,Xu> */
    E = derive[3]*derive[3]+derive[4]*derive[4]+derive[5]*derive[5];

      /* F = <Xu,Xv> */
    F = derive[3]*derive[6]+derive[4]*derive[7]+derive[5]*derive[8];

    /* G = <Xv,Xv> */
    G = derive[6]*derive[6]+derive[7]*derive[7]+derive[8]*derive[8];

    /* b = EG + F^2. */
    b = E*G-F*F;

    /* e = <N,Xuu> (/ sqrt(E*G-F*F)) */
    e = normal[0]*derive[9]+normal[1]*derive[10]+normal[2]*derive[11];

    /* f = <N,Xuv> (/ sqrt(E*G-F*F)) */
    f = normal[0]*derive[12]+normal[1]*derive[13]+normal[2]*derive[14];

    /* g = <N,Xvv> (/ sqrt(E*G-F*F)) */
    g = normal[0]*derive[15]+normal[1]*derive[16]+normal[2]*derive[17];

    /* Compute mean curvature = 0.5(eG-2fF+gE)/(E*G-F*F). */
    a = 0.5*(e*G - 2.0*f*F + g*E);
    b = b*sqrt(b);
    *meancurvature = a/b;
  }
  else /* When surf->idim != 1,2 or 3 */
  {
    goto err105;
  }




  /* Successful computations  */

  *jstat = 0;
  goto out;


   /* Error in input, surf->idim != 1,2 or 3 */
err105:
  *jstat = -105;
  s6err("s2503", *jstat, 0);
  goto out;

  /* Illegal derivative requested. */
err178:
  *jstat = -178;
  s6err("s2503",*jstat,0);
  goto out;

out:

  return;
}
