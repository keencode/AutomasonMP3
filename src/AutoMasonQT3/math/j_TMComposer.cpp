//======================================================================
#include "j_TMComposer.h"

//======================================================================
TMComposer::TMComposer(void)
{
	// set our defaults
	m_timestamp			= 0.f; // <-- get timestamp
	m_build_world   = true;
	m_build_inverse = true;
	m_parent				= 0;
}

//======================================================================
TMComposer::TMComposer(Matrix44f* tm,TMComposer* parent)
{
	// set our defaults	
	m_timestamp			= 0.f; // <-- get timestamp
	m_build_world   = true;
	m_build_inverse = true;
	m_local_tm			= *tm;
	m_parent				= parent;
}

//======================================================================
TMComposer::~TMComposer(void){}

//======================================================================
Matrix44f* TMComposer::GetWorldTM(void)
{
	// if it has a parent, then we need to update it every so often
	if(m_parent!=NULL)
	{
		// if the parents timestamp is newer than this one, then it
		// was updated recently, and thus this one needs updating.
		m_build_world=true;
		//if(m_timestamp < m_parent->m_timestamp) { m_build_world=true; }
	}

	// update our world transform if necessary
	if(m_build_world)
	{		
		// use the identity as the parent
		if(m_parent==0)	{	m_world_tm = m_local_tm; }
		else								
		{	
			// grab the parent's world transform
			m_world_tm  = *m_parent->GetWorldTM();
			m_world_tm *= m_local_tm; 
		}

		// set our time stamp
		m_timestamp = 0.f; // <-- get timestamp
		m_build_world		= false;
		m_build_inverse = true;
	}
		
	// return the transform
	return &m_world_tm;
}

//======================================================================
Matrix44f* TMComposer::GetTransform(void) { return &m_local_tm; }

//======================================================================
Matrix44f TMComposer::GetInverseTM(void)
{
	Matrix44f inverse_tm = *GetWorldTM();
	invert(inverse_tm);
	return inverse_tm;
}

//======================================================================
void TMComposer::SetTransform(const Matrix44f* tm)
{ m_local_tm = *tm; m_build_world = true; }

//======================================================================
Point3f TMComposer::GetWorldPosition(void)
{ return makeTrans< Point3f >( *GetWorldTM() ); }

//======================================================================
Point3f TMComposer::GetPosition(void)
{ return makeTrans< Point3f >( *GetTransform() ); }

//======================================================================
void TMComposer::SetPosition(const Point3f& pos) 
{ setTrans(m_local_tm,Vec3f(pos)); m_build_world=true; }

//======================================================================
bool TMComposer::HasParent(void) { return m_parent?true:false; }

//======================================================================
TMComposer* TMComposer::GetParent(void) { return m_parent; }

//======================================================================
void TMComposer::SetParent(TMComposer* parent,bool snap_to)
{
	// if it differs
	if(m_parent!=parent)
	{
		// if it's null, then we need to do extra work
		if(parent==0)
		{
			// if we want to snap to the parent
			if(snap_to)	{	identity(m_local_tm); 	 	  }
			else				{ m_local_tm = *GetWorldTM(); }

			// set our parent
			m_parent = 0;
		}
		else 
		{ 
			// if we want to snap to the parent
			if(snap_to)
			{
				// simply set the localTM to the identity
				identity(m_local_tm);
			}
			else
			{
				// get the current world transform and remove the
				// new parents world transform from it to get the
				// new local matrix. 
				// (bug-check) - transform caps
				m_local_tm = (parent->GetInverseTM()) * (*GetWorldTM());
			}

			// set our parent
			m_parent = parent; 
		}

		// flag an update of the world TM
		m_build_world = true;
	}
}

//======================================================================
void TMComposer::move(char axis,float dist)
{ 
	// find our index
	unsigned int index = 0;
	switch(axis)
	{
	case 'x': index = 0; break;
	case 'y': index = 1; break;
	case 'z': index = 2; break;
	default:  return;
	}
			
	// move along the correct vector
	m_local_tm[0][3] += dist * m_local_tm[0][index];
	m_local_tm[1][3] += dist * m_local_tm[1][index];
	m_local_tm[2][3] += dist * m_local_tm[2][index];
	m_build_world = true; 
}
		
//======================================================================
void TMComposer::move(Point3f& dist)
{
	move('x',dist[0]);
	move('y',dist[1]);
	move('z',dist[2]);
	m_build_world = true;
}		

//======================================================================
void TMComposer::MoveTo(Point3f& pos)
{	SetPosition(pos); m_build_world = true; }

//======================================================================
void TMComposer::Rotate(char axis,float angle)
{
	switch(axis)
	{
	case 'x':		
		{ EulerAngleXYZf rot(angle,0,0); 
			m_local_tm *= make<Matrix44f>(rot);
			m_build_world = true;	
		} break;
	case 'y':
		{ EulerAngleXYZf rot(0,angle,0); 
			m_local_tm *= make<Matrix44f>(rot);
			m_build_world = true;	
		} break;
	case 'z':
		{ EulerAngleXYZf rot(0,0,angle); 
			m_local_tm *= make<Matrix44f>(rot);
			m_build_world = true;	
		} break;
	}
	m_build_world = true;
}

//======================================================================
void TMComposer::Rotate(Point3f& angles)
{	
	EulerAngleXYZf rot(angles[0],angles[1],angles[2]); 
	m_local_tm *= make<Matrix44f>(rot);
	m_build_world = true;	
}

//======================================================================
void TMComposer::Identity(void)
{ identity(m_local_tm); m_build_world = true; }

//======================================================================
void TMComposer::LookAt(Point3f target,Vec3f up)
{	
	Point3f source = GetWorldPosition();
	Vec3f look = source - target; // target - source;
	normalize(look);

	Vec3f right = cross(look,up);
	normalize(right);

	Vec3f up_vec = cross(right,look);
	normalize(up_vec);

	m_local_tm[0][0] = right[0];
	m_local_tm[1][0] = right[1];
	m_local_tm[2][0] = right[2];
	m_local_tm[3][0] = 0.f;

	m_local_tm[0][1] = up_vec[0];
	m_local_tm[1][1] = up_vec[1];
	m_local_tm[2][1] = up_vec[2];
	m_local_tm[3][1] = 0.f;

	m_local_tm[0][2] = -look[0];
	m_local_tm[1][2] = -look[1];
	m_local_tm[2][2] = -look[2];
	m_local_tm[3][2] = 0.f;

	m_local_tm[0][3] = source[0];
	m_local_tm[1][3] = source[1];
	m_local_tm[2][3] = source[2];
	m_local_tm[3][3] = 1.f;
}

//======================================================================
