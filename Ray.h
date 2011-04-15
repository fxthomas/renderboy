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

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
		bool intersect (const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2, Vec3Df & intersectionPoint, float & ir) const;
		bool intersect (const Object & object, const Triangle & tri, Vec3Df & intersectionPoint, float & ir) const;
		bool intersect (const Object & object, Vec3Df & intersectionPoint, float & ir) const;
		bool intersect (const Scene & scene, Vec3Df & intersectionPoint, Object& intersectionObject) const;
    
private:
    Vec3Df origin;
    Vec3Df direction;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
