#include "PrecompiledHeaders.h"

#include "SimpleDrawing.h"

IMPLEMENT_CLASS_INFO(SimpleDrawing)

void SimpleDrawing::GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
{
	if (m_Vertex)
	{
		pmin = m_Vertex[0];
		pmax = m_Vertex[0];
		for (int i=1;i<m_VertexCount;i++)
		{
			if (m_Vertex[i].x<pmin.x) pmin.x=m_Vertex[i].x;
			if (m_Vertex[i].y<pmin.y) pmin.y=m_Vertex[i].y;
			if (m_Vertex[i].z<pmin.z) pmin.z=m_Vertex[i].z;
			if (m_Vertex[i].x>pmax.x) pmax.x=m_Vertex[i].x;
			if (m_Vertex[i].y>pmax.y) pmax.y=m_Vertex[i].y;
			if (m_Vertex[i].z>pmax.z) pmax.z=m_Vertex[i].z;
		}
	}
	else
	{
		pmin.Set(KFLOAT_CONST(0.0f));
		pmax.Set(KFLOAT_CONST(0.0f));
	}
}

SimpleDrawing::~SimpleDrawing()
{
	delete [] m_Vertex;
}

void SimpleDrawing::setColor(kfloat R, kfloat G, kfloat B)
{
	kfloat col[3];
	col[0] = R;
	col[1] = G;
	col[2] = B;
	m_Color.setArrayValue((kfloat*)col,3);
}

void SimpleDrawing::setAlpha(kfloat A)
{
	m_Alpha = A;
	//setValue("TransparencyFlag",m_Alpha!=KFLOAT_CONST(1.0));
}

void SimpleDrawing::setColor(kfloat R, kfloat G, kfloat B, kfloat A)
{
	kfloat col[3];
	col[0] = R;
	col[1] = G;
	col[2] = B;
	m_Color.setArrayValue((kfloat*)col,3);
	m_Alpha = A;
	//setValue("TransparencyFlag",m_Alpha!=KFLOAT_CONST(1.0));
}


void SimpleDrawing::setVertex(int index, Point3D Position)
{
	int i;
	if (m_Vertex==NULL)
	{
		m_Vertex = new Point3D[index+1];
		for (i=0;i<=index;i++) 
		{
			m_Vertex[i]=Position;
		}
		m_VertexCount = index+1;
	}
	else
	{
		if (index<m_VertexCount)
		{
			m_Vertex[index] = Position;
		}
		else
		{
			Point3D *Temp;
			Temp = new Point3D[index+1];
			memcpy(Temp,m_Vertex,m_VertexCount*sizeof(Point3D));
			for (i=m_VertexCount;i<=index;i++) Temp[i] = Position;
			delete [] m_Vertex;
			m_Vertex = Temp;
			m_VertexCount = index+1;
		}
	}
}

void SimpleDrawing::setVertexArray(Point3D *Position, int Count)
{
	if (m_Vertex)
	{
		delete [] m_Vertex;
		m_VertexCount = 0;
	}
	if (Count)
	{
		m_Vertex = new Point3D[Count];
		m_VertexCount = Count;
		memcpy(m_Vertex,Position,m_VertexCount*sizeof(Point3D));
	}
}

void SimpleDrawing::setToRectangle(const kfloat &w, const kfloat &h, bool bWire, char cNormal)
{
	if (m_VertexCount!=4) 
	{
		delete [] m_Vertex;
		m_Vertex = new Point3D[4];
		m_VertexCount = 4;
	}
	switch(cNormal)
	{
	case 'X':
			m_Vertex[0].Set(KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f));
			m_Vertex[1].Set(KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f));
			m_Vertex[2].Set(KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f));
			m_Vertex[3].Set(KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f));
		break;
	case 'Y':
			m_Vertex[0].Set(-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f));
			m_Vertex[1].Set(-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f));
			m_Vertex[2].Set( h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f));
			m_Vertex[3].Set( h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f));
		break;
	case 'Z':
			m_Vertex[0].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[1].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[2].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[3].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
		break;
	case 'x':
			m_Vertex[0].Set(KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f));
			m_Vertex[3].Set(KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f));
			m_Vertex[2].Set(KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f));
			m_Vertex[1].Set(KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f));
		break;
	case 'y':
			m_Vertex[0].Set(-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f));
			m_Vertex[3].Set(-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f));
			m_Vertex[2].Set( h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f), w*KFLOAT_CONST(0.5f));
			m_Vertex[1].Set( h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f),-w*KFLOAT_CONST(0.5f));
		break;
	case 'z':
			m_Vertex[0].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[3].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[2].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
			m_Vertex[1].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),KFLOAT_CONST(0.0f));
		break;
	}
	if (bWire) setValue(LABEL_TO_ID(Mode),5);
	else setValue(LABEL_TO_ID(Mode),2);
}

void SimpleDrawing::setToBox(const kfloat &w, const kfloat &h, const kfloat &d, bool bWire)
{
	if (bWire)
	{
		if (m_VertexCount!=17) 
		{
			delete [] m_Vertex;
			m_Vertex = new Point3D[17];
			m_VertexCount = 17;
		}
		setValue(LABEL_TO_ID(Mode),4);
		m_Vertex[0].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
			m_Vertex[8].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
			m_Vertex[14].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[1].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
			m_Vertex[13].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[2].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
			m_Vertex[10].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[3].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
			m_Vertex[9].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[4].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
			m_Vertex[16].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[5].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
			m_Vertex[11].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[6].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
			m_Vertex[12].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[7].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
			m_Vertex[15].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
	}
	else
	{
		if (m_VertexCount!=20) 
		{
			delete [] m_Vertex;
			m_Vertex = new Point3D[20];
			m_VertexCount = 20;
		}
		setValue(LABEL_TO_ID(Mode),1);
		m_Vertex[0].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[1].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[2].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[3].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[4].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[5].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[6].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[7].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[8].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[9].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[10].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[11].Set(-w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[12].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[13].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[14].Set( w*KFLOAT_CONST(0.5f), h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[15].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[16].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[17].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f), d*KFLOAT_CONST(0.5f));
		m_Vertex[18].Set( w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
		m_Vertex[19].Set(-w*KFLOAT_CONST(0.5f),-h*KFLOAT_CONST(0.5f),-d*KFLOAT_CONST(0.5f));
	}
}


void SimpleDrawing::setToDisk(const kfloat &r, const int &precision, bool bWire,  char cNormal)
{
	int i;
	Point3D *V = NULL;

	if (bWire)
	{
		if (m_VertexCount!=precision) 
		{
			setValue(LABEL_TO_ID(Mode),5);
			delete [] m_Vertex;
			m_Vertex = new Point3D[precision];
			m_VertexCount = precision;
			V = m_Vertex;
		}
	}
	else
	{
		if (m_VertexCount!=precision) 
		{
			setValue(LABEL_TO_ID(Mode),2);
			delete [] m_Vertex;
			m_Vertex = new Point3D[precision+2];
			m_VertexCount = precision+2;
			m_Vertex[0].Set(KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f));
			V = &m_Vertex[1];
		}
	}

	switch(cNormal)
	{
	case 'X':
		for (i=0;i<precision;i++)
			V[i].Set(KFLOAT_CONST(0.0f),r*cosf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision),r*sinf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision));
		break;
	case 'Y':
		for (i=0;i<precision;i++)
			V[i].Set(r*sinf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision),KFLOAT_CONST(0.0f),r*cosf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision));
		break;
	case 'Z':
	default:
		for (i=0;i<precision;i++)
			V[i].Set(r*cosf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision),r*sinf((kfloat)i*KFLOAT_CONST(2.0f)*KFLOAT_CONST(3.14156f)/(kfloat)precision),KFLOAT_CONST(0.0f));
		break;
	}

	if (!bWire) V[precision] = V[0];
}


