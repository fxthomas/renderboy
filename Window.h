#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLCDNumber>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QProgressBar>

#include <vector>
#include <string>

#include "QTUtils.h"
#include "Camera.hpp"
#include "GLViewer.h"
#include "QClickableLabel.hpp"
#include "KDTreeNode.hpp"

class Window : public QMainWindow {
    Q_OBJECT
public:
    Window();
    virtual ~Window();

    static void showStatusMessage (const QString & msg);  
    
public slots :
    void renderRayImage ();
    void setBGColor ();
    void exportGLImage ();
    void exportRayImage ();
    void about ();
		void displayPointInfo (QMouseEvent* me);
		void setRayImage (const QImage & img);
    
private :
    void initControlWidget ();
        
    QActionGroup * actionGroup;
    QGroupBox * controlWidget;
    QString currentDirectory;

    GLViewer * viewer;
    QClickableLabel * imageLabel;
    QImage rayImage;
		QProgressBar * progressbar ;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
