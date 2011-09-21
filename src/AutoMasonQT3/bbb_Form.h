#ifndef BBB_FORM_H
#define BBB_FORM_H
//------------------------------------------------------------------------
// system includes
//------------------------------------------------------------------------
#include <qapplication.h>
#include <qvariant.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbox.h>
#include <qcheckbox.h>

//------------------------------------------------------------------------
// local includes
//------------------------------------------------------------------------
#include "bbb_GLWidget.h"

//------------------------------------------------------------------------
// Main UI Form for the Application - Holds all UI elements
//------------------------------------------------------------------------
class BBB_Form : public QWidget
{
	Q_OBJECT

public:
	//-------------------------------------------------------
	BBB_Form( QWidget *parent=0, const char *name=0 );

protected:
	//-------------------------------------------------------
	// catches any key press events so that we can use the 
	// 'G' key to quickly regenerate our CA rules
	//-------------------------------------------------------
	void  keyPressEvent(QKeyEvent* e);
	// bool event(QEvent* e);

public:
	//------------------------------------------------------------
	// Misc Widgets
	//------------------------------------------------------------
	QToolBox*				tb_MainUI;
	QWidget*				pg_BrickCreation;
	QWidget*				pg_StartCondition;
	QWidget*				pg_Rules;
	QWidget*				pg_BeamCreation;

	//------------------------------------------------------------
	// Brick Creation Widgets
	//------------------------------------------------------------
    QGroupBox*			grp_BrickCreation;
    QLineEdit*			edit_NumBricks_Row;
    QLineEdit*			edit_BrickSize_Length;
    QLineEdit*			edit_BrickSize_Width;
    QLineEdit*			edit_Cantilevered_Height;  // CHIPP
    QLineEdit*			edit_Padding_Rows;  // CHIPP
	QLabel*	            lbl_BrickSize_Length;
    QLabel*	            lbl_BrickSize_Width;
    QLabel*	            lbl_NumBricks_Rows;
    QLabel*             lbl_NumBricks;
	QLabel*	            lbl_Cantilevered_Height; // CHIPP
	QLabel*	            lbl_Padding_Rows; // CHIPP
    QLabel*             lbl_BrickSize;
	QPushButton*		btn_BrickOffset; 

	//------------------------------------------------------------
	// Start Condition Widgets
	//------------------------------------------------------------
    QGroupBox*			grp_StartConditions;
    QPushButton*		btn_ActivateBricks;
    QPushButton*		btn_ClearActiveBricks;
	QPushButton*		btn_RandomActiveBricks;

	//------------------------------------------------------------
	// Rules and Pattern Generation Widgets
	//------------------------------------------------------------
	QButtonGroup*		grp_Rules;
	QPushButton*		btn_Rule_00000;
	QPushButton*		btn_Rule_10000;
	QPushButton*		btn_Rule_01000;
	QPushButton*		btn_Rule_00100;
	QPushButton*		btn_Rule_00010;
	QPushButton*		btn_Rule_00001;
	QPushButton*		btn_Rule_11000;
	QPushButton*		btn_Rule_10100;
	QPushButton*		btn_Rule_10010;
	QPushButton*		btn_Rule_10001;
	QPushButton*		btn_Rule_01100;
	QPushButton*		btn_Rule_01010;
	QPushButton*		btn_Rule_01001;
	QPushButton*		btn_Rule_00110;
	QPushButton*		btn_Rule_00101;
	QPushButton*		btn_Rule_00011;
	QPushButton*		btn_Rule_11100;
	QPushButton*		btn_Rule_11010;
	QPushButton*		btn_Rule_11001;
	QPushButton*		btn_Rule_11110;
	QPushButton*		btn_Rule_01111;
	QPushButton*		btn_Rule_01110;
	QPushButton*		btn_Rule_10101;
	QPushButton*		btn_Rule_10111;
	QPushButton*		btn_Rule_10011;
	QPushButton*		btn_Rule_01101;
	QPushButton*		btn_Rule_11011;
	QPushButton*		btn_Rule_01011;
	QPushButton*		btn_Rule_11101;
	QPushButton*		btn_Rule_11111;
	QPushButton*		btn_Rule_00111;
	QPushButton*		btn_Rule_10110;
	QPushButton*		btn_GeneratePattern;
	QPushButton*		btn_ClearPattern;
	QPushButton*		btn_ResetRules;

	//------------------------------------------------------------
	// Beam Creation Widgets
	//------------------------------------------------------------
    QPushButton*		btn_GenerateBeams;
	QPushButton*		btn_ClearBeams;

	//------------------------------------------------------------
	// Menu Widgets
	//------------------------------------------------------------
	QMenuBar*           menu_MainBar;
	QPopupMenu*         menu_File;
	QPopupMenu*         menu_Utility;
	QPopupMenu*         menu_Help;

	//------------------------------------------------------------
	// OpenGL Widgets
	//------------------------------------------------------------
	BBB_GLWidget*		qgl_GLWindow;
	QFrame*				qgl_GLFrame;
}; 

#endif