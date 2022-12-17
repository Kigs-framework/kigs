#ifndef _TRAVPATH_H_
#define _TRAVPATH_H_

#include <vector>

class Drawable;
class SceneNode;


// ****************************************
// * TravPath class
// * --------------------------------------
/**
 * \file	TravPath.h
 * \class	TravPath
 * \ingroup SceneGraph
 * \brief  Class used to store pathes in the scenegraph and draw sorted objects following the right path.
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
	 * \file	TravPath.h
	 * \class	Way
	 * \brief	Manage one path part to a specific Drawable
	 */
	// ****************************************
	class Way
	{
	public:
		//! a way is a list of node index in the scenegraph 
		std::vector<int>	mWay;
		//! the targeted drawable
		Drawable*			mDrawable;
	};

	/**
	 * \brief	add a way to the path
	 * \fn 		inline void Push(const Way& way)
	 * \param	way : way to add
	 */
	inline void Push(const Way& way)	
	{
		mPath.push_back(way);
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
	 * \fn 		PathNeed ComputeNeeds(std::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	what to do to go to the next mEntry from the current position in path
	 */
	PathNeed ComputeNeeds(std::vector<int>& currentway);

	/**
	 * \brief	check if the level is the same
	 * \fn 		bool IsNeedSameLevel(const std::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	TRUE if next drawable is at same level in scenegraph (no push or pop needed)
	 */
	bool IsNeedSameLevel(const std::vector<int>& currentway);

	/**
	 * \brief	get the index of the next way 
	 * \fn 		int GotoWay(std::vector<int>& currentway);
	 * \param	currentway : the current way
	 * \return	needed "jump" in index to go to the next drawable	
	 */
	int GotoWay(std::vector<int>& currentway);

	/**
	 * \brief	pop current drawable and compute how many pops are needed to go to next one
	 * \fn 		void PopWay(std::vector<int>& currentway);
	 * \param	currentway : the current way
	 */
	void PopWay(std::vector<int>& currentway);

	/**
	 * \brief	get the current drawable in path
	 * \fn 		Drawable* GetDrawable();
	 * \return	the current drawable in path
	 */
	Drawable* GetDrawable();

protected:
		//! the path
	std::vector<Way>			mPath;
	//! ?
	int							mNeedPop;
	//! current index in the path
	int							mCurrentPathIndex;

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
		mPath.clear();
		mNeedPop=0;
		mCurrentPathIndex=0;
	}

	/**
	 * \brief	get the TravPath size
	 * \fn 		inline int TravPath::GetSize()
	 * \return	the TravPath size
	 */
	inline int TravPath::GetSize(){return (int)mPath.size()-mCurrentPathIndex;}

	/**
	 * \brief	get the index of the next way
	 * \fn 		inline int TravPath::GotoWay(std::vector<int>& currentway)
	 * \param	currentway : the current way
	 * \return	the index of the next way
	 */
	inline int TravPath::GotoWay(std::vector<int>& currentway){return mPath[(unsigned long)mCurrentPathIndex].mWay[currentway.size()];}

	/**
	 * \brief	get the drawable
	 * \fn 		inline Drawable* TravPath::GetDrawable()
	 * \return	the drawable
	 */
	inline Drawable* TravPath::GetDrawable(){return mPath[(unsigned long)mCurrentPathIndex].mDrawable;}
#endif //_TRAVPATH_H_
