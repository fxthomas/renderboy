# Renderboy

![Renderboy](https://raw.github.com/fxthomas/renderboy/master/images/RenderBoy.png)

Renderboy is an experimental project on raytracing, whose features include :

  * Reflection and refraction
  * Point-based Global Illumination
  * Anti-aliasing settings

![Screenshot](https://raw.github.com/fxthomas/renderboy/master/images/Screenshot-Glass.png)

We ([François-Xavier Thomas](http://github.com/fxthomas) and Yann Jacquot) did
this project as a part of the _3D Graphics_ course at Télécom ParisTech.

The base UI and some parts of the libraries were developed by [Tamy
Boubekeur](http://perso.telecom-paristech.fr/~boubek/).

# How to build?

Unfortunately this is a rather old project I did not even intend to maintain,
so we only tested it on Linux machines. You'll need :

  * QT Libraries (along with commands like `qmake`)
  * libQGLViewer
  * OpenGL, GLU, GLUT libraries

Then, generate the Makefile with `qmake renderboy.pro`, and type `make`.
