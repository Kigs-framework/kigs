#ifndef _TRAVPATH_H_
#define _TRAVPATH_H_

#include "kstlvector.h"

class Drawable;
class SceneNode;

/*! \defgroup InternalTravPath TravPath Internal
 * \ingroup SceneGraph
*/

// ****************************************
// * TravPath class
// * --------------------------------------
/**
 * \file	TravPath.h
 * \class	TravPath
 * \ingroup SceneGraph
 * \brief	a class used to store pathes in the scenegraph and draw sorted objects following this path 
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class TravPath 
{
public:

	//! used to know where to go in the path
	enum	PathNeed
	{
		push=1,
		pop=2,
		stay=4,
		all=7
	};

	/**
	 * \brief	constructor
	 * \fn 		TravPath();
	 */
	TravPath();

	// ****************************************
	// * Way class
	// * --------------------------------------
	/**
	 * \file	Way.h
	 * \class	Way
	 * \brief	
	 * \ingroup InternalTravPath
	 * \author	ukn
	 * \version ukn
	 * \date	ukn
	 */
	// ****************************************
	class Way
	{
	public:
		//! a way is a list of node index in the scenegraph 
		kstl::vector<int>	myWay;
		//! the targeted drawable
		Drawable*			myDrawable;
	};

	/**
	 * \brief	add a way to the path
	 * \fn 		inline void Push(const Way& way)
	 * \param	way : way to add
	 */
	inline void Push(const Way& way)	
	{
		myPath.push_back(way);
	}

	/**
	 * \brief	destructor
	 * \fn 		~TravPath();
	 */
	virtual ~TravPath();  

	/**
	 * \brief	clear all entries in path
	 * \fn 		~TravPath();
	 */
	void Clear();

	/**
	 * \brief	destructor
	 * \fn 		int GetSize();
	 * \return	the remaining entries in path	
	 */
	int GetSize();
	
	/**
	 * \brief	compute what to do to go to the next mEntry
	 * \fn 		PathNeed ComputeNeeds(kstl::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	what to do to go to the next mEntry from the current position in path
	 */
	PathNeed ComputeNeeds(kstl::vector<int>& currentway);

	/**
	 * \brief	check if the level is the same
	 * \fn 		bool IsNeedSameLevel(const kstl::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	TRUE if next drawable is at same level in scenegraph (no push or pop needed)
	 */
	bool IsNeedSameLevel(const kstl::vector<int>& currentway);

	/**
	 * \brief	get the index of the next way 
	 * \fn 		int GotoWay(kstl::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	needed "jump" in index to go to the next drawable	
	 */
	int GotoWay(kstl::vector<int>& currentway);

	/**
	 * \brief	pop current drawable and compute how many pops are needed to go to next one
	 * \fn 		void PopWay(kstl::vector<int>& currentway);
	 * \param	currentway : the current way
	 */
	void PopWay(kstl::vector<int>& currentway);

	/**
	 * \brief	get the current drawable in path
	 * \fn 		Drawable* GetDrawable();
	 * \return	the current drawable in path
	 */
	Drawable* GetDrawable();

protected:
		//! the path
	kstl::vector<Way>			myPath;
	//! ?
	int							myNeedPop;
	//! current index in the path
	int							myCurrentPathIndex;

}; 

/*
 inline code
*/
	/**
	 * \brief	clear the TravPath
	 * \fn 		inline void TravPath::Clear()
	 */
	inline void TravPath::Clear()
	{
		myPath.clear();
		myNeedPop=0;
		myCurrentPathIndex=0;
	}

	/**
	 * \brief	get the TravPath size
	 * \fn 		inline int TravPath::GetSize()
	 * \return	the TravPath size
	 */
	inline int TravPath::GetSize(){return (int)myPath.size()-myCurrentPathIndex;}

	/**
	 * \brief	get the index of the next way
	 * \fn 		inline int TravPath::GotoWay(kstl::vector<int>& currentway)
	 * \param	currentway : the current way
	 * \return	the index of the next way
	 */
	inline int TravPath::GotoWay(kstl::vector<int>& currentway){return myPath[(unsigned long)myCurrentPathIndex].myWay[currentway.size()];}

	/**
	 * \brief	get the drawable
	 * \fn 		inline Drawable* TravPath::GetDrawable()
	 * \return	the drawable
	 */
	inline Drawable* TravPath::GetDrawable(){return myPath[(unsigned long)myCurrentPathIndex].myDrawable;}
#endif //_TRAVPATH_H_
