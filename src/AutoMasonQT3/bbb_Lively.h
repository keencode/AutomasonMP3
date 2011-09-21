#include <qapplication.h>
#include <qthread.h>
#include <qgl.h>

class bbb_Lively : public QThread
{
	public:
		bbb_Lively(QApplication * _qApp, QGLWidget * _qWgt);
		~bbb_Lively(void);
        virtual void run();
		bool quit;
	
	private:
		QApplication * qApp;
		QGLWidget * qWgt;
};
