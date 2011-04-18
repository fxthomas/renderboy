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
#include "Vertex.h"
#include "Vec3D.h"
#include "Camera.hpp"

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
		Mesh mesh;
		Camera cam;
		KDTreeNode *kleft;
		KDTreeNode *kright;
		vector <unsigned int> data;

		vector<unsigned int> _find (const Vec3Df & v, unsigned int axis) const;

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
		KDTreeNode(const Camera & cam) : split(0),cam(cam),kleft(NULL),kright(NULL) { }

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
		KDTreeNode(const Camera & cam, const Mesh & m) : mesh(m),cam(cam),kleft(NULL),kright(NULL) { load (); }

		/**
		 * Clear KD-Tree
		 */
		void clear () {
			data.clear();
			split = 0;
			kleft = NULL;
			kright = NULL;
		}

		/**
		 * Number of random samples for the median
		 */
		const static unsigned int NSAMPLES = 1000;

		/**
		 * Maximum leaf size
		 */
		const static unsigned int LEAFSIZE = 20;

		/**
		 * Setters and getters
		 */
		inline KDTreeNode* getLeft () const { return kleft; }
		inline KDTreeNode* getRight () const { return kright; }
		inline vector<unsigned int> & getData () { return data; };
		inline float getSplit() const { return split; }
		inline Mesh & getMesh () { return mesh; }

		/**
		 * Find vertices
		 */
		inline vector<unsigned int> find (const Vec3Df & v) { return _find (cam.toScreen (v), 0); }
};
