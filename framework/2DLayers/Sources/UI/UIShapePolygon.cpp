// #### 2DLayers Include 
#include "UI/UIShapePolygon.h"
// #### Core Include
#include "Core.h"
// #### Renderer Include
#include "ModuleRenderer.h"
#include "Texture.h"
#include "UI/UITexturedItem.h"

using namespace Kigs::Draw2D;

//#//////////////////////////////
//#		UIShapePolygon
//#//////////////////////////////
IMPLEMENT_CLASS_INFO(UIShapePolygon)

IMPLEMENT_CONSTRUCTOR(UIShapePolygon)
{
	
}

void UIShapePolygon::InitModifiable()
{
	ParentClassType::InitModifiable();
	setOwnerNotification("Vertices", true);
	triangulatePolygon();
}


void UIShapePolygon::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == KigsID("Vertices")._id )
	{
		triangulatePolygon();
		for (auto p : GetParents())
		{
			if (p->isSubType(Node2D::mClassID))
			{
				p->setUserFlag(Node2D::Node2D_NeedVerticeInfoUpdate);
			}
		}
	}
	ParentClassType::NotifyUpdate(labelid);
}

void	UIShapePolygon::triangulatePolygon()
{
	CoreItemSP poly = mVertices.ref().SharedFromThis();
	size_t vcount = poly->size();
	if (vcount >2)
	{
		if (isConvex())
		{
			mTriangulatedPoly.resize((vcount - 2) * 3);

			int tindex = 0;
			for (size_t vi = 1; vi < vcount - 1; vi++)
			{
				mTriangulatedPoly[tindex] = (const v2f&)poly[0];
				mTriangulatedPoly[tindex + 1] = (const v2f&)poly[(int)vi];
				mTriangulatedPoly[tindex + 2] = (const v2f&)poly[(int)vi+1];
				tindex += 3;
			}
		}
		else
		{
			//TODO
		}
	}
}


void UIShapePolygon::SetTexUV(UIItem* item, UIVerticesInfo* aQI)
{
	if (!mTriangulatedPoly.size())
	{
		return;
	}
	UITexturedItem* texturedLocalThis = static_cast<UITexturedItem*>(item);

	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	std::vector<v2f>	transformed = mTriangulatedPoly;
	texturedLocalThis->TransformUV(transformed.data(), (int)transformed.size());

	size_t j = 0;
	for (const auto& p : transformed)
	{
		buf[j++].setTexUV(p);
	}

}
void UIShapePolygon::SetVertexArray(UIItem* item, UIVerticesInfo* aQI)
{
	if (!mTriangulatedPoly.size())
	{
		return;
	}	

	v2f realsize = item->GetSize();
	aQI->Resize((unsigned int)mTriangulatedPoly.size()); 
	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	std::vector<v2f>	transformed = mTriangulatedPoly;

	for (auto& p : transformed)
	{
		p*= realsize;
	}

	item->TransformPoints(transformed.data(), (int)transformed.size());

	size_t j = 0;
	for (const auto& p : transformed)
	{
		buf[j++].setVertex(p);
	}

}
