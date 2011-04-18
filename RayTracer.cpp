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
Vec3Df RayTracer::raytraceSingle (const Camera & cam, unsigned int i,	unsigned int j, bool debug) {
	Scene * scene = Scene::getInstance ();
	
	const Vec3Df camPos = cam.position();
	const Vec3Df direction = cam.viewDirection();
	const Vec3Df upVector = cam.upVector();
	const Vec3Df rightVector = cam.rightVector();
	float fieldOfView = cam.horizontalFieldOfView();
	float aspectRatio = cam.aspectRatio();
	unsigned int screenWidth = cam.screenWidth();
	unsigned int screenHeight = cam.screenHeight();

	float tanX = tan (fieldOfView);
	float tanY = tanX/aspectRatio;
	Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
	Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
	Vec3Df step = stepX + stepY;
	Vec3Df dir = direction + step;
	if (debug) cout << " (I) Ray Direction: " << dir << endl;

	Ray ray (camPos, dir);
	Vertex intersectionPoint;
	Object intersectionObject;
	unsigned int triangle;
	bool hasIntersection = ray.intersect (*scene, intersectionPoint, intersectionObject, triangle);
	if (hasIntersection) {
		if (debug) {
			cout << " (I) Computing KD-Tree..." << endl;
			KDTreeNode kt (&intersectionObject.getMesh());
			KDTreeNode ktf = kt.find (intersectionPoint.getPos());
			cout << " (I) KDTree Bounds : [Min: " << ktf.getBoundingBox().getMin() << ", Max: " << ktf.getBoundingBox().getMax() << "]" << endl;
			cout << "Point: " << intersectionObject.getMesh().getVertices()[intersectionObject.getMesh().getTriangles()[triangle].getVertex(0)].getPos() << endl;
			cout << "Point: " << intersectionObject.getMesh().getVertices()[intersectionObject.getMesh().getTriangles()[triangle].getVertex(1)].getPos() << endl;
			cout << "Point: " << intersectionObject.getMesh().getVertices()[intersectionObject.getMesh().getTriangles()[triangle].getVertex(2)].getPos() << endl;
			for (vector<unsigned int>::iterator it = ktf.getVertices().begin(); it != ktf.getVertices().end(); it++) cout << intersectionObject.getMesh().getVertices()[*it].getPos() << endl;
			for (vector<unsigned int>::iterator it = ktf.getTriangles().begin(); it != ktf.getTriangles().end(); it++) cout << "Triangle " << *it << endl;
			cout << " (I) Intersection with " << intersectionPoint.getPos() << endl;
			cout << " (I) Object number of triangles: " << intersectionObject.getMesh().getTriangles().size() << endl;
			cout << " (I) Intersection with triangle: " << triangle << endl;
			cout << " (I) Material: " << intersectionObject.getMaterial() << endl << endl;
		}

		Vec3Df c = intersectionObject.getMaterial().getColor();
		Vec3Df diffuse;
		Vec3Df specular;

		for (vector<Light>::iterator light = scene->getLights().begin(); light != scene->getLights().end(); light++) {
			Vec3Df lpos = cam.toWorld (light->getPos());
			Vec3Df lm = lpos - intersectionPoint.getPos();
			lm.normalize();
			Vec3Df vv = camPos - intersectionPoint.getPos();
			vv.normalize();

			// Diffuse Light
			float sc = Vec3D<float>::dotProduct(lm, intersectionPoint.getNormal());
			if (sc > 0.) {
				diffuse = light->getColor() * sc;
			}

			// Specular Light
			sc = Vec3D<float>::dotProduct(intersectionPoint.getNormal()*sc*2.f-lm, vv);
			if (sc > 0.) {
				sc = pow (sc, intersectionObject.getMaterial().getShininess() * 12.8f);
				specular = light->getColor() * sc;
			}

			// Raytracing debug
			if (debug) {
				cout << " ------ Light -------" << endl;
				cout << " (I) Normal: " << intersectionPoint.getNormal() << endl;
				cout << " (I) Diffuse Factor: " << sc << endl;
				cout << " (I) Diffuse Color: " << diffuse << endl;
				cout << " (I) Specular Factor: " << sc << endl;
				cout << " (I) Specular Color: " << specular << endl << endl;
			}
		}

		// Total color blend
		c[0] = (intersectionObject.getMaterial().getDiffuse()*intersectionObject.getMaterial().getColor()[0]*diffuse[0] + intersectionObject.getMaterial().getSpecular()*specular[0]);
		c[1] = (intersectionObject.getMaterial().getDiffuse()*intersectionObject.getMaterial().getColor()[1]*diffuse[1] + intersectionObject.getMaterial().getSpecular()*specular[1]);
		c[2] = (intersectionObject.getMaterial().getDiffuse()*intersectionObject.getMaterial().getColor()[2]*diffuse[2] + intersectionObject.getMaterial().getSpecular()*specular[2]);

		// Debug total color
		if (debug) {
			cout << " ------ Color blend ------" << endl;
			cout << " (I) Computed Color: " << c << endl;
			cout << " (I) Computed Clamped Color: (" << clamp (c[0]*255.,0,255) << ", " << clamp (c[1]*255.,0,255) << ", " << clamp (c[2]*255.,0,255) << ")" << endl << endl;
		}

		return c;
	} else {
		if (debug) cout << " (I) No intersection" << endl << endl;
		return backgroundColor;
	}
}

/**
 * Renders the given scene with the given camera parameters into a QImage, and returns it.
 */
QImage RayTracer::render (const Camera & cam) {
	// Create an image to hold the final raytraced render
	QImage image (QSize (cam.screenWidth(), cam.screenHeight()), QImage::Format_RGB888);
	
	// For each camera pixel, cast a ray and compute its reflecting color
	for (unsigned int i = 0; i < (unsigned int)cam.screenWidth(); i++) {
		cout << "Done: " << float(i)/float(cam.screenWidth())*100. << "%" << endl;

#pragma omp parallel for schedule(static) default(shared)
		for (unsigned int j = 0; j < (unsigned int)cam.screenHeight(); j++) {
			Vec3Df c = raytraceSingle (cam, i, j, false);
			image.setPixel (i, ((cam.screenHeight()-1)-j), qRgb (clamp (c[0]*255., 0, 255), clamp (c[1]*255., 0, 255), clamp (c[2]*255., 0, 255)));
		}

	}

	// Return image
	return image;
}

void RayTracer::debug (const Camera & cam, unsigned int i, unsigned int j) {
	raytraceSingle (cam, i, j, true);
}
