// spline methods taken from netlib pppack. translated to c using f2c.

/* ppvalu.f -- translated by f2c (version 20160102).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

/* Subroutine */ int interv_(float *xt, int *lxt, float *x, int *left, 
	int *mflag)
{
    /* Initialized data */

    static int ilo = 1;

    static int ihi, istep, middle;

/*  from  * a practical guide to splines *  by C. de Boor */
/* omputes  left = max( i :  xt(i) .lt. xt(lxt) .and.  xt(i) .le. x )  . */

/* ******  i n p u t  ****** */
/*  xt.....a real sequence, of length  lxt , assumed to be nondecreasing */
/*  lxt.....number of terms in the sequence  xt . */
/*  x.....the point whose location with respect to the sequence  xt  is */
/*        to be determined. */

/* ******  o u t p u t  ****** */
/*  left, mflag.....both ints, whose value is */

/*   1     -1      if               x .lt.  xt(1) */
/*   i      0      if   xt(i)  .le. x .lt. xt(i+1) */
/*   i      0      if   xt(i)  .lt. x .eq. xt(i+1) .eq. xt(lxt) */
/*   i      1      if   xt(i)  .lt.        xt(i+1) .eq. xt(lxt) .lt. x */

/*        In particular,  mflag = 0  is the 'usual' case.  mflag .ne. 0 */
/*        indicates that  x  lies outside the CLOSED interval */
/*        xt(1) .le. y .le. xt(lxt) . The asymmetric treatment of the */
/*        intervals is due to the decision to make all pp functions cont- */
/*        inuous from the right, but, by returning  mflag = 0  even if */
/*        x = xt(lxt), there is the option of having the computed pp function */
/*        continuous from the left at  xt(lxt) . */

/* ******  m e t h o d  ****** */
/*  The program is designed to be efficient in the common situation that */
/*  it is called repeatedly, with  x  taken from an increasing or decrea- */
/*  sing sequence. This will happen, e.g., when a pp function is to be */
/*  graphed. The first guess for  left  is therefore taken to be the val- */
/*  ue returned at the previous call and stored in the  l o c a l  varia- */
/*  ble  ilo . A first check ascertains that  ilo .lt. lxt (this is nec- */
/*  essary since the present call may have nothing to do with the previ- */
/*  ous call). Then, if  xt(ilo) .le. x .lt. xt(ilo+1), we set  left = */
/*  ilo  and are done after just three comparisons. */
/*     Otherwise, we repeatedly double the difference  istep = ihi - ilo */
/*  while also moving  ilo  and  ihi  in the direction of  x , until */
/*                      xt(ilo) .le. x .lt. xt(ihi) , */
/*  after which we use bisection to get, in addition, ilo+1 = ihi . */
/*  left = ilo  is then returned. */

    /* Parameter adjustments */
    --xt;

    /* Function Body */
    ihi = ilo + 1;
    if (ihi < *lxt) {
	goto L20;
    }
    if (*x >= xt[*lxt]) {
	goto L110;
    }
    if (*lxt <= 1) {
	goto L90;
    }
    ilo = *lxt - 1;
    ihi = *lxt;

L20:
    if (*x >= xt[ihi]) {
	goto L40;
    }
    if (*x >= xt[ilo]) {
	goto L100;
    }

/*              **** now x .lt. xt(ilo) . decrease  ilo  to capture  x . */
    istep = 1;
L31:
    ihi = ilo;
    ilo = ihi - istep;
    if (ilo <= 1) {
	goto L35;
    }
    if (*x >= xt[ilo]) {
	goto L50;
    }
    istep <<= 1;
    goto L31;
L35:
    ilo = 1;
    if (*x < xt[1]) {
	goto L90;
    }
    goto L50;
/*              **** now x .ge. xt(ihi) . increase  ihi  to capture  x . */
L40:
    istep = 1;
L41:
    ilo = ihi;
    ihi = ilo + istep;
    if (ihi >= *lxt) {
	goto L45;
    }
    if (*x < xt[ihi]) {
	goto L50;
    }
    istep <<= 1;
    goto L41;
L45:
    if (*x >= xt[*lxt]) {
	goto L110;
    }
    ihi = *lxt;

/*           **** now xt(ilo) .le. x .lt. xt(ihi) . narrow the interval. */
L50:
    middle = (ilo + ihi) / 2;
    if (middle == ilo) {
	goto L100;
    }
/*     note. it is assumed that middle = ilo in case ihi = ilo+1 . */
    if (*x < xt[middle]) {
	goto L53;
    }
    ilo = middle;
    goto L50;
L53:
    ihi = middle;
    goto L50;
/* **** set output and return. */
L90:
    *mflag = -1;
    *left = 1;
    return 0;
L100:
    *mflag = 0;
    *left = ilo;
    return 0;
L110:
    *mflag = 1;
    if (*x == xt[*lxt]) {
	*mflag = 0;
    }
    *left = *lxt;
L111:
    if (*left == 1) {
	return 0;
    }
    --(*left);
    if (xt[*left] < xt[*lxt]) {
	return 0;
    }
    goto L111;
} /* interv_ */


// The following is translated from ppvalu.f in pppack

float lspleval(float *break__, float *coef, int *l, int *k, float *x,
	 int *jderiv)
{
    /* System generated locals */
    int coef_dim1, coef_offset, i__1;
    float ret_val;

    /* Local variables */
    static float h__;
    static int i__, m;
    static int fmmjdr;
    extern /* Subroutine */ int interv_(float *, int *, float *, int *, 
	    int *);
    static int ndummy;

/*  from  * a practical guide to splines *  by c. de boor */
/* alls  interv */
/* alculates value at  x  of  jderiv-th derivative of pp fct from pp-repr */

/* ******  i n p u t  ****** */
/*  break, coef, l, k.....forms the pp-representation of the function  f */
/*        to be evaluated. specifically, the j-th derivative of  f  is */
/*        given by */

/*     (d**j)f(x) = coef(j+1,i) + h*(coef(j+2,i) + h*( ... (coef(k-1,i) + */
/*                             + h*coef(k,i)/(k-j-1))/(k-j-2) ... )/2)/1 */

/*        with  h = x - break(i),  and */

/*       i  =  max( 1 , max( j ,  break(j) .le. x , 1 .le. j .le. l ) ). */

/*  x.....the point at which to evaluate. */
/*  jderiv.....integer giving the order of the derivative to be evaluat- */
/*        ed.  a s s u m e d  to be zero or positive. */

/* ******  o u t p u t  ****** */
/*  ppvalu.....the value of the (jderiv)-th derivative of  f  at  x. */

/* ******  m e t h o d  ****** */
/*     the interval index  i , appropriate for  x , is found through a */
/*  call to  interv . the formula above for the  jderiv-th derivative */
/*  of  f  is then evaluated (by nested multiplication). */

    /* Parameter adjustments */
    --break__;
    coef_dim1 = *k;
    coef_offset = 1 + coef_dim1;
    coef -= coef_offset;

    /* Function Body */
    ret_val = 0.f;
    fmmjdr = (int) (*k - *jderiv);
/*              derivatives of order  k  or higher are identically zero. */
    if (fmmjdr <= 0) {
	goto L99;
    }

/*              find index  i  of largest breakpoint to the left of  x . */
    i__1 = *l + 1;
    interv_(&break__[1], &i__1, x, &i__, &ndummy);

/*      Evaluate  jderiv-th derivative of  i-th polynomial piece at  x . */
    h__ = *x - break__[i__];
    m = *k;
L9:
    ret_val = ret_val / fmmjdr * h__ + coef[m + i__ * coef_dim1];
    --m;
    fmmjdr -= 1;
    if (fmmjdr > 0) {
	goto L9;
    }
L99:
    return ret_val;
} /* ppvalu_ */


// The following is translated from cubspl in pppack

/* Subroutine */ int lspline(float *tau, float *c__, int *n, int *
	ibcbeg, int *ibcend)
{
    /* System generated locals */
    int i__1;
    float r__1;

    /* Local variables */
    static float g;
    static int i__, j, l, m;
    static float dtau, divdf1, divdf3;

/*  from  * a practical guide to splines *  by c. de boor */
/*     ************************  input  *************************** */
/*     n = number of data points. assumed to be .ge. 2. */
/*     (tau(i), c(1,i), i=1,...,n) = abscissae and ordinates of the */
/*        data points. tau is assumed to be strictly increasing. */
/*     ibcbeg, ibcend = boundary condition indicators, and */
/*     c(2,1), c(2,n) = boundary condition information. specifically, */
/*        ibcbeg = 0  means no boundary condition at tau(1) is given. */
/*           in this case, the not-a-knot condition is used, i.e. the */
/*           jump in the third derivative across tau(2) is forced to */
/*           zero, thus the first and the second cubic polynomial pieces */
/*           are made to coincide.) */
/*        ibcbeg = 1  means that the slope at tau(1) is made to equal */
/*           c(2,1), supplied by input. */
/*        ibcbeg = 2  means that the second derivative at tau(1) is */
/*           made to equal c(2,1), supplied by input. */
/*        ibcend = 0, 1, or 2 has analogous meaning concerning the */
/*           boundary condition at tau(n), with the additional infor- */
/*           mation taken from c(2,n). */
/*     ***********************  output  ************************** */
/*     c(j,i), j=1,...,4; i=1,...,l (= n-1) = the polynomial coefficients */
/*        of the cubic interpolating spline with interior knots (or */
/*        joints) tau(2), ..., tau(n-1). precisely, in the interval */
/*        (tau(i), tau(i+1)), the spline f is given by */
/*           f(x) = c(1,i)+h*(c(2,i)+h*(c(3,i)+h*c(4,i)/3.)/2.) */
/*        where h = x - tau(i). the function program *ppvalu* may be */
/*        used to evaluate f or its derivatives from tau,c, l = n-1, */
/*        and k=4. */
/* ****** a tridiagonal linear system for the unknown slopes s(i) of */
/*  f  at tau(i), i=1,...,n, is generated and then solved by gauss elim- */
/*  ination, with s(i) ending up in c(2,i), all i. */
/*     c(3,.) and c(4,.) are used initially for temporary storage. */
    /* Parameter adjustments */
    c__ -= 5;
    --tau;

    /* Function Body */
    l = *n - 1;
/* ompute first differences of tau sequence and store in c(3,.). also, */
/* ompute first divided difference of data and store in c(4,.). */
    i__1 = *n;
    for (m = 2; m <= i__1; ++m) {
	c__[(m << 2) + 3] = tau[m] - tau[m - 1];
/* L10: */
	c__[(m << 2) + 4] = (c__[(m << 2) + 1] - c__[((m - 1) << 2) + 1]) / c__[
		(m << 2) + 3];
    }
/* onstruct first equation from the boundary condition, of the form */
/*             c(4,1)*s(1) + c(3,1)*s(2) = c(2,1) */
    if ((i__1 = *ibcbeg - 1) < 0) {
	goto L11;
    } else if (i__1 == 0) {
	goto L15;
    } else {
	goto L16;
    }
L11:
    if (*n > 2) {
	goto L12;
    }
/*     no condition at left end and n = 2. */
    c__[8] = 1.f;
    c__[7] = 1.f;
    c__[6] = c__[12] * 2.f;
    goto L25;
/*     not-a-knot condition at left end and n .gt. 2. */
L12:
    c__[8] = c__[15];
    c__[7] = c__[11] + c__[15];
/* Computing 2nd power */
    r__1 = c__[11];
    c__[6] = ((c__[11] + c__[7] * 2.f) * c__[12] * c__[15] + r__1 * r__1 * 
	    c__[16]) / c__[7];
    goto L19;
/*     slope prescribed at left end. */
L15:
    c__[8] = 1.f;
    c__[7] = 0.f;
    goto L18;
/*     second derivative prescribed at left end. */
L16:
    c__[8] = 2.f;
    c__[7] = 1.f;
    c__[6] = c__[12] * 3.f - c__[11] / 2.f * c__[6];
L18:
    if (*n == 2) {
	goto L25;
    }
/*  if there are interior knots, generate the corresp. equations and car- */
/*  ry out the forward pass of gauss elimination, after which the m-th */
/*  equation reads    c(4,m)*s(m) + c(3,m)*s(m+1) = c(2,m). */
L19:
    i__1 = l;
    for (m = 2; m <= i__1; ++m) {
      g = -c__[((m + 1) << 2) + 3] / c__[((m - 1) << 2) + 4];
      c__[(m << 2) + 2] = g * c__[((m - 1) << 2) + 2] + (c__[(m << 2) + 3] * 
							 c__[((m + 1) << 2) + 4] + c__[((m + 1) << 2) + 3] * c__[(m << 2) 
														 + 4]) * 3.f;
/* L20: */
      c__[(m << 2) + 4] = g * c__[((m - 1) << 2) + 3] + (c__[(m << 2) + 3] + 
							 c__[((m + 1) << 2) + 3]) * 2.f;
    }
/* onstruct last equation from the second boundary condition, of the form */
/*           (-g*c(4,n-1))*s(n-1) + c(4,n)*s(n) = c(2,n) */
/*     if slope is prescribed at right end, one can go directly to back- */
/*     substitution, since c array happens to be set up just right for it */
/*     at this point. */
    if ((i__1 = *ibcend - 1) < 0) {
	goto L21;
    } else if (i__1 == 0) {
	goto L30;
    } else {
	goto L24;
    }
L21:
    if (*n == 3 && *ibcbeg == 0) {
	goto L22;
    }
/*     not-a-knot and n .ge. 3, and either n.gt.3 or  also not-a-knot at */
/*     left end point. */
    g = c__[((*n - 1) << 2) + 3] + c__[(*n << 2) + 3];
/* Computing 2nd power */
    r__1 = c__[(*n << 2) + 3];
    c__[(*n << 2) + 2] = ((c__[(*n << 2) + 3] + g * 2.f) * c__[(*n << 2) + 4] 
			  * c__[((*n - 1) << 2) + 3] + r__1 * r__1 * (c__[((*n - 1) << 2) + 1] 
								      - c__[((*n - 2) << 2) + 1]) / c__[((*n - 1) << 2) + 3]) / g;
    g = -g / c__[((*n - 1) << 2) + 4];
    c__[(*n << 2) + 4] = c__[((*n - 1) << 2) + 3];
    goto L29;
/*     either (n=3 and not-a-knot also at left) or (n=2 and not not-a- */
/*     knot at left end point). */
L22:
    c__[(*n << 2) + 2] = c__[(*n << 2) + 4] * 2.f;
    c__[(*n << 2) + 4] = 1.f;
    goto L28;
/*     second derivative prescribed at right endpoint. */
L24:
    c__[(*n << 2) + 2] = c__[(*n << 2) + 4] * 3.f + c__[(*n << 2) + 3] / 2.f *
	     c__[(*n << 2) + 2];
    c__[(*n << 2) + 4] = 2.f;
    goto L28;
L25:
    if ((i__1 = *ibcend - 1) < 0) {
	goto L26;
    } else if (i__1 == 0) {
	goto L30;
    } else {
	goto L24;
    }
L26:
    if (*ibcbeg > 0) {
	goto L22;
    }
/*     not-a-knot at right endpoint and at left endpoint and n = 2. */
    c__[(*n << 2) + 2] = c__[(*n << 2) + 4];
    goto L30;
L28:
    g = -1.f / c__[((*n - 1) << 2) + 4];
/* omplete forward pass of gauss elimination. */
L29:
    c__[(*n << 2) + 4] = g * c__[((*n - 1) << 2) + 3] + c__[(*n << 2) + 4];
    c__[(*n << 2) + 2] = (g * c__[((*n - 1) << 2) + 2] + c__[(*n << 2) + 2]) / 
	    c__[(*n << 2) + 4];
/* arry out back substitution */
L30:
    j = l;
L40:
    c__[(j << 2) + 2] = (c__[(j << 2) + 2] - c__[(j << 2) + 3] * c__[((j + 1) <<
	     2) + 2]) / c__[(j << 2) + 4];
    --j;
    if (j > 0) {
	goto L40;
    }
/* ****** generate cubic coefficients in each interval, i.e., the deriv.s */
/*  at its left endpoint, from value and slope at its endpoints. */
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	dtau = c__[(i__ << 2) + 3];
	divdf1 = (c__[(i__ << 2) + 1] - c__[((i__ - 1) << 2) + 1]) / dtau;
	divdf3 = c__[((i__ - 1) << 2) + 2] + c__[(i__ << 2) + 2] - divdf1 * 2.f;
	c__[((i__ - 1) << 2) + 3] = (divdf1 - c__[((i__ - 1) << 2) + 2] - divdf3) 
		* 2.f / dtau;
/* L50: */
	c__[((i__ - 1) << 2) + 4] = divdf3 / dtau * (6.f / dtau);
    }
    return 0;
} /* cubspl_ */

