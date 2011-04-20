#include <QApplication>
#include <Window.h>
#include <QDir>
#include <QPixmap>
#include <QSplashScreen>
#include <QPlastiqueStyle>
#include <QCleanlooksStyle>
#include <string>
#include <iostream>

#include "QTUtils.h"

using namespace std;

int main (int argc, char **argv)
{
	cout << "Renderboy \\o/" << endl;
	cout << " (I) Based on GUI code by Tamy Boubekeur" << endl;
	cout << " (I) Yann Jacquot: yann.jacquot@telecom-paristech.fr" << endl;
	cout << " (I) François-Xavier Thomas: fthomas@telecom-paristech.fr" << endl;
	cout << endl;
	cout << " (I) See: http://bitbucket.org/fxthomas/renderboy" << endl;
	cout << endl;

	cout << " --- Loading..." << endl << endl;
  QApplication raymini (argc, argv);
  setBoubekQTStyle (raymini);
  QApplication::setStyle (new QPlastiqueStyle);
  Window * window = new Window ();
  window->setWindowTitle ("Renderboy");
  window->showMaximized ();
  window->show();
  raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));
  
	cout << endl << " --- Starting Main GUI" << endl << endl;
  return raymini.exec ();
}

