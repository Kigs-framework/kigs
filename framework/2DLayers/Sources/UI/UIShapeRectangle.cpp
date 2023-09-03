// #### 2DLayers Include 
#include "UI/UIShapeRectangle.h"
// #### Core Include
#include "Core.h"
// #### Renderer Include
#include "ModuleRenderer.h"
#include "Texture.h"
#include "UI/UITexturedItem.h"

using namespace Kigs::Draw2D;
//#//////////////////////////////
//#		UIShapeRectangle
//#//////////////////////////////
IMPLEMENT_CLASS_INFO(UIShapeRectangle)

IMPLEMENT_CONSTRUCTOR(UIShapeRectangle)
{
	
}

void UIShapeRectangle::SetTexUV(UIItem* item, UIVerticesInfo* aQI)
{
	UITexturedItem* titem = static_cast<UITexturedItem*>(item);
	aQI->Flag |= UIVerticesInfo_Texture;

	// TODO Steph
	//SP<TextureHandler> texture = titem->GetTexture();
	/*bool is_bgr = false;
	if (texture->getValue("IsBGR", is_bgr) && is_bgr)
	{
		aQI->Flag |= UIVerticesInfo_BGRTexture;
	}*/

	/*v2f uv_min = titem->getUVMin();
	v2f uv_max = titem->getUVMax();


	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	// triangle strip order
	buf[0].setTexUV(uv_min.x , uv_min.y );
	buf[1].setTexUV(uv_min.x , uv_max.y );
	buf[3].setTexUV(uv_max.x , uv_max.y );
	buf[2].setTexUV(uv_max.x , uv_min.y );*/
}
void UIShapeRectangle::SetVertexArray(UIItem* item, UIVerticesInfo* aQI)
{
	aQI->Flag |= UIVerticesInfo_Vertex;

	aQI->Resize(4);
	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	v2f pt[4];
	item->GetTransformedPoints(pt);
	// triangle strip order
	buf[0].setVertex(pt[0].x, pt[0].y);
	buf[1].setVertex(pt[1].x, pt[1].y);
	buf[3].setVertex(pt[2].x, pt[2].y);
	buf[2].setVertex(pt[3].x, pt[3].y);
}
