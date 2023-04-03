#include "Graphics3D.h"
#include <cmath>
#include "Vertex.h"
using namespace al::graphics;
using namespace std;

// Clips a line segment to an axis-aligned rectangle
// Returns true if clipping is successful
// Returns false if line segment lies outside the rectangle
bool Graphics3D::clipLineToRect(int a[2], int b[2],
                                int xmin, int ymin, int xmax, int ymax)
{
  int mins[2] = {xmin, ymin};
  int maxs[2] = {xmax, ymax};
  int normals[2] = {1, -1};
  for (int axis=0; axis<2; axis++) {
    for (int plane=0; plane<2; plane++) {
      // Check both points
      for (int pt=1; pt<=2; pt++) {
        int* pt1 = pt==1 ? a : b;
        int* pt2 = pt==1 ? b : a;

        // If both points are outside the same plane, the line is
        // outside the rectangle
        if ( (a[0]<xmin && b[0]<xmin) || (a[0]>xmax && b[0]>xmax) ||
             (a[1]<ymin && b[1]<ymin) || (a[1]>ymax && b[1]>ymax)) {
          return false;
        }

        const int n = normals[plane];
        if ( (n==1 && pt1[axis]<mins[axis]) ||   // check left/top plane
             (n==-1 && pt1[axis]>maxs[axis]) ) { // check right/bottom plane

          // Calculate interpolation factor t using ratio of signed distance
          // of each point from the plane
          const float p = (n==1) ? mins[axis] : maxs[axis];
          const float q1 = pt1[axis];
          const float q2 = pt2[axis];
          const float d1 = n * (q1-p);
          const float d2 = n * (q2-p);
          const float t = d1 / (d1-d2);

          // t should always be between 0 and 1
          if (t<0 || t >1) {
            return false;
          }

          // Interpolate to find the new point
          pt1[0] = (int)(pt1[0] +  (pt2[0] - pt1[0]) * t );
          pt1[1] = (int)(pt1[1] +  (pt2[1] - pt1[1]) * t );
        }
      }
    }
  }
  return true;
}
