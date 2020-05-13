#ifndef _BASE3DIMPORTER_H_
#define _BASE3DIMPORTER_H_


#include "CoreModifiable.h"

// structure shared by importers
struct MeshConvertParams
{
	MeshConvertParams()
	{
		myAddVertexTangent = 0;
		myAddVertexNormal = false;
		myImportScaleFactor = 1.0f;
		myImportFlipAxis[0] = 1;
		myImportFlipAxis[1] = 2;
		myImportFlipAxis[2] = 3;
	}
	int								myAddVertexTangent;
	bool							myAddVertexNormal;
	kfloat							myImportScaleFactor;
	int								myImportFlipAxis[3];

	Matrix3x3	getFlipMatrix()
	{
		Matrix3x3 flipped;
		flipped.Clear();

		flipped.XAxis[abs(myImportFlipAxis[0]) - 1] = (myImportFlipAxis[0] < 0) ? -1 : 1;
		flipped.YAxis[abs(myImportFlipAxis[1]) - 1] = (myImportFlipAxis[1] < 0) ? -1 : 1;
		flipped.ZAxis[abs(myImportFlipAxis[2]) - 1] = (myImportFlipAxis[2] < 0) ? -1 : 1;

		return flipped;
	}

};



class Base3DImporter : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Base3DImporter, CoreModifiable, Renderer);
	DECLARE_INLINE_CONSTRUCTOR(Base3DImporter){}

	void	setParams(const MeshConvertParams& params)
	{
		myConvertParams = params;
	}

	MeshConvertParams	myConvertParams;
};



#endif //_BASE3DIMPORTER_H_