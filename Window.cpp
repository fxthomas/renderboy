#include "Window.h"

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <QDockWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMenuBar>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QLCDNumber>
#include <QPixmap>
#include <QFrame>
#include <QSplitter>
#include <QMenu>
#include <QScrollArea>
#include <QCoreApplication>
#include <QFont>
#include <QSizePolicy>
#include <QImageReader>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QSplashScreen>

#include "RayTracer.h"

using namespace std;


Window::Window () : QMainWindow (NULL) {
	// Load splash
	QPixmap pixmap("RenderBoy.png");
	QSplashScreen *splash = new QSplashScreen(pixmap);
	splash->show();

	// Loading some items
	splash->showMessage("Loading...");
	qApp->processEvents();

	try {
		viewer = new GLViewer;
	} catch (GLViewer::Exception e) {
		cerr << e.getMessage () << endl;
		exit (1);
	}

	// Initialize scene
	splash->showMessage("Loading scene...");
	qApp->processEvents();
	(void) Scene::getInstance();

	splash->showMessage("Loading main window...");
	qApp->processEvents();
	QGroupBox * renderingGroupBox = new QGroupBox (this);
	QHBoxLayout * renderingLayout = new QHBoxLayout (renderingGroupBox);

	imageLabel = new QClickableLabel;
	imageLabel->setBackgroundRole (QPalette::Base);
	imageLabel->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents (true);
	imageLabel->setPixmap (QPixmap::fromImage (rayImage));
	connect (RayTracer::getInstance(), SIGNAL(finished(const QImage&)), this, SLOT(setRayImage (const QImage&)));

	renderingLayout->addWidget (viewer);
	renderingLayout->addWidget (imageLabel);

	setCentralWidget (renderingGroupBox);

	QDockWidget * controlDockWidget = new QDockWidget (this);
	initControlWidget ();

	controlDockWidget->setWidget (controlWidget);
	controlDockWidget->adjustSize ();
	addDockWidget (Qt::RightDockWidgetArea, controlDockWidget);
	controlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);

	setMinimumWidth (800);
	setMinimumHeight (400);

	// Remove splash
	splash->finish(this);
	delete splash;
}

Window::~Window () {

}

void Window::setRayImage (const QImage & img) {
	imageLabel->setPixmap (QPixmap::fromImage (img));
}

void Window::renderRayImage () {
	RayTracer::getInstance ()->setCamera (viewer->getCamera());
	RayTracer::getInstance ()->start ();
}

void Window::setBGColor () {
	QColor c = QColorDialog::getColor (QColor (133, 152, 181), this);
	if (c.isValid () == true) {
		cout << c.red () << endl;
		RayTracer::getInstance ()->setBackgroundColor (Vec3Df (c.red (), c.green (), c.blue ()));
		viewer->setBackgroundColor (c);
		viewer->updateGL ();
	}
}

void Window::exportGLImage () {
	viewer->saveSnapshot (false, false);
}

void Window::exportRayImage () {
	QString filename = QFileDialog::getSaveFileName (this,
			"Save ray-traced image",
			".",
			"*.jpg *.bmp *.png");
	if (!filename.isNull () && !filename.isEmpty ()) {
		rayImage.save (filename);
	}
}

void Window::about () {

	QMessageBox::about (this, 
			"About This Program", 
			"<b>RayMini</b> <br> by <i>Tamy Boubekeur</i>.");
}

void Window::displayPointInfo (QMouseEvent* me) {
	Camera cam = viewer->getCamera ();
	RayTracer * rayTracer = RayTracer::getInstance ();

	cout << " (I) Raytracing: Click at (" << me->x() << ", " << me->y() << ")" << endl;

	Scene::getInstance()->setSelectedBoundingBox(rayTracer->debug ((unsigned int)me->x(), cam.screenHeight() - (unsigned int)me->y() + 1));
	viewer->updateGL ();
}

void Window::initControlWidget () {
	controlWidget = new QGroupBox ();
	QVBoxLayout * layout = new QVBoxLayout (controlWidget);

	QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
	QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);

	QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
	connect (wireframeCheckBox, SIGNAL (toggled (bool)), viewer, SLOT (setWireframe (bool)));
	previewLayout->addWidget (wireframeCheckBox);

	QButtonGroup * modeButtonGroup = new QButtonGroup (previewGroupBox);
	modeButtonGroup->setExclusive (true);
	QRadioButton * flatButton = new QRadioButton ("Flat", previewGroupBox);
	QRadioButton * smoothButton = new QRadioButton ("Smooth", previewGroupBox);
	modeButtonGroup->addButton (flatButton, static_cast<int>(GLViewer::Flat));
	modeButtonGroup->addButton (smoothButton, static_cast<int>(GLViewer::Smooth));
	connect (modeButtonGroup, SIGNAL (buttonClicked (int)), viewer, SLOT (setRenderingMode (int)));
	previewLayout->addWidget (flatButton);
	previewLayout->addWidget (smoothButton);

	QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
	connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
	previewLayout->addWidget (snapshotButton);

	layout->addWidget (previewGroupBox);

	QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
	QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);

	QLabel * fuzzyLabel = new QLabel ("kD-Tree fuzziness", rayGroupBox);
	rayLayout->addWidget (fuzzyLabel);

	QSlider * fuzzySlider = new QSlider (Qt::Horizontal, rayGroupBox);
	fuzzySlider->setTracking (false);
	fuzzySlider->setMinimum (0); // Fuzziness = 10^(-3 + Value/8))
	fuzzySlider->setMaximum (20);
	fuzzySlider->setValue (6);
	connect (fuzzySlider, SIGNAL (valueChanged(int)), Scene::getInstance(), SLOT (setFuzziness(int)));
	rayLayout->addWidget (fuzzySlider);

	QLabel * radiusLabel = new QLabel ("Light Source Radius", rayGroupBox);
	rayLayout->addWidget (radiusLabel);

	QSlider * radiusSlider = new QSlider (Qt::Horizontal, rayGroupBox);
	radiusSlider->setTracking (false);
	radiusSlider->setMinimum (0); // Fuzziness = 10^(-3 + Value/8))
	radiusSlider->setMaximum (5);
	radiusSlider->setValue (1);
	connect (radiusSlider, SIGNAL (valueChanged(int)), Scene::getInstance(), SLOT (setRadius(int)));
	rayLayout->addWidget (radiusSlider);

	QPushButton * rayButton = new QPushButton ("Render", rayGroupBox);
	rayLayout->addWidget (rayButton);
	connect (rayButton, SIGNAL (clicked ()), this, SLOT (renderRayImage ()));
	connect (imageLabel, SIGNAL (clicked(QMouseEvent*)), this, SLOT (displayPointInfo(QMouseEvent*)));

	QPushButton * saveButton  = new QPushButton ("Save", rayGroupBox);
	connect (saveButton, SIGNAL (clicked ()) , this, SLOT (exportRayImage ()));
	rayLayout->addWidget (saveButton);

	progressbar = new QProgressBar (rayGroupBox);
	rayLayout->addWidget (progressbar);
	connect (RayTracer::getInstance(), SIGNAL(init(int,int)), progressbar, SLOT(setRange(int,int)));
	connect (RayTracer::getInstance(), SIGNAL(progress(int)), progressbar, SLOT(setValue(int)));

	layout->addWidget (rayGroupBox);

	QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", controlWidget);
	QVBoxLayout * globalLayout = new QVBoxLayout (globalGroupBox);

	QPushButton * bgColorButton  = new QPushButton ("Background Color", globalGroupBox);
	connect (bgColorButton, SIGNAL (clicked()) , this, SLOT (setBGColor()));
	globalLayout->addWidget (bgColorButton);

	QPushButton * aboutButton  = new QPushButton ("About", globalGroupBox);
	connect (aboutButton, SIGNAL (clicked()) , this, SLOT (about()));
	globalLayout->addWidget (aboutButton);

	QPushButton * quitButton  = new QPushButton ("Quit", globalGroupBox);
	connect (quitButton, SIGNAL (clicked()), qApp, SLOT (closeAllWindows()));
	globalLayout->addWidget (quitButton);

	layout->addWidget (globalGroupBox);

	layout->addStretch (0);
}
