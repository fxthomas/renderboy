// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>
#include <QObject>
#include <cmath>

#include "Object.h"
#include "Light.h"
#include "BoundingBox.h"

class Scene : public QObject {
	Q_OBJECT

	public:
    static Scene * getInstance ();
    static void destroyInstance ();
    
    inline std::vector<Object> & getObjects () { return objects; }
    inline const std::vector<Object> & getObjects () const { return objects; }
    
    inline std::vector<Light> & getLights () { return lights; }
    inline const std::vector<Light> & getLights () const { return lights; }
    
    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

		inline const BoundingBox & getSelectedBoundingBox() const { return selbb; }
		inline void setSelectedBoundingBox(const BoundingBox & bb) { selbb = bb; };
    
	protected:
    Scene ();
    virtual ~Scene ();
    
	private:
    void buildDefaultScene (bool HD);
    std::vector<Object> objects;
    std::vector<Light> lights;
    BoundingBox bbox;
		BoundingBox selbb;

	public slots:
		void setFuzziness (int f) {
			float ff = pow (10.f, -3.f + float(f)/8.f);
			cout << " (I) Setting Fuzziness to " << ff << endl;
			for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++) {
				cout << " (I) Rebuilding kD-Tree...";
				it->getKdTree()->setFuzziness (ff);
				it->getKdTree()->rebuild();
				cout << "done" << endl;
			}
			cout << endl;
		}

		void setRadius (int r) {
			float rr = (float) r/5;
			cout << " (I) Setting Radius to " << rr << endl;
			for (vector<Light>::iterator light = getLights().begin(); light != getLights().end(); light++) {
				cout<<"Changing the radius of the light source...";				
				light->setRadius(rr);
				cout<<"done"<<endl;
			}
			cout << endl;
		}
};


#endif // SCENE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
