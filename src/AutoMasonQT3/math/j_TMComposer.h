#ifndef E_TM_COMPOSER_H
#define E_TM_COMPOSER_H
	//======================================================================
	#include "j_Math.h"

	//======================================================================
	// defines transforms in a hierarchical fashion allowing for specific	//
	// updating of a particular transform without the need to update all	//
	// transforms all the time. uses either a timestamp or a flag to			//
	// denote when an update is necessary.																//
	//======================================================================
	// todo: additional routines to that accept standard floats instead   //
	//			 of requiring Point3f.																				//
	//======================================================================
	class TMComposer
	{
	public:
		////////////////////////////////////////////////////////
		// basic constructor.																	//
		////////////////////////////////////////////////////////
		TMComposer(void);
		TMComposer(Matrix44f* tm, TMComposer* parent);

		////////////////////////////////////////////////////////
		~TMComposer(void);

		////////////////////////////////////////////////////////
		// obtain the world transform matrix. will traverse		//
		// up the tree to ensure that all parents are valid.	//
		////////////////////////////////////////////////////////
		Matrix44f*	GetWorldTM(void);

		////////////////////////////////////////////////////////
		// get the inverse of the world matrix. this equates	//
		// to the world->object matrix.												//
		////////////////////////////////////////////////////////
		Matrix44f	GetInverseTM(void);

		////////////////////////////////////////////////////////
		// checks to see whether this composer has a parent		//
		// or is solo.																				//
		////////////////////////////////////////////////////////
		bool HasParent(void);

		////////////////////////////////////////////////////////
		// returns the parent composer. if one doesn't exist	//
		// NULL is returned.																	//
		////////////////////////////////////////////////////////
		TMComposer*	GetParent(void);

		////////////////////////////////////////////////////////
		// sets the parent composer. if NULL is passed as the //
		// composer, then the TMComposer loses its parent and //
		// becomes parent to the world root. if a new parent  //
		// is passed, then it detaches the TMComposer from		//
		// its current parent and attachs it to the new one.	//
		// 'snapTo' is used to handle the method in which a		//
		// TMComposer is linked to its parent. if snapTo is		//
		// true, then it the TMComposer is positioned and			//
		// oriented on the parents origin. if false, the			//
		// TMComposer maintains its current world position,		//
		// just with a new parent.														//
		////////////////////////////////////////////////////////
		void SetParent(TMComposer* parent,const bool snapTo);

		////////////////////////////////////////////////////////
		// set the local transform matrix. will flag the			//
		// world matrix for update.														//
		////////////////////////////////////////////////////////
		void SetTransform(const Matrix44f* tm);

		////////////////////////////////////////////////////////
		// return the local matrix														//
		////////////////////////////////////////////////////////
		Matrix44f* GetTransform(void);

		////////////////////////////////////////////////////////
		// get the absolute world position. good for quickly  //
		// finding out where an object is for culling or			//
		// collision.																					//
		////////////////////////////////////////////////////////
		Point3f	GetWorldPosition(void);

		////////////////////////////////////////////////////////
		// get/set the position relative to its parent. if		//
		// its parent is NULL, then this equates to the world	//
		// position.																					//
		////////////////////////////////////////////////////////
		Point3f	GetPosition(void);
		void		SetPosition(const Point3f& pos);

		////////////////////////////////////////////////////////
		// translates the local matrix the desired amount.		//
		// this should be used over direct manipulation of		//
		// the matrices to ensure the world matrix gets				//
		// updated correctly. can pass in a particular axis &	//
		// distance or a Point3 with the distances along each	//
		// axis.																							//
		////////////////////////////////////////////////////////
		void	move(char axis,float dist);
		
		////////////////////////////////////////////////////////
		// move a certain distance along each axis						//
		////////////////////////////////////////////////////////
		void	move(Point3f& dist);

		////////////////////////////////////////////////////////
		// move to a particular location											//
		////////////////////////////////////////////////////////
		void MoveTo(Point3f& pos);

		////////////////////////////////////////////////////////
		// similar to above just with rotation. this rotates	//
		// the local matrix around its coordinate axis, not		//
		// the world axis. it's done this way to avoid gimbal	//
		// lock and other inconsistencies that occur when			//
		// transforming in world space.												//
		////////////////////////////////////////////////////////
		void	Rotate(char axis,float angle);
		void	Rotate(Point3f& angles);

		////////////////////////////////////////////////////////
		void	Identity(void);

		////////////////////////////////////////////////////////
		void	LookAt(Point3f target,Vec3f up_vec);

	private:
		////////////////////////////////////////////////////////
		Matrix44f			m_world_tm;
		Matrix44f			m_local_tm;
		TMComposer*		m_parent;
		bool					m_build_world;
		bool					m_build_inverse;
		float					m_timestamp;
	};
	//======================================================================

#endif