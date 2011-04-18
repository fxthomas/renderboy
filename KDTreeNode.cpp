/**
 * KDTreeNode C++ Source code (KDTreeNode.cpp)
 * Created: Sun 17 Apr 2011 08:27:03 PM CEST
 *
 * This C source code was developped by François-Xavier Thomas.
 * You are free to copy, adapt or modify it.
 * If you do so, however, leave my name somewhere in the credits, I'd appreciate it ;)
 * 
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 * @version 1.0
 */

#include "KDTreeNode.hpp"

void KDTreeNode::load () {
	// Generate vertex list
	vector<unsigned int> verts (mesh.getVertices().size(), 0);
	for (unsigned int i = 0; i < mesh.getVertices().size(); i++) verts[i] = i;
	
	// Load vertices
	loadVertices (verts, 0);
}

void KDTreeNode::loadVertices (vector<unsigned int> & verts, unsigned int axis) {
	// Initialize stuff
	kleft = NULL;
	kright = NULL;
	data = verts;

	// If leaf is too small, we don't do anything
	if (verts.size() < LEAFSIZE) return;
	
	// Compute split plane
	split = 0.;
	for (unsigned int i = 0; i < NSAMPLES; i++) split += cam.toScreen(mesh.getVertices()[verts[rand()%verts.size()]].getPos())[axis];
	split /= (float)NSAMPLES;

	// Sort each vertex into a split area
	vector<unsigned int> lverts;
	vector<unsigned int> rverts;
	for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
		if (cam.toScreen(mesh.getVertices()[*v].getPos())[axis] < split) lverts.push_back (*v);
		else rverts.push_back (*v);
	}

	// Create left node
	kleft = new KDTreeNode (cam);
	kleft->mesh = mesh;

	// Create right node
	kright = new KDTreeNode (cam);
	kright->mesh = mesh;

	// Load vertices for sub-kd-trees
	kleft->loadVertices(lverts, axis==0?1:0);
	kright->loadVertices(rverts, axis==0?1:0);
}

vector<unsigned int> KDTreeNode::_find (const Vec3Df & v, unsigned int axis) const {
	if (v[axis] < split) {
		if (kleft != NULL) return kleft->_find (v, axis==0?1:0);
		else return data;
	} else {
		if (kright != NULL)	return kright->_find (v, axis==0?1:0);
		else return data;
	}
}
