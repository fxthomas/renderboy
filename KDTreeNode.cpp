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
	vector<unsigned int> verts (mesh->getVertices().size(), 0);
	for (unsigned int i = 0; i < mesh->getVertices().size(); i++) verts[i] = i;
	
	// Load vertices
	loadVertices (verts, 0);
}

void KDTreeNode::loadVertices (vector<unsigned int> & verts, unsigned int axis) {
	// Initialize stuff
	kleft = NULL;
	kright = NULL;
	Vec3Df min = mesh->getVertices()[verts[0]].getPos(), max = min;

	// If leaf is too small, we don't do anything but initialize bounding box and loading triangles
	if (verts.size() < LEAFSIZE) {
		data = verts;

		// Initialize bounding box
		vector<unsigned int> belongs (mesh->getVertices().size(), false);
		for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
			belongs[*v] = true;
			for (unsigned int i = 0; i < 3; i++) {
				if (mesh->getVertices()[*v].getPos()[i] < min[i]) min[i] = mesh->getVertices()[*v].getPos()[i];
				if (mesh->getVertices()[*v].getPos()[i] > max[i]) max[i] = mesh->getVertices()[*v].getPos()[i];
			}
		}
		bbox = BoundingBox (min, max);

		// Initialize triangles
		for (unsigned int it = 0; it < mesh->getTriangles().size(); it++) {
			Triangle t = mesh->getTriangles()[it];
			if (belongs[t.getVertex(0)] || belongs[t.getVertex(1)] || belongs[t.getVertex(2)]) triangles.push_back (it);
		}

	// Else, we split the node in 2 children
	} else {
		// Compute split plane
		split = 0.;
		for (unsigned int i = 0; i < NSAMPLES; i++) split += mesh->getVertices()[verts[rand()%verts.size()]].getPos()[axis];
		split /= (float)NSAMPLES;

		// Sort each vertex into a split voxel
		vector<unsigned int> lverts;
		vector<unsigned int> rverts;
		for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
			for (unsigned int i = 0; i < 3; i++) {
				if (mesh->getVertices()[*v].getPos()[i] < min[i]) min[i] = mesh->getVertices()[*v].getPos()[i];
				if (mesh->getVertices()[*v].getPos()[i] > max[i]) max[i] = mesh->getVertices()[*v].getPos()[i];
			}
			if (mesh->getVertices()[*v].getPos()[axis] < split) lverts.push_back (*v);
			else rverts.push_back (*v);
		}
		bbox = BoundingBox (min, max);

		// Create left node
		kleft = new KDTreeNode ();
		kleft->mesh = mesh;

		// Create right node
		kright = new KDTreeNode ();
		kright->mesh = mesh;

		// Load vertices for sub-kd-trees
		kleft->loadVertices(lverts, (axis+1)%3);
		kright->loadVertices(rverts, (axis+1)%3);
	}
}

const KDTreeNode & KDTreeNode::_find (const Vec3Df & v, unsigned int axis) const {
	if (v[axis] < split) {
		if (kleft != NULL) return kleft->_find (v, (axis+1)%3);
		else return *this;
	} else {
		if (kright != NULL)	return kright->_find (v, (axis+1)%3);
		else return *this;
	}
}
