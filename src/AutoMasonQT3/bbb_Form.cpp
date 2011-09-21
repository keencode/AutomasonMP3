//----------------------------------------------------------------------------------
#include "bbb_Form.h"
#include <qmessagebox.h>
//----------------------------------------------------------------------------------
// our main form for Brick-by-Brick
//----------------------------------------------------------------------------------
BBB_Form::BBB_Form( QWidget *parent, const char *name ) : QWidget( parent, name )
{
	// set our forms size and name
	setName("AutoMason");
	// setMinimumSize( 1024,768 );
	setMinimumSize( 1024,600 );
	
	setCaption("AutoMason v 2.0");

	//--------------------------------------------------------------------------------
	// Main Palette Creation
	//--------------------------------------------------------------------------------
	QPalette pal;
	QColorGroup cg;
	cg.setColor( QColorGroup::Foreground, black );
	cg.setColor( QColorGroup::Button, QColor( 158, 158, 158) );
	cg.setColor( QColorGroup::Light, QColor( 237, 237, 237) );
	cg.setColor( QColorGroup::Midlight, QColor( 197, 197, 197) );
	cg.setColor( QColorGroup::Dark, QColor( 79, 79, 79) );
	cg.setColor( QColorGroup::Mid, QColor( 105, 105, 105) );
	cg.setColor( QColorGroup::Text, black );
	cg.setColor( QColorGroup::BrightText, white );
	cg.setColor( QColorGroup::ButtonText, black );
	cg.setColor( QColorGroup::Base, white );
	cg.setColor( QColorGroup::Background, QColor( 197, 197, 197) );
	cg.setColor( QColorGroup::Shadow, black );
	cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
	cg.setColor( QColorGroup::HighlightedText, white );
	cg.setColor( QColorGroup::Link, black );
	cg.setColor( QColorGroup::LinkVisited, black );
	pal.setActive( cg );
	cg.setColor( QColorGroup::Foreground, black );
	cg.setColor( QColorGroup::Button, QColor( 158, 158, 158) );
	cg.setColor( QColorGroup::Light, QColor( 237, 237, 237) );
	cg.setColor( QColorGroup::Midlight, QColor( 181, 181, 181) );
	cg.setColor( QColorGroup::Dark, QColor( 79, 79, 79) );
	cg.setColor( QColorGroup::Mid, QColor( 105, 105, 105) );
	cg.setColor( QColorGroup::Text, black );
	cg.setColor( QColorGroup::BrightText, white );
	cg.setColor( QColorGroup::ButtonText, black );
	cg.setColor( QColorGroup::Base, white );
	cg.setColor( QColorGroup::Background, QColor( 197, 197, 197) );
	cg.setColor( QColorGroup::Shadow, black );
	cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
	cg.setColor( QColorGroup::HighlightedText, white );
	cg.setColor( QColorGroup::Link, black );
	cg.setColor( QColorGroup::LinkVisited, black );
	pal.setInactive( cg );
	cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
	cg.setColor( QColorGroup::Button, QColor( 158, 158, 158) );
	cg.setColor( QColorGroup::Light, QColor( 237, 237, 237) );
	cg.setColor( QColorGroup::Midlight, QColor( 181, 181, 181) );
	cg.setColor( QColorGroup::Dark, QColor( 79, 79, 79) );
	cg.setColor( QColorGroup::Mid, QColor( 105, 105, 105) );
	cg.setColor( QColorGroup::Text, QColor( 128, 128, 128) );
	cg.setColor( QColorGroup::BrightText, white );
	cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
	cg.setColor( QColorGroup::Base, white );
	cg.setColor( QColorGroup::Background, QColor( 197, 197, 197) );
	cg.setColor( QColorGroup::Shadow, black );
	cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
	cg.setColor( QColorGroup::HighlightedText, white );
	cg.setColor( QColorGroup::Link, white );
	cg.setColor( QColorGroup::LinkVisited, white );
	pal.setDisabled( cg );
	setPalette( pal );

	//--------------------------------------------------------------------------------
	// UI - Toolbox Container Setup
	//--------------------------------------------------------------------------------
	tb_MainUI = new QToolBox( this, "tb_MainUI" );
	tb_MainUI->setGeometry( QRect( 855, 10, 165, 750 ) );
	tb_MainUI->setFrameShape( QToolBox::NoFrame );
	tb_MainUI->setFrameShadow( QToolBox::Plain );
	tb_MainUI->setCurrentIndex( 0 );

	pg_BrickCreation = new QWidget( tb_MainUI, "pg_BrickCreation" );
    pg_BrickCreation->setBackgroundMode( QWidget::PaletteBackground );

	pg_StartCondition = new QWidget( tb_MainUI, "pg_StartCondition" );
    pg_StartCondition->setBackgroundMode( QWidget::PaletteBackground );

	pg_Rules = new QWidget( tb_MainUI, "pg_Rules" );
    pg_Rules->setBackgroundMode( QWidget::PaletteBackground );
	pg_Rules->setFocusPolicy(QWidget::StrongFocus);

	pg_BeamCreation = new QWidget( tb_MainUI, "pg_BeamCreation" );
    pg_BeamCreation->setBackgroundMode( QWidget::PaletteBackground );

	tb_MainUI->addItem( pg_BrickCreation, QString::fromLatin1("Brick Creation") );
	tb_MainUI->addItem( pg_StartCondition, QString::fromLatin1("Start Conditions") );
	tb_MainUI->addItem( pg_Rules, QString::fromLatin1("Pattern Generation") );
	tb_MainUI->addItem( pg_BeamCreation, QString::fromLatin1("Beam Generation") );

	//--------------------------------------------------------------------------------
	// UI - Brick Generation
	//--------------------------------------------------------------------------------
	grp_BrickCreation = new QGroupBox( pg_BrickCreation, "grp_BrickCreation" );
	grp_BrickCreation->setGeometry( QRect( 0, 0, 165, 165) ); // , 140) );
	
	// brick size
	lbl_BrickSize = new QLabel( grp_BrickCreation, "lbl_BrickSize" );
	lbl_BrickSize->setGeometry( QRect( 5, 7, 50, 14 ) );
	lbl_BrickSize->setText("Brick Size:");
	
	lbl_BrickSize_Length = new QLabel( grp_BrickCreation, "lbl_BrickSize_Length" );
	lbl_BrickSize_Length->setGeometry( QRect( 5, 23, 30, 20 ) );
	lbl_BrickSize_Length->setText("width:");
	
	edit_BrickSize_Length = new QLineEdit( grp_BrickCreation, "edit_BrickSize_Length" );
	edit_BrickSize_Length->setGeometry( QRect( 35, 22, 40, 20 ) );
	edit_BrickSize_Length->setText("8");
	
	lbl_BrickSize_Width = new QLabel( grp_BrickCreation, "lbl_BrickSize_Width" );
	lbl_BrickSize_Width->setGeometry( QRect( 80, 22, 33, 20 ) );
	lbl_BrickSize_Width->setText("height:");
	
	edit_BrickSize_Width = new QLineEdit( grp_BrickCreation, "edit_BrickSize_Width" );
	edit_BrickSize_Width->setGeometry( QRect( 115, 22, 40, 20 ) );
	edit_BrickSize_Width->setText("4");
	
	// number of bricks
	lbl_NumBricks = new QLabel( grp_BrickCreation, "lbl_NumBricks" );
	lbl_NumBricks->setGeometry( QRect( 5, 53, 85, 20 ) );
	lbl_NumBricks->setText("Number of Bricks:");
	
	lbl_NumBricks_Rows = new QLabel( grp_BrickCreation, "lbl_NumBricks_Rows" );
	lbl_NumBricks_Rows->setGeometry( QRect( 5, 72, 26, 20 ) );
	lbl_NumBricks_Rows->setText("rows:");
	
	edit_NumBricks_Row = new QLineEdit( grp_BrickCreation, "edit_NumBricks_Row" );
	edit_NumBricks_Row->setGeometry( QRect( 35, 72, 40, 20 ) );
	edit_NumBricks_Row->setText("20");

	// CHIPP - Begin cantilevered height edit
	// cantilevered height
	lbl_Cantilevered_Height = new QLabel( grp_BrickCreation, "lbl_Cantilevered_Height" );
	lbl_Cantilevered_Height->setGeometry( QRect( 5, 91, 100, 20 ) );
	lbl_Cantilevered_Height->setText("cantilevered height:");
	lbl_Cantilevered_Height->hide();

	edit_Cantilevered_Height = new QLineEdit( grp_BrickCreation, "edit_Cantilevered_Height" );
	edit_Cantilevered_Height->setGeometry( QRect( 104, 91, 40, 20 ) );
	edit_Cantilevered_Height->setText("12");
	edit_Cantilevered_Height->hide();
	// CHIPP - End cantilevered height edit

	// CHIPP - Swap cantilevered height with Padding Rows
	// padding height
	lbl_Padding_Rows = new QLabel( grp_BrickCreation, "lbl_Padding_Rows" );
	lbl_Padding_Rows->setGeometry( QRect( 5, 91, 100, 20 ) );
	lbl_Padding_Rows->setText("padding rows:");
	
	edit_Padding_Rows = new QLineEdit( grp_BrickCreation, "edit_Padding_Rows" );
	edit_Padding_Rows->setGeometry( QRect( 74, 91, 40, 20 ) ); 
	edit_Padding_Rows->setText("1");
	// CHIPP - Swap cantilevered height with Padding Rows
	
	// layout toggle
	btn_BrickOffset = new QPushButton( grp_BrickCreation, "btn_BrickOffset" );
	// btn_BrickOffset->setGeometry( QRect( 10, 102, 140, 25 ));
	btn_BrickOffset->setGeometry( QRect( 10, 121, 140, 25 )); // CHIPP - bumped down brick offset button
	btn_BrickOffset->setText("Offset Bricks");
	btn_BrickOffset->setToggleButton(true);
	btn_BrickOffset->setPalette( pal );

	//--------------------------------------------------------------------------------
	// UI - Start Conditions
	//--------------------------------------------------------------------------------
	btn_ActivateBricks = new QPushButton( pg_StartCondition, "btn_ActivateBricks" );
	btn_ActivateBricks->setGeometry( QRect( 10, 10, 140, 25 ));
	btn_ActivateBricks->setText("Activate Bricks");
	btn_ActivateBricks->setToggleButton(true);
	btn_ActivateBricks->setPalette( pal );
	btn_ActivateBricks->setFocusProxy(pg_StartCondition);
	
	btn_ClearActiveBricks = new QPushButton( pg_StartCondition, "btn_ClearActiveBricks" );
	btn_ClearActiveBricks->setGeometry( QRect( 10, 40, 140, 25 ) );
	btn_ClearActiveBricks->setText("Clear Start Condition");
	btn_ClearActiveBricks->setPalette( pal );
	btn_ClearActiveBricks->setFocusProxy(pg_StartCondition);

	btn_RandomActiveBricks = new QPushButton( pg_StartCondition, "btn_RandomActiveBricks" );
	btn_RandomActiveBricks->setGeometry( QRect( 10, 70, 140, 25 ) );
	btn_RandomActiveBricks->setText("Random Start Condition");
	btn_RandomActiveBricks->setPalette( pal );
	btn_RandomActiveBricks->setFocusProxy(pg_StartCondition);

	//--------------------------------------------------------------------------------
	// UI - Rules
	//--------------------------------------------------------------------------------
	btn_Rule_00000 = new QPushButton( pg_Rules, "btn_Rule_00000" );
	btn_Rule_00000->setGeometry( QRect( 10, 9, 70, 20 ) );
	btn_Rule_00000->setPixmap( QPixmap::fromMimeSource( "data\\rule_00000.bmp" ) );
	btn_Rule_00000->setToggleButton(true);
	btn_Rule_00000->setPalette( pal );
	btn_Rule_00000->setFocusProxy(pg_Rules);
	
	btn_Rule_10000 = new QPushButton( pg_Rules, "btn_Rule_10000" );
	btn_Rule_10000->setGeometry( QRect( 10, 29, 70, 20 ) );
	btn_Rule_10000->setPixmap( QPixmap::fromMimeSource( "data\\rule_10000.bmp" ) );
	btn_Rule_10000->setToggleButton(true);
	btn_Rule_10000->setPalette( pal );
	btn_Rule_10000->setFocusProxy(pg_Rules);

	btn_Rule_01000 = new QPushButton( pg_Rules, "btn_Rule_01000" );
	btn_Rule_01000->setGeometry( QRect( 10, 50, 70, 20 ) );
	btn_Rule_01000->setPixmap( QPixmap::fromMimeSource( "data\\rule_01000.bmp" ) );
	btn_Rule_01000->setToggleButton(true);
	btn_Rule_01000->setPalette( pal );
	btn_Rule_01000->setFocusProxy(pg_Rules);

	btn_Rule_00100 = new QPushButton( pg_Rules, "btn_Rule_00100" );
	btn_Rule_00100->setGeometry( QRect( 10, 70, 70, 20 ) );
	btn_Rule_00100->setPixmap( QPixmap::fromMimeSource( "data\\rule_00100.bmp" ) );
	btn_Rule_00100->setToggleButton(true);
	btn_Rule_00100->setPalette( pal );
	btn_Rule_00100->setFocusProxy(pg_Rules);

	btn_Rule_00010 = new QPushButton( pg_Rules, "btn_Rule_00010" );
	btn_Rule_00010->setGeometry( QRect( 10, 90, 70, 20 ) );
	btn_Rule_00010->setPixmap( QPixmap::fromMimeSource( "data\\rule_00010.bmp" ) );
	btn_Rule_00010->setToggleButton(true);
	btn_Rule_00010->setPalette( pal );
	btn_Rule_00010->setFocusProxy(pg_Rules);

	btn_Rule_00001 = new QPushButton( pg_Rules, "btn_Rule_00001" );
	btn_Rule_00001->setGeometry( QRect( 10, 110, 70, 20 ) );
	btn_Rule_00001->setPixmap( QPixmap::fromMimeSource( "data\\rule_00001.bmp" ) );
	btn_Rule_00001->setToggleButton(true);
	btn_Rule_00001->setPalette( pal );
	btn_Rule_00001->setFocusProxy(pg_Rules);
	
	btn_Rule_11000 = new QPushButton( pg_Rules, "btn_Rule_11000" );
	btn_Rule_11000->setGeometry( QRect( 10, 130, 70, 20 ) );
	btn_Rule_11000->setPixmap( QPixmap::fromMimeSource( "data\\rule_11000.bmp" ) );
	btn_Rule_11000->setToggleButton(true);
	btn_Rule_11000->setPalette( pal );
	btn_Rule_11000->setFocusProxy(pg_Rules);
	
	btn_Rule_10100 = new QPushButton( pg_Rules, "btn_Rule_10100" );
	btn_Rule_10100->setGeometry( QRect( 10, 150, 70, 20 ) );
	btn_Rule_10100->setPixmap( QPixmap::fromMimeSource( "data\\rule_10100.bmp" ) );
	btn_Rule_10100->setToggleButton(true);
	btn_Rule_10100->setPalette( pal );
	btn_Rule_10100->setFocusProxy(pg_Rules);
	
	btn_Rule_10010 = new QPushButton( pg_Rules, "btn_Rule_10010" );
	btn_Rule_10010->setGeometry( QRect( 10, 170, 70, 20 ) );
	btn_Rule_10010->setPixmap( QPixmap::fromMimeSource( "data\\rule_10010.bmp" ) );
	btn_Rule_10010->setToggleButton(true);
	btn_Rule_10010->setPalette( pal );
	btn_Rule_10010->setFocusProxy(pg_Rules);
	
	btn_Rule_10001 = new QPushButton( pg_Rules, "btn_Rule_10001" );
	btn_Rule_10001->setGeometry( QRect( 10, 190, 70, 20 ) );
	btn_Rule_10001->setPixmap( QPixmap::fromMimeSource( "data\\rule_10001.bmp" ) );
	btn_Rule_10001->setToggleButton(true);
	btn_Rule_10001->setPalette( pal );
	btn_Rule_10001->setFocusProxy(pg_Rules);
	
	btn_Rule_01100 = new QPushButton( pg_Rules, "btn_Rule_01100" );
	btn_Rule_01100->setGeometry( QRect( 10, 210, 70, 20 ) );
	btn_Rule_01100->setPixmap( QPixmap::fromMimeSource( "data\\rule_01100.bmp" ) );
	btn_Rule_01100->setToggleButton(true);
	btn_Rule_01100->setPalette( pal );
	btn_Rule_01100->setFocusProxy(pg_Rules);
	
	btn_Rule_01010 = new QPushButton( pg_Rules, "btn_Rule_01010" );
	btn_Rule_01010->setGeometry( QRect( 10, 230, 70, 20 ) );
	btn_Rule_01010->setPixmap( QPixmap::fromMimeSource( "data\\rule_01010.bmp" ) );
	btn_Rule_01010->setToggleButton(true);
	btn_Rule_01010->setPalette( pal );
	btn_Rule_01010->setFocusProxy(pg_Rules);
	
	btn_Rule_01001 = new QPushButton( pg_Rules, "btn_Rule_01001" );
	btn_Rule_01001->setGeometry( QRect( 10, 250, 70, 20 ) );
	btn_Rule_01001->setPixmap( QPixmap::fromMimeSource( "data\\rule_01001.bmp" ) );
	btn_Rule_01001->setToggleButton(true);
	btn_Rule_01001->setPalette( pal );
	btn_Rule_01001->setFocusProxy(pg_Rules);
	
	btn_Rule_00110 = new QPushButton( pg_Rules, "btn_Rule_00110" );
	btn_Rule_00110->setGeometry( QRect( 10, 270, 70, 20 ) );
	btn_Rule_00110->setPixmap( QPixmap::fromMimeSource( "data\\rule_00110.bmp" ) );
	btn_Rule_00110->setToggleButton(true);
	btn_Rule_00110->setPalette( pal );
	btn_Rule_00110->setFocusProxy(pg_Rules);
	
	btn_Rule_00101 = new QPushButton( pg_Rules, "btn_Rule_00101" );
	btn_Rule_00101->setGeometry( QRect( 10, 290, 70, 20 ) );
	btn_Rule_00101->setPixmap( QPixmap::fromMimeSource( "data\\rule_00101.bmp" ) );
	btn_Rule_00101->setToggleButton(true);
	btn_Rule_00101->setPalette( pal );
	btn_Rule_00101->setFocusProxy(pg_Rules);
	
	btn_Rule_00011 = new QPushButton( pg_Rules, "btn_Rule_00011" );
	btn_Rule_00011->setGeometry( QRect( 10, 310, 70, 20 ) );
	btn_Rule_00011->setPixmap( QPixmap::fromMimeSource( "data\\rule_00011.bmp" ) );
	btn_Rule_00011->setToggleButton(true);
	btn_Rule_00011->setPalette( pal );
	btn_Rule_00011->setFocusProxy(pg_Rules);
	
	btn_Rule_11100 = new QPushButton( pg_Rules, "btn_Rule_11100" );
	btn_Rule_11100->setGeometry( QRect( 85, 9, 70, 20 ) );
	btn_Rule_11100->setPixmap( QPixmap::fromMimeSource( "data\\rule_11100.bmp" ) );
	btn_Rule_11100->setToggleButton(true);
	btn_Rule_11100->setPalette( pal );
	btn_Rule_11100->setFocusProxy(pg_Rules);
	
	btn_Rule_11010 = new QPushButton( pg_Rules, "btn_Rule_11010" );
	btn_Rule_11010->setGeometry( QRect( 85, 29, 70, 20 ) );
	btn_Rule_11010->setPixmap( QPixmap::fromMimeSource( "data\\rule_11010.bmp" ) );
	btn_Rule_11010->setToggleButton(true);
	btn_Rule_11010->setPalette( pal );
	btn_Rule_11010->setFocusProxy(pg_Rules);
	
	btn_Rule_11001 = new QPushButton( pg_Rules, "btn_Rule_11001" );
	btn_Rule_11001->setGeometry( QRect( 85, 50, 70, 20 ) );
	btn_Rule_11001->setPixmap( QPixmap::fromMimeSource( "data\\rule_11001.bmp" ) );
	btn_Rule_11001->setToggleButton(true);
	btn_Rule_11001->setPalette( pal );
	btn_Rule_11001->setFocusProxy(pg_Rules);
	
	btn_Rule_11110 = new QPushButton( pg_Rules, "btn_Rule_11110" );
	btn_Rule_11110->setGeometry( QRect( 85, 190, 70, 20 ) );
	btn_Rule_11110->setPixmap( QPixmap::fromMimeSource( "data\\rule_11001.bmp" ) );
	btn_Rule_11110->setToggleButton(true);
	btn_Rule_11110->setPalette( pal );
	btn_Rule_11110->setFocusProxy(pg_Rules);
	
	btn_Rule_01111 = new QPushButton( pg_Rules, "btn_Rule_01111" );
	btn_Rule_01111->setGeometry( QRect( 85, 270, 70, 20 ) );
	btn_Rule_01111->setPixmap( QPixmap::fromMimeSource( "data\\rule_01111.bmp" ) );
	btn_Rule_01111->setToggleButton(true);
	btn_Rule_01111->setPalette( pal );
	btn_Rule_01111->setFocusProxy(pg_Rules);
	
	btn_Rule_01110 = new QPushButton( pg_Rules, "btn_Rule_01110" );
	btn_Rule_01110->setGeometry( QRect( 85, 90, 70, 20 ) );
	btn_Rule_01110->setPixmap( QPixmap::fromMimeSource( "data\\rule_01110.bmp" ) );
	btn_Rule_01110->setToggleButton(true);
	btn_Rule_01110->setPalette( pal );
	btn_Rule_01110->setFocusProxy(pg_Rules);
	
	btn_Rule_10101 = new QPushButton( pg_Rules, "btn_Rule_10101" );
	btn_Rule_10101->setGeometry( QRect( 85, 170, 70, 20 ) );
	btn_Rule_10101->setPixmap( QPixmap::fromMimeSource( "data\\rule_10101.bmp" ) );
	btn_Rule_10101->setToggleButton(true);
	btn_Rule_10101->setPalette( pal );
	btn_Rule_10101->setFocusProxy(pg_Rules);
	
	btn_Rule_10111 = new QPushButton( pg_Rules, "btn_Rule_10111" );
	btn_Rule_10111->setGeometry( QRect( 85, 250, 70, 20 ) );
	btn_Rule_10111->setPixmap( QPixmap::fromMimeSource( "data\\rule_10111.bmp" ) );
	btn_Rule_10111->setToggleButton(true);
	btn_Rule_10111->setPalette( pal );
	btn_Rule_10111->setFocusProxy(pg_Rules);
	
	btn_Rule_10011 = new QPushButton( pg_Rules, "btn_Rule_10011" );
	btn_Rule_10011->setGeometry( QRect( 85, 70, 70, 20 ) );
	btn_Rule_10011->setPixmap( QPixmap::fromMimeSource( "data\\rule_10011.bmp" ) );
	btn_Rule_10011->setToggleButton(true);
	btn_Rule_10011->setPalette( pal );
	btn_Rule_10011->setFocusProxy(pg_Rules);
	
	btn_Rule_01101 = new QPushButton( pg_Rules, "btn_Rule_01101" );
	btn_Rule_01101->setGeometry( QRect( 85, 150, 70, 20 ) );
	btn_Rule_01101->setPixmap( QPixmap::fromMimeSource( "data\\rule_01101.bmp" ) );
	btn_Rule_01101->setToggleButton(true);
	btn_Rule_01101->setPalette( pal );
	btn_Rule_01101->setFocusProxy(pg_Rules);
	
	btn_Rule_11011 = new QPushButton( pg_Rules, "btn_Rule_11011" );
	btn_Rule_11011->setGeometry( QRect( 85, 230, 70, 20 ) );
	btn_Rule_11011->setPixmap( QPixmap::fromMimeSource( "data\\rule_11011.bmp" ) );
	btn_Rule_11011->setToggleButton(true);
	btn_Rule_11011->setPalette( pal );
	btn_Rule_11011->setFocusProxy(pg_Rules);
	
	btn_Rule_01011 = new QPushButton( pg_Rules, "btn_Rule_01011" );
	btn_Rule_01011->setGeometry( QRect( 85, 130, 70, 20 ) );
	btn_Rule_01011->setPixmap( QPixmap::fromMimeSource( "data\\rule_01011.bmp" ) );
	btn_Rule_01011->setToggleButton(true);
	btn_Rule_01011->setPalette( pal );
	btn_Rule_01011->setFocusProxy(pg_Rules);
	
	btn_Rule_11101 = new QPushButton( pg_Rules, "btn_Rule_11101" );
	btn_Rule_11101->setGeometry( QRect( 85, 210, 70, 20 ) );
	btn_Rule_11101->setPixmap( QPixmap::fromMimeSource( "data\\rule_11101.bmp" ) );
	btn_Rule_11101->setToggleButton(true);
	btn_Rule_11101->setPalette( pal );
	btn_Rule_11101->setFocusProxy(pg_Rules);
	
	btn_Rule_11111 = new QPushButton( pg_Rules, "btn_Rule_11111" );
	btn_Rule_11111->setGeometry( QRect( 85, 290, 70, 20 ) );
	btn_Rule_11111->setPixmap( QPixmap::fromMimeSource( "data\\rule_11111.bmp" ) );
	btn_Rule_11111->setToggleButton(true);
	btn_Rule_11111->setPalette( pal );
	btn_Rule_11111->setFocusProxy(pg_Rules);
	
	btn_Rule_00111 = new QPushButton( pg_Rules, "btn_Rule_00111" );
	btn_Rule_00111->setGeometry( QRect( 85, 110, 70, 20 ) );
	btn_Rule_00111->setPixmap( QPixmap::fromMimeSource( "data\\rule_00111.bmp" ) );
	btn_Rule_00111->setToggleButton(true);
	btn_Rule_00111->setPalette( pal );
	btn_Rule_00111->setFocusProxy(pg_Rules);
	
	btn_Rule_10110 = new QPushButton( pg_Rules, "btn_Rule_10110" );
	btn_Rule_10110->setGeometry( QRect( 85, 310, 70, 20 ) );
	btn_Rule_10110->setPixmap( QPixmap::fromMimeSource( "data\\rule_10110.bmp" ) );
	btn_Rule_10110->setToggleButton(true);
	btn_Rule_10110->setPalette( pal );
	btn_Rule_10110->setFocusProxy(pg_Rules);
	
	btn_GeneratePattern = new QPushButton( pg_Rules, "btn_GeneratePattern" );
	btn_GeneratePattern->setGeometry( QRect( 16, 340, 135, 35 ) );
	btn_GeneratePattern->setText("Generate Pattern");
	//btn_GeneratePattern->setToggleButton(true);
	btn_GeneratePattern->setPalette( pal );
	btn_GeneratePattern->setFocusProxy(pg_Rules);

	btn_ClearPattern = new QPushButton( pg_Rules, "btn_ClearPattern" );
	btn_ClearPattern->setGeometry( QRect( 16, 380, 135, 25 ) );
	btn_ClearPattern->setText("Clear Pattern");
	btn_ClearPattern->setPalette( pal );
	btn_ClearPattern->setFocusProxy(pg_Rules);

	btn_ResetRules = new QPushButton( pg_Rules, "btn_ResetRules" );
	btn_ResetRules->setGeometry( QRect( 16, 410, 135, 25 ) );
	btn_ResetRules->setText("Reset Rules");
	btn_ResetRules->setPalette( pal );
	btn_ResetRules->setFocusProxy(pg_Rules);

	//--------------------------------------------------------------------------------
	// UI - Beam Creation
	//--------------------------------------------------------------------------------
	btn_GenerateBeams = new QPushButton( pg_BeamCreation, "btn_GenerateBeams" );
	btn_GenerateBeams->setGeometry( QRect( 10, 10, 145, 25 ) );
	btn_GenerateBeams->setText("Generate Beams");
	btn_GenerateBeams->setPalette( pal );

	btn_ClearBeams = new QPushButton( pg_BeamCreation, "btn_ClearBeams" );
	btn_ClearBeams->setGeometry( QRect( 10, 42, 145, 25 ) );
	btn_ClearBeams->setText("Clear Beams");
	btn_ClearBeams->setPalette( pal );

	//--------------------------------------------------------------------------------
	// UI - OpenGL Window
	//--------------------------------------------------------------------------------
	// Create a nice frame to put around the OpenGL widget
	qgl_GLFrame = new QFrame( this, "qgl_GLFrame" );
	qgl_GLFrame->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	qgl_GLFrame->setLineWidth( 2 );
	qgl_GLFrame->setMinimumSize(840,550);
	
	// our OpenGL derived object/widget
	qgl_GLWindow = new BBB_GLWidget( qgl_GLFrame, "qgl_GLWindow" );
	qgl_GLWindow->setBackgroundColor(QColor(128,128,128));
	qgl_GLWindow->bindForm(this);

	//--------------------------------------------------------------------------------
	// Signals & Slots
	//--------------------------------------------------------------------------------
	QObject::connect( btn_ActivateBricks,		SIGNAL(clicked()),                   qgl_GLWindow, SLOT(activateBricks()) );
	QObject::connect( btn_ClearActiveBricks,	SIGNAL(clicked()),                   qgl_GLWindow, SLOT(clearActiveBricks()) );
	QObject::connect( btn_RandomActiveBricks, 	SIGNAL(clicked()),                   qgl_GLWindow, SLOT(randomActiveBricks()) );
	QObject::connect( btn_GeneratePattern,      SIGNAL(clicked()),                   qgl_GLWindow, SLOT(generatePattern()) );
	QObject::connect( btn_ClearPattern,		    SIGNAL(clicked()),                   qgl_GLWindow, SLOT(clearPattern()) );
	QObject::connect( btn_ResetRules,		 	SIGNAL(clicked()),                   qgl_GLWindow, SLOT(clearRules()) );
	QObject::connect( btn_GenerateBeams,	    SIGNAL(clicked()),                   qgl_GLWindow, SLOT(generateBeams()) );
	QObject::connect( btn_ClearBeams, 		    SIGNAL(clicked()),                   qgl_GLWindow, SLOT(clearBeams()) );
	QObject::connect( btn_BrickOffset, 		    SIGNAL(clicked()),                   qgl_GLWindow, SLOT(toggleOffset()) );

	QObject::connect( edit_NumBricks_Row, 		SIGNAL(textChanged(const QString&)), qgl_GLWindow, SLOT(rebuildBricks()) );
	QObject::connect( edit_BrickSize_Length, 	SIGNAL(textChanged(const QString&)), qgl_GLWindow, SLOT(rebuildBricks()) );
	QObject::connect( edit_BrickSize_Width, 	SIGNAL(textChanged(const QString&)), qgl_GLWindow, SLOT(rebuildBricks()) );
	QObject::connect( edit_Cantilevered_Height, SIGNAL(textChanged(const QString&)), qgl_GLWindow, SLOT(cantileverPattern()) );
	QObject::connect( edit_Padding_Rows, 		SIGNAL(textChanged(const QString&)), qgl_GLWindow, SLOT(rebuildBricks()) );

	//--------------------------------------------------------------------------------
	// Signals & Slots - Rules (used for "real-time" CA generation
	//--------------------------------------------------------------------------------
	//QObject::connect( btn_Rule_00000, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10000, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01000, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00100, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00010, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00001, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11000, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10100, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10010, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10001, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01100, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01010, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01001, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00110, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00101, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00011, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11100, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11010, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11001, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11110, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01111, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01110, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10101, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10111, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10011, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01101, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11011, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_01011, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11101, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_11111, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_00111, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	
	//QObject::connect( btn_Rule_10110, 			SIGNAL(clicked()), qgl_GLWindow, SLOT(generatePattern()) );	

	//--------------------------------------------------------------------------------
	// Menu
	//--------------------------------------------------------------------------------
	// Create a menu with "file" elements
	menu_File = new QPopupMenu( this );
	menu_File->insertItem( "Save Pattern",			qgl_GLWindow, SLOT(savePattern()), CTRL+Key_S );
	menu_File->insertItem( "Load Pattern",			qgl_GLWindow, SLOT(loadPattern()), CTRL+Key_L );
	menu_File->insertItem( "Clear Pattern",		qgl_GLWindow, SLOT(resetAll()),    CTRL+Key_N );
	menu_File->insertSeparator();
	menu_File->insertItem( "Import Path",			qgl_GLWindow, SLOT(importPath()),  CTRL+Key_I );
	menu_File->insertItem( "Export to OBJ",			qgl_GLWindow, SLOT(exportToObj()), CTRL+Key_X );
	menu_File->insertItem( "Export to RLE",			qgl_GLWindow, SLOT(exportToRLE()), CTRL+Key_R );
	menu_File->insertItem( "Export to Playlist",	qgl_GLWindow, SLOT(exportToMP3()), CTRL+Key_P );
	menu_File->insertSeparator();
	menu_File->insertItem( "Exit",					qApp, SLOT(quit()),		   CTRL+Key_Q );
	
	// Create a menu with "utility" elements
	menu_Utility = new QPopupMenu( this );
	menu_Utility->insertItem( "Invert Active",      qgl_GLWindow, SLOT(invertActive()),   ALT+Key_I);
	menu_Utility->insertItem( "Hide/Show Inactive",	qgl_GLWindow, SLOT(showInactive()),	  ALT+Key_H);
	menu_Utility->insertItem( "Take Screenshot",    qgl_GLWindow, SLOT(takeScreenshot()), ALT+Key_S);
	menu_Utility->insertItem( "Zoom Extensts",      qgl_GLWindow, SLOT(zoomExtents()),    ALT+Key_Z);
	menu_Utility->insertItem( "Draw Borders",       qgl_GLWindow, SLOT(toggleBorders()),  ALT+Key_B);
	menu_Utility->insertItem( "Draw Grid",          qgl_GLWindow, SLOT(toggleGrid()),     ALT+Key_G);
		
	// Create a menu with "help" elements
	//menu_Help = new QPopupMenu( this );
	//menu_Help->insertItem( "Online Help",	qApp, SLOT(quit()), 0);
	//menu_Help->insertItem( "About",		qApp, SLOT(quit()), 0);
	
	// create a menu bar to store the popup menu
	menu_MainBar = new QMenuBar( this );
	menu_MainBar->setSeparator( QMenuBar::InWindowsStyle );
	menu_MainBar->insertItem("&File",	 menu_File );
	menu_MainBar->insertItem("&Utility", menu_Utility );
	//menu_MainBar->insertItem("&Help",	  menu_Help );

	//--------------------------------------------------------------------------------
	// Layouts
	//--------------------------------------------------------------------------------
	QVBoxLayout* vlayout = new QVBoxLayout(1,"vlayout");
	vlayout->add(tb_MainUI);
	vlayout->setAlignment(Qt::AlignTop);
  
	QHBoxLayout* flayout = new QHBoxLayout( qgl_GLFrame, 2, 2, "flayout");
    flayout->addWidget( qgl_GLWindow, 1 );

	QHBoxLayout* hlayout = new QHBoxLayout( this, 5, 5, "hlayout");
	hlayout->setMenuBar( menu_MainBar );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( qgl_GLFrame, 1 );
}

//----------------------------------------------------------------------------------
void BBB_Form::keyPressEvent(QKeyEvent* e)
{
	if(e->key()==Qt::Key_G)
	{ qgl_GLWindow->generatePattern(); }
}

/*
bool BBB_Form::event(QEvent* e) 
{
	return false;
}
*/