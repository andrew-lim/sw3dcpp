#ifndef GRID_H
#define GRID_H
#include <vector>
#include <stdexcept>

template <class T>
class Grid
{
public:
  explicit Grid( size_t w=0, size_t h=0 );
  Grid( const Grid& );
  ~Grid();

  bool isWithinRange( size_t x, size_t y ) const ;

  T get(size_t x, size_t y)
  {
    if (x>width()) {
      throw std::runtime_error("x > width()");
    }
    if (y>height()) {
      throw std::runtime_error("x > height()");
    }
    return _elements[x][y];
  }

  void set(size_t x, size_t y, const T& value)
  {
    if (x>width()) {
      throw std::runtime_error("x > width()");
    }
    if (y>height()) {
      throw std::runtime_error("x > height()");
    }
    _elements[x][y] = value;
  }

  size_t width() const ;
  size_t height() const ;
  size_t size() const ;

  T& operator() ( size_t x, size_t y ) ;
  const T& operator() ( size_t x, size_t y ) const ;

  Grid& operator=( const Grid& );

  void fill(const T& t);

protected:
  std::vector< std::vector<T> > _elements ;
};

template <class T>
Grid<T>::Grid( size_t w, size_t h )
  : _elements( w )
{
  for ( size_t x=0; x<w; ++x )
  {
    _elements.at( x ) = std::vector<T>( h ) ;
  }
}

//------------------------------------------------------------------------------
//  Constructors & Destructors
//------------------------------------------------------------------------------

template <class T>
Grid<T>::Grid( const Grid& grid )
  : _elements( grid._elements )
{}

template <class T>
Grid<T>::~Grid()
{}

//------------------------------------------------------------------------------
//  Operators
//------------------------------------------------------------------------------

template <class T>
T& Grid<T>::operator() ( size_t x, size_t y )
{
  return _elements[x][y];
}

template <class T>
const T& Grid<T>::operator() ( size_t x, size_t y ) const
{
  return _elements[x][y];
}

template <class T>
Grid<T>& Grid<T>::operator=( const Grid<T>& grid )
{
  _elements = grid._elements ;
  return *this ;
}

//------------------------------------------------------------------------------
//  Member Functions
//------------------------------------------------------------------------------

template <class T>
size_t Grid<T>::width() const
{
  return _elements.size() ;
}

template <class T>
size_t Grid<T>::height() const
{
  return _elements.empty() ? 0 : _elements[ 0 ].size() ;
}

template <class T>
size_t Grid<T>::size() const
{
  return width() * height() ;
}

template <class T>
bool Grid<T>::isWithinRange( size_t x, size_t y ) const
{
  const size_t w = width() ;
  const size_t h = height() ;

  if ( w == 0 || h == 0 )
    return false ;

  return x >= 0 && x < w &&
         y >= 0 && y < h ;
}

template <class T>
void Grid<T>::fill(const T& t)
{
  for (size_t x=0; x<width(); ++x ) {
    for (size_t y=0; y<height(); ++y) {
      _elements[x][y] = t;
    }
  }
}


#endif
