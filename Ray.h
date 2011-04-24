// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAY_H
#define RAY_H

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>

#include "Vec3D.h"
#include "BoundingBox.h"
#include "KDTreeNode.hpp"
#include "Scene.h"

using namespace std;

class Ray {
public:
    inline Ray () {}
    inline Ray (const Vec3Df & origin, const Vec3Df & direction)
        : origin (origin), direction (direction) {}
    inline virtual ~Ray () {}

    inline const Vec3Df & getOrigin () const { return origin; }
    inline Vec3Df & getOrigin () { return origin; }
    inline const Vec3Df & getDirection () const { return direction; }
    inline Vec3Df & getDirection () { return direction; }

		const KDTreeNode* intersect (const KDTreeNode* kdtree, Vertex & intersectionPoint, float & ir, float & iu, float & iv, unsigned int & triangle) const;

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
		bool intersectFuzzy (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
		bool intersect (const Vertex & v0, const Vertex & v1, const Vertex & v2, Vertex & intersectionPoint, float & ir, float & iu, float & iv) const;
		bool intersect (const Object & object, const Triangle & tri, Vertex & intersectionPoint, float & ir, float & iu, float & iv) const;
		bool intersect (const Object & object, Vertex & intersectionPoint, float & ir, float & iu, float & iv, unsigned int & triangle) const;
		bool intersect (const Scene & scene, Vertex & intersectionPoint, const Object ** intersectionObject, float & iu, float & iv, unsigned int & triangle) const;
    
private:
    Vec3Df origin;
    Vec3Df direction;
		const static float EPSILON = 1e-2;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
