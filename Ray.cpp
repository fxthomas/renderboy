// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

bool Ray::intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    bool inside = true;
    unsigned int  quadrant[NUMDIM];
    register unsigned int i;
    unsigned int whichPlane;
    Vec3Df maxT;
    Vec3Df candidatePlane;
    
    for (i=0; i<NUMDIM; i++)
        if (origin[i] < minBb[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minBb[i];
            inside = false;
        } else if (origin[i] > maxBb[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxBb[i];
            inside = false;
        } else	{
            quadrant[i] = MIDDLE;
        }

    if (inside)	{
        intersectionPoint = origin;
        return (true);
    }

    for (i = 0; i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && direction[i] !=0.)
            maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
        else
            maxT[i] = -1.;

    whichPlane = 0;
    for (i = 1; i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < NUMDIM; i++)
        if (whichPlane != i) {
            intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
            if (intersectionPoint[i] < minBb[i] || intersectionPoint[i] > maxBb[i])
                return (false);
        } else {
            intersectionPoint[i] = candidatePlane[i];
        }
    return (true);			
}

/**
 * Computes the intersection of a light ray and a triangle, defined by 3 vertices
 * @param v0,v1,v2          
 * @param ir                Distance between intersection point and origin
 * @param intersectionPoint Intersection point
 *
 * @see http://fr.wikipedia.org/wiki/Lancer_de_rayon#Exemple_du_calcul_de_l.27intersection_d.27un_rayon_et_d.27un_triangle
 */
bool Ray::intersect (const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2, Vec3Df & intersectionPoint, float & ir) const {
	Vec3Df u = v1 - v0;
	Vec3Df v = v2 - v0;
	Vec3Df nn = Vec3Df::crossProduct(u, v);
	Vec3Df otr = origin - v0;
	Vec3Df otrv = Vec3Df::crossProduct (otr, v);
	Vec3Df uotr = Vec3Df::crossProduct (u, otr);

	float c = Vec3Df::dotProduct (nn, direction);
	float iu = Vec3Df::dotProduct (otrv, direction)/c;
	float iv = Vec3Df::dotProduct (uotr, direction)/c;
	ir = -Vec3Df::dotProduct (nn, otr)/c;

	// We return the intersection point
	bool hasIntersection = (0 <= iu && iu <= 1 && 0 <= iv && iv <= 1 && ir >= 0 && iu + iv <= 1);
	if (hasIntersection) {
		intersectionPoint = origin + ir*direction;
	}
 
	// We return true if the ray really intersects
	return hasIntersection;
}

/**
 * Computes the intersection of a light ray and a triangle
 */
bool Ray::intersect (const Object & object, const Triangle & tri, Vec3Df & intersectionPoint, float & ir) const {
	Vec3Df v0 = object.getMesh().getVertices()[tri.getVertex(0)].getPos();
	Vec3Df v1 = object.getMesh().getVertices()[tri.getVertex(1)].getPos();
	Vec3Df v2 = object.getMesh().getVertices()[tri.getVertex(2)].getPos();
	return intersect (v0, v1, v2, intersectionPoint, ir);
}

/**
 * Tests intersection with an object
 */
bool Ray::intersect (const Object & object, Vec3Df & intersectionPoint, float & ir) const {
	ir = INFINITY;
	bool hasIntersection = false;

	float tmpIr = 0.;
	bool tmpIntersection = false;
	Vec3Df tmpPoint;
	for (unsigned int i = 0; i < object.getMesh().getTriangles().size(); i++) {
		Triangle tri = object.getMesh().getTriangles()[i];
		tmpIntersection = intersect (object, tri, tmpPoint, tmpIr);
		if (tmpIntersection) {
			hasIntersection = true;
			if (tmpIr < ir) {
				ir = tmpIr;
				intersectionPoint = tmpPoint;
			}
		}
	}
	return hasIntersection;
}

/**
 * Tests intersection with the scene
 */
bool Ray::intersect (const Scene & scene, Vec3Df & intersectionPoint, Object & intersectionObject) const {
	float ir = INFINITY;
	bool hasIntersection = false;

	float tmpIr = 0.;
	bool tmpIntersection = false;
	Vec3Df tmpPoint;
	for (vector<Object>::const_iterator obj = scene.getObjects().begin(); obj != scene.getObjects().end(); obj++) {
		tmpIntersection = intersect (*obj, tmpPoint, tmpIr);
		if (tmpIntersection) {
			hasIntersection = true;
			if (tmpIr < ir) {
				intersectionObject = *obj;
				ir = tmpIr;
				intersectionPoint = tmpPoint;
			}
		}
	}
	return hasIntersection;
}
