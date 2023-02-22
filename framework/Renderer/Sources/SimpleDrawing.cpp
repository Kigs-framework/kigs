#include "PrecompiledHeaders.h"

#include "SimpleDrawing.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(SimpleDrawing)

void SimpleDrawing::GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
{
	if (mVertex)
	{
		pmin = mVertex[0];
		pmax = mVertex[0];
		for (int i=1;i<mVertexCount;i++)
		{
			if (mVertex[i].x<pmin.x) pmin.x=mVertex[i].x;
			if (mVertex[i].y<pmin.y) pmin.y=mVertex[i].y;
			if (mVertex[i].z<pmin.z) pmin.z=mVertex[i].z;
			if (mVertex[i].x>pmax.x) pmax.x=mVertex[i].x;
			if (mVertex[i].y>pmax.y) pmax.y=mVertex[i].y;
			if (mVertex[i].z>pmax.z) pmax.z=mVertex[i].z;
		}
	}
	else
	{
		pmin.Set(0.0f);
		pmax.Set(0.0f);
	}
}

SimpleDrawing::~SimpleDrawing()
{
	delete [] mVertex;
}

void SimpleDrawing::setColor(float R, float G, float B)
{
	mColor.Set(R,G,B);
}

void SimpleDrawing::setAlpha(float A)
{
	mAlpha = A;
}

void SimpleDrawing::setColor(float R, float G, float B, float A)
{
	mColor.Set(R, G, B);
	mAlpha = A;
}


void SimpleDrawing::setVertex(int index, Point3D Position)
{
	int i;
	if (mVertex==NULL)
	{
		mVertex = new Point3D[index+1];
		for (i=0;i<=index;i++) 
		{
			mVertex[i]=Position;
		}
		mVertexCount = index+1;
	}
	else
	{
		if (index<mVertexCount)
		{
			mVertex[index] = Position;
		}
		else
		{
			Point3D *Temp;
			Temp = new Point3D[index+1];
			memcpy(Temp,mVertex,mVertexCount*sizeof(Point3D));
			for (i=mVertexCount;i<=index;i++) Temp[i] = Position;
			delete [] mVertex;
			mVertex = Temp;
			mVertexCount = index+1;
		}
	}
}

void SimpleDrawing::setVertexArray(Point3D *Position, int Count)
{
	if (mVertex)
	{
		delete [] mVertex;
		mVertexCount = 0;
	}
	if (Count)
	{
		mVertex = new Point3D[Count];
		mVertexCount = Count;
		memcpy(mVertex,Position,mVertexCount*sizeof(Point3D));
	}
}

void SimpleDrawing::setToRectangle(const float &w, const float &h, bool bWire, char cNormal)
{
	if (mVertexCount!=4) 
	{
		delete [] mVertex;
		mVertex = new Point3D[4];
		mVertexCount = 4;
	}
	switch(cNormal)
	{
	case 'X':
			mVertex[0].Set(0.0f,-w*0.5f,-h*0.5f);
			mVertex[1].Set(0.0f, w*0.5f,-h*0.5f);
			mVertex[2].Set(0.0f, w*0.5f, h*0.5f);
			mVertex[3].Set(0.0f,-w*0.5f, h*0.5f);
		break;
	case 'Y':
			mVertex[0].Set(-h*0.5f,0.0f,-w*0.5f);
			mVertex[1].Set(-h*0.5f,0.0f, w*0.5f);
			mVertex[2].Set( h*0.5f,0.0f, w*0.5f);
			mVertex[3].Set( h*0.5f,0.0f,-w*0.5f);
		break;
	case 'Z':
			mVertex[0].Set(-w*0.5f,-h*0.5f,0.0f);
			mVertex[1].Set( w*0.5f,-h*0.5f,0.0f);
			mVertex[2].Set( w*0.5f, h*0.5f,0.0f);
			mVertex[3].Set(-w*0.5f, h*0.5f,0.0f);
		break;
	case 'x':
			mVertex[0].Set(0.0f,-w*0.5f,-h*0.5f);
			mVertex[3].Set(0.0f, w*0.5f,-h*0.5f);
			mVertex[2].Set(0.0f, w*0.5f, h*0.5f);
			mVertex[1].Set(0.0f,-w*0.5f, h*0.5f);
		break;
	case 'y':
			mVertex[0].Set(-h*0.5f,0.0f,-w*0.5f);
			mVertex[3].Set(-h*0.5f,0.0f, w*0.5f);
			mVertex[2].Set( h*0.5f,0.0f, w*0.5f);
			mVertex[1].Set( h*0.5f,0.0f,-w*0.5f);
		break;
	case 'z':
			mVertex[0].Set(-w*0.5f,-h*0.5f,0.0f);
			mVertex[3].Set( w*0.5f,-h*0.5f,0.0f);
			mVertex[2].Set( w*0.5f, h*0.5f,0.0f);
			mVertex[1].Set(-w*0.5f, h*0.5f,0.0f);
		break;
	}
	if (bWire) setValue("Mode",5);
	else setValue("Mode",2);
}

void SimpleDrawing::setToBox(const float &w, const float &h, const float &d, bool bWire)
{
	if (bWire)
	{
		if (mVertexCount!=17) 
		{
			delete [] mVertex;
			mVertex = new Point3D[17];
			mVertexCount = 17;
		}
		setValue("Mode",4);
		mVertex[0].Set(-w*0.5f,-h*0.5f,-d*0.5f);
			mVertex[8].Set(-w*0.5f,-h*0.5f,-d*0.5f);
			mVertex[14].Set(-w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[1].Set( w*0.5f,-h*0.5f,-d*0.5f);
			mVertex[13].Set( w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[2].Set( w*0.5f, h*0.5f,-d*0.5f);
			mVertex[10].Set( w*0.5f, h*0.5f,-d*0.5f);
		mVertex[3].Set(-w*0.5f, h*0.5f,-d*0.5f);
			mVertex[9].Set(-w*0.5f, h*0.5f,-d*0.5f);
		mVertex[4].Set(-w*0.5f, h*0.5f, d*0.5f);
			mVertex[16].Set(-w*0.5f, h*0.5f, d*0.5f);
		mVertex[5].Set( w*0.5f, h*0.5f, d*0.5f);
			mVertex[11].Set( w*0.5f, h*0.5f, d*0.5f);
		mVertex[6].Set( w*0.5f,-h*0.5f, d*0.5f);
			mVertex[12].Set( w*0.5f,-h*0.5f, d*0.5f);
		mVertex[7].Set(-w*0.5f,-h*0.5f, d*0.5f);
			mVertex[15].Set(-w*0.5f,-h*0.5f, d*0.5f);
	}
	else
	{
		if (mVertexCount!=20) 
		{
			delete [] mVertex;
			mVertex = new Point3D[20];
			mVertexCount = 20;
		}
		setValue("Mode",1);
		mVertex[0].Set(-w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[1].Set(-w*0.5f, h*0.5f,-d*0.5f);
		mVertex[2].Set( w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[3].Set( w*0.5f, h*0.5f,-d*0.5f);
		mVertex[4].Set( w*0.5f,-h*0.5f, d*0.5f);
		mVertex[5].Set( w*0.5f, h*0.5f, d*0.5f);
		mVertex[6].Set(-w*0.5f,-h*0.5f, d*0.5f);
		mVertex[7].Set(-w*0.5f, h*0.5f, d*0.5f);
		mVertex[8].Set(-w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[9].Set(-w*0.5f, h*0.5f,-d*0.5f);
		mVertex[10].Set(-w*0.5f, h*0.5f,-d*0.5f);
		mVertex[11].Set(-w*0.5f, h*0.5f, d*0.5f);
		mVertex[12].Set( w*0.5f, h*0.5f,-d*0.5f);
		mVertex[13].Set( w*0.5f, h*0.5f, d*0.5f);
		mVertex[14].Set( w*0.5f, h*0.5f, d*0.5f);
		mVertex[15].Set( w*0.5f,-h*0.5f, d*0.5f);
		mVertex[16].Set( w*0.5f,-h*0.5f, d*0.5f);
		mVertex[17].Set(-w*0.5f,-h*0.5f, d*0.5f);
		mVertex[18].Set( w*0.5f,-h*0.5f,-d*0.5f);
		mVertex[19].Set(-w*0.5f,-h*0.5f,-d*0.5f);
	}
}


void SimpleDrawing::setToDisk(const float &r, const int &precision, bool bWire,  char cNormal)
{
	int i;
	Point3D *V = NULL;

	if (bWire)
	{
		if (mVertexCount!=precision) 
		{
			setValue("Mode",5);
			delete [] mVertex;
			mVertex = new Point3D[precision];
			mVertexCount = precision;
			V = mVertex;
		}
	}
	else
	{
		if (mVertexCount!=precision) 
		{
			setValue("Mode",2);
			delete [] mVertex;
			mVertex = new Point3D[precision+2];
			mVertexCount = precision+2;
			mVertex[0].Set(0.0f,0.0f,0.0f);
			V = &mVertex[1];
		}
	}

	switch(cNormal)
	{
	case 'X':
		for (i=0;i<precision;i++)
			V[i].Set(0.0f,r*cosf((float)i*2.0f*3.14156f/(float)precision),r*sinf((float)i*2.0f*3.14156f/(float)precision));
		break;
	case 'Y':
		for (i=0;i<precision;i++)
			V[i].Set(r*sinf((float)i*2.0f*3.14156f/(float)precision),0.0f,r*cosf((float)i*2.0f*3.14156f/(float)precision));
		break;
	case 'Z':
	default:
		for (i=0;i<precision;i++)
			V[i].Set(r*cosf((float)i*2.0f*3.14156f/(float)precision),r*sinf((float)i*2.0f*3.14156f/(float)precision),0.0f);
		break;
	}

	if (!bWire) V[precision] = V[0];
}


