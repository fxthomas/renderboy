/**
 * Camera C++ Header (Camera.hpp)
 * Created: Sun 17 Apr 2011 07:32:17 PM CEST
 *
 * This C++ Header was developped by François-Xavier Thomas.
 * You are free to copy, adapt or modify it.
 * If you do so, however, leave my name somewhere in the credits, I'd appreciate it ;)
 * 
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 * @version 1.0
 */

#pragma once
#include <QGLViewer/camera.h>
#include "Vec3D.h"

/**
 * Camera Class
 * This class does something you ought to say here!
 *
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 */
class Camera {
	public:
		/**
		 * Camera Class Constructor
		 * 
		 * @author François-Xavier Thomas
		 */
		Camera(const qglviewer::Camera & cam) : camera(cam) {
			computeProjectionViewMatrix();
		};

		/**
		 * Camera Class Destructor
		 *
		 * @author François-Xavier Thomas
		 */
		~Camera() { };

		/**
		 * Transform vector into world coordinates
		 */
		inline Vec3Df toWorld (const Vec3Df & v) const {
			float nv[3];
			camera.getWorldCoordinatesOf (v.getData(), nv);
			return Vec3Df (nv);
		}

		inline void computeProjectionViewMatrix () {
			camera.getModelViewProjectionMatrix(matrix);
			camera.getViewport(viewport);
		}

		/**
		 * Transform vector into screen coordinates
		 * 
		 * @see http://www.libqglviewer.com/refManual/classqglviewer_1_1Camera.html#adeb5e39686b9fcb3db090dff84c13be4
		 */
		inline Vec3Df toScreen (const Vec3Df & point) const {
			double vs[4];

			vs[0]=matrix[0 ]*point[0] + matrix[4 ]*point[1] + matrix[8 ]*point[2] + matrix[12 ];
			vs[1]=matrix[1 ]*point[0] + matrix[5 ]*point[1] + matrix[9 ]*point[2] + matrix[13 ];
			vs[2]=matrix[2 ]*point[0] + matrix[6 ]*point[1] + matrix[10]*point[2] + matrix[14 ];
			vs[3]=matrix[3 ]*point[0] + matrix[7 ]*point[1] + matrix[11]*point[2] + matrix[15 ];

			vs[0] /= vs[3];
			vs[1] /= vs[3];
			vs[2] /= vs[3];

			vs[0] = vs[0] * 0.5 + 0.5;
			vs[1] = vs[1] * 0.5 + 0.5;
			vs[2] = vs[2] * 0.5 + 0.5;

			vs[0] = vs[0] * viewport[2] + viewport[0];
			vs[1] = vs[1] * viewport[3] + viewport[1];

			return Vec3Df (vs[0], viewport[3]-vs[1], vs[2]);
		}

		/**
		 * Camera position
		 */
		inline Vec3Df position () const { return Vec3Df (camera.position()[0], camera.position()[1], camera.position()[2]); }

		/**
		 * Camera up vector
		 */
		inline Vec3Df upVector () const { return Vec3Df (camera.upVector()[0], camera.upVector()[1], camera.upVector()[2]); }

		/**
		 * Camera right vector
		 */
		inline Vec3Df rightVector () const { return Vec3Df (camera.rightVector()[0], camera.rightVector()[1], camera.rightVector()[2]); }

		/**
		 * Camera direction vector
		 */
		inline Vec3Df viewDirection () const { return Vec3Df (camera.viewDirection()[0], camera.viewDirection()[1], camera.viewDirection()[2]); }

		/**
		 * Returns screen height
		 */
		inline int screenHeight () const { return camera.screenHeight(); }

		/**
		 * Returns screen width
		 */
		inline int screenWidth () const { return camera.screenWidth(); }

		/**
		 * Returns horizontal FOV
		 */
		inline int horizontalFieldOfView () const { return camera.horizontalFieldOfView(); }

		/**
		 * Returns aspect ratio
		 */
		inline int aspectRatio() const { return camera.aspectRatio(); }
		
	protected:
		qglviewer::Camera camera;
		double matrix[16];
		int viewport[4];
};
