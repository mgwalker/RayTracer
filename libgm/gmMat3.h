// gmMatrix3.h - 3x3 element matrix class
//
// libgm++: Graphics Math Library
// Ferdi Scheepers and Stephen F May
// 15 June 1994
// 1-17-2005: Interface with C++ Standard Library: removed bool.h type; 
//    renamed headers (J. Edward Swan II)
// 1-29-2005: Added input and output operators; fixed an indexing problem 
//    (J. Edward Swan II)

#ifndef GMMATRIX3_H
#define GMMATRIX3_H

#include <cassert>
#include "gmUtils.h"

class gmVector2;
class gmVector3;

class gmMatrix3 {

protected:
  double m_[3][3];

public:
  gmMatrix3();
  gmMatrix3(const gmMatrix3&);
  gmMatrix3(double, double, double,
  	    double, double, double,
            double, double, double);

  // array access

  double* operator [](int);
  const double* operator [](int) const;

  // assignment
  
  gmMatrix3& assign(double, double, double,
		    double, double, double,
                    double, double, double);
  gmMatrix3& operator =(const gmMatrix3&);

  // operators

  gmMatrix3& operator +=(const gmMatrix3&);
  gmMatrix3& operator -=(const gmMatrix3&);
  gmMatrix3& operator *=(const gmMatrix3&);
  gmMatrix3& operator *=(double);
  gmMatrix3& operator /=(double);
 
  gmMatrix3 operator +(const gmMatrix3&) const;
  gmMatrix3 operator -(const gmMatrix3&) const;
  gmMatrix3 operator -() const;
  gmMatrix3 operator *(const gmMatrix3&) const;
  gmMatrix3 operator *(double) const;
  gmMatrix3 operator /(double) const;

  friend gmMatrix3 operator *(double, const gmMatrix3&);

  bool operator ==(const gmMatrix3&) const; // fuzzy equaltiy
  bool operator !=(const gmMatrix3&) const;

  gmVector3 operator *(const gmVector3&) const;

  friend gmVector3 operator *(const gmVector3&, const gmMatrix3&);

  // operations

  gmMatrix3 inverse() const;
  gmMatrix3 transpose() const;
  gmMatrix3 adjoint() const;
  
  double determinant() const;
  bool isSingular() const;

  gmVector2 transform(const gmVector2&) const;
  
  void copyTo(float [3][3]) const;
  void copyTo(double [3][3]) const;

  // transformation matrices

  static gmMatrix3 identity();
  static gmMatrix3 rotate(double);
  static gmMatrix3 scale(double, double);
  static gmMatrix3 translate(double, double);

  // input & output

  friend std::istream & operator >> ( std::istream &, gmMatrix3 & );
  friend std::ostream & operator << ( std::ostream &, const gmMatrix3 & );
};

// ARRAY ACCESS

inline double* gmMatrix3::operator [](int i)
{
  assert(i == 0 || i == 1 || i == 2);
  return &m_[i][0];
}

inline const double* gmMatrix3::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2);
  return &m_[i][0];
}

inline void gmMatrix3::copyTo(float f[3][3]) const
{
  f[0][0] = m_[0][0]; f[0][1] = m_[0][1]; f[0][2] = m_[0][2];
  f[1][0] = m_[1][0]; f[1][1] = m_[1][1]; f[1][2] = m_[1][2];
  f[2][0] = m_[2][0]; f[2][1] = m_[2][1]; f[2][2] = m_[2][2];
}

inline void gmMatrix3::copyTo(double f[3][3]) const
{
  f[0][0] = m_[0][0]; f[0][1] = m_[0][1]; f[0][2] = m_[0][2];
  f[1][0] = m_[1][0]; f[1][1] = m_[1][1]; f[1][2] = m_[1][2];
  f[2][0] = m_[2][0]; f[2][1] = m_[2][1]; f[2][2] = m_[2][2];
}

#endif


