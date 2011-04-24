// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>

#include "Vec3D.h"

class Light {
public:
    inline Light () : color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f), radius (0.0f) {}
    inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity, float radius, Vec3Df orientation )
        : pos (pos), color (color), intensity (intensity), radius (radius), orientation (orientation) {}
    virtual ~Light () {}

    inline const Vec3Df & getPos () const { return pos; }
    inline const Vec3Df & getColor () const { return color; }
    inline const Vec3Df & getOrientation () const { return orientation; }
    inline float getIntensity () const { return intensity; }
    inline float getRadius () const { return radius; }

    inline void setPos (const Vec3Df & p) { pos = p; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setOrientation (const Vec3Df & o) { orientation = o; }
    inline void setIntensity (float i) { intensity = i; }
    inline void setRadius (float r) { radius = r; }

    
private:
    Vec3Df pos;
    Vec3Df color;
    float intensity;
    float radius;
    Vec3Df orientation;
};


#endif // LIGHT_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
