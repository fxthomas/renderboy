// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

using namespace std;

static Scene * instance = NULL;

Scene * Scene::getInstance () {
    if (instance == NULL)
        instance = new Scene ();
    return instance;
}

void Scene::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Scene::Scene () {
    buildDefaultScene (true);
    updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        bbox = objects[0].getBoundingBox ();
        for (unsigned int i = 1; i < objects.size (); i++)
            bbox.extendTo (objects[i].getBoundingBox ());
    }
}

// Changer ce code pour créer des scènes originales
void Scene::buildDefaultScene (bool HD) {
	cout << " (I) Building Default Scene..." << endl;

	// Create basic materials
	Material groundMat (1.f, 0.f, 1.f, Vec3Df (.1f, .1f, .1f), 1.f, 0.f, 0.f);
	Material glassMat1 (1.f, 1.f, 1.f, Vec3Df (1.f, .0f, .2f), 1.6f, 0.90f, 0.2f);
	Material glassMat2 (1.f, 1.f, 1.f, Vec3Df (.2f, 1.f, 0.f), 1.2f, 0.90f, 0.2f);
	Material glassMat3 (1.f, 1.f, 1.f, Vec3Df (0.f, .2f, 1.f), 1.3f, 0.80f, 0.2f);
	Material planeMat (1.f, 1.f, 1.f, Vec3Df (1.f, 1.f, 1.f), 1.f, 0.f, 0.0f);

	// Create ground
/*	Mesh groundMesh;
	if (HD)
		groundMesh.loadOFF ("models/ground_HD.off");
	else
		groundMesh.loadOFF ("models/ground.off");
	Object ground (groundMesh,planeMat);    
	objects.push_back (ground);

	//Create ram
	
	Mesh ramMesh;
	ramMesh.loadOFF ("models/ram_HD.off");
	Object ram (ramMesh,glassMat1);    
	objects.push_back (ram);

*/


	// Load meshes
	Mesh glassMesh1, glassMesh2, glassMesh3, planeMesh, sphereMesh;
	glassMesh1.loadOFF ("models/wine_crate_000.off");
	glassMesh2.loadOFF ("models/wine_crate_000.off");
	glassMesh3.loadOFF ("models/wine_crate_000.off");
	planeMesh.loadOFF ("models/plane.off");
	sphereMesh.loadOFF ("models/sphere.off");

	// Translate meshes
	glassMesh2.translate (Vec3Df (1.f, 0.f, 0.f));
	glassMesh3.translate (Vec3Df (0.f, 1.f, 0.f));

	// Create objects
	Object glass1 (glassMesh1, glassMat1);
	Object glass2 (glassMesh2, glassMat2);
	Object glass3 (glassMesh3, glassMat3);
	Object sphere (sphereMesh, glassMat1);
	Object plane (planeMesh, planeMat);
	objects.push_back (glass1);
	objects.push_back (glass2);
	objects.push_back (glass3);
	objects.push_back (plane);
	//objects.push_back (sphere);


	// Create lights
	Light l (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 1.0f, Vec3Df(-1.0f, -1.0f, -1.0f));
	lights.push_back (l);

	// Recompute kD-trees for each object
	for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++) it->computeKdTree();
	cout << " (I) End scene build" << endl;
}
