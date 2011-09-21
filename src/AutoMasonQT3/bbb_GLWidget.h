#ifndef BBB_GLWIDGET_H
#define BBB_GLWIDGET_H
//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------
#include <qgl.h>
#include <vector>
#include "Math/bbb_Math.h"

using namespace std;

//------------------------------------------------------------------------
// BrickState Enum
//----------------------------
// INACTIVE - hollow brick
// ACTIVE - solid brick
// NOTE: order matters!
enum BrickState {INACTIVE = 0, ACTIVE};
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Brick Class
//----------------------------
// class to hold data for a single brick
//------------------------------------------------------------------------
class Brick
{
	public:
		gmtl::Point3f corners[8];
		gmtl::Vec3f	  normals[6];
		//bool          state;
		BrickState    state;
		bool cantilevered;
		bool corner;
	
	public:
		bool isCantilevered() { return cantilevered; }
		void setCantilever() { cantilevered = true; }
		void clearCantilever() { cantilevered = false; }
		bool isCorner() { return corner; }
		void setCorner() { corner = true; }
		void clearCorner() { corner = false; }
		bool isSolid() { return (state == ACTIVE); }
		bool isHollow() { return (state == INACTIVE); }
};

//------------------------------------------------------------------------
struct WallSegment
{	std::vector<gmtl::Point3f> knots; };

//------------------------------------------------------------------------
// WallPath Class
//----------------------------
// holds data for our wall path.
//------------------------------------------------------------------------
struct WallPath
{ 
	std::vector<WallSegment> segments;
	char                     filename[256];
};

//------------------------------------------------------------------------
// GLWidget Class
//----------------------------
// This class derives from the Qt OpenGL widget to provide our own draw
// routines and resizing options.
//------------------------------------------------------------------------
class BBB_Form;
class BBB_GLWidget : public QGLWidget
{
   Q_OBJECT

public:
	//-------------------------------------------------------
  BBB_GLWidget( QWidget* parent, const char* name );
  ~BBB_GLWidget();

	//-------------------------------------------------------
	// binds our main UI form to a local variable so we can
	// access all the widgets from within our functions.
	//-------------------------------------------------------
	void bindForm(BBB_Form* ref_form);
			
public slots:
	//-------------------------------------------------------
	// rebuild all bricks
	//-------------------------------------------------------
	void rebuildBricks(void);

	//-------------------------------------------------------
	// resize our wall/block, grabbing info from the UI
	// - this is called whenever the rows and columns change
	//-------------------------------------------------------
	void  resizeWall(void);

	//-------------------------------------------------------
	// changes the number of columns being used.
	// - this is called when using a "block" formation and
	//   the user changes the width/depth settings
	//-------------------------------------------------------
	void  resizeColumns(void);

	//-------------------------------------------------------
	// changes the number of rows which are "padded" with the initial condition
	//-------------------------------------------------------
	void resizePadding(void);

	//-------------------------------------------------------
	// toggles whether or not we should draw the bricks in
	// a block pattern
	//-------------------------------------------------------
	//void  blockToggle(void);

	//-------------------------------------------------------
	// cantilever the current pattern
	//-------------------------------------------------------
	void cantileverPattern();

	//-------------------------------------------------------
	// cantilevers a currnet column in the current pattern
	//-------------------------------------------------------
	void cantileverPattern(int col);

	//-------------------------------------------------------
	// sets the program into "set active brick" mode so the
	// user can manually pick bricks to make active/inactive
	//-------------------------------------------------------
	void activateBricks(void);

	//-------------------------------------------------------
	// clears the active bricks (start condition).
	//-------------------------------------------------------
	void clearActiveBricks(void);

	//-------------------------------------------------------
	// utility function for getting a quick start condition
	//-------------------------------------------------------
	void  randomActiveBricks(void);
	
	//-------------------------------------------------------
	// generates the CA pattern.
	//-------------------------------------------------------
	void generatePattern(void);

	//-------------------------------------------------------
	// clears all bricks except the start condition so the
	// user can try out a different rule set with the same
	// start conditions
	//-------------------------------------------------------
	void clearPattern(void);

	//-------------------------------------------------------
	// resets all the rules buttons
	//-------------------------------------------------------
	void clearRules(void);
	
	//-------------------------------------------------------
	// generates the beams for the current start condition
	//-------------------------------------------------------
	void generateBeams(void);

	//-------------------------------------------------------
	// clears any created beams
	//-------------------------------------------------------
	void clearBeams(void);

	//-------------------------------------------------------
	// clears any columns by index
	//-------------------------------------------------------
	void clearColumn(int index);

	//-------------------------------------------------------
	// couple UI toggle routines to turn brick borders on/off
	// and whether or not we should show the inactive bricks
	//-------------------------------------------------------
	void  toggleGrid(void)	  { m_drawGrid = !m_drawGrid; }
	void  toggleBorders(void) { m_drawBorder = !m_drawBorder; }
	void  showInactive(void)  { m_showInactive = !m_showInactive; }
	
	//-------------------------------------------------------
	// toggles whether we use uniform stacking or offset
	//-------------------------------------------------------
	void toggleOffset(void);

	//-------------------------------------------------------
	// inverts the state of all bricks including the start 
	// condition.
	//-------------------------------------------------------
	void  invertActive(void);	

	//-------------------------------------------------------
	// zooms the window out so that the whole wall/block is
	// visible.
	//-------------------------------------------------------
	void  zoomExtents(void);

	//-------------------------------------------------------
	// resets the entire scene and UI to the defaults
	//-------------------------------------------------------
	void  resetAll(void);

	//-------------------------------------------------------
	// takes a screenshot of the OpenGL viewport and saves
	// it to a file
	//-------------------------------------------------------
	void  takeScreenshot(void);

	//-------------------------------------------------------
	// load spline from filename
	//-------------------------------------------------------
	bool importPath(char* filename=0);

	//-------------------------------------------------------
	bool savePattern(void);
	bool loadPattern(void);

	//-------------------------------------------------------
	bool exportToObj(void);
	bool exportToRLE(void);
	bool exportToMP3(void);

protected:
	//-------------------------------------------------------
	// this is where we initialize all of out OpenGL states
	//-------------------------------------------------------
    void initializeGL();

	//-------------------------------------------------------
	// our basic draw routine for OpenGL
	//-------------------------------------------------------
    void paintGL();

	//-------------------------------------------------------
	// this gets called each time the window is moved or 
	// resized. it gives us an opportunity to adjust our
	// viewports and settings for the new size
	//-------------------------------------------------------
    void resizeGL( int w, int h );

	//-------------------------------------------------------
	// we wrote our own event handler so that we can easily 
	// grab key, mouse, and draw events from the system
	//-------------------------------------------------------
    bool event(QEvent* e);

private:
	//-------------------------------------------------------
	// draw a grid under our building (currently not used)
	//-------------------------------------------------------
	void drawGrid(float size, float step);

	//-------------------------------------------------------
	void drawBricks(void);
	
	//-------------------------------------------------------
	void drawBorders(void);

	//-------------------------------------------------------
	// clears the states of all bricks. the arguments specify
	// where the clearing should start. so, if "1" is passed
	// in for the row argument, then the bottom row of bricks
	// will be untouched, but everything else is cleared
	//-------------------------------------------------------
	void clearBricks(int row=0);

	// Manipulate Bricks
	void toggleInitialCondition(int index);
	void setInitialCondition(int index, BrickState bs);
	void setCornerColumn(int index);
	void clearCornerColumn(int index);

	//-------------------------------------------------------
	// two routines which are the meat of the program. these
	// evaluate the CA ruleset with the given start condition
	//-------------------------------------------------------
	void evaluateNeighborhood(int index,int n1,int n2,int n3,int n4,int n5);
	void evaluateRule(int index);

	//-------------------------------------------------------
	// check and create the cantilever depending on the 
	// cantilever height criteria set in the brick creation
	// dialog
	//-------------------------------------------------------
	bool cantileverCheck(int index);
	void setCantileverColumn(int index);
	void clearCantileverColumn(int index);

	// sub-routine used by exportTo_ function
	void printBrickCount(FILE * fOut, unsigned int s, unsigned int h, unsigned int c, unsigned int r);
	void speakBrickCount(QString dirpath, FILE * fOut, 
						 unsigned int s, unsigned int h, unsigned int c, unsigned int r, 
					     unsigned int & key, unsigned int rowCount, vector<vector<int>> & playlists);
	
	// Writes the wav file
	bool text2wav(QString filepath, QString txt);

private:
	BBB_Form* m_form;
	QWidget* p;

	// navigation variables
	float				m_zoom;
    float				m_xpos,m_ypos;
    float				m_xrot,m_yrot;
	int					m_lastMouseX, m_lastMouseY;	

	// various flags and program states
	bool				m_mouseDown;
	bool				m_pickingBricks;
	bool				m_hasStartCondition;
	bool				m_generatingPattern;
	bool				m_drawBorder;
	bool				m_drawGrid;
	bool				m_showInactive;
	bool				m_uniformStack;

	// min/max points for building
	gmtl::Point3f       m_minimum;
	gmtl::Point3f		m_maximum;

	// our brick objects
	WallPath*           m_path;
	std::vector<Brick>  m_newBricks;

	// storage of our brick's state & brick flags
	int                 m_numBricksPerRow;
	int					m_selectedBrick;
	int					m_cornerWidth; // CHIPP
	int					m_paddingRows; // CHIPP
	int					m_cantileveredHeight;  // CHIPP
	int					m_selectedStartBrick;  // CHIPP
	int					m_selectedEndBrick;  // CHIPP
	std::vector<bool>	m_cantileverableBricks;   // CHIPP
	int					m_screenshotIndex;
	
	// export settings
	QString				m_creator; // CHIPP
	QString				m_projectname; // CHIPP
};

#endif
