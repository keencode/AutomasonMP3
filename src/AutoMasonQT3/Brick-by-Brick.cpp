//----------------------------------------------------------------------------
// out include files
//----------------------------------------------------------------------------
#include <qapplication.h>
#include "bbb_Form.h"

//----------------------------------------------------------------------------
// Main
//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
	// initialize Qt
    QApplication app( argc, argv );

	// create our main form object
    BBB_Form form;
    form.setGeometry( 100, 100, 200, 120 );
    app.setMainWidget( &form );
    form.show();

	// run Qt app
    return app.exec();
}

