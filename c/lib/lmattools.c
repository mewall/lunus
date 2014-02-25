#include<mwmask.h>

struct xyzmatrix lrotmat(float rotx, float roty, float rotz)
{ 
  //Define Rx, Ry, and Rz:

  struct xyzmatrix R,Rx,Ry,Rz;

  Rx.xx = 1.; Rx.xy = 0.; Rx.xz = 0.;
  Rx.yx = 0.; Rx.yy = cosf(rotx); Rx.yz = sinf(rotx);
  Rx.zx = 0.; Rx.zy = -sinf(rotx); Rx.zz = cosf(rotx);

  Ry.xx = cosf(roty); Ry.xy = 0.; Ry.xz = -sinf(roty);
  Ry.yx = 0.; Ry.yy = 1.; Ry.yz = 0.;
  Ry.zx = sinf(roty); Ry.zy = 0.; Ry.zz = cosf(roty);

  Rz.xx = cosf(rotz); Rz.xy = sinf(rotz); Rz.xz = 0.;
  Rz.yx = -sinf(rotz); Rz.yy = cosf(rotz); Rz.yz = 0.;
  Rz.zx = 0.; Rz.zy = 0.; Rz.zz = 1.;

  R = lmatmul(Ry,Rx);
  R = lmatmul(Rz,R);

  return R;

}

struct xyzmatrix lmatmul(struct xyzmatrix a, struct xyzmatrix b)
{
  struct xyzmatrix c;

  c.xx = a.xx*b.xx + a.xy*b.yx + a.xz*b.zx;
  c.xy = a.xx*b.xy + a.xy*b.yy + a.xz*b.zy;
  c.xz = a.xx*b.xz + a.xy*b.yz + a.xz*b.zz;

  c.yx = a.yx*b.xx + a.yy*b.yx + a.yz*b.zx;
  c.yy = a.yx*b.xy + a.yy*b.yy + a.yz*b.zy;
  c.yz = a.yx*b.xz + a.yy*b.yz + a.yz*b.zz;

  c.zx = a.zx*b.xx + a.zy*b.yx + a.zz*b.zx;
  c.zy = a.zx*b.xy + a.zy*b.yy + a.zz*b.zy;
  c.zz = a.zx*b.xz + a.zy*b.yz + a.zz*b.zz;

  return c;
}

struct xyzcoords lmatvecmul(struct xyzmatrix b,struct xyzcoords a)
{
  struct xyzcoords c;

  c.x = b.xx*a.x + b.xy*a.y + b.xz*a.z;
  c.y = b.yx*a.x + b.yy*a.y + b.yz*a.z;
  c.z = b.zx*a.x + b.zy*a.y + b.zz*a.z;

  return c;
}

XYZCOORDS_DATA ldotvec(struct xyzcoords b,struct xyzcoords a)
{
  XYZCOORDS_DATA c;

  c = a.x*b.x + a.y*b.y + a.z*b.z;

  return c;
}

struct xyzcoords linvvec(struct xyzcoords a)
{
  struct xyzcoords c;

  c.x = - a.x;
  c.y = - a.y;
  c.z = - a.z;

  return c;
}

struct xyzcoords laddvec(struct xyzcoords b,struct xyzcoords a)
{
  struct xyzcoords c;

  c.x = a.x + b.x;
  c.y = a.y + b.y;
  c.z = a.z + b.z;

  return c;
}

struct xyzcoords lsubvec(struct xyzcoords b,struct xyzcoords a)
{
  struct xyzcoords c;

  c.x = b.x - a.x;
  c.y = b.y - a.y;
  c.z = b.z - a.z;

  return c;
}

struct xyzcoords lmulscvec(XYZCOORDS_DATA b,struct xyzcoords a)
{
  struct xyzcoords c;

  c.x = b*a.x;
  c.y = b*a.y;
  c.z = b*a.z;

  return c;
}
