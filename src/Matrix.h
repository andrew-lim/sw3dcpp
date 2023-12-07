#ifndef ANDREW_LIM_GRAPHICS_MATRIX_H
#define ANDREW_LIM_GRAPHICS_MATRIX_H

#include <cmath>

namespace al { namespace graphics {

template <class T>
class Matrix4
{
public:
  T values[16];

  Matrix4()
  {
    fill(0);
    values[0] = 1;
    values[5] = 1;
    values[10] = 1;
    values[15] = 1;
  }

  Matrix4(T v)
  {
    fill(v);
  }

  Matrix4(T t1, T t2, T t3, T t4, T t5, T t6, T t7, T t8,
          T t9, T t10, T t11, T t12, T t13, T t14, T t15, T t16)
  {
    fill(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16);
  }

  T get(int i) const
  {
    return values[i];
  }

  T* operator[](int col)
  {
    return &(values[col*4]);
  }

  const T* operator[](int col) const
  {
    return &(values[col*4]);
  }

  T operator()(int i) const
  {
    return values[i];
  }

  T& get(int col, int row)
  {
    return values[col * 4 + row];
  }

  T get(int col, int row) const
  {
    return values[col * 4 + row];
  }

  T& operator()(int col, int row)
  {
    return values[col * 4 + row];
  }

  T operator()(int col, int row) const
  {
    return values[col * 4 + row];
  }

  void set(int index, T val)
  {
    values[index] = val;
  }

  void set(int col, int row, T val)
  {
    values[col * 4 + row] = val;
  }

  void fill(T val)
  {
    for (int i=0; i<16; i++) {
      values[i] = val;
    }
  }

  void fill(T t1, T t2, T t3, T t4, T t5, T t6, T t7, T t8,
            T t9, T t10, T t11, T t12, T t13, T t14, T t15, T t16)
  {
    values[0] = t1;
    values[1] = t2;
    values[2] = t3;
    values[3] = t4;

    values[4] = t5;
    values[5] = t6;
    values[6] = t7;
    values[7] = t8;

    values[8] = t9;
    values[9] = t10;
    values[10] = t11;
    values[11] = t12;

    values[12] = t13;
    values[13] = t14;
    values[14] = t15;
    values[15] = t16;
  }

  bool equals(Matrix4& m) const {
    return
    values[0] == m.values[0] &&
    values[1] == m.values[1] &&
    values[2] == m.values[2] &&
    values[3] == m.values[3] &&

    values[4] == m.values[4] &&
    values[5] == m.values[5] &&
    values[6] == m.values[6] &&
    values[7] == m.values[7] &&

    values[8] == m.values[8] &&
    values[9] == m.values[9] &&
    values[10] == m.values[10] &&
    values[11] == m.values[11] &&

    values[12] == m.values[12] &&
    values[13] == m.values[13] &&
    values[14] == m.values[14] &&
    values[15] == m.values[15] ;
  }

  Matrix4 mul(const Matrix4& m) const
  {
    Matrix4 r;

    for(int row = 0; row < 4; row++) {
      for(int col = 0; col < 4; col++) {
        r.set(col, row,
          get(0,row) * m.get(col, 0) +
          get(1,row) * m.get(col, 1) +
          get(2,row) * m.get(col, 2) +
          get(3,row) * m.get(col, 3)
        );
      } // col
    } // row

    return r;
  }

  Vector4g<T> mul(const Vector4g<T>& vec) const
  {
    return Vector4g<T>(
      get(0)*vec[0] + get(4)*vec[1] + get(8) *vec[2] + get(12)*vec[3],
      get(1)*vec[0] + get(5)*vec[1] + get(9) *vec[2] + get(13)*vec[3],
      get(2)*vec[0] + get(6)*vec[1] + get(10)*vec[2] + get(14)*vec[3],
      get(3)*vec[0] + get(7)*vec[1] + get(11)*vec[2] + get(15)*vec[3]
    );
  }

  Matrix4 operator*(const Matrix4& m) const
  {
    return mul(m);
  }

  Vector4g<T> operator*(const Vector4g<T>& vec) const
  {
    return mul(vec);
  }

  static Matrix4 makeTranslation(T x, T y, T z)
  {
    Matrix4 m;
    m.set(12, x);
    m.set(13, y);
    m.set(14, z);
    return m;
  }

  static Matrix4 makeXRotation(T rad)
  {
    const T s = sin(rad);
    const T c = cos(rad);
    Matrix4 m;
    m.set(1, 1, c);
    m.set(1, 2, -s);
    m.set(2, 1, s);
    m.set(2, 2, c);
    return m;
  }

  static Matrix4 makeYRotation(T rad)
  {
    const T s = sin(rad);
    const T c = cos(rad);
    Matrix4 m;
    m.set(0, 0, c);
    m.set(0, 2, s);
    m.set(2, 0, -s);
    m.set(2, 2, c);
    return m;
  }

  static Matrix4 makePerspective(T fovy, T aspect, T zNear, T zFar)
  {
    // See glm perspectiveRH_NO
    // https://github.com/g-truc/glm/blob/47585fde0c49fa77a2bf2fb1d2ead06999fd4b6e/glm/ext/matrix_clip_space.inl#L249
    T const tanHalfFovy = tan(fovy / static_cast<T>(2));
    Matrix4 m(0);
    m.set(0, 0, static_cast<T>(1) / (aspect * tanHalfFovy));
    m.set(1, 1, static_cast<T>(1) / (tanHalfFovy));
    m.set(2, 2, - (zFar + zNear) / (zFar - zNear));
    m.set(2, 3, - static_cast<T>(1));
    m.set(3, 2, - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear));
    return m;
  }

}; // class Matrix4

typedef Matrix4<float> Matrix4f;
typedef Matrix4<int> Matrix4i;

} // namespace al
} // namespace graphics

#endif

