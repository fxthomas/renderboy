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

	// Create basic box materials
	Material planeWhite (1.f, 0.f, 0.f, Vec3Df (1.f, 1.f, 1.f), 1.f, 0.f, 0.f);
	Material planeRed   (1.f, 0.f, 0.f, Vec3Df (1.f, 0.f, 0.f), 1.f, 0.f, 0.f);
	Material planeGreen (1.f, 0.f, 0.f, Vec3Df (0.f, 1.f, 0.f), 1.f, 0.f, 0.f);

	// Load planes
	Mesh planeFloor, planeTop, planeBottom, planeLeft, planeRight;
	planeFloor .loadOFF ("models/Box/plane_floor.off");
	planeTop   .loadOFF ("models/Box/plane_top.off");
	planeBottom.loadOFF ("models/Box/plane_bottom.off");
	planeLeft  .loadOFF ("models/Box/plane_left.off");
	planeRight .loadOFF ("models/Box/plane_right.off");
	objects.push_back (Object (planeFloor, planeWhite));
	objects.push_back (Object (planeTop, planeWhite));
	objects.push_back (Object (planeBottom, planeWhite));
	objects.push_back (Object (planeLeft, planeRed));
	objects.push_back (Object (planeRight, planeGreen));

	// Create glass materials
	Material glassMat1 (1.f, 1.f, 1.f, Vec3Df (1.f, .0f, .2f), 1.6f, 0.90f, 0.2f);
	Material glassMat2 (1.f, 1.f, 1.f, Vec3Df (.2f, 1.f, 0.f), 1.2f, 0.90f, 0.2f);
	Material glassMat3 (1.f, 1.f, 1.f, Vec3Df (0.f, .2f, 1.f), 1.3f, 0.80f, 0.2f);

	// Load glass meshes
	Mesh glassMesh1, glassMesh2, glassMesh3;
	glassMesh1.loadOFF ("models/wine_crate_000.off");
	glassMesh2.loadOFF ("models/wine_crate_000.off");
	glassMesh3.loadOFF ("models/wine_crate_000.off");
	glassMesh2.translate (Vec3Df (1.f, 0.f, 0.f));
	glassMesh3.translate (Vec3Df (0.f, 1.f, 0.f));

	// Create glass objects
	Object glass1 (glassMesh1, glassMat1);
	Object glass2 (glassMesh2, glassMat2);
	Object glass3 (glassMesh3, glassMat3);
	objects.push_back (glass1);
	objects.push_back (glass2);
	objects.push_back (glass3);

	// Create lights
	Light l (Vec3Df (3.0f, 3.0f, 3.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 1.0f, Vec3Df(-1.0f, -1.0f, -1.0f));
	lights.push_back (l);

	// Recompute kD-trees for each object
	for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++) it->computeKdTree();
	cout << " (I) End scene build" << endl;
}
