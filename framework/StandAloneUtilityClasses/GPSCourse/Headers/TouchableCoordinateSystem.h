#ifndef _TOUCHABLECOORDINATESYSTEM_H_
#define _TOUCHABLECOORDINATESYSTEM_H_

class MouseDevice;

#include "CoordinateSystem.h"

// ****************************************
// * TouchableCoordinateSystem class
// * --------------------------------------
/**
 * \file	TouchableCoordinateSystem.h
 * \class	TouchableCoordinateSystem
 * \ingroup SceneGraph
 * \brief	A TouchableCoordinateSystem is defined by a rotation, a translation and a scale in 3D space. Finger make him rotate
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		kfloat <strong>PositionX</strong> : position x
 * </li>
 * <li>
 *		kfloat <strong>PositionY</strong> : position y
 * </li>
 * <li>
 *		kfloat <strong>PositionZ</strong> : position z
 * </li>
 * <li>
 *		kfloat <strong>RotationX</strong> : rotation x
 * </li>
 * <li>
 *		kfloat <strong>RotationY</strong> : rotation y
 * </li>
 * <li>
 *		kfloat <strong>RotationZ</strong> : rotation z
 * </li>
 * <li>
 *		kfloat <strong>Scale</strong> : scale
 * </li>
 * </ul>
 */
// ****************************************
class TouchableCoordinateSystem : public Node3D
{
public:
	//! a coordinate system is a Node3D
	DECLARE_CLASS_INFO(TouchableCoordinateSystem, Node3D,SceneGraph)

	/**
	 * \brief	constructor
	 * \fn 		CoordinateSystem(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	TouchableCoordinateSystem(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	void Update(const Timer& timer, void* addParam) override;
	void	InitModifiable() override;

	virtual ~TouchableCoordinateSystem();

protected:
	bool leftClicked;
	MouseDevice* myMouse;

}; 

#endif //_TOUCHABLECOORDINATESYSTEM_H_
