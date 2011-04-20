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
		bool loadVertices (vector<unsigned int> & verts, vector<unsigned int> & tri, unsigned int axis);

		float split;
		const Mesh *mesh;
		KDTreeNode *kleft;
		KDTreeNode *kright;
		vector <unsigned int> data;
		vector <unsigned int> triangles;
		BoundingBox bbox;

		const KDTreeNode & _find (const Vec3Df & v, unsigned int axis) const;
		bool _lineInBox (const Vec3Df & origin, const Vec3Df & direction, const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
		bool _triangleInBox (const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2, const BoundingBox & bbox) const;

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
		KDTreeNode() : split(0),kleft(NULL),kright(NULL) { /*cout << "Creating KD-Tree " << this << endl;*/ }

		/**
		 * Class destructor
		 */
		~KDTreeNode () {
			cout << "Destroying KD-Tree " << this << endl;
			if (kleft != NULL) { delete kleft; kleft = NULL; }
			if (kright != NULL) { delete kright; kright = NULL; }
		}

		/**
		 * KDTreeNode Class Constructor, with direct mesh loading.
		 * 
		 * @author François-Xavier Thomas
		 */
		KDTreeNode(const Mesh & m) : mesh(&m),kleft(NULL),kright(NULL) { cout << "Creating KD-Tree " << this << endl; load (); }

		/**
		 * Clear KD-Tree
		 */
		void clear () {
			data.clear();
			triangles.clear();
			split = 0;
			if (kleft != NULL) { delete kleft; kleft = NULL; }
			if (kright != NULL) { delete kright; kright = NULL; }
		}

		/**
		 * Number of random samples for the median
		 */
		const static unsigned int NSAMPLES = 100;

		/**
		 * Maximum leaf size
		 */
		const static unsigned int LEAFSIZE = 1;

		/**
		 * Tree fuzziness
		 */
<<<<<<< local
		//const static float FUZZINESS=0.12f;
		const static float FUZZINESS=0.05f;
=======
		const static float FUZZINESS=0.12f;
>>>>>>> other

		/**
		 * Setters and getters
		 */
		inline const KDTreeNode* getLeft () const { return kleft; }
		inline const KDTreeNode* getRight () const { return kright; }
		inline const vector<unsigned int> & getVertices () const { return data; }
		inline float getSplit() const { return split; }
		inline const Mesh * getMesh () const { return mesh; }
		inline const BoundingBox & getBoundingBox () const { return bbox; }
		inline const vector<unsigned int> & getTriangles () const { return triangles; }

		inline KDTreeNode & operator= (const KDTreeNode & kd) {
			clear();
			mesh = kd.mesh;
			bbox = bbox;
			return *this;
		}

		void show () const {
			if (kleft == NULL && kright == NULL) {
				for (vector<unsigned int>::const_iterator it = triangles.begin(); it != triangles.end(); it++) cout << "Tri: " << *it << endl;
			}
			cout << "-- " << this << endl << "left: " << kleft << ", right: " << kright << endl;
			if (kleft != NULL) kleft->show();
			if (kright != NULL) kright->show();
		}

		/**
		 * Find vertices
		 */
		inline const KDTreeNode & find (const Vec3Df & v) const { return _find (v, 0); }
};
