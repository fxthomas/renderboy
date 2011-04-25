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

#include "PointCloud.hpp"
#include "Camera.hpp"
#include "Material.h"
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
    
		Vec3Df getColor (const Vec3Df & eye, const Vec3Df & point, const Vec3Df & normal, const Material & mat);
		Vec3Df lightModel (const Vec3Df & eye, const Vec3Df & point, const Vec3Df & normal, const Material & mat, const PointCloud & pc, bool debug);
		Vec3Df lightBounce (const Vec3Df & eye, const Vec3Df & dir, const Vec3Df & point, const Vec3Df & normal, const Material & mat, const PointCloud & pc, bool debug, int d);
    Vec3Df raytraceSingle (const PointCloud & pc, unsigned int i, unsigned int j, bool debug, BoundingBox & bb);
    QImage render ();
    BoundingBox debug (unsigned int i, unsigned int j);

		void setCamera (const Camera _cam) { cam = _cam; }
		const Camera & getCamera () const { return cam; }

		void setDepth (const int d) { depth = d; }
		int getDepth () const { return depth; }

	signals:
		void init (int min, int max);
		void progress (int val);
		void finished (const QImage & pixm);
    
	protected:
    inline RayTracer (QObject* parent = 0) : QThread(parent), depth(8) { }
    inline virtual ~RayTracer () {}
		virtual void run() { 
			cout << " (I) RayTracer: Starting thread" << endl;
			// Initialize everything listening to the raytracer status
			emit init (0, cam.screenWidth());

			// Render image
			renderedimage = render (); 

			// Signel the listeners the image is ready!
			emit finished (renderedimage);
			cout << " (I) RayTracer: End of thread" << endl;
		}
    
	private:
    Vec3Df backgroundColor;
		Camera cam;
		QImage renderedimage;
		int depth;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
