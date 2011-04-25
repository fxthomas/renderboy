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
    buildDefaultScene (false);
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
    Mesh groundMesh;
    if (HD)
        groundMesh.loadOFF ("models/ground_HD.off");
    else
        groundMesh.loadOFF ("models/ground.off");
    Material groundMat (1.f, 0.f, 1.f, Vec3Df (.1f, .1f, .1f), 1.f, 0.f, 0.f);
    Material glassMat1 (1.f, 1.f, 1.f, Vec3Df (1.f, .0f, .2f), 1.6f, 0.90f, 0.2f);
		Material glassMat2 (1.f, 1.f, 1.f, Vec3Df (.2f, 1.f, 0.f), 1.2f, 0.90f, 0.2f);
		Material glassMat3 (1.f, 1.f, 1.f, Vec3Df (0.f, .2f, 1.f), 1.3f, 0.80f, 0.2f);
		Material planeMat (1.f, 1.f, 1.f, Vec3Df (1.f, 1.f, 1.f), 1.f, 0.f, 0.0f);

    Object ground (groundMesh,planeMat);    
		//cout << " (I) Ground address: " << &ground << endl;
    objects.push_back (ground);
    //Mesh ramMesh;
    //if (HD)
        //ramMesh.loadOFF ("models/ram_HD.off");
        //ramMesh.loadOFF ("models/wine_crate.off");
    //else
        //ramMesh.loadOFF ("models/ram.off");
        //ramMesh.loadOFF ("models/wine_crate.off");
    //Material ramMat (1.f, 1.f, 1.f, Vec3Df (1.f, .6f, .2f), 2.f, 0.8f);
		Mesh glassMesh1, glassMesh2, glassMesh3, planeMesh, sphereMesh;
		glassMesh1.loadOFF ("models/wine_crate_000.off");
		glassMesh2.loadOFF ("models/wine_crate_001.off");
		glassMesh3.loadOFF ("models/wine_crate_003.off");
		sphereMesh.loadOFF ("models/sphere.off");
		planeMesh.loadOFF ("models/plane.off");
		Object glass1 (glassMesh1, glassMat1);
		Object glass2 (glassMesh2, glassMat2);
		Object glass3 (glassMesh3, glassMat3);
		Object sphere (sphereMesh, glassMat1);
		Object plane (planeMesh, groundMat);
		objects.push_back (glass1);
		objects.push_back (glass2);
		objects.push_back (glass3);
		objects.push_back (plane);
		//objects.push_back (sphere);

    //Object ram (ramMesh, ramMat);    
		//cout << " (I) Ram address: " << &ram << endl;
    //objects.push_back (ram);

    Light l (Vec3Df (1.0f, 0.0f, 1.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.5f);
    lights.push_back (l);

		for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++) it->computeKdTree();
	cout << " (I) End scene build" << endl;
}
