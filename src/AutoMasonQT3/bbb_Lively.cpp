#include "bbb_Lively.h"


void bbb_Lively::run()
{
	// for( int count = 0; count < 20; count++ ) {
	while(quit == false) {
		sleep( 1 );
        // qDebug( "Ping!" );
		qApp->processEvents();
		qWgt->updateGL();
	}
}

bbb_Lively::bbb_Lively(QApplication * _qApp, QGLWidget * _qWgt)
{
		quit = false;
		qApp = _qApp;
		qWgt = _qWgt;
}

bbb_Lively::~bbb_Lively(void)
{
}
