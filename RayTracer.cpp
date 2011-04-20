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
Vec3Df RayTracer::raytraceSingle (unsigned int i,	unsigned int j, bool debug, BoundingBox & bb) {
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
	if (debug) {
		cout << "     [ Basic Information ]" << endl;
		cout << "       Ray Direction: " << dir << endl << endl;
	}

	Ray ray (camPos, dir);
	Vertex intersectionPoint;
	const Object* intersectionObject = NULL;
	unsigned int triangle;
	float iu, iv;
	bool hasIntersection = ray.intersect (*scene, intersectionPoint, &intersectionObject, iu, iv, triangle);
	if (debug) {
		cout << "     [ kD-Tree ]" << endl;
		float f,fu,fv;
		Vertex vd;
		const KDTreeNode* kdt = ray.intersect (Scene::getInstance()->getObjects()[1].getKdTree(), vd, f, fu, fv, triangle);
		if (kdt != NULL) {
			bb = kdt->getBoundingBox();
			cout << "       Point distance: " << f << endl;
			for (vector<unsigned int>::const_iterator it = kdt->getTriangles().begin(); it != kdt->getTriangles().end(); it++) cout << "       Triangle: " << *it << endl;
		} else cout << "       Not found... ;(" << endl;
		cout << endl;
	}
	if (hasIntersection) {
		if (debug) {
			cout << "     [ Intersection ]" << endl;
			cout << "       Intersection with " << intersectionPoint.getPos() << endl;
			cout << "       Object number of triangles: " << intersectionObject->getMesh().getTriangles().size() << endl;
			cout << "       Intersection with triangle: " << triangle << endl;
			cout << "       Material: " << intersectionObject->getMaterial() << endl << endl;
		}

		Vec3Df c = intersectionObject->getMaterial().getColor();
		Vec3Df diffuse;
		Vec3Df specular;

		Vec3Df p0 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (0)].getNormal();
		Vec3Df p1 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (1)].getNormal();
		Vec3Df p2 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (2)].getNormal();
		Vec3Df normal = (1-iu-iv)*p0 + iv*p1 + iu*p2;
		cout<<"iu= " <<iu<<endl;
		cout<<"iv= " <<iv<<endl;
		normal.normalize();
		Vec3Df vv = camPos - intersectionPoint.getPos();
		vv.normalize();

		for (vector<Light>::iterator light = scene->getLights().begin(); light != scene->getLights().end(); light++) {
			Vec3Df lpos = cam.toWorld (light->getPos());
			Vec3Df lm = lpos - intersectionPoint.getPos();
			lm.normalize();

			// Diffuse Light
			float sc = Vec3D<float>::dotProduct(lm, normal);
			if (sc > 0.) {
				diffuse = light->getColor() * sc;
			}

			// Specular Light
			sc = Vec3D<float>::dotProduct(normal*sc*2.f-lm, vv);
			if (sc > 0.) {
				sc = pow (sc, intersectionObject->getMaterial().getShininess() * 12.8f);
				specular = light->getColor() * sc;
			}

			// Raytracing debug
			if (debug) {
				cout << "     [ Light ]" << endl;
				cout << "       Normal: " << normal << endl;
				cout << "       Diffuse Factor: " << sc << endl;
				cout << "       Diffuse Color: " << diffuse << endl;
				cout << "       Specular Factor: " << sc << endl;
				cout << "       Specular Color: " << specular << endl << endl;
			}
		}
		// Total color blend
		c[0] = (intersectionObject->getMaterial().getDiffuse()*c[0]*diffuse[0] + intersectionObject->getMaterial().getSpecular()*specular[0]);
		c[1] = (intersectionObject->getMaterial().getDiffuse()*c[1]*diffuse[1] + intersectionObject->getMaterial().getSpecular()*specular[1]);
		c[2] = (intersectionObject->getMaterial().getDiffuse()*c[2]*diffuse[2] + intersectionObject->getMaterial().getSpecular()*specular[2]);

		// Debug total color
		if (debug) {
			cout << "     [ Color blend ]" << endl;
			cout << "       Computed Color: " << c << endl;
			cout << "       Computed Clamped Color: (" << clamp (c[0]*255.,0,255) << ", " << clamp (c[1]*255.,0,255) << ", " << clamp (c[2]*255.,0,255) << ")" << endl << endl;
		}

		return c;
	} else {
		if (debug) cout << "     [ No intersection ]" << endl << endl;
		return backgroundColor;
	}
}

/**
 * Renders the given scene with the given camera parameters into a QImage, and returns it.
 */
QImage RayTracer::render () {
	// Count elapsed time
	
	for (vector<Object>::iterator it = Scene::getInstance()->getObjects().begin(); it != Scene::getInstance()->getObjects().end(); it++) it->getMesh().recomputeSmoothVertexNormals(1);

	QTime timer;
	timer.start();
	cout << " (R) Raytracing: Start" << endl;

	//for (vector<Object>::iterator it = Scene::getInstance()->getObjects().begin(); it != Scene::getInstance()->getObjects().end(); it++) it->getKdTree()->show();
	// Create an image to hold the final raytraced render
	QImage image (QSize (cam.screenWidth(), cam.screenHeight()), QImage::Format_RGB888);

	// For each camera pixel, cast a ray and compute its reflecting color
	BoundingBox b;
	for (unsigned int i = 0; i < (unsigned int)cam.screenWidth(); i++) {
		emit progress (i);

//pragma omp parallel for schedule(static) default(shared)
		for (unsigned int j = 0; j < (unsigned int)cam.screenHeight(); j++) {
			// Raytrace
			Vec3Df c = raytraceSingle (i, j, false, b);
			
			// Depth map
			//float f = (c - cam.position()).getSquaredLength();
			//image.setPixel (i, ((cam.screenHeight()-1)-j), qRgb (clamp (f, 0, 255), clamp (f, 0, 255), clamp (f, 0, 255)));
			
			// Computed lighting
			image.setPixel (i, ((cam.screenHeight()-1)-j), qRgb (clamp (c[0]*255., 0, 255), clamp (c[1]*255., 0, 255), clamp (c[2]*255., 0, 255)));
		}

	}

	// Return image
	cout << " (R) Raytracing done! (" << timer.elapsed() << " ms)" << endl;
	return image;
}

BoundingBox RayTracer::debug (unsigned int i, unsigned int j) {
	BoundingBox bb;
	raytraceSingle (i, j, true, bb);
	return bb;
}
