/**
 * PointCloud C++ Header (PointCloud.hpp)
 * Created: Mon 25 Apr 2011 05:23:13 PM CEST
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
#include <iostream>
#include <cstdlib>

#include "Scene.h"
#include "Camera.hpp"
#include "Material.h"
#include "Surfel.hpp"
#include "Triangle.h"
#include "Vec3D.h"
#include "Vertex.h"
#include "Mesh.h"

/**
 * PointCloud Class
 * This class represents a point cloud in the point-based global illumination (GI) method.
 *
 * @see http://cgg.mff.cuni.cz/~jaroslav/gicourse2010/giai2010-03-per_christensen-notes.pdf
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 */
class PointCloud {
	public:
		/**
		 * PointCloud Class Constructor
		 * 
		 * @author François-Xavier Thomas
		 */
		PointCloud() { }

		inline void clear () { surfels.clear(); }
		inline void add (Surfel s) { surfels.push_back (s); }
		inline void add (const Object & o, const Camera & cam) {
			const unsigned int MAX_POINT = 100;
			for (unsigned int i = 0; i < MAX_POINT && i < (unsigned int)(o.getMesh().getTriangles().size()); i++) {
				Triangle it = o.getMesh().getTriangles ()[rand()%o.getMesh().getTriangles().size()];
				Vec3Df v0 = o.getMesh().getVertices()[it.getVertex(0)].getPos();
				Vec3Df v1 = o.getMesh().getVertices()[it.getVertex(1)].getPos();
				Vec3Df v2 = o.getMesh().getVertices()[it.getVertex(2)].getPos();
				Vec3Df u = v1 - v0;
				Vec3Df v = v2 - v0;

				// Compute Surfel radius
				float dp = Vec3Df::dotProduct (u, v);
				float sin0 = sqrtf (1.f - dp*dp);
				float radius = (v2 - v1).getLength() / (2.f * sin0);

				// Compute Surfel color
				Vec3Df eye = cam.position();
				Vec3Df point = (v0 + v1 + v2)/3.f;
				Vec3Df normal = Vec3Df::crossProduct (u, v);
				Material mat = o.getMaterial();

				Vec3Df diffuseSelf,specularSelf;
				Vec3Df c = mat.getColor();
				Vec3Df vv = eye - point;
				Vec3Df lpos, lm;
				vv.normalize();

				for (vector<Light>::iterator light = Scene::getInstance()->getLights().begin(); light != Scene::getInstance()->getLights().end(); light++) {
					lpos = cam.toWorld (light->getPos());
					lm = lpos - point;
					lm.normalize();

					// Diffuse Light
					float sc = Vec3D<float>::dotProduct(lm, normal);
					diffuseSelf += light->getColor() * fabs (sc);

					// Specular Light
					sc = Vec3D<float>::dotProduct(normal*sc*2.f-lm, vv);
					if (sc > 0.) {
						sc = pow (sc, mat.getShininess() * 40.f);
						specularSelf += light->getColor() * sc;
					}
				}

				// Total color blend
				c[0] = (mat.getDiffuse()*c[0]*diffuseSelf[0] + mat.getSpecular()*specularSelf[0]);
				c[1] = (mat.getDiffuse()*c[1]*diffuseSelf[1] + mat.getSpecular()*specularSelf[1]);
				c[2] = (mat.getDiffuse()*c[2]*diffuseSelf[2] + mat.getSpecular()*specularSelf[2]);

				surfels.push_back (Surfel (point, normal, radius, c));
			}

			cout << " (I) Point cloud size is now: " << surfels.size() << endl;
		}

		inline const vector<Surfel> & getSurfels() const { return surfels; }
		
	protected:
		vector<Surfel> surfels;
};
