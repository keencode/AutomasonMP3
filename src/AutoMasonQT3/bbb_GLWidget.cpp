//----------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qinputdialog.h>
#include <qtimer.h>

// For TTS SAPI
#include <atlbase.h>

extern CComModule _Module;
#include <atlcom.h>

#include <sapi.h> 
#include <sphelper.h>

using namespace std;

//----------------------------------------------------------------------------------
#include "bbb_GLWidget.h"
#include "bbb_Form.h"
#include "bbb_Lively.h"

//----------------------------------------------------------------------------------
// reads a single line of text from the passed file
//----------------------------------------------------------------------------------
void ReadLine(FILE* f,char* string) 
{	
	do { fgets(string, 255, f); } 
	while ((string[0] == ';') || (string[0] == '\n'));
	return;	
}

//----------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------
BBB_GLWidget::BBB_GLWidget( QWidget* parent, const char* name ) : QGLWidget( parent, name )
{
	p = parent;

	// initialize tranformation variabls
	m_xpos			 = 0.0f;
	m_ypos			 = 20.0f;
	m_zoom			 = 300.0f;
	m_xrot			 = 20.0f;
	m_yrot			 = 45.0f;
	m_lastMouseX = 0;
	m_lastMouseY = 0;

	// brick init
	m_numBricksPerRow		= 0;
	m_selectedBrick			= 0;
	m_selectedStartBrick	= 0; // start RLE from the first Brick
	m_selectedEndBrick		= -1; // end RLE from the first Brick (gets set to the last brick in the row)
	m_cornerWidth			= 1; // Corners are corner brick plus 1 neighboring brick on each side
	m_paddingRows			= 1; // 1 row of initial conditions

	// Cantilever Set-Up
	m_cantileveredHeight	= 12; // CHIPP - initial height of cantilevered column

	// very important to set pointers to o (null) 
	// before use to avoid memory allocation problems
	m_form		= 0;
	m_path		= 0;

	// internal flags
	m_mouseDown		= false;
	m_pickingBricks = false;
	m_drawBorder    = true;
	m_showInactive  = true;
	m_uniformStack	= true;

	m_minimum.set(1000000.f,1000000.f,1000000.f);
	m_maximum.set(-1000000.f,-1000000.f,-1000000.f);

	// Export Settings
	m_creator = "Your Architect";
	m_projectname = "Your Project";
}

//----------------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------------
BBB_GLWidget::~BBB_GLWidget()
{
	// make our context current
	makeCurrent();

	// cleanup our array of bricks if we have them
	m_newBricks.clear();
	m_cantileverableBricks.clear();
}

//----------------------------------------------------------------------------------
// Bind the UI to this Class for Reference
//----------------------------------------------------------------------------------
void BBB_GLWidget::bindForm(BBB_Form* ref_form)
{ 
	m_form = ref_form; 
	
	// build initial wall of bricks
	rebuildBricks();
}

//==================================================================================
// OpenGL Routines
//==================================================================================

//----------------------------------------------------------------------------------
// Initialize OpenGL Specific States
//----------------------------------------------------------------------------------
void BBB_GLWidget::initializeGL()
{
	// set our initial OpenGL states
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);				// Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// create our light
	static float pos[4] = { 0, 0, 100 };
	glLightfv( GL_LIGHT0, GL_POSITION, pos );
	glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

	// tell Qt to automatically swap our frame buffers for us
	this->setAutoBufferSwap(true);
}

//----------------------------------------------------------------------------------
// Paint/Draw our OpenGL Scene
//----------------------------------------------------------------------------------
void BBB_GLWidget::paintGL() 
{
	// clear our buffer & load our identity matrix
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

	// Position the camera
	glTranslatef( m_xpos, -m_ypos, -m_zoom );

	// Rotate the camera
	glRotatef( m_xrot, 1.0f, 0.0f, 0.0f );
	glRotatef( m_yrot, 0.0f, 1.0f, 0.0f );

	// Draw our origin
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);	glVertex3f(0.0f, 0.0f, 0.0f);	glVertex3f(10.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);	glVertex3f(0.0f, 0.0f, 0.0f);	glVertex3f(0.0f, 10.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);	glVertex3f(0.0f, 0.0f, 0.0f);	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	// if we have a form assigned (safety), draw our scene
	if(m_form) 
	{ 
		// pus GL matrix so we can return to this state later
		glPushMatrix();

		// draw our grid
		if(m_drawGrid)
		{
			if(m_path)
			{
				float dist = Max(m_maximum[0]-m_minimum[0],m_maximum[1]-m_minimum[1]);
				dist *= 1.2f;
				drawGrid(dist,dist/10.f);
			}
			else { drawGrid(100,10); }
		}

		// figure out which way we're going to draw the bricks
		// based on the UI elements being used. if you wanted to
		// create another formation, add it here.
		drawBricks();

		// draw our borders
		if(m_drawBorder) { drawBorders(); }

		// restore our translation matrix
		glPopMatrix();	
	}

	// force an update of the window (real-time playback)
	update();
}


//----------------------------------------------------------------------------------
// Resize the OpenGL Window
//----------------------------------------------------------------------------------
void BBB_GLWidget::resizeGL(int w,int h )
{
	// if our height is zero, set to one to prevent divide by zero
	if (h==0) {	h=1; }

	// set our viewpor
	glViewport(0,0,w,h);

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(float)w/(float)h,10.0f,10000.0f);

	// switch back to modelview after setting our projection
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//==================================================================================
// Drawing Routines
//==================================================================================

//----------------------------------------------------------------------------------
// Draw our Bricks Along a Predefined Path
//----------------------------------------------------------------------------------
void BBB_GLWidget::drawBricks(void)
{
	float r, g, b;

	// make sure we have a path
	if(!m_path) { return; }

	// disable lighting
	glDisable(GL_LIGHTING);

	// cycle through bricks
	for(unsigned int i=0; i<m_newBricks.size(); ++i)
	{
		Brick* brick = &m_newBricks[i];



		// make sure we want to draw inactive bricks and 
		// set our colors
//		if(brick->state) 
//		{	if(!m_showInactive) { continue; }	glColor3f(1,1,1); } 
//		else { glColor3f(0,0,0); }

		/*
		if((i == m_selectedStartBrick)&&(i == m_selectedBrick)&&(m_pickingBricks)) // shade the "selected" AND "start"
		{
			if(brick->state == INACTIVE) //  
			{ glColor3f(0.0f,0.7f,0.3f); } // light-blue		
			//else if(brick->state == CANTILEVERED)
			else if(brick->isCantilevered())
			{ glColor3f(0.2f,0.5f,0.2f); } // dark-green
			else // ACTIVE
			{ glColor3f(0.0f,0.5f,0.2f); } // dark-green
		}
		else if((i == m_selectedStartBrick)&&(m_pickingBricks)) // shade the "start" Brick Green
		{
			if(brick->state == INACTIVE) //  
			{ glColor3f(0.0f,1.0f,0.0f); } // light-green
			//else if(brick->state == CANTILEVERED)
			else if(brick->isCantilevered())
			{ glColor3f(0.2f,0.5f,0.0f); } // dark-green
			else 
			{ glColor3f(0.0f,0.5f,0.0f); } // dark-green
		}		
		else if((i == m_selectedBrick)&&(m_pickingBricks)) // shade the "selected" Brick Blue 
		{
			if(brick->state == INACTIVE) //  
			{	glColor3f(0.5f,0.5f,1.0f); } // blue
			// else if(brick->state == CANTILEVERED)
			else if(brick->isCantilevered())
			{ glColor3f(1.0f,0,0); } // draw CANTILEVERED bricks red
			else 
			{ glColor3f(0.1f,0.1f,0.5f); } // dark-blue				
		}
		else // not "picking" bricks
		{
			if(brick->state == INACTIVE) //  
			{	
				if(!m_showInactive) { continue; } // skips drawing any INACTIVE bricks 
												  // if m_showInactive is flipped
				glColor3f(1,1,1); // draw INACTIVE bricks white
			} 
			// else if(brick->state == CANTILEVERED)
			else if(brick->isCantilevered())
			{ glColor3f(1.0f,0,0); } // draw CANTILEVERED bricks red
			else 
			{ glColor3f(0,0,0); } // draw ACTIVE bricks black
		}
*/
		if(brick->isSolid()) // Solid
		{
			r = 0.0;
			g = 0.0;
			b = 0.0;
		}
		else if(brick->isHollow()) // Hollow
		{
			r = 1.0;
			g = 1.0;
			b = 1.0;
		}
		
		if(brick->isCantilevered()) // Cantilevered
		{
			if(r < 0.75){ r = r + 0.75; }
			if(g > 0.8) { g = g - 0.8; }
			if(b > 0.8) { b = b - 0.8; }
		}

		if(brick->isCorner()) // Corner
		{
			if(b < 0.75){ b = b + 0.75; }
			if(r > 0.8) { r = r - 0.8; }
			if(g > 0.8) { g = g - 0.8; }
		}

		if(m_pickingBricks) // Picking brick
		{
			if(i == m_selectedStartBrick) // Paint the start of the pattern brick 'Green'
			{
				if(g < 0.75){ g = g + 0.75; }
				if(r > 0.8) { r = r - 0.8; }
				if(b > 0.8) { b = b - 0.8; }
			}

			if(i == m_selectedBrick) // selected brick
			{
				if(r > 0.2) { r = r - 0.2; }
				if(g > 0.2) { g = g - 0.2; }
				if(b > 0.2) { b = b - 0.2; }
			}
		}
		glColor3f(r,g,b); // set the colors

		// draw solid brick
		glBegin(GL_QUADS);
			glNormal3fv(brick->normals[0].mData);
			glVertex3fv(brick->corners[3].mData);   glVertex3fv(brick->corners[2].mData);
			glVertex3fv(brick->corners[1].mData);   glVertex3fv(brick->corners[0].mData);

			glNormal3fv(brick->normals[1].mData);
			glVertex3fv(brick->corners[5].mData);	glVertex3fv(brick->corners[6].mData);
			glVertex3fv(brick->corners[7].mData);   glVertex3fv(brick->corners[4].mData);
			
			glNormal3fv(brick->normals[2].mData);
			glVertex3fv(brick->corners[1].mData);	glVertex3fv(brick->corners[2].mData);
			glVertex3fv(brick->corners[6].mData);	glVertex3fv(brick->corners[5].mData);

			glNormal3fv(brick->normals[3].mData);
			glVertex3fv(brick->corners[4].mData);	glVertex3fv(brick->corners[7].mData);
			glVertex3fv(brick->corners[3].mData);	glVertex3fv(brick->corners[0].mData);

			glNormal3fv(brick->normals[5].mData);
			glVertex3fv(brick->corners[4].mData);	glVertex3fv(brick->corners[0].mData);
			glVertex3fv(brick->corners[1].mData);	glVertex3fv(brick->corners[5].mData);

			glNormal3fv(brick->normals[4].mData);
			glVertex3fv(brick->corners[3].mData);	glVertex3fv(brick->corners[7].mData);
			glVertex3fv(brick->corners[6].mData);	glVertex3fv(brick->corners[2].mData);
		glEnd();
	}
}

//----------------------------------------------------------------------------------
void BBB_GLWidget::drawBorders(void)
{
	// braw border
	glDisable(GL_LIGHTING);
	//glColor3f(.4,0,0);
	glLineWidth(2.f);
	for(unsigned int i=0; i<m_newBricks.size(); ++i)
	{
		Brick* brick = &m_newBricks[i];

		// if we're the active brick and selecting, set different color
		// ONLY on the first row
		// if((m_pickingBricks) && (i < m_numBricksPerRow))
		
		// Only draw the Borders if we are actively picking bricks
		// AND within the number of padding bricks and rows
		if((m_pickingBricks) && (i < (m_numBricksPerRow * m_paddingRows)))
		{	
			// the shading of the "selected" bricks is done in the drawing
			// if(i == m_selectedBrick)// color the "selected" Brick Blue 
			// { glLineWidth(4.f); glColor3f(0.f,0.f,1.f); }

			// indicated starting "selected" brick with a green outline is not clear enough
			// moved to Brick Shading
			// if((i == m_selectedStartBrick) && (m_cantileverableBricks[i % m_numBricksPerRow] == true)) // Start and Cantileverable
			// { glLineWidth(4.f); glColor3f(0.5f,0.5f,0.0f); } // red and green
			// else if(i == m_selectedStartBrick)
			//{ glLineWidth(4.f); glColor3f(0.0f,1.0f,0.0f); } // green
			// else 
/*
			if(m_cantileverableBricks[i % m_numBricksPerRow] == true)
			{ 
				glLineWidth(4.f); 
				//if(brick->state == CANTILEVERED)
				if(brick->isCantilevered())
				{ glColor3f(0.8f,0.0f,0.0f); } // darker red		
				else
				{ glColor3f(1.0f,0.0f,0.0f); } // red
			}
			else
			{ glLineWidth(2.f); glColor3f(.4,0,0); }
*/
			// Render the bricks within the padding rows 
			glLineWidth(4.f);  // thick
			glColor3f(1.0f,0.0f,0.0f);  // red
		}
		else if(i == m_selectedEndBrick) // Paint the end of the pattern brick 'Red'
		{
			glLineWidth(4.f);  // thick
			glColor3f(0.0f,0.0f,1.0f);  // blue
		}
		else // the non-picking Bricks Border color
		{ 
			glLineWidth(2.f); // thin
			glColor3f(.4,0,0); // lighter-red
		}

		// draw shape
		glBegin(GL_LINES);
			glVertex3fv(brick->corners[0].mData); glVertex3fv(brick->corners[1].mData);
			glVertex3fv(brick->corners[1].mData); glVertex3fv(brick->corners[2].mData);
			glVertex3fv(brick->corners[2].mData); glVertex3fv(brick->corners[3].mData);
			glVertex3fv(brick->corners[3].mData); glVertex3fv(brick->corners[0].mData);

			glVertex3fv(brick->corners[4].mData); glVertex3fv(brick->corners[5].mData);
			glVertex3fv(brick->corners[5].mData); glVertex3fv(brick->corners[6].mData);
			glVertex3fv(brick->corners[6].mData); glVertex3fv(brick->corners[7].mData);
			glVertex3fv(brick->corners[7].mData); glVertex3fv(brick->corners[4].mData);

			glVertex3fv(brick->corners[0].mData); glVertex3fv(brick->corners[4].mData);
			glVertex3fv(brick->corners[1].mData); glVertex3fv(brick->corners[5].mData);
			glVertex3fv(brick->corners[2].mData); glVertex3fv(brick->corners[6].mData);
			glVertex3fv(brick->corners[3].mData); glVertex3fv(brick->corners[7].mData);
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

//----------------------------------------------------------------------------------
// Draw a Gound Plane or Grid
//----------------------------------------------------------------------------------
void BBB_GLWidget::drawGrid(float size, float step)
{
	float f;

	// set our grid color
	glDisable(GL_LIGHTING);
	glColor3f(0.4f, 0.4f, 0.4f);

	// draw our lines
	glBegin(GL_LINES);
		for(f = -size; f <= size; f+= step )
		{
			glVertex3f(f, 0.0f, -size);
			glVertex3f(f, 0.0f, size);
		}
		for(f = -size; f <= size; f+= step )
		{
			glVertex3f(size, 0.0f, f);
			glVertex3f(-size, 0.0f, f);
		}
	glEnd();

	glEnable(GL_LIGHTING);
}

//==================================================================================
// Helper Routines
//==================================================================================
void BBB_GLWidget::clearBricks(int row)
{
	if(m_newBricks.size()==0) { return; }

	// grab our row count
	const int num_rows = m_form->edit_NumBricks_Row->text().toInt();
	
	for(int r=row; r<num_rows; ++r)
	{
		for(int i=0; i<m_numBricksPerRow; ++i)
//		{ m_newBricks[(r*m_numBricksPerRow)+i].state = false; }
		{ m_newBricks[(r*m_numBricksPerRow)+i].state = INACTIVE; }
	}
}

void BBB_GLWidget::clearColumn(int index)
{
	if(m_newBricks.size()==0) { return; }

	// grab our row count
	const int num_rows = m_form->edit_NumBricks_Row->text().toInt();
	
	for(int i = (index % m_numBricksPerRow); i < m_newBricks.size(); i = i + m_numBricksPerRow)
	{ 
		// Retain the state
		// m_newBricks[i].state = INACTIVE; 
		m_newBricks[i].clearCantilever();
		m_newBricks[i].clearCorner(); 
	}
}


//----------------------------------------------------------------------------------
// Load spline
//----------------------------------------------------------------------------------
bool BBB_GLWidget::importPath(char* filename)
{
	if(!filename)
	{
		// create the filename to open
		QString fname = QFileDialog::getOpenFileName("./","SPL Images (*.spl)", this,"open file dialog","Choose a path file to open");
		if(fname.isEmpty()){ return false; }
		filename = new char[256];
		strcpy(filename,fname);
	}

	// open file
	FILE* fIn = fopen(filename,"rt");
	if(!fIn) 
	{ QMessageBox::information(0,"Error","couldn't open path file!"); return false; }

	// cleanup a little
	if(m_path) { delete m_path; m_path=0; }
	m_newBricks.clear();

	// read number of splines
	char oneline[256];
	ReadLine(fIn,oneline);
	unsigned int num_splines=0; 
	sscanf(oneline,"num_splines = %d ",&num_splines);

	// if knots is > 0 create new path
	if(num_splines > 0) { m_path = new WallPath();   }
	else { fclose(fIn); return false; }

	// copy our filename into the path
	strcpy(m_path->filename,filename);

	// cycle through splines
	for(unsigned int j=0; j<num_splines; ++j)
	{
		// read number of knots
		ReadLine(fIn,oneline);
		unsigned int num_knots=0; 
		sscanf(oneline,"num_knots = %d ",&num_knots);

		// create a new segment
		WallSegment seg;

		// cycle through knots
		for(unsigned int i=0; i<num_knots; ++i)
		{
			gmtl::Point3f position;

			// read line
			ReadLine(fIn,oneline);
			sscanf(oneline,"%f %f %f",&position[0],&position[1],&position[2]);

			// store in current segment
			seg.knots.push_back(position);
		}

		// push segment onto path
		m_path->segments.push_back(seg);
	}
	
	// close our file
	fclose(fIn); 

	// rebuild our bricks
	rebuildBricks();

	// return success
	return true;
}

//==================================================================================
// Signals and Slots
//==================================================================================
void BBB_GLWidget::resizeWall(void)		{	rebuildBricks(); }
void BBB_GLWidget::resizeColumns(void)	{	rebuildBricks(); }
void  BBB_GLWidget::resizePadding(void)	{	rebuildBricks(); }

//----------------------------------------------------------------------------------
// Rebuild Wall
//----------------------------------------------------------------------------------
void BBB_GLWidget::rebuildBricks(void)
{
	// if we don't have a form yet, bail
	if(!m_form || !m_path) { return; }

	// if we already have bricks, kill them
	if(m_newBricks.size()>0) 
	{ 
		m_newBricks.clear(); 
		m_cantileverableBricks.clear();
	}

	// some basic variables
	const int brick_w   = m_form->edit_BrickSize_Length->text().toInt();
	const int brick_h   = m_form->edit_BrickSize_Width->text().toInt();
	const int num_rows	= m_form->edit_NumBricks_Row->text().toInt();
	const float halfw		= brick_w * 0.5f;
	const float halfh		= brick_h * 0.5f;
	m_numBricksPerRow		= 0;

	// reset our min/max	
	m_minimum.set(1000000.f,1000000.f,1000000.f);
	m_maximum.set(-1000000.f,-1000000.f,-1000000.f);

	// catch to make sure we don't draw invalid walls
	if(brick_w<=0 || brick_h<=0 || num_rows<=0) { return; }

	// cycle through rows
	for(int r=0; r<num_rows; ++r)
	{
		const float row_h = brick_h * r;

		// cycle through segments
		for(unsigned int i=0; i<m_path->segments.size(); ++i)
		{
			// cycle through knots
			for(unsigned int j=0; j<m_path->segments[i].knots.size(); ++j)
			{
				// if on first loop, skip guts
				if(j==0) continue;

				// get start and end points
				gmtl::Point3f start		= m_path->segments[i].knots[j-1];
				gmtl::Point3f end			= m_path->segments[i].knots[j];
				gmtl::Point3f current	= start;
				
				// find out vector and right vector of segment
				gmtl::Vec3f row_offset = gmtl::Vec3f(0.f,r*brick_h,0.f);
				gmtl::Vec3f out_vec		= end - start; normalize(out_vec);
				gmtl::Vec3f right_vec = cross(out_vec,gmtl::Vec3f(0.f,1.f,0.f)); normalize(right_vec);

				// offset the start position so it lines up
				current += out_vec * halfh;

				// if we are on an "odd" row, then we increase
				// our current half the brick length
				if(!m_uniformStack && (r%2!=0)) 
				{ 
					// make new brick
					Brick brick;
//					brick.state = false;
					brick.state = INACTIVE; // CHIPP
					brick.cantilevered = false;
					brick.corner = false;

					// build normals
					brick.normals[0] = -out_vec;
					brick.normals[1] =  out_vec;
					brick.normals[2] = -right_vec;
					brick.normals[3] =  right_vec;
					brick.normals[4] =  gmtl::Vec3f(0,1.f,0.f);
					brick.normals[5] =  gmtl::Vec3f(0,-1.f,0.f);

					// build our front corners
					brick.corners[0] =  (current + row_offset) + (right_vec * halfh);
					brick.corners[1] =  (current + row_offset) - (right_vec * halfh);
					brick.corners[2] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
					brick.corners[3] =  (current + row_offset) + (right_vec * halfh)  + gmtl::Vec3f(0.f,brick_h,0.f);

					// update our position and move it to the end
					current += out_vec * halfw;

					// build our back corners
					brick.corners[4] =  (current + row_offset) + (right_vec * halfh);
					brick.corners[5] =  (current + row_offset) - (right_vec * halfh);
					brick.corners[6] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
					brick.corners[7] =  (current + row_offset) + (right_vec * halfh)  + gmtl::Vec3f(0.f,brick_h,0.f);

					// store brick
					m_newBricks.push_back(brick);
					if(r==0) m_numBricksPerRow++;
				}

				// infinite loop while we create our bricks
				while(true)
				{
					// store our min/max values
					m_minimum[0] = Min(m_minimum[0],current[0]);
					m_minimum[1] = Min(m_minimum[1],current[1]);
					m_minimum[2] = Min(m_minimum[2],current[2]);
					m_maximum[0] = Max(m_maximum[0],current[0]);
					m_maximum[1] = Max(m_maximum[1],current[1]);
					m_maximum[2] = Max(m_maximum[2],current[2]);

					// see if distance from current to end
					const float dist = Distance(current, end);

					// make new brick
					Brick brick;
//					brick.state = false;
					brick.state = INACTIVE; // CHIPP
					brick.cantilevered = false;
					brick.corner = false;

					// build normals
					brick.normals[0] = -out_vec;
					brick.normals[1] =  out_vec;
					brick.normals[2] = -right_vec;
					brick.normals[3] =  right_vec;
					brick.normals[4] =  gmtl::Vec3f(0,1.f,0.f);
					brick.normals[5] =  gmtl::Vec3f(0,-1.f,0.f);

					// if our distance to the end is greater than
					// our brick size, progress.
					if(dist >= brick_w)
					{
						// build our front corners
						brick.corners[0] =  (current + row_offset) + (right_vec * halfh);
						brick.corners[1] =  (current + row_offset) - (right_vec * halfh);
						brick.corners[2] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
						brick.corners[3] =  (current + row_offset) + (right_vec * halfh)  + gmtl::Vec3f(0.f,brick_h,0.f);

						// update our position and move it to the end
						current += out_vec * brick_w;

						// build our back corners
						brick.corners[4] =  (current + row_offset) + (right_vec * halfh);
						brick.corners[5] =  (current + row_offset) - (right_vec * halfh);
						brick.corners[6] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
						brick.corners[7] =  (current + row_offset) + (right_vec * halfh)  + gmtl::Vec3f(0.f,brick_h,0.f);

						// store brick
						m_newBricks.push_back(brick);
						if(r==0) m_numBricksPerRow++;
					}
					else 
					{ 
						// if our distance is less than 2, don't create
						//if(dist < 2.f) { break; }

						// make new brick
						Brick brick;
//						brick.state = false;
						brick.state = INACTIVE; // CHIPP
						brick.cantilevered = false;
						brick.corner = false;

						// build our front corners
						brick.corners[0] =  (current + row_offset) + (right_vec * halfh);
						brick.corners[1] =  (current + row_offset) - (right_vec * halfh);
						brick.corners[2] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
						brick.corners[3] =  (current + row_offset) + (right_vec * halfh) + gmtl::Vec3f(0.f,brick_h,0.f);

						// update our position and move it to the end
						current = end + (out_vec * halfh);

						// build our back corners
						brick.corners[4] =  (current + row_offset) + (right_vec * halfh);
						brick.corners[5] =  (current + row_offset) - (right_vec * halfh);
						brick.corners[6] = ((current + row_offset) - (right_vec * halfh)) + gmtl::Vec3f(0.f,brick_h,0.f);
						brick.corners[7] =  (current + row_offset) + (right_vec * halfh) + gmtl::Vec3f(0.f,brick_h,0.f);

						// store brick
						m_newBricks.push_back(brick);
						if(r==0) m_numBricksPerRow++;

						// since we're on the last brick, exit loop
						break; 
					}
				} // end while loop
			}
		} // end rows
	}

	// Bricks are created - initilize Cantileverable Flags
	for(int i = 0; i < m_numBricksPerRow; i++)
	{	
		// Initially all Brick columns are set to true
		m_cantileverableBricks.push_back(true);
	}

	// update the padding
	m_paddingRows = m_form->edit_Padding_Rows->text().toInt();

	// update selected start brick
	int offset = m_selectedStartBrick % m_numBricksPerRow;
	m_selectedStartBrick = (m_numBricksPerRow * m_paddingRows) + offset;
	offset = m_selectedEndBrick % m_numBricksPerRow;
	m_selectedEndBrick = (m_numBricksPerRow * m_paddingRows) + offset;

	// zoom our window
	zoomExtents();

	qDebug("Rebuilt bricks\n"); 
}

//----------------------------------------------------------------------------------
// Reset All to Defaults
//----------------------------------------------------------------------------------
void BBB_GLWidget::resetAll(void)
{
	// reset our UI
	m_form->edit_BrickSize_Width->setText("4");
	m_form->edit_BrickSize_Length->setText("8");
	m_form->edit_NumBricks_Row->setText("10");
	m_form->edit_Padding_Rows->setText("1");
	
	// reset our view rotation
	m_xrot			 = 20.0f;
	m_yrot			 = 45.0f;

	if(m_path) { delete m_path; m_path=0; }
	m_newBricks.clear();
	m_cantileverableBricks.clear();

	// update our wall
	rebuildBricks();
	zoomExtents();

	// clear all bricks, patterns, and button states
	clearBricks();
	clearRules();
}

//----------------------------------------------------------------------------------
// Reset our View in Case we Zoomed to Far In/Out
//----------------------------------------------------------------------------------
void BBB_GLWidget::zoomExtents(void)
{
	const float dist = Distance(m_minimum,m_maximum);
	m_zoom = dist * 1.5f;
}

//----------------------------------------------------------------------------------
// Invert the State of All Blocks - WITH the cantilever rule
//----------------------------------------------------------------------------------
/*
void BBB_GLWidget::invertActive(void)
{
	if(m_newBricks.size()<=0) { return; }

	const int num_rows = m_form->edit_NumBricks_Row->text().toInt();

	for(int r=0; r<num_rows; ++r)
	{
		for(int i=0; i<m_numBricksPerRow; ++i)
//		{ m_newBricks[(r*m_numBricksPerRow)+i].state = !m_newBricks[(r*m_numBricksPerRow)+i].state; }
		{
			// First Clear any Cantilever
			m_newBricks[(r*m_numBricksPerRow)+i].clearCantilever(); 

			// Invert the Pattern
			if( (m_newBricks[(r*m_numBricksPerRow)+i].state == ACTIVE) )
			{ m_newBricks[(r*m_numBricksPerRow)+i].state = INACTIVE; }
			else if(m_newBricks[(r*m_numBricksPerRow)+i].state == INACTIVE)
			{ m_newBricks[(r*m_numBricksPerRow)+i].state = ACTIVE; }
		} 
	}

	// New "pattern" needs to have a cantilever 
	cantileverPattern();
}
*/
void BBB_GLWidget::invertActive(void)
{
	if(m_newBricks.size()<=0) { return; }

	const int num_rows = m_form->edit_NumBricks_Row->text().toInt();

	for(int r=0; r<num_rows; ++r)
	{
		for(int i=0; i<m_numBricksPerRow; ++i)
		{
			// Invert the Pattern
			if( (m_newBricks[(r*m_numBricksPerRow)+i].state == ACTIVE) )
			{ m_newBricks[(r*m_numBricksPerRow)+i].state = INACTIVE; }
			else if(m_newBricks[(r*m_numBricksPerRow)+i].state == INACTIVE)
			{ m_newBricks[(r*m_numBricksPerRow)+i].state = ACTIVE; }
		} 
	}
}

//----------------------------------------------------------------------------------
// Take a Screenshot of Our OpenGL Window
//----------------------------------------------------------------------------------
void BBB_GLWidget::takeScreenshot(void)
{
	// make our context current
	makeCurrent();

	// grab the frame buffer (i.e. pixels of the screen)
	QImage screen = this->grabFrameBuffer(false);

	// create the filename to save it too
	QString filename = QFileDialog::getSaveFileName("/","BMP Images (*.bmp)", this,"open file dialog","Choose a file to open" );

	// if we canceled, bail
	if(filename.isEmpty()) { return; }

	// save the data to an file
	if(!screen.save(filename,"BMP",-1))
	{ qDebug("couldn't save file\n"); }
}

//----------------------------------------------------------------------------------
// Activate Bricks for Start Condition
//----------------------------------------------------------------------------------
void BBB_GLWidget::activateBricks(void)
{ 
	if(m_form->btn_ActivateBricks->state()==QButton::ToggleState::On)
	{ 
		qDebug("activating bricks..."); 

		// we're creating our start condition...update UI
		m_form->btn_ActivateBricks->setText("Select Bricks"); 

		// clear the states of all bricks
		// clearBricks(1 + m_paddingRows); // will explicitly "clear" the pattern with Clear Pattern

		// set the internal flag to let our event handler know
		// we're selecting bricks and we grab the keyboard so
		// that we receive all key events
		m_pickingBricks = true;
		grabKeyboard();
	}
	else
	{ 
		qDebug("done\n"); 

		// we're done selecting bricks
		m_form->btn_ActivateBricks->setText("Activate Bricks"); 

		// set our flag to get our of activation mode
		// and release the keyboard.
		m_pickingBricks = false;
		releaseKeyboard();
	}
}

//----------------------------------------------------------------------------------
// Clear Active Bricks
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearActiveBricks(void)
{ 
	qDebug("clearing ALL active bricks...done\n"); 

	// clear the states of all bricks
	clearBricks(0);
}

//----------------------------------------------------------------------------------
// Toggle Initial Condition - toggles the ACTIVE/INACTIVE flag for all rows withing padding
//----------------------------------------------------------------------------------
void BBB_GLWidget::toggleInitialCondition(int index)
{ 
	int offset = (index % m_numBricksPerRow);

	qDebug("toggleInitialCondition offset: %d\n", offset);
	//if(index < (m_numBricksPerRow * (m_paddingRows + 1))) { // toggle only within padding rows
		if(m_newBricks[offset].state == ACTIVE) {
			setInitialCondition(offset, INACTIVE);
		} else if(m_newBricks[offset].state == INACTIVE) {
			setInitialCondition(offset, ACTIVE);
		} 
	//}
}

//----------------------------------------------------------------------------------
// Set Initial Condition
//----------------------------------------------------------------------------------
void BBB_GLWidget::setInitialCondition(int index, BrickState bs)
{ 
	int offset = (index % m_numBricksPerRow);

	if(bs == ACTIVE) {
		qDebug("setInitialCondition state: ACTIVE\n");
	} else if(bs == INACTIVE) {
		qDebug("setInitialCondition state: INACTIVE\n");
	} else {
		qDebug("setInitialCondition state: UNKNOWN\n");
	}
		
	for(int i = offset; i < (m_numBricksPerRow * (m_paddingRows + 1)); i = i + m_numBricksPerRow)
	{
		m_newBricks[i].state = bs;
	}
}

//----------------------------------------------------------------------------------
// Randomly set the Initial Conditions
//----------------------------------------------------------------------------------
void BBB_GLWidget::randomActiveBricks(void)
{
	// debug
	qDebug("generating start condition...");

	// seed random number generator
	srand((unsigned)time(0));

	// cycle over first row
	for(int i=0; i<m_numBricksPerRow; ++i)
	{
		//		if(rand()%2==0) { m_newBricks[i].state = ACTIVE;  }
		//		else { m_newBricks[i].state = INACTIVE; }
		// Sets entire row
		if(rand()%2==0) 
		{ setInitialCondition(i, ACTIVE); }
		else 
		{ setInitialCondition(i, INACTIVE); }
	} 
	
	update(); // update the interface

	// debug
	qDebug("random done\n");
}

//----------------------------------------------------------------------------------
// Evaluate Neighborhood
//----------------------------------------------------------------------------------
void BBB_GLWidget::evaluateNeighborhood(int index,int n1,int n2,int n3,int n4,int n5)
{
	// CHIPP - Changed a condition test from: m_newBricks[n1].state 
	// to: (m_newBricks[n1].state != INACTIVE.  Test is TRUE for ACTIVE and CANTILEVERED.

	// 00000
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00000->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00001
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00001->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00010
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00010->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00011
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00011->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00100
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00100->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00101
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00101->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00110
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00110->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 00111
	if(!(m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_00111->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01000
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01000->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01001
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01001->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01010
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01010->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01011
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01011->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01100
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01100->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01101
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01101->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01110
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01110->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 01111
	if(!(m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_01111->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10000
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10000->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10001
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10001->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10010
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10010->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10011
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10011->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10100
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10100->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10101
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10101->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10110
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10110->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 10111
	if((m_newBricks[n1].state != INACTIVE) && !(m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_10111->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11000
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11000->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11001
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11001->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11010
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11010->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11011
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && !(m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11011->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11100
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11100->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11101
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && !(m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11101->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11110
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && !(m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11110->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}

	// 11111
	if((m_newBricks[n1].state != INACTIVE) && (m_newBricks[n2].state != INACTIVE) && (m_newBricks[n3].state != INACTIVE) && (m_newBricks[n4].state != INACTIVE) && (m_newBricks[n5].state != INACTIVE))
	{ if(m_form->btn_Rule_11111->isOn()) { m_newBricks[index].state=ACTIVE; }} // CHIPP // { m_newBricks[index].state=true; }}
}

//----------------------------------------------------------------------------------
// Evaluate Rule
//----------------------------------------------------------------------------------
void BBB_GLWidget::evaluateRule(int index)
{
	// some temp variables for neighbors
	int n1,n2,n3,n4,n5;
	int c_index = index + 1;

	// get the # of rows and columns
	const unsigned int num_rows = m_form->edit_NumBricks_Row->text().toInt();
	const unsigned int num_cols = m_numBricksPerRow;

	// index value refers to cell being evaluated
	// if cell being evaluated is in the last column
	if(c_index % num_cols == 0)
	{
		// establish neighborhood parameters
		n1 = c_index - num_cols - 2;
		n2 = c_index - num_cols - 1;
		n3 = c_index - num_cols;
		n4 = c_index - (2 * num_cols) + 1;
		n5 = c_index - (2 * num_cols) + 2;
	}
	// if cell being evaluated is in the first column
	else if(c_index % num_cols == 1)
	{
		// establish neighborhood parameters
		n1 = c_index - 2;
		n2 = c_index - 1;
		n3 = c_index - num_cols;
		n4 = c_index - num_cols + 1;
		n5 = c_index - num_cols + 2;
	}
	// if cell being evaluated is in the second column
	else if (c_index % num_cols == 2)
	{
		// establish neighborhood parameters
		n1 = c_index - 2;
		n2 = c_index - num_cols - 1;
		n3 = c_index - num_cols;
		n4 = c_index - num_cols + 1;
		n5 = c_index - num_cols + 2;
	}
	// if cell being evaluated is in the second to last column
	else if (num_cols - (c_index % num_cols) == 1)
	{
		// establish neighborhood parameters
		n1 = c_index - num_cols - 2;
		n2 = c_index - num_cols - 1;
		n3 = c_index - num_cols;
		n4 = c_index - num_cols + 1;
		n5 = c_index - num_cols - (c_index % num_cols) + 1;
	}
	// for all other conditions
	else 
	{
		// establish neighborhood parameters
		n1 = c_index - num_cols - 2;
		n2 = c_index - num_cols - 1;
		n3 = c_index - num_cols;
		n4 = c_index - num_cols + 1;
		n5 = c_index - num_cols + 2;
	}

	// evaluate neighbors
	evaluateNeighborhood(index, abs(n1)-1, abs(n2)-1, abs(n3)-1, abs(n4)-1, abs(n5)-1);
}

//----------------------------------------------------------------------------------
// Generate CA Pattern
//----------------------------------------------------------------------------------
void BBB_GLWidget::generatePattern(void)
{ 
	// debug statement and clear existing pattern
	qDebug("generating CA pattern..."); 
	//clearBricks(1);
	clearBricks(1 + m_paddingRows);

	// get our rows and columns
	const int num_rows = m_form->edit_NumBricks_Row->text().toInt();

	// OLD Pattern Generations, starting at index 0, row 1
/*
	for(int i=1; i<num_rows; ++i)
	{	
		for(int j=0; j<m_numBricksPerRow; ++j)
		{	
			// evaluate our rule
			const int index = (i*m_numBricksPerRow)+j;
			evaluateRule(index);
		}
	}
*/
	const int offset = m_selectedStartBrick % m_numBricksPerRow; // offset of selected brick

	for(int r = m_paddingRows+1; r < num_rows; r++) // Cycle through rows
	{	
		for(int i=0; i < m_numBricksPerRow; i++) // Cycle through columns in rows
		{	
			// evaluate our rule
			const int index = (r*m_numBricksPerRow) + ((offset+i) % m_numBricksPerRow);
			evaluateRule(index);
		}
	}



	// OLD CANTILEVER RULE
	// Set any ACTIVE brick rows with m_cantileveredHeight
	// cantileverPattern();

	update(); // update the interface

	// debug statement
	qDebug("generate done\n");
}

//----------------------------------------------------------------------------------
// Create Cantilever within Pattern
//----------------------------------------------------------------------------------
void BBB_GLWidget::cantileverPattern()
{ 
	// refresh the cantilevered Height criterea
	m_cantileveredHeight = m_form->edit_Cantilevered_Height->text().toInt();

	qDebug("  generating cantilever with criteria: %d\n", m_cantileveredHeight);
	for(unsigned int i=0; i < m_newBricks.size(); i++)
	{ 
		if(cantileverCheck(i)) // && (m_newBricks[i].state == ACTIVE)) 
		{
			qDebug("  cantilever columns %d...\n", i);
			setCantileverColumn(i);
		}
	} 
} // CHIPP

//----------------------------------------------------------------------------------
// Refresh the Cantilever on a certain columns
//----------------------------------------------------------------------------------
void BBB_GLWidget::cantileverPattern(int col)
{ 
	// refresh the cantilevered Height criterea
	m_cantileveredHeight = m_form->edit_Cantilevered_Height->text().toInt();

	// check if the row is cantileverable
	if(m_cantileverableBricks[col % m_numBricksPerRow] == false)
	{ 
		clearCantileverColumn(col);
	}
	else // row is cantileverabley
	{
		qDebug("  generating cantilever with criteria: %d, on col: %d\n", m_cantileveredHeight, col);
		for(int i = col; i < m_newBricks.size(); i += m_numBricksPerRow) // check each brick in the row
		{	
			if(cantileverCheck(i)) // && (m_newBricks[i].state == ACTIVE)) 
			{
				qDebug("  cantilever columns %d...\n", i);
				setCantileverColumn(i);
			}
		}
	} 
} // CHIPP

//----------------------------------------------------------------------------------
// Check a column of bricks above that they are not ACTIVE
// A consecutive row of m_cantileveredHeight'd INACTIVE Bricks 
//----------------------------------------------------------------------------------
bool BBB_GLWidget::cantileverCheck(int index)
{
	unsigned int i = index % m_numBricksPerRow; // the index of the bricks above 'index'
	
	// Check if the index is in a cantileverable column
	if(i < m_cantileverableBricks.size())
	{
		if(m_cantileverableBricks[i] == false)
		{ 
			return false; 
		}
	}
	else // ERROR in the m_cantileverableBricks cantilever flag vectors
	{
		QMessageBox::information(0,"Error","Reset the Canilever settings.");
		return false;
	}

	// check no more than 'm_cantileveredHeight' bricks in the height column
	i = index; // the index of the bricks above 'index'
	int n = 0; // number of rows checked
	for(; (n < m_cantileveredHeight) && (i < m_newBricks.size()); n++, i = index + (n * m_numBricksPerRow)) 
	{		
		if(m_newBricks[i].state == ACTIVE) // an ACTIVE rejects the cantilever test
			return false;
	}

	// Check again if n is less than m_cantileveredHeight in case of running out of bricks at the top
 	if(n < m_cantileveredHeight) // Remove this check if you want to Cantilever the upper rows
	{	return false; } 
	else
	{	return true; } // passed the cantilever test
} // CHIPP

//----------------------------------------------------------------------------------
// Set the column of bricks to be CANTILEVERED starting at the row with index
//----------------------------------------------------------------------------------
void BBB_GLWidget::setCantileverColumn(int index)
{
	// set the entire column of 'index' to CANTILEVERED_* bricks
	// starting the row that contains 'index'
	for(unsigned int i = index; i < m_newBricks.size(); i+=m_numBricksPerRow) 
	{  m_newBricks[i].setCantilever(); }
} // CHIPP

//----------------------------------------------------------------------------------
// Clear the column of bricks of any CANTILEVERED
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearCantileverColumn(int index)
{
	// reset the entire column of 'index' of any CANTILEVERED bricks
	for(unsigned int i = index; i < m_newBricks.size(); i+=m_numBricksPerRow) 
	{ m_newBricks[i].clearCantilever(); }
} // CHIPP


//----------------------------------------------------------------------------------
// Set the entire Column of Bricks as CORNER Bricks
//----------------------------------------------------------------------------------
void BBB_GLWidget::setCornerColumn(int index)
{
	// Sets the entire column (and neighboring N columns) that the selected brick is in to Corner Bricks
	// for(unsigned int i = (index%m_numBricksPerRow); i < m_newBricks.size(); i+=m_numBricksPerRow) 
	for(unsigned int i = index; i < m_newBricks.size(); i+=m_numBricksPerRow) 
	{  
		int j = (i - m_cornerWidth);

		for(; (j < 0); j++)
		{ 
			m_newBricks[j + m_newBricks.size()].setCorner(); // wrap around the corner
			qDebug("* setCornerColumn underflow: %d\n", j);
		}	

		for(; (j < m_newBricks.size()) && (j < (i + m_cornerWidth + 1)); j++)
		{ 
			m_newBricks[j].setCorner(); 
		}

		for(; (j < (i + m_cornerWidth + 1)); j++)
		{ 
			// m_newBricks[j - m_newBricks.size() - m_numBricksPerRow + index + 1].setCorner(); 
			m_newBricks[index + 1 - m_numBricksPerRow].setCorner(); 
			qDebug("*  setCornerColumn overflow: %d, index: %d, i: %d, array_index: %d\n", j, index, i, j - m_newBricks.size() + index);
		}
	}
}

//----------------------------------------------------------------------------------
// Clear the entire Column of Bricks as CORNER Bricks
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearCornerColumn(int index)
{
	// Sets the entire column (and neighboring N columns) that the selected brick is in to Corner Bricks
	// for(unsigned int i = (index%m_numBricksPerRow); i < m_newBricks.size(); i+=m_numBricksPerRow) 
	for(unsigned int i = index; i < m_newBricks.size(); i+=m_numBricksPerRow) 
	{  
		int j = (i - m_cornerWidth);

		for(; (j < 0); j++)
		{ 
			m_newBricks[j + m_newBricks.size()].clearCorner(); // wrap around the corner
			qDebug("* setCornerColumn underflow: %d\n", j);
		}	

		for(; (j < m_newBricks.size()) && (j < (i + m_cornerWidth + 1)); j++)
		{ 
			m_newBricks[j].clearCorner(); 
		}

		for(; (j < (i + m_cornerWidth + 1)); j++)
		{ 
			m_newBricks[index + 1 - m_numBricksPerRow].clearCorner(); 
			qDebug("*  setCornerColumn overflow: %d, index: %d, i: %d, array_index: %d\n", j, index, i, j - m_newBricks.size() + index);

		}
	}
}

/*
{
	// Clears the entire column (and neighboring N columns) that the selected brick is in to Corner Bricks
	for(unsigned int i = (index%m_numBricksPerRow); i < m_newBricks.size(); i+=m_numBricksPerRow) 
	{  
		int j = (i - m_cornerWidth);
		if(j < 0) j=0;

		for(; (j < m_newBricks.size()) && (j < (i + m_cornerWidth)); j++)
		{ m_newBricks[j].clearCorner(); }
	}
}

*/



//----------------------------------------------------------------------------------
// Clear CA Pattern
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearPattern(void)
{
	// START OLD CANTILEVER:
	// for(int i=0; i < m_numBricksPerRow; ++i)
	//{ 
	//	m_newBricks[i].clearCantilever();
	//} // END OLD CANTILEVER

	// Clear all Bricks from m_padding on up
	clearBricks(m_paddingRows + 1);

	update();
}

//----------------------------------------------------------------------------------
// Reset Rules
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearRules(void)
{
	m_form->btn_Rule_00000->setOn(false);
	m_form->btn_Rule_10000->setOn(false);
	m_form->btn_Rule_01000->setOn(false);
	m_form->btn_Rule_00100->setOn(false);
	m_form->btn_Rule_00010->setOn(false);
	m_form->btn_Rule_00001->setOn(false);
	m_form->btn_Rule_11000->setOn(false);
	m_form->btn_Rule_10100->setOn(false);
	m_form->btn_Rule_10010->setOn(false);
	m_form->btn_Rule_10001->setOn(false);
	m_form->btn_Rule_01100->setOn(false);
	m_form->btn_Rule_01010->setOn(false);
	m_form->btn_Rule_01001->setOn(false);
	m_form->btn_Rule_00110->setOn(false);
	m_form->btn_Rule_00101->setOn(false);
	m_form->btn_Rule_00011->setOn(false);
	m_form->btn_Rule_11100->setOn(false);
	m_form->btn_Rule_11010->setOn(false);
	m_form->btn_Rule_11001->setOn(false);
	m_form->btn_Rule_11110->setOn(false);
	m_form->btn_Rule_01111->setOn(false);
	m_form->btn_Rule_01110->setOn(false);
	m_form->btn_Rule_10101->setOn(false);
	m_form->btn_Rule_10111->setOn(false);
	m_form->btn_Rule_10011->setOn(false);
	m_form->btn_Rule_01101->setOn(false);
	m_form->btn_Rule_11011->setOn(false);
	m_form->btn_Rule_01011->setOn(false);
	m_form->btn_Rule_11101->setOn(false);
	m_form->btn_Rule_11111->setOn(false);
	m_form->btn_Rule_00111->setOn(false);
	m_form->btn_Rule_10110->setOn(false);
}

//----------------------------------------------------------------------------------
void BBB_GLWidget::toggleOffset(void)
{ m_uniformStack = !m_uniformStack; rebuildBricks(); }

//----------------------------------------------------------------------------------
// Generate Beams
//----------------------------------------------------------------------------------
void BBB_GLWidget::generateBeams(void) {}

//----------------------------------------------------------------------------------
// Clear Beams
//----------------------------------------------------------------------------------
void BBB_GLWidget::clearBeams(void) {}

//----------------------------------------------------------------------------------
// Event Handler
//----------------------------------------------------------------------------------
bool BBB_GLWidget::event(QEvent* e) 
{
	switch(e->type())
	{
	//------------------------------------------------------------
	case QEvent::Paint:	
		{ 
			updateGL();	
			return true;
		} break;

	//------------------------------------------------------------
	// if our window is resized or moved we need to explicitly
	// call our resizeGL routine since we're handling events 
	// ourselves.
	//------------------------------------------------------------
	case QEvent::Resize: 
		{ 
			QSize s = ((QResizeEvent*)e)->size();
			resizeGL(s.width(),s.height());
			updateGL(); 
			return true;
		} break;
	
	//------------------------------------------------------------
	// when the mouse button is pressed (left) we need to reset
	// our positions or our transforms won't respond correctly
	//------------------------------------------------------------
	case QEvent::MouseButtonPress:		
		{ 
			m_lastMouseY = ((QMouseEvent*)e)->y(); 
			m_lastMouseX = ((QMouseEvent*)e)->x();
			m_mouseDown = true;	 
			return true;
		} break;
	
	//------------------------------------------------------------
	case QEvent::MouseButtonRelease:	
		{ 
			m_mouseDown = false; 
			return true;		
		} break;
	
	//------------------------------------------------------------
	// if the mouse is moving we see if we should rotate, pan,
	// or zoom. 
	//------------------------------------------------------------
	case QEvent::MouseMove:
		{ 
			if(m_mouseDown) 
			{ 
				// get our current mouse positions
				int c_xpos = ((QMouseEvent*)e)->x();
				int c_ypos = ((QMouseEvent*)e)->y();

				// pan the viewport
				if(((QMouseEvent*)e)->state()&QEvent::ControlButton)		
				{ 
					m_xpos += (c_xpos - m_lastMouseX);
					m_ypos += (c_ypos - m_lastMouseY);
					m_lastMouseX = c_xpos;
					m_lastMouseY = c_ypos;
					update();
				}

				// zoom the viewport
				else if(((QMouseEvent*)e)->state()&QEvent::ShiftButton) 
				{ 
					m_zoom += (c_ypos - m_lastMouseY);
					m_lastMouseY = c_ypos;
					update();
				}

				// rotate the viewport
				else
				{ 
					m_xrot += c_ypos - m_lastMouseY;
					m_yrot += c_xpos - m_lastMouseX;
					m_lastMouseX = c_xpos;
					m_lastMouseY = c_ypos;
					update();
				}
			}
			return true;
		} break;

	//------------------------------------------------------------
	// handle key events
	//------------------------------------------------------------
	case QEvent::KeyPress:
		{
			// if not in selection mode, bail
			if(!m_pickingBricks) { break; }

			// if pressing Up, increase the selected brick row
			if(((QKeyEvent*)e)->key()==Qt::Key_Up)
			{ 
				m_selectedBrick += m_numBricksPerRow;
				if(m_selectedBrick >= m_newBricks.size()) { m_selectedBrick -= m_numBricksPerRow; }
			}

			// if pressing Down, decrease the selected brick row
			if(((QKeyEvent*)e)->key()==Qt::Key_Down)
			{ 
				m_selectedBrick -= m_numBricksPerRow;
				if(m_selectedBrick < 0) { m_selectedBrick += m_numBricksPerRow; }
			}

			// if pressing left, increase the selected brick position/index
			if(((QKeyEvent*)e)->key()==Qt::Key_Left)
			{ 
				int row = m_selectedBrick / m_numBricksPerRow;
				m_selectedBrick -= 1;
				
				if((m_selectedBrick / m_numBricksPerRow) != row) 
				{ m_selectedBrick += m_numBricksPerRow; }
				
				if(m_selectedBrick < 0) { m_selectedBrick += m_numBricksPerRow; }
							
				// if control is pressed, select the brick too
				if(((QKeyEvent*)e)->state()&QEvent::ControlButton)
				{
					if(m_newBricks[m_selectedBrick].state == ACTIVE) 
					{ m_newBricks[m_selectedBrick].state = INACTIVE; }
					else if(m_newBricks[m_selectedBrick].state == INACTIVE) 
					{ m_newBricks[m_selectedBrick].state = ACTIVE; }
				} // CHIPP
			}
			
			// if pressing right, decrease index
			else if(((QKeyEvent*)e)->key()==Qt::Key_Right)
			{ 
				int row = m_selectedBrick / m_numBricksPerRow;
				m_selectedBrick += 1;
				if((m_selectedBrick / m_numBricksPerRow) != row)
				{ m_selectedBrick -= m_numBricksPerRow; }

				// if control is pressed, select the brick too
				if(((QKeyEvent*)e)->state()&QEvent::ControlButton) // CHIPP - changed to match above comment
				{
					if(m_newBricks[m_selectedBrick].state == ACTIVE) 
					{ m_newBricks[m_selectedBrick].state = INACTIVE; }
					else if(m_newBricks[m_selectedBrick].state == INACTIVE) 
					{ m_newBricks[m_selectedBrick].state = ACTIVE; }
				} // CHIPP
			}
			
			// if "space" is pressed, toggle state of brick
			// else if(((QKeyEvent*)e)->state()&QEvent::AltButton) // BROKEN
			else if(((QKeyEvent*)e)->key()==Qt::Key_Space)			
			{	
				toggleInitialCondition(m_selectedBrick);
			} // CHIPP

			else if(((QKeyEvent*)e)->key()==Qt::Key_Z)			
			{	
				toggleInitialCondition(m_selectedBrick);
			} // CHIPP


			// if "s" is pressed, then the current brick is 
			// set as a GREEN starting brick
			else if(((QKeyEvent*)e)->key()==Qt::Key_S)		
			{ 	
				// m_selectedStartBrick = m_selectedBrick; 
				int offset = m_selectedBrick % m_numBricksPerRow;
				m_selectedStartBrick = (m_numBricksPerRow * (m_paddingRows)) + offset;
			}

			// if "e" is pressed, then the current brick is 
			// set as a PURPLE ending brick
			else if(((QKeyEvent*)e)->key()==Qt::Key_E)		
			{ 	
				// m_selectedStartBrick = m_selectedBrick; 
				int offset = m_selectedBrick % m_numBricksPerRow;
				m_selectedEndBrick = (m_numBricksPerRow * (m_paddingRows)) + offset;
			}

/*
			// if "c" is pressed then the above column is set as an cantilever column
			else if(((QKeyEvent*)e)->key()==Qt::Key_C)		
			{ 
				m_cantileverableBricks[m_selectedBrick % m_numBricksPerRow] = !m_cantileverableBricks[m_selectedBrick % m_numBricksPerRow]; 
				cantileverPattern(m_selectedBrick); // refresh the column's cantilever
			}
*/
			// if "c" is pressed then the brick and all bricks above are cantilevered
			else if(((QKeyEvent*)e)->key()==Qt::Key_C)		
			{ 
				if(m_newBricks[m_selectedBrick].isCantilevered())
				{
					clearCantileverColumn(m_selectedBrick);
				} 
				else 
				{
					setCantileverColumn(m_selectedBrick);
				}
			}

			// if "r" is pressed then the brick's column and the column +1 and -1 are converted to corner
			else if(((QKeyEvent*)e)->key()==Qt::Key_R)		
			{ 	
				if(m_newBricks[m_selectedBrick].isCorner())
				{
					clearCornerColumn(m_selectedBrick);
				} 
				else 
				{
					setCornerColumn(m_selectedBrick);
				}
			}

			// if "x" then clear the column's values
			else if(((QKeyEvent*)e)->key()==Qt::Key_X)		
			{ 
				clearColumn(m_selectedBrick);
			}

			// Individually set Brick Values:
			else if(((QKeyEvent*)e)->key()==Qt::Key_1)		
			{
				m_newBricks[m_selectedBrick].state = ACTIVE;
			}
			else if(((QKeyEvent*)e)->key()==Qt::Key_2)		
			{
				m_newBricks[m_selectedBrick].state = INACTIVE;
			}
			else if(((QKeyEvent*)e)->key()==Qt::Key_3)		
			{
				if(m_newBricks[m_selectedBrick].isCantilevered())
				{
					m_newBricks[m_selectedBrick].clearCantilever();
				} 
				else 
				{
					m_newBricks[m_selectedBrick].setCantilever();
				}
			}
			else if(((QKeyEvent*)e)->key()==Qt::Key_4)		
			{
				if(m_newBricks[m_selectedBrick].isCorner())
				{
					m_newBricks[m_selectedBrick].clearCorner();
				} 
				else 
				{
					m_newBricks[m_selectedBrick].setCorner();
				}
			}
		} 
		qDebug("*  selected brick: %d\n", m_selectedBrick);

		update(); // CHIPP - refresh after key presses

		return true;
		break;
	};

	// return success
	return false; 
}

//----------------------------------------------------------------------------------
bool BBB_GLWidget::savePattern(void)
{
	// if no path, bail
	if(!m_path) 
	{ QMessageBox::information(0,"Error","No pattern to save!"); return false; }

	// create the filename to save it too
	QString filename = QFileDialog::getSaveFileName("/","AutoMason Pattern (*.amp)",
																									this,"save pattern","choose where you want to save" );
	if(filename.isEmpty()) { return false; }

	// open file for writing
	FILE* fOut = fopen(filename,"wb");
	if(!fOut) { return false; }
	
	// some temp variables
	bool value	= false;

	// grab our creation data
	const int brick_l   = m_form->edit_BrickSize_Length->text().toInt();
	const int brick_w   = m_form->edit_BrickSize_Width->text().toInt();
	const int num_rows	= m_form->edit_NumBricks_Row->text().toInt();

	// write brick creation data
	fwrite(&brick_l,sizeof(int),1,fOut);
	fwrite(&brick_w,sizeof(int),1,fOut);
	fwrite(&num_rows,sizeof(int),1,fOut);

	// write out our loaded path (if any)
	fwrite(m_path->filename,sizeof(char),256,fOut);

	// write start condition
	for(int i=0; i<m_numBricksPerRow; ++i)
	{ value = m_newBricks[i].state; fwrite(&value,sizeof(bool),1,fOut);	}

	// write rules
	value = m_form->btn_Rule_00000->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00001->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00010->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00011->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00100->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00101->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00110->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_00111->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01000->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01001->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01010->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01011->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01100->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01101->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01110->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_01111->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10000->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10001->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10010->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10011->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10100->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10101->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10110->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_10111->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11000->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11001->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11010->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11011->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11100->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11101->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11110->isOn();	fwrite(&value,sizeof(bool),1,fOut);
	value = m_form->btn_Rule_11111->isOn();	fwrite(&value,sizeof(bool),1,fOut);

	// return success
	fclose(fOut);
	return true;
}

//----------------------------------------------------------------------------------
bool BBB_GLWidget::loadPattern(void)
{
	// create the filename to save it too
	QString filename = QFileDialog::getOpenFileName("./","AutoMason Pattern (*.amp)", this,"load pattern","choose the pattern you wish to load" );
	if(filename.isEmpty()) { return false; }

	// open file for writing
	FILE* fIn = fopen(filename,"rb");
	if(!fIn) { return false; }

	// some temp variables
	bool value	= false;
	int tmp_int = 0; 
	char tmp_str[256];

	// read brick creation data
	fread(&tmp_int,sizeof(int),1,fIn); sprintf(tmp_str,"%d",tmp_int); m_form->edit_BrickSize_Length->setText(tmp_str);
	fread(&tmp_int,sizeof(int),1,fIn); sprintf(tmp_str,"%d",tmp_int); m_form->edit_BrickSize_Width->setText(tmp_str);
	fread(&tmp_int,sizeof(int),1,fIn); sprintf(tmp_str,"%d",tmp_int); m_form->edit_NumBricks_Row->setText(tmp_str);

	// read our loaded path (if any)
	fread(tmp_str,sizeof(char),256,fIn);
	importPath(tmp_str);

	// rebuild our bricks
	rebuildBricks();

	// read in our start condition
	for(int i=0; i<m_numBricksPerRow; ++i)
//	{ fread(&value,sizeof(bool),1,fIn);	m_newBricks[i].state=value; }
	{
		 fread(&value,sizeof(bool),1,fIn);
		 if(value) 
		 { m_newBricks[i].state=ACTIVE; }
		 else 
		 { m_newBricks[i].state=INACTIVE; }
	} // CHIPP

	// read in our rules
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00000->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00001->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00010->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00011->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00100->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00101->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00110->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_00111->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01000->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01001->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01010->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01011->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01100->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01101->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01110->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_01111->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10000->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10001->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10010->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10011->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10100->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10101->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10110->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_10111->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11000->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11001->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11010->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11011->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11100->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11101->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11110->setOn(value);
	fread(&value,sizeof(bool),1,fIn); m_form->btn_Rule_11111->setOn(value);
	generatePattern();

	// return success
	fclose(fIn);
	return true;
}

//----------------------------------------------------------------------------------
bool BBB_GLWidget::exportToObj(void)
{
	// if no path, bail
	if(!m_path) 
	{ QMessageBox::information(0,"Error","No pattern to export!"); return false; }

	// create the filename to save it too
	QString filename = QFileDialog::getSaveFileName("./","OBJ Object File (*.obj)", this,"export pattern","choose where you want to export" );
	
	if(filename.isEmpty()) { return false; }

	// open file for writing
	FILE* fOut = fopen(filename,"wt");
	if(!fOut) { return false; }

	// print header info
	fprintf(fOut,"# Max2Obj Version 4.0 Mar 10th, 2001\n");
	fprintf(fOut,"#\n");

	// print material name
	fprintf(fOut,"mtllib ./AutoMason.mtl\n");
	fprintf(fOut,"g\n");

	// object header
	fprintf(fOut,"# object AutoMason_obj to come ...\n");
	fprintf(fOut,"#\n");

	// vertices
	for(unsigned int i=0; i<m_newBricks.size(); ++i)
	{
		for(unsigned int j=0; j<8; ++j)
		{
			gmtl::Point3f pos = m_newBricks[i].corners[j];
			fprintf(fOut,"v %f %f %f\n",pos[0],pos[1],pos[2]);
		}
	}

	// build up vert count
	int num_verts = m_numBricksPerRow * m_form->edit_NumBricks_Row->text().toInt() * 8;
	fprintf(fOut,"# %d texture vertices\n\n",num_verts);

	// object define
	fprintf(fOut,"g AutoMason_obj\n");
	fprintf(fOut,"usemtl inactive\n");

	// faces & smoothing groups
	for(unsigned int i=0; i<m_newBricks.size(); ++i)
	{
		if(!m_newBricks[i].state) continue;

		int offset = (i*8);
		fprintf(fOut,"s 1\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+4,offset+3,offset+2,offset+1);
		fprintf(fOut,"s 2\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+6,offset+7,offset+8,offset+5);
		fprintf(fOut,"s 3\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+2,offset+3,offset+7,offset+6);
		fprintf(fOut,"s 4\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+5,offset+8,offset+4,offset+1);
		fprintf(fOut,"s 5\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+5,offset+1,offset+2,offset+6);
		fprintf(fOut,"s 6\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+4,offset+8,offset+7,offset+3);
	}

	fprintf(fOut,"usemtl active\n");
	for(unsigned int i=0; i<m_newBricks.size(); ++i)
	{
		if(m_newBricks[i].state) continue;

		int offset = (i*8);
		fprintf(fOut,"s 1\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+4,offset+3,offset+2,offset+1);
		fprintf(fOut,"s 2\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+6,offset+7,offset+8,offset+5);
		fprintf(fOut,"s 3\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+2,offset+3,offset+7,offset+6);
		fprintf(fOut,"s 4\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+5,offset+8,offset+4,offset+1);
		fprintf(fOut,"s 5\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+5,offset+1,offset+2,offset+6);
		fprintf(fOut,"s 6\n");
		fprintf(fOut,"f %d %d %d %d\n",offset+4,offset+8,offset+7,offset+3);
	}
	fprintf(fOut,"# %d faces\n\n",m_newBricks.size()*6);

	// closer
	fprintf(fOut,"g");
	fclose(fOut);

	// return
	return true;
}

//----------------------------------------------------------------------------------
bool BBB_GLWidget::exportToRLE(void)
{ 	// if no path, bail
	if(!m_path) 
	{ QMessageBox::information(0,"Error","No pattern to export!"); return false; }

	// create the filename to save it too
	QString filename = QFileDialog::getSaveFileName("./","RLE Object File (*.txt)", this,"export pattern","choose where you want to export" );
	
	if(filename.isEmpty()) { return false; }

	// open file for writing
	FILE* fOut = fopen(filename,"wt");
	if(!fOut) { return false; }

	// print header info
	fprintf(fOut,"# Automason Run Length Encoded Brick Pattern\n");
	fprintf(fOut,"# Total Bricks: %d\n", m_newBricks.size());
	fprintf(fOut,"# Bricks Per Row: %d\n", m_numBricksPerRow);

	// Cycle through all of the Bricks using the starting offset startOffset
	unsigned int startOffset = m_selectedStartBrick % m_numBricksPerRow;
	unsigned int endOffset = m_selectedEndBrick % m_numBricksPerRow;
	unsigned int exportRowWidth = endOffset - startOffset;
	unsigned int numRows = m_form->edit_NumBricks_Row->text().toInt();
	unsigned int i; // Brick Index used in m_newBricks array

	fprintf(fOut,"# Start Offset: %d\n", startOffset);
	fprintf(fOut,"# Num Rows: %d\n", numRows);

	// Accumulate (in the order of presidence):
	//  r - corner (1)
	unsigned int r = 0;
	//  c - cantilevered (2)
	unsigned int c = 0;
	//  s - solid (active) (3)
	unsigned int s = 0;
	//  h - hollow (inactive) (4)
	unsigned int h = 0;

	for(unsigned int curRow = 0; curRow < numRows; curRow++) // Cycle through the rows r
	{ 
		// Start a new Row
		fprintf(fOut,"\nRow %d\n    ", curRow+1); // Indent a little bit, start counting at Row 1
			
		// reset all accumulators
		r = 0;
		c = 0;
		s = 0;
		h = 0;

		for(int offset = 0; (offset < m_numBricksPerRow) && (offset < exportRowWidth); offset++) // Cycle within each row
		{
			// Get the Brick Index from the row, offset, and startOffset
			i = (curRow * m_numBricksPerRow) + ((offset + startOffset) % m_numBricksPerRow);

			if(i > m_newBricks.size()) // Buffer overflow!
			{ 
				QMessageBox::information(0,"Error", "Problem with Brick Index!  Try chaning RLE Origin."); 

				fprintf(fOut,"### ERROR using Brick Index: %d ###\n\n", i);
				fclose(fOut);
				return false; 
			}

			// Check Brick Type
			if(m_newBricks[i].isCorner())  // "Corner"
			{
					r++; // count 1 Corner Brick
					if(r == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, s, h, c, 0); // set "Cantilivered" to 0 to avoid printing it
						if((r+s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels
						c = 0;
						h = 0;
						s = 0;
					}
			}
			else if(m_newBricks[i].isCantilevered())  // "Cantilevered"
			{
					c++; // count 1 Cantilivered Brick
					if(c == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, s, h, 0, r); // set "Cantilivered" to 0 to avoid printing it
						if((r+s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels
						r = 0;
						h = 0;
						s = 0;
					}
			}
			else if(m_newBricks[i].isSolid()) // "Solid"
			{
					s++; // count 1 Solid Brick
					if(s == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, 0, h, c, r); // set "Hollow" to 0 to avoid printing it
						if((r+s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels
						h = 0;
						c = 0;
						r = 0;
					}
			} 
			else if(m_newBricks[i].isHollow())  // "Hollow"
			{
					h++; // count 1 Solid Brick
					if(h == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, s, 0, c, r); // set "Hollow" to 0 to avoid printing it
						if((r+s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels
						s = 0;
						c = 0;
						r = 0;
					}
			}

/*
			switch(m_newBricks[i].state) 
			{
				case INACTIVE: // INACTIVE Bricks (white) are "Solid"
					s++; // count 1 Solid Brick
					if(s == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, 0, h, c); // set "Hollow" to 0 to avoid printing it
						if((s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels

						h = 0;
						c = 0;
					}
					break;
				case ACTIVE: // ACTIVE Bricks (black) are "Hollow"
					h++; // count 1 Solid Brick
					if(h == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, s, 0, c); // set "Hollow" to 0 to avoid printing it
						if((s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels

						s = 0;
						c = 0;
					}
					break;
				// TO DO: replace with isCantilevered() check instead of two 'CASE' statements
				case CANTILEVERED_INACTIVE: // are "Cantilivered"
				case CANTILEVERED_ACTIVE: // are "Cantilivered"
					c++; // count 1 Cantilivered Brick
					if(c == 1) // Start a new run, print the last accumulation
					{
						printBrickCount(fOut, s, h, 0); // set "Cantilivered" to 0 to avoid printing it
						if((s+h+c) > 1) fprintf(fOut,", "); // spacer between Brick Labels

						h = 0;
						s = 0;
					}
					break;	
			}
*/
		} // End of Row
		printBrickCount(fOut, s, h, c, r);	// Print the last Brick Label for the Row	
	}

	// closer
	fprintf(fOut,"\n\n # End");
	fclose(fOut);

	// return
	return true;
}

// Sub-Routine to Print the Brick Count Label
void BBB_GLWidget::printBrickCount(FILE * fOut, unsigned int s, unsigned int h, unsigned int c, unsigned int r) 
{
	if((s+h+c+r) == 0) return; // print no label if nothing is accumulated
	// Print Short Labels
	if(s > 0) 
	{ fprintf(fOut, "%d Solid", s); }
	if(h > 0) 
	{ fprintf(fOut, "%d Hollow", h); }
	if(c > 0) 
	{ fprintf(fOut, "%d Cantilevered", c); }
	if(r > 0) 
	{ fprintf(fOut, "%d Corner", r); }
}

//----------------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------------

bool BBB_GLWidget::exportToMP3(void)
{ 
/*
    ISpVoice * pVoice = NULL;

    if (FAILED(CoInitialize(NULL)))
    {
        printf("Error to intiliaze COM");
        return false;
    }

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    if( SUCCEEDED( hr ) )
    {
        hr = pVoice->Speak(L"Hello World", 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }

    CoUninitialize();
*/
 	// if no path, bail
	if(!m_path) 
	{ QMessageBox::information(0,"Error","No pattern to export!"); return false; }

    bbb_Lively myThread(qApp, this);
    myThread.start();

	// Get Project Architect and Project Name
	bool ok1 = false;
    m_projectname = QInputDialog::getText(
            "Automason v 2.0", "Project Name:", QLineEdit::Normal,
            QString::null, &ok1, p );

    if ( ok1 && !m_projectname.isEmpty() ) {
        // user entered something and pressed OK
    } else {
        // user entered nothing or pressed Cancel
		m_projectname = "My Project";
    }
	
	bool ok2 = false;
    m_creator = QInputDialog::getText(
            "Automason v 2.0", "Enter Project Architect's Name:", QLineEdit::Normal,
            QString::null, &ok2, p );

	if ( ok2 && !m_creator.isEmpty() ) {
        // user entered something and pressed OK
    } else {
        // user entered nothing or pressed Cancel
		m_creator = "My Architect";
    }

	// Get the directory to export to	 
	QString dirpath = QFileDialog::getExistingDirectory(
                    "./",
                    this,
                    "get existing directory",
                    "Choose a directory to export pattern as a Playlist",
                    TRUE );
	// qDebug(dirpath);

	if(dirpath.isEmpty()) { 
		myThread.quit = true;
		myThread.wait();
		return false; 
	}

	// Open Library.xml file for writing
	QString filename = dirpath + "/Library.xml";
	FILE* fOut = fopen(filename,"wt");
	if(!fOut) { 
		myThread.quit = true;
		myThread.wait();
		return false; 
	}

	fprintf(fOut, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fOut, "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
	fprintf(fOut, "<plist version=\"1.0\">\n");
	fprintf(fOut, "<dict>\n");
	fprintf(fOut, "		<key>Major Version</key><integer>1</integer>\n");
	fprintf(fOut, "		<key>Minor Version</key><integer>1</integer>\n");
	fprintf(fOut, "		<key>Application Version</key><string>6.0.5</string>\n");
	fprintf(fOut, "		<key>Features</key><integer>1</integer>\n");
	fprintf(fOut, "		<key>Music Folder</key><string>file://localhost/%s/</string>\n", dirpath.ascii());
//	fprintf(fOut, "		<key>Library Persistent ID</key><string>C80CDED106580E94</string>\n");
	fprintf(fOut, "		<key>Tracks</key>\n");
	fprintf(fOut, "		<dict>\n");

	// Cycle through all of the Bricks using the starting offset startOffset
	unsigned int startOffset = m_selectedStartBrick % m_numBricksPerRow;
	unsigned int endOffset = m_selectedEndBrick % m_numBricksPerRow;
	unsigned int exportRowWidth = endOffset - startOffset;
	unsigned int numRows = m_form->edit_NumBricks_Row->text().toInt();
	unsigned int i; // Brick Index used in m_newBricks array

	qDebug("   startOffset: " + QString::number(startOffset));
	qDebug("   endOffset: " + QString::number(endOffset));
	qDebug("   exportRowWidth: " + QString::number(exportRowWidth));
	qDebug("   numRows: " + QString::number(numRows));

	// Accumulate (in the order of presidence):
	//  r - corner (1)
	unsigned int r = 0;
	//  c - cantilevered (2)
	unsigned int c = 0;
	//  s - solid (active) (3)
	unsigned int s = 0;
	//  h - hollow (inactive) (4)
	unsigned int h = 0;

	// Counts the number of runs/messages
	unsigned int key = 0;
	// stores the row-key relationship for playlist generation (a 2 dimensional dynamic array)
    vector< vector<int> > playlists(numRows, vector<int>(0,0));  

	// Start up a progress dialog
	QProgressDialog progress( "Exporting pattern to playlist...", "Cancel Export", numRows, this, "progress", TRUE );

	// Cancel the Lively Thread
	myThread.quit = true;

	for(unsigned int curRow = 0; curRow < numRows; curRow++) // Cycle through the rows r
	{ 
		progress.setProgress( curRow );
		qApp->processEvents();
		if ( progress.wasCanceled() )
			break;

		// Start a new Row / Add a New Playlist!
		// fprintf(fOut,"\nRow %d\n    ", curRow+1); // Indent a little bit, start counting at Row 1
			
		// reset all accumulators
		r = 0;
		c = 0;
		s = 0;
		h = 0;
		
		for(int offset = 0; (offset < m_numBricksPerRow) && (offset < exportRowWidth); offset++) // Cycle within each row
		{
			// Get the Brick Index from the row, offset, and startOffset
			i = (curRow * m_numBricksPerRow) + ((offset + startOffset) % m_numBricksPerRow);

			if(i > m_newBricks.size()) // Buffer overflow!
			{ 
				QMessageBox::information(0,"Error", "Problem with Brick Index!  Try changing Origin."); 

				fprintf(fOut,"### ERROR using Brick Index: %d ###\n\n", i);
				fclose(fOut);
				return false; 
			}

			// Check Brick Type
			if(m_newBricks[i].isCorner())  // "Corner"
			{
					r++; // count 1 Corner Brick
					if(r == 1) // Start a new run, print the last accumulation
					{
						speakBrickCount(dirpath, fOut, s, h, c, 0, key, curRow, playlists); // set "Cantilivered" to 0 to avoid printing it
						// if((r+s+h+c) > 1) fprintf(fOut, "\n"); // spacer between Brick Labels
						c = 0;
						h = 0;
						s = 0;
					}
			}
			else if(m_newBricks[i].isCantilevered())  // "Cantilevered"
			{
					c++; // count 1 Cantilivered Brick
					if(c == 1) // Start a new run, print the last accumulation
					{
						speakBrickCount(dirpath, fOut, s, h, 0, r, key, curRow, playlists); // set "Cantilivered" to 0 to avoid printing it
						// if((r+s+h+c) > 1) fprintf(fOut,"\n"); // spacer between Brick Labels
						r = 0;
						h = 0;
						s = 0;
					}
			}
			else if(m_newBricks[i].isSolid()) // "Solid"
			{
					s++; // count 1 Solid Brick
					if(s == 1) // Start a new run, print the last accumulation
					{
						speakBrickCount(dirpath, fOut, 0, h, c, r, key, curRow, playlists); // set "Hollow" to 0 to avoid printing it
						// if((r+s+h+c) > 1) fprintf(fOut,"\n"); // spacer between Brick Labels
						h = 0;
						c = 0;
						r = 0;
					}
			} 
			else if(m_newBricks[i].isHollow())  // "Hollow"
			{
					h++; // count 1 Solid Brick
					if(h == 1) // Start a new run, print the last accumulation
					{
						speakBrickCount(dirpath, fOut, s, 0, c, r, key, curRow, playlists); // set "Hollow" to 0 to avoid printing it
						// if((r+s+h+c) > 1) fprintf(fOut,"\n"); // spacer between Brick Labels
						s = 0;
						c = 0;
						r = 0;
					}
			}
		} // End of Row
		key++; // Increment the key (i.e. run count)
		speakBrickCount(dirpath, fOut, s, h, c, r, key, curRow, playlists);	// Print the last Brick Label for the Row	
	}
	progress.setProgress( numRows ); // done with the Progress Bar

	// End of the Track listings, start of the Playlists
	fprintf(fOut, "	</dict>\n");
	fprintf(fOut, "	<key>Playlists</key>\n");
	fprintf(fOut, "	<array>\n");

	// Create the Playlists by cycling through the rows and keys
	vector<int>::iterator iter_jj;
	for(int ii = 0; ii < numRows; ii++) {
//		fprintf(fOut, "\nRow: %d\n", ii);
		fprintf(fOut, "		<dict>\n");
		if(numRows > 99) {
			fprintf(fOut, "			<key>Name</key><string>Row %03d</string>\n", ii+1); // add one to the Row
			fprintf(fOut, "			<key>Playlist ID</key><integer>%03d</integer>\n", ii+1); 
		} else if(numRows > 9) {
			fprintf(fOut, "			<key>Name</key><string>Row %02d</string>\n", ii+1); // add one to the Row
			fprintf(fOut, "			<key>Playlist ID</key><integer>%02d</integer>\n", ii+1); 
		} else {
			fprintf(fOut, "			<key>Name</key><string>Row %d</string>\n", ii+1); // add one to the Row
			fprintf(fOut, "			<key>Playlist ID</key><integer>%d</integer>\n", ii+1); 
		}
		//fprintf(fOut, "			<key>Playlist Persistent ID</key><string>C48B5FCD382CDB6F</string>\n");
		fprintf(fOut, "			<key>All Items</key><true/>\n");
		fprintf(fOut, "			<key>Playlist Items</key>\n");
		fprintf(fOut, "			<array>\n");
		for(iter_jj = playlists[ii].begin(); iter_jj != playlists[ii].end(); iter_jj++)
		{		
			fprintf(fOut, "				<dict>\n");
			fprintf(fOut, "					<key>Track ID</key><integer>%d</integer>\n", *iter_jj);
			fprintf(fOut, "				</dict>\n");
//			fprintf(fOut, "  key: %d\n", *iter_jj);		
		}
		fprintf(fOut, "			</array>\n");
		fprintf(fOut, "		</dict>\n");
	}

	// closer
	// fprintf(fOut,"\n\n # End");
	fprintf(fOut, "		</array>\n");
	fprintf(fOut, "</dict>\n");
	fprintf(fOut, "</plist>\n");

	fclose(fOut);

	// return
	return true;
}

// Sub-Routine to Print the Brick Count Label
void BBB_GLWidget::speakBrickCount(QString dirpath, FILE * fOut, 
								   unsigned int s, unsigned int h, unsigned int c, unsigned int r, 
								   unsigned int & key, unsigned int rowCount, vector<vector<int>> & playlists) 
{

	if((s+h+c+r) == 0) return; // print no label if nothing is accumulated

	// Speak something
	QString speakThis = ""; // spoken text with mark-up
	QString speakThisRaw = ""; // no mark-up
	
	if(s > 0) 
	{	
		if(s > 1) 
		{
			speakThis = QString::number(s) + " Solid Blocks";
			speakThisRaw = QString::number(s) + " Solid Blocks";
		}
		else 
		{
			speakThis = QString::number(s) + " Solid Block";
			speakThisRaw = QString::number(s) + " Solid Block";
		}
	}
	if(h > 0) 
	{
		if(h > 1)
		{
			speakThis = QString::number(h) + " Hollow Blocks";
			speakThisRaw = QString::number(h) + " Hollow Blocks";
		}
		else
		{
			speakThis = QString::number(h) + " Hollow Block";
			speakThisRaw = QString::number(h) + " Hollow Block";
		}
	}
	if(c > 0) 
	{ 
		if(c > 1)
		{
			speakThis = QString::number(c) + " Cantilevered Blocks";
			speakThisRaw = QString::number(c) + " Cantilevered Blocks";
		}
		else
		{
			speakThis = QString::number(c) + " Cantilevered Block";
			speakThisRaw = QString::number(c) + " Cantilevered Block";
		}
	}
	if(r > 0) 
	{ 
		speakThis = "Corner Blocks, Refer to Design Plan Please";
		speakThisRaw = "Corner Blocks Refer to Design Plan Please";
	}

	key++; // increment the key count
	playlists[rowCount].push_back(key); // relates the row and the key

	// Export the .wav
	QString rowCountStr = QString::number(rowCount+1); // add one to rows
	QString keyStr = QString::number(key);
	QString filename = dirpath + "/" + "row" + rowCountStr + "key" + keyStr + ".wav";
	qDebug("Writing " + filename);

	// Wrap the Rate, add some buffers of silence
	speakThis = "<RATE SPEED=\"-5\"> <SILENCE  MSEC =\"1000\"/>" 
	 		  + speakThis 
			  + "<SILENCE  MSEC =\"1000\"/> </RATE>";

	text2wav(filename, speakThis);

	// Write the Library Entry for the "track"
	fprintf(fOut,"		<key>%d</key>\n", key);
	fprintf(fOut,"		<dict>\n");
	fprintf(fOut,"			<key>Track ID</key><integer>%d</integer>\n", key);
	fprintf(fOut,"			<key>Name</key><string>Run %03d: %s</string>\n", playlists[rowCount].size(), speakThisRaw.ascii());
	fprintf(fOut,"			<key>Artist</key><string>%s</string>\n", m_creator.ascii());
	fprintf(fOut,"			<key>Composer</key><string>Automason v 2.0</string>\n");
	fprintf(fOut,"			<key>Kind</key><string>WAV audio file</string>\n");
	fprintf(fOut,"			<key>Album</key><string>%s</string>\n", m_projectname.ascii());
	fprintf(fOut,"			<key>Genre</key><string>Computational Architecture</string>\n");
// 
//	fprintf(fOut,"			<key>Size</key><integer>3894374</integer>\n"); // in bytes
//	fprintf(fOut,"			<key>Total Time</key><integer>240162</integer>\n"); // in ms ? 

// Put the current date in?
//	fprintf(fOut,"			<key>Date Modified</key><date>2006-07-16T22:53:32Z</date>\n");
//	fprintf(fOut,"			<key>Date Added</key><date>2006-07-16T23:33:27Z</date>\n");
	fprintf(fOut,"			<key>Bit Rate</key><integer>352</integer>\n"); // .wav characteristics
	fprintf(fOut,"			<key>Sample Rate</key><integer>22050</integer>\n"); // .wav characteristics

//	fprintf(fOut,"			<key>Persistent ID</key><string>CE1D9517F1EFBCD0</string>\n"); // Internal iTunes thing?
	fprintf(fOut,"			<key>Track Type</key><string>File</string>\n");
	fprintf(fOut,"			<key>Location</key><string>file://localhost/%s</string>\n", filename.ascii());
	fprintf(fOut,"			<key>File Folder Count</key><integer>1</integer>\n"); // Leave to one time
	fprintf(fOut,"			<key>Library Folder Count</key><integer>1</integer>\n");  // Leave to one time
	fprintf(fOut,"		</dict>\n");
}

bool BBB_GLWidget::text2wav(QString filepath, QString txt) 
{
	HRESULT					hr = S_OK;
	CComPtr <ISpVoice>		cpVoice;
	CComPtr <ISpStream>		cpStream;
	CSpStreamFormat			cAudioFmt;

	//Create a SAPI Voice
	hr = cpVoice.CoCreateInstance( CLSID_SpVoice );

	//Set the audio format
    if(SUCCEEDED(hr))
	{
		hr = cAudioFmt.AssignFormat(SPSF_22kHz16BitMono);
	}
	
	//Call SPBindToFile, a SAPI helper method,  to bind the audio stream to the file
	if(SUCCEEDED(hr))
	{
//		hr = SPBindToFile( L"c:\\ttstemp.wav",  SPFM_CREATE_ALWAYS,
//			&cpStream, & cAudioFmt.FormatId(),cAudioFmt.WaveFormatExPtr() );
		hr = SPBindToFile( (const WCHAR * )filepath.ucs2(),  SPFM_CREATE_ALWAYS,
			&cpStream, & cAudioFmt.FormatId(),cAudioFmt.WaveFormatExPtr() );
	}
	
	//set the output to cpStream so that the output audio data will be stored in cpStream
    if(SUCCEEDED(hr))
	{
		hr = cpVoice->SetOutput( cpStream, TRUE );
	}

 	//Speak the text "hello world" synchronously
    if(SUCCEEDED(hr))
	{
//	  hr = cpVoice->Speak( L"Hello World",  SPF_DEFAULT, NULL );
//		qDebug(txt);
		hr = cpVoice->Speak((const WCHAR * )txt.ucs2(),  SPF_DEFAULT, NULL );
	}
	
	//close the stream
	if(SUCCEEDED(hr))
	{
		hr = cpStream->Close();
	}

	//Release the stream and voice object
	cpStream.Release ();
	cpVoice.Release();
	return true;
}
