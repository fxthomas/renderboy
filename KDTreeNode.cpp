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
	vector<unsigned int> belongs (mesh->getVertices().size(), false);
	vector<unsigned int> verts (mesh->getVertices().size(), 0);
	Vec3Df min = mesh->getVertices()[verts[0]].getPos(), max = min;

	for (unsigned int i = 0; i < mesh->getVertices().size(); i++) verts[i] = i;
	for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
		for (unsigned int i = 0; i < 3; i++) {
			if (mesh->getVertices()[*v].getPos()[i] < min[i]) min[i] = mesh->getVertices()[*v].getPos()[i];
			if (mesh->getVertices()[*v].getPos()[i] > max[i]) max[i] = mesh->getVertices()[*v].getPos()[i];
		}
	}

	bbox = BoundingBox (min, max);
	
	// Load vertices
	loadVertices (verts, 0);
}

bool KDTreeNode::loadVertices (vector<unsigned int> & verts, unsigned int axis) {
	// Initialize stuff
	if (kleft != NULL) { delete kleft; kleft = NULL; }
	if (kright != NULL) { delete kright; kright = NULL; }
	if (verts.size() == 0) { cout << "Hum... bizarre at KDTreeNode.cpp:28!" << endl; return false; }

	// If leaf is too small, we don't do anything but initialize bounding box and loading triangles
	if (verts.size() <= LEAFSIZE) {
		data = verts;

		// Initialize bounding box
		vector<unsigned int> belongs (mesh->getVertices().size(), false);
		for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) belongs[*v] = true;

		// Initialize triangles
		for (unsigned int it = 0; it < mesh->getTriangles().size(); it++) {
			Triangle t = mesh->getTriangles()[it];
			if (belongs[t.getVertex(0)] || belongs[t.getVertex(1)] || belongs[t.getVertex(2)]) triangles.push_back (it);
		}

	// Else, we split the node in 2 children
	} else {
		// Compute split plane
		split = 0.;
		for (unsigned int i = 0; i < NSAMPLES && i < verts.size(); i++) split += mesh->getVertices()[verts[rand()%verts.size()]].getPos()[axis];
		if (NSAMPLES < verts.size()) split /= (float)NSAMPLES;
		else split /= float(verts.size());

		// Sort each vertex into a split voxel
		vector<unsigned int> lverts;
		vector<unsigned int> rverts;
		for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
			if (mesh->getVertices()[*v].getPos()[axis] < split) lverts.push_back (*v);
			else rverts.push_back (*v);
		}

		// Create left node
		if (lverts.size() > 0) {
			Vec3Df vmi = bbox.getMin();
			Vec3Df vma = bbox.getMax();
			vma[axis] = split;

			kleft = new KDTreeNode ();
			kleft->mesh = mesh;
			kleft->bbox = BoundingBox (vmi, vma);
			kleft->loadVertices(lverts, (axis+1)%3);
		}

		// Create right node
		if (rverts.size() > 0) {
			Vec3Df vmi = bbox.getMin();
			Vec3Df vma = bbox.getMax();
			vmi[axis] = split;

			kright = new KDTreeNode ();
			kright->mesh = mesh;
			kright->bbox = BoundingBox (vmi, vma);
			kright->loadVertices(rverts, (axis+1)%3);
		}
	}
	return true;
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
