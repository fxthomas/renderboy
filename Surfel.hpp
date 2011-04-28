/**
 * Surfel C++ Header (Surfel.hpp)
 * Created: Mon 25 Apr 2011 05:10:58 PM CEST
 *
 * This C++ Header was developped by François-Xavier Thomas.
 * You are free to copy, adapt or modify it.
 * If you do so, however, leave my name somewhere in the credits, I'd appreciate it ;)
 * 
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 * @version 1.0
 */

#pragma once
#include "Vec3D.h"
#include <cmath>

/**
 * Surfel Class
 * This class represents a surfel, according to the Point-based Global Illumination Method
 *
 * @see http://cgg.mff.cuni.cz/~jaroslav/gicourse2010/giai2010-03-per_christensen-notes.pdf
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 */
class Surfel {
	public:
		/**
		 * Surfel Class Constructor
		 * 
		 * @author François-Xavier Thomas
		 */
		Surfel() {}

		/**
		 * Surfel Class Constructor
		 * 
		 * @author François-Xavier Thomas
		 */
		Surfel(const Vec3Df & position, const Vec3Df & normal, float radius, const Vec3Df & color) : position(position), normal(normal), radius(radius), color(color) { }

		/**
		 * Surfel Class Constructor, knowing a triangle and its computed color.
		 * 
		 * @author François-Xavier Thomas
		 */
		Surfel(const Vec3Df & v0, const Vec3Df & v1, const Vec3Df & v2, const Vec3Df & color) : color(color) {
			Vec3Df u = v1 - v0;
			Vec3Df v = v2 - v0;
			u.normalize();
			v.normalize();
			float dp = Vec3Df::dotProduct (u, v);
			float sin0 = sqrtf (1.f - dp*dp);

			radius = (v2 - v1).getLength() / (2.f * sin0);
			position = (v0 + v1 + v2) / 3.f;
			normal = Vec3Df::crossProduct (u, v);
		}

		inline const Vec3Df & getPosition () const { return position; }
		inline const Vec3Df & getNormal () const { return normal; }
		inline float getRadius() const { return radius; }
		inline const Vec3Df & getColor() const { return color; }
		
	protected:
		Vec3Df position;
		Vec3Df normal;
		float radius;
		Vec3Df color;
};

