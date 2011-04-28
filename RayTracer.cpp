// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"

#define NB_RAY 16

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
		//lpos = light->getPos();
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
Vec3Df RayTracer::raytraceSingle (const PointCloud & pc, unsigned int i,	unsigned int j, bool debug, BoundingBox & bb, unsigned int nb_iter, vector <vector<Vec3Df> > rand_lpoints) {
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
	float ir, iu, iv;
	bool hasIntersection = ray.intersect (*scene, intersectionPoint, &intersectionObject, ir, iu, iv, triangle);
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

		Vec3Df color = lightBounce (camPos, dir, intersectionPoint.getPos(), normal, intersectionObject->getMaterial(), pc, debug, 0);

		// Occlusion (shadows)
		bool occlusion= false;
		Vec3Df oc_dir;
		Vertex tmp;
		float ir, iu_tmp, iv_tmp;
		unsigned int tri_tmp;
		double visibility =1.0f;

		for(unsigned int i=0;i<nb_iter;i++) {
			//don't forget to change lpos to rand_lpos for an extended source of light

			// Test Occlusion

			oc_dir=rand_lpoints[i][j]-intersectionPoint.getPos();	
			Ray oc_ray (intersectionPoint.getPos(), oc_dir);
			occlusion = (oc_ray.intersect(*scene, tmp, &intersectionObject, ir, iu_tmp, iv_tmp, tri_tmp)) && (ir<oc_dir.getLength());

			lm = oc_dir;
			lm.normalize();

			if (occlusion && (ir < 0.000001)) {
				Ray oc_ray2(intersectionPoint.getPos()+ oc_dir*(ir + 0.000001), oc_dir);
				occlusion = (oc_ray2.intersect(*scene, tmp, &intersectionObject, ir, iu_tmp, iv_tmp, tri_tmp)) && (ir<oc_dir.getLength());
			}

			if (occlusion) {
				visibility-= 1.0f/nb_iter;
				continue;
			}
		}

		oc_dir=rand_lpoints[0][j]-intersectionPoint.getPos();	
		Ray oc_ray (intersectionPoint.getPos(), oc_dir);

		return color*visibility;

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
	
	Scene * scene = Scene::getInstance ();

	unsigned int nb_iter = NB_RAY;
	vector <vector<Vec3Df> > rand_lpoints(nb_iter, vector<Vec3Df>(scene->getLights().size(), Vec3Df(0.f,0.f,0.f)));

	for (vector<Light>::iterator light = scene->getLights().begin(); light != scene->getLights().end(); light++) {
		
		int num_light=0;
		Vec3Df lpos = cam.toWorld (light->getPos());
		float lrad= light->getRadius();
		Vec3Df lor= cam.toWorld (light->getOrientation());

		const float PI = 3.1415926535;

		if (lrad==0) nb_iter=1;

		rand_lpoints[0][num_light]=lpos;

		for(unsigned int j=1;j<nb_iter;j++){
			double rand_rad =((double)rand() / ((double)RAND_MAX + 1) * lrad);
			double rand_ang =((double)rand() / ((double)RAND_MAX + 1) * 2 * PI);
				
			Vec3Df v0 (0.0f, -lor[2], lor[1]);
			Vec3Df v1 (lor[1]*lor[1]+lor[2]*lor[2], -lor[0]*lor[1], -lor[0]*lor[2]);
			v0.normalize();
			v1.normalize(); 

			rand_lpoints[j][num_light]= lpos+ rand_rad* (cos(rand_ang)*v0+sin(rand_ang)*v1);
		}
		num_light++;
	} 

#pragma omp parallel for default(shared) schedule(dynamic)
	for (unsigned int i = 0; i < (unsigned int)cam.screenWidth(); i++) {
		emit progress (i);

		for (unsigned int j = 0; j < (unsigned int)cam.screenHeight(); j++) {
			// Raytrace
			Vec3Df c = raytraceSingle (pc, i, j, false, b, nb_iter, rand_lpoints);
			
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
	Scene * scene = Scene::getInstance ();

	unsigned int nb_iter = NB_RAY;
	vector <vector<Vec3Df> > rand_lpoints(nb_iter, vector<Vec3Df>(scene->getLights().size(), Vec3Df(0.f,0.f,0.f)));

	for (vector<Light>::iterator light = scene->getLights().begin(); light != scene->getLights().end(); light++) {
		
		int num_light=0;
		Vec3Df lpos = cam.toWorld (light->getPos());
		float lrad= light->getRadius();
		Vec3Df lor= cam.toWorld (light->getOrientation());
		
		

		const float PI = 3.1415926535;

		if (lrad==0) nb_iter=1;

		for(unsigned int k=0;k<scene->getLights().size();k++){
			rand_lpoints[0][k]=lpos;
		}

		for(unsigned int j=1;j<nb_iter;j++){
	
			double rand_rad =((double)rand() / ((double)RAND_MAX + 1) * lrad);
			double rand_ang =((double)rand() / ((double)RAND_MAX + 1) * 2 * PI);
				
			Vec3Df v0 (0.0f, -lor[2], lor[1]);
			Vec3Df v1 (lor[1]*lor[1]+lor[2]*lor[2], -lor[0]*lor[1], -lor[0]*lor[2]);
			v0.normalize();
			v1.normalize(); 

			rand_lpoints[j][num_light]= lpos+ rand_rad* (cos(rand_ang)*v0+sin(rand_ang)*v1);
		}
		num_light++;
	} 
	raytraceSingle (PointCloud(), i, j, true, bb, nb_iter, rand_lpoints);
	return bb;
}
