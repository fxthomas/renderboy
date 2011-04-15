/**
 * QClickableLabel C++ Header (QClickableLabel.hpp)
 * Created: Fri 15 Apr 2011 05:26:47 PM CEST
 *
 * This C++ Header was developped by François-Xavier Thomas.
 * You are free to copy, adapt or modify it.
 * If you do so, however, leave my name somewhere in the credits, I'd appreciate it ;)
 * 
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 * @version 1.0
 */

#include <QLabel>
#include <QMouseEvent>

/**
 * QClickableLabel Class
 * This class does something you ought to say here!
 *
 * @author François-Xavier Thomas <fx.thomas@gmail.com>
 */
class QClickableLabel : public QLabel {
	Q_OBJECT

	public:
		/**
		 * QClickableLabel Class Constructor
		 * 
		 * @author François-Xavier Thomas
		 */
		QClickableLabel() : QLabel() { }

	signals:
		void clicked (QMouseEvent * me);
	
	protected:
		virtual void mousePressEvent (QMouseEvent * me) {
			emit clicked (me);
		}
};

