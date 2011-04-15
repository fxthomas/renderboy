// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"

static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL)
        instance = new RayTracer ();
    return instance;
}

void RayTracer::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

inline int clamp (float f, int inf, int sup) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

/**
 * Raytrace a single point
 */
Vec3Df RayTracer::raytraceSingle (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight,
													unsigned int i,
													unsigned int j, bool debug) {
	Scene * scene = Scene::getInstance ();
	const BoundingBox & bbox = scene->getBoundingBox ();
	const Vec3Df & minBb = bbox.getMin ();
	const Vec3Df & maxBb = bbox.getMax ();
	const Vec3Df rangeBb = maxBb-minBb;
	
	float tanX = tan (fieldOfView);
	float tanY = tanX/aspectRatio;
	Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
	Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
	Vec3Df step = stepX + stepY;
	Vec3Df dir = direction + step;
	dir.normalize ();
	Ray ray (camPos, dir);
	Vertex intersectionPoint;
	Object intersectionObject;
	bool hasIntersection = ray.intersect (*scene, intersectionPoint, intersectionObject);
	if (hasIntersection) {
		if (debug) {
			cout << " (I) Intersection" << endl;
			cout << " (I) Material: " << intersectionObject.getMaterial() << endl;
		}

		Vec3Df c = intersectionObject.getMaterial().getColor();
		float Dr = 0.;
		float Dg = 0.;
		float Db = 0.;
		float Sr = 0.;
		float Sg = 0.;
		float Sb = 0.;

		for (vector<Light>::iterator light = scene->getLights().begin(); light != scene->getLights().end(); light++) {
			Vec3Df lm = light->getPos() - intersectionPoint.getPos();
			lm.normalize();
			Vec3Df vv = camPos - intersectionPoint.getPos();
			vv.normalize();
			float sc = Vec3D<float>::dotProduct(lm, intersectionPoint.getNormal());

			Dr += c[0]*sc;
			Dg += c[1]*sc;
			Db += c[2]*sc;

			/*sc = Vec3D<float>::dotProduct(v.n*sc*2.f-lm, vv);
			if (sc > 0.) {
				sc = pow (sc, shine);
				Sr += c[0]*sc;
				Sg += c[1]*sc;
				Sb += c[2]*sc;
			}*/
		}
		c[0] = intersectionObject.getMaterial().getDiffuse()*Dr + intersectionObject.getMaterial().getSpecular()*Sr;
		c[1] = intersectionObject.getMaterial().getDiffuse()*Dg + intersectionObject.getMaterial().getSpecular()*Sg;
		c[2] = intersectionObject.getMaterial().getDiffuse()*Db + intersectionObject.getMaterial().getSpecular()*Sb;

		if (debug) {
			cout << " (I) Computed Color: " << c << endl;
			cout << " (I) Computed Clamped Color: (" << clamp (c[0]*255.,0,255) << ", " << clamp (c[1]*255.,0,255) << ", " << clamp (c[2]*255.,0,255) << ")" << endl;
		}

		return c;
	} else {
		if (debug) cout << " (I) No intersection" << endl;
		return backgroundColor;
	}
}

/**
 * Renders the given scene with the given camera parameters into a QImage, and returns it.
 */
QImage RayTracer::render (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight) {
	QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
	
	Scene * scene = Scene::getInstance ();
	const BoundingBox & bbox = scene->getBoundingBox ();
	const Vec3Df & minBb = bbox.getMin ();
	const Vec3Df & maxBb = bbox.getMax ();
	const Vec3Df rangeBb = maxBb-minBb;
	
	for (unsigned int i = 0; i < screenWidth; i++) {
		cout << "Done: " << float(i)/float(screenWidth)*100. << "%" << endl;
#pragma omp parallel for schedule(static) default(shared)
		for (unsigned int j = 0; j < screenHeight; j++) {
			Vec3Df c = raytraceSingle (camPos, direction, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight, i, j, false);
			image.setPixel (i, ((screenHeight-1)-j), qRgb (clamp (c[0]*255., 0, 255), clamp (c[1]*255., 0, 255), clamp (c[2]*255., 0, 255)));
		}
	}
	return image;
}

void RayTracer::debug (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight,
													unsigned int i,
													unsigned int j) {
	raytraceSingle (camPos, direction, upVector, rightVector, fieldOfView, aspectRatio, screenWidth, screenHeight, i, j, true);
}
