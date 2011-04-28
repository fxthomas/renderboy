// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>

#include "Vec3D.h"

// Ce modèle suppose une couleur spéculaire blanche (1.0, 1.0, 1.0)

class Material {
public:
    inline Material () : diffuse (0.8f), specular (0.2f), shine (1.0f), color (0.5f, 0.5f, 0.5f), ior(1.0f), refract(0.0f), reflect(0.0) {}
    inline Material (float diffuse, float specular, float shine, const Vec3Df & color, const float ior, const float refract, const float reflect)
        : diffuse (diffuse), specular (specular), shine (shine), color (color), ior (ior), refract (refract), reflect (reflect) {}
    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
		inline float getShininess () const { return shine; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }
		inline float getIOR () const { return ior; }
		inline float getRefract () const { return refract; }
		inline float getReflect () const { return reflect; }

    inline void setDiffuse (float d) { diffuse = d; }
		inline void setShininess (float sh) { shine = sh; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }
		inline void setIOR (const float f) { ior = f; }
		inline void setRefract (const float r) { refract = r; }
		inline void setReflect (const float r) { reflect = r; }

private:
    float diffuse;
    float specular;
		float shine;
    Vec3Df color;
		float ior;
		float refract;
		float reflect;
};

inline std::ostream & operator<< (std::ostream & out, const Material & m) {
	out << "[Di: " << m.getDiffuse() << " | Sp: " << m.getSpecular() << "/" << m.getShininess() << " | Col: " << m.getColor() << " | IOR: " << m.getIOR() << "/" << m.getRefract() << "]";
	return out;
}

#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
