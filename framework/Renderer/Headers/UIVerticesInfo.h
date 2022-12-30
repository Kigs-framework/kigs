#pragma once

#include "RendererDefines.h"

namespace Kigs
{

	namespace Draw2D
	{
#define UIVerticesInfo_Vertex  1
#define UIVerticesInfo_Color   2
#define UIVerticesInfo_Texture 4
#define UIVerticesInfo_UseModelMatrix 8
#define UIVerticesInfo_BGRTexture 16
#define UIVerticesInfo_TriangleStrip 32

		class VInfo
		{
		public:
			virtual int getSize() = 0;
			virtual void getVertexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) = 0;
			virtual void getColorInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) = 0;
			virtual void getTexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) = 0;
		};

		class VInfo2D : public VInfo
		{
		public:
			virtual int getSize() { return 20; } // 2f, 4c, 2f
			virtual void getVertexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 0; comp = 2; size = 8; }
			virtual void getColorInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 8; comp = 4; size = 4; }
			virtual void getTexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 12; comp = 2; size = 8; }

			class Data
			{
				float v[2];
				unsigned char  c[4];
				float t[2];

			public:
				inline void setValue(float vx, float vy, unsigned char cr, unsigned char  cg, unsigned char  cb, unsigned char  ca, float tx, float ty)
				{
					v[0] = vx;			v[1] = vy;
					c[0] = cr;			c[1] = cg;			c[2] = cb;			c[3] = ca;
					t[0] = tx;			t[1] = ty;
				}

				inline void setVertex(float vx, float vy)
				{
					v[0] = vx;			v[1] = vy;
				}

				inline void setVertex(v2f pos)
				{
					v[0] = pos.x;			v[1] = pos.y;
				}
				//set color as u8 in [0,255] range
				inline void setColor(unsigned char cr, unsigned char  cg, unsigned char  cb, unsigned char  ca)
				{
					c[0] = cr;			c[1] = cg;			c[2] = cb;			c[3] = ca;
				}
				// set color as float in [0,1] range
				inline void setColorF(float cr, float  cg, float  cb, float  ca)
				{
					c[0] = (u8)(cr * 255.0f);			c[1] = (u8)(cg * 255.0f);			c[2] = (u8)(cb * 255.0f);			c[3] = (u8)(ca * 255.0f);
				}

				inline void setTexUV(float tx, float ty)
				{
					t[0] = tx;			t[1] = ty;
				}

				inline void setTexUV(v2f tex)
				{
					t[0] = tex.x;			t[1] = tex.y;
				}
			};
		};

		class VInfo3D : public VInfo
		{
		public:
			virtual int getSize() { return 24; } // 3f, 4c, 2f
			virtual void getVertexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 0; comp = 3; size = 12; }
			virtual void getColorInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 12; comp = 4; size = 4; }
			virtual void getTexInfo(unsigned int& stride, unsigned int& comp, unsigned int& size) { stride = 16; comp = 2; size = 8; }

			class Data
			{
				float v[3];
				unsigned char  c[4];
				float t[2];

			public:
				inline void setValue(float vx, float vy, float vz, unsigned char cr, unsigned char  cg, unsigned char  cb, unsigned char  ca, float tx, float ty)
				{
					v[0] = vx;			v[1] = vy;			v[2] = vz;
					c[0] = cr;			c[1] = cg;			c[2] = cb;			c[3] = ca;
					t[0] = tx;			t[1] = ty;
				}

				inline void setVertex(float vx, float vy, float vz)
				{
					v[0] = vx;			v[1] = vy;			v[2] = vz;
				}

				inline void setColor(unsigned char cr, unsigned char  cg, unsigned char  cb, unsigned char  ca)
				{
					c[0] = cr;			c[1] = cg;			c[2] = cb;			c[3] = ca;
				}

				inline void setTexUV(float tx, float ty)
				{
					t[0] = tx;			t[1] = ty;
				}
			};
		};

		class UIVerticesInfo
		{
		public:
			UIVerticesInfo() = default;

			UIVerticesInfo(VInfo* structInfo, unsigned int vCount = 4) :Flag(0)
			{
				// retreive VInfo data
				Offset = structInfo->getSize();
				structInfo->getVertexInfo(vertexStride, vertexComp, vertexSize);
				structInfo->getColorInfo(colorStride, colorComp, colorSize);
				structInfo->getTexInfo(texStride, texComp, texSize);
				Resize(vCount);
			}

			//! warning, resize reset previous data
			void	Resize(unsigned int vCount)
			{
				data.resize(vCount * Offset);
				vertexCount = vCount;
			}

			void* Buffer() const
			{
				return (void*)data.data();
			}



			void SetFlag(int aFlag) { Flag = aFlag; }

			std::vector<char> data;

			unsigned int Flag;
			unsigned int Offset;
			unsigned int vertexCount;

			unsigned int vertexStride;
			unsigned int vertexComp;
			unsigned int vertexSize;

			unsigned int colorStride;
			unsigned int colorComp;
			unsigned int colorSize;

			unsigned int texStride;
			unsigned int texComp;
			unsigned int texSize;
		};
	}
}
