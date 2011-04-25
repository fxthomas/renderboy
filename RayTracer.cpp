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

Vec3Df RayTracer::getColor (const Vec3Df & eye, const Vec3Df & point, const Vec3Df & normal, const Material & mat) {
	Vec3Df diffuseSelf,specularSelf;
	Vec3Df c = mat.getColor();
	Vec3Df vv = eye - point;
	Vec3Df lpos, lm;
	vv.normalize();

	for (vector<Light>::iterator light = Scene::getInstance()->getLights().begin(); light != Scene::getInstance()->getLights().end(); light++) {
		lpos = cam.toWorld (light->getPos());
		lm = lpos - point;
		lm.normalize();

		// Diffuse Light
		float sc = Vec3D<float>::dotProduct(lm, normal);
		diffuseSelf += light->getColor() * fabs (sc);

		// Specular Light
		sc = Vec3D<float>::dotProduct(normal*sc*2.f-lm, vv);
		if (sc > 0.) {
			sc = pow (sc, mat.getShininess() * 40.f);
			specularSelf += light->getColor() * sc;
		}
	}

	// Total color blend
	c[0] = (mat.getDiffuse()*c[0]*diffuseSelf[0] + mat.getSpecular()*specularSelf[0]);
	c[1] = (mat.getDiffuse()*c[1]*diffuseSelf[1] + mat.getSpecular()*specularSelf[1]);
	c[2] = (mat.getDiffuse()*c[2]*diffuseSelf[2] + mat.getSpecular()*specularSelf[2]);
	return c;
}


Vec3Df RayTracer::lightModel (const Vec3Df & eye, const Vec3Df & point, const Vec3Df & normal, const Material & mat, const PointCloud & pc, bool debug) {
	Vec3Df cdirect = getColor (eye, point, normal, mat);
	Vec3Df cindirect;
	Vec3Df diffuseSelf,specularSelf;
	Vec3Df c = mat.getColor();
	Vec3Df vv = eye - point;
	Vec3Df lpos, lm;
	vv.normalize();
	float ff = 0.f;

	for (vector<Surfel>::const_iterator surfel = pc.getSurfels().begin(); surfel != pc.getSurfels().end(); surfel++) {
		ff += (surfel->getRadius()*surfel->getRadius());
		lpos = surfel->getPosition();
		lm = lpos - point;
		lm.normalize();

		// Diffuse Light
		float sc = Vec3D<float>::dotProduct(lm, normal);
		diffuseSelf += surfel->getColor() * fabs (sc);

		// Specular Light
		sc = Vec3D<float>::dotProduct(normal*sc*2.f-lm, vv);
		if (sc > 0.) {
			sc = pow (sc, mat.getShininess() * 40.f);
			specularSelf += surfel->getColor() * sc;
		}
	}

	// Total color blend
	cindirect[0] = (mat.getDiffuse()*c[0]*diffuseSelf[0] + mat.getSpecular()*specularSelf[0]);
	cindirect[1] = (mat.getDiffuse()*c[1]*diffuseSelf[1] + mat.getSpecular()*specularSelf[1]);
	cindirect[2] = (mat.getDiffuse()*c[2]*diffuseSelf[2] + mat.getSpecular()*specularSelf[2]);
	cindirect /= ff;

	c = (cindirect + cdirect) / 2.f;
	//Vec3Df c = cdirect;

	// Debug total color
	if (debug) {
		cout << "     [ Color blend ]" << endl;
		cout << "       Computed Color: " << c << endl;
		cout << "       Computed Clamped Color: (" << clamp (c[0]*255.,0,255) << ", " << clamp (c[1]*255.,0,255) << ", " << clamp (c[2]*255.,0,255) << ")" << endl << endl;
	}

	return c;
}

Vec3Df RayTracer::lightBounce (const Vec3Df & eye, const Vec3Df & dir, const Vec3Df & point, const Vec3Df & normal, const Material & mat, const PointCloud & pc, bool debug, int d) {
	// If no refraction, or depth too big, return classic light model
	if (d >= depth) return lightModel (eye, point, normal, mat, pc, debug);
	if (debug) cout << " (I) Bounce depth " << d << endl;
	if (debug) cout << " (I) For point " << point << endl;

	// Prepare color variables
	Vec3Df dirRefr, dirRefl;
	Vec3Df colRefr, colRefl, colSelf;
	Ray ray;
	Vertex intersectionPoint;
	const Object* intersectionObject;
	unsigned int triangle;
	float iu, iv;
	bool hasIntersection;

	// Get color from self lighting
	colSelf   = lightModel (eye, point, normal, mat, pc, debug);

	// Compute refraction/reflection vector
	bool refract = dir.bounce (mat.getIOR(), normal, dirRefr, dirRefl);

	// Compute refracted ray
	if (refract && mat.getIOR() != 1.f && mat.getRefract() > 0.f) {
		ray = Ray (point, dirRefr);
		intersectionObject = NULL;
		hasIntersection = ray.intersect (*Scene::getInstance(), intersectionPoint, &intersectionObject, iu, iv, triangle);

		if (!hasIntersection) {
			colRefr = backgroundColor;
		} else {
			// Get color from bouncing, and compute next normal
			Vec3Df p0 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (0)].getNormal();
			Vec3Df p1 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (1)].getNormal();
			Vec3Df p2 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (2)].getNormal();
			Vec3Df nor = (1-iu-iv)*p0 + iv*p1 + iu*p2;
			nor.normalize();
			colRefr = lightBounce (point, dirRefr, intersectionPoint.getPos(), nor, intersectionObject->getMaterial(), pc, debug, d+1);
		}
	}

	// Compute reflected ray
	if (mat.getReflect() > 0.f) {
		ray = Ray (point, dirRefl);
		intersectionObject = NULL;
		hasIntersection = ray.intersect (*Scene::getInstance(), intersectionPoint, &intersectionObject, iu, iv, triangle);

		if (!hasIntersection) {
			colRefl = backgroundColor;
		} else {
			// Get color from bouncing, and compute next normal
			Vec3Df p0 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (0)].getNormal();
			Vec3Df p1 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (1)].getNormal();
			Vec3Df p2 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (2)].getNormal();
			Vec3Df nor = (1-iu-iv)*p0 + iv*p1 + iu*p2;
			nor.normalize();
			colRefl = lightBounce (point, dirRefl, intersectionPoint.getPos(), nor, intersectionObject->getMaterial(), pc, debug, d+1);
		}
	}

	// Total color blend
	Vec3Df c = colRefl*mat.getReflect() + colRefr*mat.getRefract() + colSelf*(1.f-mat.getRefract());

	if (debug) {
		cout << "     [ Total color blend ]" << endl;
		cout << "       Computed Color: " << c << endl;
		cout << "       Computed Clamped Color: (" << clamp (c[0]*255.,0,255) << ", " << clamp (c[1]*255.,0,255) << ", " << clamp (c[2]*255.,0,255) << ")" << endl << endl;
	}

	c.scale (1.f);
	return c;
}

/**
 * Raytrace a single point
 */
Vec3Df RayTracer::raytraceSingle (const PointCloud & pc, unsigned int i,	unsigned int j, bool debug, BoundingBox & bb) {
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
	dir.normalize();
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

		Vec3Df p0 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (0)].getNormal();
		Vec3Df p1 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (1)].getNormal();
		Vec3Df p2 = intersectionObject->getMesh().getVertices()[intersectionObject->getMesh().getTriangles()[triangle].getVertex (2)].getNormal();
		Vec3Df normal = (1-iu-iv)*p0 + iv*p1 + iu*p2;
		normal.normalize();

		return lightBounce (camPos, dir, intersectionPoint.getPos(), normal, intersectionObject->getMaterial(), pc, debug, 0);

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
	cout << " (R) Generating point cloud..." << endl;
	PointCloud pc;
	
	for (vector<Object>::iterator it = Scene::getInstance()->getObjects().begin(); it != Scene::getInstance()->getObjects().end(); it++) {
		it->getMesh().recomputeSmoothVertexNormals(1);
		pc.add (*it, cam);
	}

	QTime timer;
	timer.start();

	cout << " (R) Raytracing: Start" << endl;

	//for (vector<Object>::iterator it = Scene::getInstance()->getObjects().begin(); it != Scene::getInstance()->getObjects().end(); it++) it->getKdTree()->show();
	// Create an image to hold the final raytraced render
	QImage image (QSize (cam.screenWidth(), cam.screenHeight()), QImage::Format_RGB888);

	// For each camera pixel, cast a ray and compute its reflecting color
	BoundingBox b;
#pragma omp parallel for default(shared) schedule(dynamic)
	for (unsigned int i = 0; i < (unsigned int)cam.screenWidth(); i++) {
		emit progress (i);

		for (unsigned int j = 0; j < (unsigned int)cam.screenHeight(); j++) {
			// Raytrace
			Vec3Df c = raytraceSingle (pc, i, j, false, b);
			
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
	raytraceSingle (PointCloud(), i, j, true, bb);
	return bb;
}
