// *********************************************************
// Object Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "KDTreeNode.hpp"
#include "Material.h"
#include "BoundingBox.h"

using namespace std;

class Object {
public:
    inline Object () { cout << "Creating object " << this << endl; }
    inline Object (const Mesh & mesh, const Material & mat) :mesh (mesh), mat (mat) {
				cout << "Creating object " << this << endl;
        updateBoundingBox ();
				kdt = NULL;
    }

    virtual ~Object () {
			cout << "Destroying object " << this << endl;
			if (kdt != NULL) { delete kdt; kdt = NULL; }
		}

    inline const Mesh & getMesh () const { return mesh; }
    inline Mesh & getMesh () { return mesh; }
    
    inline const Material & getMaterial () const { return mat; }
    inline Material & getMaterial () { return mat; }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

		inline void computeKdTree () {
			if (kdt == NULL) {
				cout << "Building KD-Tree..." << endl;
				kdt = new KDTreeNode (mesh);
			}
		}

		inline const KDTreeNode * getKdTree () const {
			return kdt;
		}

		inline Object & operator= (const Object & o) {
			mesh = o.mesh;
			mat = o.mat;
			bbox = o.bbox;
			if (kdt != NULL) { delete kdt; kdt = NULL; }
			return *this;
		}
    
private:
    Mesh mesh;
    Material mat;
    BoundingBox bbox;
		KDTreeNode *kdt;
};


#endif // Scene_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
