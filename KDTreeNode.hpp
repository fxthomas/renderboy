/**
 * KDTreeNode C++ Header (KDTreeNode.hpp)
 * Created: Sun 17 Apr 2011 07:16:34 PM CEST
 *
 * This C++ Header was developped by François-Xavier Thomas.
 * You are free to copy, adapt or modify it.
 * If you do so, however, leave my name somewhere in the credits, I'd appreciate it ;)
 * 
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 * @version 1.0
 */

#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>

#include "Mesh.h"
#include "BoundingBox.h"
#include "Vertex.h"
#include "Vec3D.h"

using namespace std;

/**
 * KDTreeNode Class
 * This class does something you ought to say here!
 *
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 */
class KDTreeNode {
	protected:
		/**
		 * Generate sub-KD-Tree data
		 */
		void loadVertices (vector<unsigned int> & verts, unsigned int axis);

		float split;
		const Mesh *mesh;
		KDTreeNode *kleft;
		KDTreeNode *kright;
		vector <unsigned int> data;
		vector <unsigned int> triangles;
		BoundingBox bbox;

		const KDTreeNode & _find (const Vec3Df & v, unsigned int axis) const;

	public:
		/**
		 * Generate KD-Tree data
		 */
		void load ();

		/**
		 * KDTreeNode Class Constructor. You first have to load the KD-Tree with a mesh to use it.
		 * 
		 * @see load
		 * @author François-Xavier Thomas
		 */
		KDTreeNode() : split(0),kleft(NULL),kright(NULL) { }

		/**
		 * Class destructor
		 */
		~KDTreeNode () {
			if (kleft != NULL) { delete kleft; kleft = NULL; }
			if (kright != NULL) { delete kright; kright = NULL; }
		}

		/**
		 * KDTreeNode Class Constructor, with direct mesh loading.
		 * 
		 * @author François-Xavier Thomas
		 */
		KDTreeNode(const Mesh * m) : mesh(m),kleft(NULL),kright(NULL) { load (); }

		/**
		 * Clear KD-Tree
		 */
		void clear () {
			data.clear();
			triangles.clear();
			split = 0;
			kleft = NULL;
			kright = NULL;
		}

		/**
		 * Number of random samples for the median
		 */
		const static unsigned int NSAMPLES = 200;

		/**
		 * Maximum leaf size
		 */
		const static unsigned int LEAFSIZE = 10;

		/**
		 * Setters and getters
		 */
		inline KDTreeNode* getLeft () const { return kleft; }
		inline KDTreeNode* getRight () const { return kright; }
		inline vector<unsigned int> & getVertices () { return data; }
		inline float getSplit() const { return split; }
		inline const Mesh * getMesh () const { return mesh; }
		inline const BoundingBox & getBoundingBox () const { return bbox; }
		inline vector<unsigned int> & getTriangles () { return triangles; }

		/**
		 * Find vertices
		 */
		inline const KDTreeNode & find (const Vec3Df & v) const { return _find (v, 0); }
};
