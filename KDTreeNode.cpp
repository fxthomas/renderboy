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

bool KDTreeNode::_lineInBox (const Vec3Df & origin, const Vec3Df & direction, const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
	unsigned int NUMDIM = 3;
	unsigned int LEFT = 0;
	unsigned int MIDDLE = 1;
	unsigned int RIGHT = 2;
	const Vec3Df & minBb = bbox.getMin ();
	const Vec3Df & maxBb = bbox.getMax ();
	bool inside = true;
	unsigned int  quadrant[NUMDIM];
	register unsigned int i;
	unsigned int whichPlane;
	Vec3Df maxT;
	Vec3Df candidatePlane;

	for (i=0; i<NUMDIM; i++)
		if (origin[i] < minBb[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = minBb[i];
			inside = false;
		} else if (origin[i] > maxBb[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxBb[i];
			inside = false;
		} else	{
			quadrant[i] = MIDDLE;
		}

	if (inside)	{
		intersectionPoint = origin;
		return (true);
	}

	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && direction[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
		else
			maxT[i] = -1.;

	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	if (maxT[whichPlane] < 0.) return (false);
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
			if (intersectionPoint[i] < minBb[i] || intersectionPoint[i] > maxBb[i])
				return (false);
		} else {
			intersectionPoint[i] = candidatePlane[i];
		}
	return (true);
}

bool KDTreeNode::_triangleInBox (const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2, const BoundingBox & bbox) const {
	Vec3Df d = (bbox.getMax() - bbox.getMin())*FUZZINESS;
	BoundingBox bbox2 = BoundingBox (bbox.getMin() - d, bbox.getMax() + d);
	Vec3Df d0 = v1 - v0;
	Vec3Df d1 = v2 - v1;
	Vec3Df d2 = v0 - v2;
	d0.normalize();
	d1.normalize();
	d2.normalize();

	Vec3Df a,b,c,ab,bb,cb;
	bool b0 = _lineInBox (v0, d0, bbox2, a);
	bool b1 = _lineInBox (v1, d1, bbox2, b);
	bool b2 = _lineInBox (v2, d2, bbox2, c);
	bool bb0 = _lineInBox (v0, -d0, bbox2, ab);
	bool bb1 = _lineInBox (v1, -d1, bbox2, bb);
	bool bb2 = _lineInBox (v2, -d2, bbox2, cb);
	if (b0 && b1 && bbox2.contains((a+b)/2)) return true;
	else if (b0 && b2 && bbox2.contains((a+c)/2)) return true;
	else if (b1 && b2 && bbox2.contains((b+c)/2)) return true;
	else if (bb0 && bb1 && bbox2.contains((ab+bb)/2)) return true;
	else if (bb0 && bb2 && bbox2.contains((ab+cb)/2)) return true;
	else if (bb1 && bb2 && bbox2.contains((bb+cb)/2)) return true;
	else return false;
}

void KDTreeNode::load () {
	// Generate vertex list
	vector<unsigned int> belongs (mesh->getVertices().size(), false);
	vector<unsigned int> verts (mesh->getVertices().size(), 0);
	vector<unsigned int> tri (mesh->getTriangles().size(), 0);
	Vec3Df min = mesh->getVertices()[verts[0]].getPos(), max = min;

	for (unsigned int v = 0; v < mesh->getVertices().size(); v++) {
		verts[v] = v;
		for (unsigned int i = 0; i < 3; i++) {
			if (mesh->getVertices()[v].getPos()[i] < min[i]) min[i] = mesh->getVertices()[v].getPos()[i];
			if (mesh->getVertices()[v].getPos()[i] > max[i]) max[i] = mesh->getVertices()[v].getPos()[i];
		}
	}

	for (unsigned int t = 0; t < mesh->getTriangles().size(); t++) tri[t] = t;

	bbox = BoundingBox (min, max);
	
	// Load vertices
	loadVertices (verts, tri, 0);
}

bool KDTreeNode::loadVertices (vector<unsigned int> & verts, vector<unsigned int> & tri, unsigned int axis) {
	// Initialize stuff
	if (kleft != NULL) { delete kleft; kleft = NULL; }
	if (kright != NULL) { delete kright; kright = NULL; }
	if (verts.size() == 0) { cout << "Hum... bizarre at KDTreeNode.cpp:28!" << endl; return false; }

	// If leaf is too small, we don't do anything but initialize bounding box and loading triangles
	if (verts.size() <= LEAFSIZE) {
		data = verts;
		triangles = tri;

	// Else, we split the node in 2 children
	} else {
		// Compute split plane
		split = 0.;
		for (unsigned int i = 0; i < NSAMPLES && i < verts.size(); i++) split += mesh->getVertices()[verts[rand()%verts.size()]].getPos()[axis];
		if (NSAMPLES < verts.size()) split /= (float)NSAMPLES;
		else split /= float(verts.size());

		// Compute new bounding boxes
		Vec3Df l_vmi = bbox.getMin();
		Vec3Df l_vma = bbox.getMax();
		l_vma[axis] = split;
		BoundingBox l_bbox (l_vmi, l_vma);

		Vec3Df r_vmi = bbox.getMin();
		Vec3Df r_vma = bbox.getMax();
		r_vmi[axis] = split;
		BoundingBox r_bbox (r_vmi, r_vma);

		// Sort each vertex into a split voxel
		vector<unsigned int> lverts;
		vector<unsigned int> rverts;

		for (vector<unsigned int>::iterator v = verts.begin(); v != verts.end(); v++) {
			if (mesh->getVertices()[*v].getPos()[axis] < split) lverts.push_back (*v);
			else rverts.push_back (*v);
		}

		// Do the same for triangles
		vector<unsigned int> ltri;
		vector<unsigned int> rtri;

		for (vector<unsigned int>::iterator t = tri.begin(); t != tri.end(); t++) {
			Triangle tr = mesh->getTriangles()[*t];
			bool sl0 = l_bbox.contains (mesh->getVertices()[tr.getVertex(0)].getPos());
			bool sl1 = l_bbox.contains (mesh->getVertices()[tr.getVertex(1)].getPos());
			bool sl2 = l_bbox.contains (mesh->getVertices()[tr.getVertex(2)].getPos());
			int sl = (sl0?1:0) + (sl1?1:0) + (sl2?1:0);

			sl0 = r_bbox.contains (mesh->getVertices()[tr.getVertex(0)].getPos());
			sl1 = r_bbox.contains (mesh->getVertices()[tr.getVertex(1)].getPos());
			sl2 = r_bbox.contains (mesh->getVertices()[tr.getVertex(2)].getPos());
			int sr = (sl0?1:0) + (sl1?1:0) + (sl2?1:0);

			bool bl = false, br = false;
			if (sl > 0) bl = true;
			else if (_triangleInBox (mesh->getVertices()[tr.getVertex(0)].getPos(), mesh->getVertices()[tr.getVertex(1)].getPos(), mesh->getVertices()[tr.getVertex(2)].getPos(), l_bbox)) bl = true;

			if (sr > 0) br = true;
			else if (_triangleInBox (mesh->getVertices()[tr.getVertex(0)].getPos(), mesh->getVertices()[tr.getVertex(1)].getPos(), mesh->getVertices()[tr.getVertex(2)].getPos(), r_bbox)) br = true;

			if (bl) ltri.push_back (*t);
			//else cout << "No tri left (" << sl << "," << sr << ")" << endl;
			if (br) rtri.push_back (*t);
			//else cout << "No tri right (" << sl << "," << sr << ")" << endl;
		}

		if (lverts.size() > 0 && rverts.size() > 0) {
			// Create left node
			kleft = new KDTreeNode ();
			kleft->mesh = mesh;
			kleft->bbox = l_bbox;
			kleft->loadVertices(lverts, ltri, (axis+1)%3);

			// Create right node
			kright = new KDTreeNode ();
			kright->mesh = mesh;
			kright->bbox = r_bbox;
			kright->loadVertices(rverts, rtri, (axis+1)%3);
		} else {
			kleft = NULL;
			kright = NULL;
			data = verts;
			triangles = tri;
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
