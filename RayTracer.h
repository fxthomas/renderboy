// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <vector>
#include <QImage>
#include <QThread>
#include <QObject>
#include <QTime>

#include "Camera.hpp"
#include "Vec3D.h"
#include "KDTreeNode.hpp"

using namespace std;

class RayTracer : public QThread {
	Q_OBJECT

	public:
    static RayTracer * getInstance ();
    static void destroyInstance ();

    inline const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    inline void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }
    
    Vec3Df raytraceSingle (unsigned int i, unsigned int j, bool debug, BoundingBox & bb);
    QImage render ();
    BoundingBox debug (unsigned int i, unsigned int j);

		void setCamera (const Camera _cam) { cam = _cam; }
		const Camera & getCamera () const { return cam; }

	signals:
		void init (int min, int max);
		void progress (int val);
		void finished (const QImage & pixm);
    
	protected:
    inline RayTracer (QObject* parent = 0) : QThread(parent) { }
    inline virtual ~RayTracer () {}
		virtual void run() { 
			// Initialize everything listening to the raytracer status
			emit init (0, cam.screenWidth());

			// Render image
			renderedimage = render (); 

			// Signel the listeners the image is ready!
			emit finished (renderedimage);
		}
    
	private:
    Vec3Df backgroundColor;
		Camera cam;
		QImage renderedimage;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
