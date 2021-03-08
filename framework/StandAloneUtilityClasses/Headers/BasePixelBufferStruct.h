#ifndef _BASEPIXELBUFFERSTRUCT_H_
#define _BASEPIXELBUFFERSTRUCT_H_

#include "TecLibs\Tec3D.h"
#include "CoreRawBuffer.h"
#include "TinyImage.h"
#include "PNGClass.h"
#include <memory>
#include <algorithm>

#pragma pack(push, 1)
struct Pixel24
{
	Pixel24() = default;
	explicit Pixel24(const Point3DI& other) : r(other.x), g(other.y), b(other.z) {}
	Pixel24(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
	union
	{
		unsigned char r;
		unsigned char y;
	};
	union
	{
		unsigned char g;
		unsigned char u;
	};
	union
	{
		unsigned char b;
		unsigned char v;
	};
	
	bool operator==(const Pixel24& other) const
	{
		return r == other.r && g == other.g && b == other.b;
	}
	Pixel24 Negative() const
	{
		return Pixel24(255 - r, 255 - g, 255 - b);
	}
	
	kfloat getHue() const
	{
		kfloat hue;
		
		kfloat R = r / 255.0f;
		kfloat G = g / 255.0f;
		kfloat B = b / 255.0f;
		
		
		kfloat maxi = MAX(R, MAX(G, B));
		kfloat mini = MIN(R, MIN(G, B));
		
		if (mini == maxi) return 0.0f;
		
		if (maxi == R)
		{
			hue = (G - B) / (maxi - mini);
		}
		else if (maxi == G)
		{
			hue = 2.f + (B - R) / (maxi - mini);
		}
		else
		{
			hue = 4.f + (R - G) / (maxi - mini);
		}
		hue *= 60.0f;
		if (hue < 0) hue += 360.0f;
		
		
		return hue;
	}
	
	explicit operator Point3D() const { return Point3D{(Float)r,(Float)g,(Float)b}; }
	explicit operator Point3DI() const { return Point3DI(r,g,b); }
	
	Pixel24 operator+=(const Pixel24& other)
	{
		r = std::max(std::min(r + other.r, 255), 0);
		g = std::max(std::min(g + other.g, 255), 0);
		b = std::max(std::min(b + other.b, 255), 0);
		return *this;
	}
	
	static Pixel24	blend(const Pixel24& p1, const Pixel24& p2, float t)
	{
		Pixel24 result;
		
		float V = (float)p1.r*(1.0f - t) + (float)p2.r*t;
		result.r = (unsigned char)V;
		V = (float)p1.g*(1.0f - t) + (float)p2.g*t;
		result.g = (unsigned char)V;
		V = (float)p1.b*(1.0f - t) + (float)p2.b*t;
		result.b = (unsigned char)V;
		
		return result;
		
	}
};



static_assert(sizeof(Pixel24) == 3, "Pixel24 struct is not 3 byte long");
#pragma pack(pop)


template<typename T1, typename T2>
inline bool IsInBounds(const T1& pt, const T2& size)
{
	return pt.x >= 0 && pt.y >= 0 && pt.x < size.x && pt.y < size.y;
}


// ****************************************
// * PixelBuffer class
// * --------------------------------------
/**
* \file	BasePixelBufferStruct.h
* \class	PixelBuffer
* \ingroup Camera
* \brief Shared interface between all pixel buffer types (allows for generic PixelBuffer pointers)
*/
// ****************************************

class PixelBuffer
{
	public:
	
	// Use copyTo instead
	PixelBuffer& operator=(const PixelBuffer& other) = delete;
	
	
	virtual ~PixelBuffer() {};
	virtual int getLineSizeInByte() const = 0;
	virtual int getXPaddingInByte() const = 0;
	virtual int getPixelSizeInByte() const = 0;
	virtual int getComponentCount() const = 0;
	virtual int getComponentSize() const = 0;
	virtual Point2DI getSizeInPixel() const = 0;
	virtual unsigned char* char_buffer() = 0;
	virtual void resize(int sx, int sy) = 0;
	void resize(Point2DI size) { resize(size.x, size.y); }
	
	template<typename ptr_type>
		inline ptr_type* movePointer(ptr_type* ptr, int x, int y) const
	{
		return (ptr_type*)((unsigned char*)(ptr + x) + y*getLineSizeInByte());
	}
	
	template<typename ptr_type>
		inline ptr_type* getPixel(int x, int y)
	{
		return movePointer(buffer<ptr_type>(), x, y);
	}
	
	template<typename ptr_type>
		inline void advancePointer(ptr_type*& ptr, int n = 1) const
	{
		ptr = movePointer(ptr, 0, n);
	}
	
	template<typename ptr_type = unsigned char>
		ptr_type* buffer()
	{
		return reinterpret_cast<ptr_type*>(char_buffer());
	}
	
	void getSection(Point2DI startpos, Point2DI endpos, PixelBuffer& outputBuffer)
	{
		Point2DI real_start(startpos.x < endpos.x ? startpos.x : endpos.x, startpos.y < endpos.y ? startpos.y : endpos.y);
		Point2DI real_end(startpos.x > endpos.x ? startpos.x : endpos.x, startpos.y > endpos.y ? startpos.y : endpos.y);
		Point2DI size(real_end.x - real_start.x, real_end.y - real_start.y);
		
		outputBuffer.resize(size.x, size.y);
		
		int linesize = getLineSizeInByte();
		int out_linesize = outputBuffer.getLineSizeInByte();
		
		int copysize = getPixelSizeInByte()*size.x;
		
		unsigned char* src_ptr = buffer() + linesize*real_start.y + real_start.x*getPixelSizeInByte();
		unsigned char* dest_ptr = outputBuffer.buffer();
		
		for (int y = 0; y < size.y; ++y)
		{
			memcpy(dest_ptr, src_ptr, copysize);
			src_ptr += linesize;
			dest_ptr += out_linesize;
		}
	}

	
	
	void pasteImage(Point2DI position, PixelBuffer& image)
	{
		Point2DI image_size = image.getSizeInPixel();
		Point2DI end_position = image_size + position;
		Point2DI src_offset(0, 0);
		if (position.x < 0)
			src_offset.x = -position.x;
		if (position.y < 0)
			src_offset.y = -position.y;
		Point2DI size = getSizeInPixel();
		
		if (end_position.x >= size.x)
			image_size.x -= end_position.x - size.x;
		if (end_position.y >= size.y)
			image_size.y -= end_position.y - size.y;
		
		// Pasting out of bounds, nothing to do
		if (end_position.x <= 0 || end_position.y <= 0 || image_size.x <= 0 || image_size.y <= 0) return;
		
		int stride_dest = getLineSizeInByte();
		int stride_src = image.getLineSizeInByte();
		
		if (position.x < 0) position.x = 0;
		if (position.y < 0) position.y = 0;
		
		unsigned char* src_buffer = image.buffer() + src_offset.y*stride_src + src_offset.x*image.getPixelSizeInByte();
		unsigned char* dest_buffer = buffer() + position.y * stride_dest + position.x*getPixelSizeInByte();
		
		int copy_size = image_size.x * getPixelSizeInByte();
		for (int y = 0; y < image_size.y; ++y)
		{
			memcpy(dest_buffer, src_buffer, copy_size);
			dest_buffer += stride_dest;
			src_buffer += stride_src;
		}
	}
	
	template<typename pixel_type>
		void pasteImageWithTransparency(Point2DI position, PixelBuffer& image, pixel_type pixel)
	{
		Point2DI image_size = image.getSizeInPixel();
		Point2DI end_position = image_size + position;
		Point2DI src_offset(0, 0);
		if (position.x < 0)
			src_offset.x = -position.x;
		if (position.y < 0)
			src_offset.y = -position.y;
		Point2DI size = getSizeInPixel();
		
		if (end_position.x >= size.x)
			image_size.x -= end_position.x - size.x;
		if (end_position.y >= size.y)
			image_size.y -= end_position.y - size.y;
		
		// Pasting out of bounds, nothing to do
		if (end_position.x <= 0 || end_position.y <= 0 || image_size.x <= 0 || image_size.y <= 0) return;
		
		int stride_dest = getLineSizeInByte();
		int stride_src = image.getLineSizeInByte();
		
		if (position.x < 0) position.x = 0;
		if (position.y < 0) position.y = 0;
		
		auto src_buffer = (pixel_type*)(image.buffer() + src_offset.y*stride_src + src_offset.x*image.getPixelSizeInByte());
		auto dest_buffer = (pixel_type*)(buffer() + position.y * stride_dest + position.x*getPixelSizeInByte());
		
		for (int y = 0; y < image_size.y; ++y)
		{
			for (int x = 0; x < image_size.x; ++x)
			{
				dest_buffer[x] = src_buffer[x] == pixel ? dest_buffer[x] : src_buffer[x];
			}
			dest_buffer = (pixel_type*)((unsigned char*)dest_buffer + stride_dest);
			src_buffer = (pixel_type*)((unsigned char*)src_buffer + stride_src);
		}
	}
	
	
	virtual PixelBuffer*	CreateSameType() const = 0;
	
	bool isSameType(const PixelBuffer& other) const
	{
		return (getTypeID() == other.getTypeID());
	}
	
	void copyTo(PixelBuffer& to)
	{
		Point2DI size = getSizeInPixel();
		if (size.x && size.y)
		{
			to.resize(size.x, size.y);
			unsigned char* read = buffer();
			unsigned char* write = to.buffer();
			if (getLineSizeInByte() == to.getLineSizeInByte())
			{
				memcpy(write, read, getLineSizeInByte()* size.y);
			}
			else
			{
				for (int y = 0; y < size.y; ++y)
				{
					memcpy(write, read, getLineSizeInByte());
					advancePointer(read);
					to.advancePointer(write);
				}
			}
		}
	}
	
	template<typename pixel_type>
		void drawLine(Point2DI p1, Point2DI p2, pixel_type pixel_color);
	
	template<typename pixel_type>
		void drawGradientLine(Point2DI p1, Point2DI p2, pixel_type pixel_color1, pixel_type pixel_color2);
	
	template<typename pixel_type>
		void drawLineAdditive(Point2DI p1, Point2DI p2, pixel_type pixel_color);
	
	template<typename pixel_type, typename F>
		void drawLineFunc(Point2DI p1, Point2DI p2, F f);
	
	template<typename pixel_type>
		void drawRect(Point2DI p1, Point2DI p2, pixel_type pixel_color);
	
	template<typename pixel_type>
		void fillRect(Point2DI p1, Point2DI p2, pixel_type pixel_color);
	
	template<typename pixel_type>
		void drawCircle(Point2DI c, int radius, pixel_type pixel_color);
	
	template <typename pixel_type>
		void fillCircle(Point2DI c, int radius, pixel_type pixel_color);
	
	template<typename pixel_type>
		void drawAndresCircle(Point2DI c, int radius, pixel_type pixel_color);
	
	template<typename pixel_type>
		void fillAndresCircle(Point2DI c, int radius, pixel_type pixel_color);
	
	template <typename pixel_type, typename buffer_type, typename func>
		void drawRawBuffer(Point2DI pos, Point2DI size, buffer_type* buffer, func f, bool flip = false, int repeat_size = 1, int stride_in_byte = 0);
	
	template <typename pixel_type, typename buffer_read_type, typename func>
		void drawBuffer(Point2DI pos, PixelBuffer* buffer, func f, int repeat_size = 1);
	
	// Rescale current buffer to fit in target buffer
	void rescale(PixelBuffer& target);
	
	virtual std::vector<unsigned char> toRegularBuffer() = 0;
	virtual unsigned int	getTypeID() const = 0;
	
	protected:
	
	
};


template <typename pixel_type>
void PixelBuffer::drawLine(Point2DI p1, Point2DI p2, pixel_type pixel_color)
{
	pixel_type* ptr = buffer<pixel_type>();
	
	Point2DI size = getSizeInPixel();
	
	Point2DI dv(p2);
	dv -= p1;
	
	int dvx = (dv.x > 0) ? dv.x : -dv.x;
	int dvy = (dv.y > 0) ? dv.y : -dv.y;
	
	if (dvx > dvy) // horizontal
	{
		int startX = (int)(p1.x + 0.5f);
		int endX = (int)(p2.x + 0.5f);
		float startY = (float)p1.y;
		float slope = ((float)dv.y) / ((float)dv.x);
		if (dv.x < 0.0f)
		{
			//slope=-slope;
			int swap = startX;
			startX = endX;
			endX = swap;
			startY = (float)p2.y;
		}
		int i;
		for (i = startX; i <= endX; i++)
		{
			int posX = i;
			int posY = ((int)(startY + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				*movePointer<pixel_type>(ptr, posX, posY) = pixel_color;
			}
			startY += slope;
		}
	}
	else // vertical
	{
		int startY = (int)(p1.y + 0.5f);
		int endY = (int)(p2.y + 0.5f);
		float startX = (float)p1.x;
		float slope = ((float)dv.x) / ((float)dv.y);
		if (dv.y < 0.0f)
		{
			//slope=-slope;
			int swap = startY;
			startY = endY;
			endY = swap;
			startX = (float)p2.x;
		}
		int i;
		for (i = startY; i <= endY; i++)
		{
			int posY = i;
			int posX = ((int)(startX + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				*movePointer<pixel_type>(ptr, posX, posY) = pixel_color;
			}
			startX += slope;
		}
	}
}


template <typename pixel_type>
void PixelBuffer::drawGradientLine(Point2DI p1, Point2DI p2, pixel_type pixel_color1, pixel_type pixel_color2)
{
	pixel_type* ptr = buffer<pixel_type>();
	
	Point2DI size = getSizeInPixel();
	
	Point2DI dv(p2);
	dv -= p1;
	
	int dvx = (dv.x > 0) ? dv.x : -dv.x;
	int dvy = (dv.y > 0) ? dv.y : -dv.y;
	
	if (dvx > dvy) // horizontal
	{
		int startX = (int)(p1.x + 0.5f);
		int endX = (int)(p2.x + 0.5f);
		float startY = (float)p1.y;
		float slope = ((float)dv.y) / ((float)dv.x);
		if (dv.x < 0.0f)
		{
			//slope=-slope;
			int swap = startX;
			startX = endX;
			endX = swap;
			startY = (float)p2.y;
			
			// swap colors
			pixel_type swapc = pixel_color1;
			pixel_color1 = pixel_color2;
			pixel_color2 = swapc;
			
		}
		int i;
		float t = 0.0f;
		float dt = 1.0f / (endX - startX + 1.0f);
		for (i = startX; i <= endX; i++)
		{
			int posX = i;
			int posY = ((int)(startY + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				
				pixel_type pixel_color = pixel_type::blend(pixel_color1, pixel_color2, t);
				
				*movePointer<pixel_type>(ptr, posX, posY) = pixel_color;
				t += dt;
			}
			startY += slope;
		}
	}
	else // vertical
	{
		int startY = (int)(p1.y + 0.5f);
		int endY = (int)(p2.y + 0.5f);
		float startX = (float)p1.x;
		float slope = ((float)dv.x) / ((float)dv.y);
		if (dv.y < 0.0f)
		{
			//slope=-slope;
			int swap = startY;
			startY = endY;
			endY = swap;
			startX = (float)p2.x;
			
			// swap colors
			pixel_type swapc = pixel_color1;
			pixel_color1 = pixel_color2;
			pixel_color2 = swapc;
			
		}
		int i;
		float t = 0.0f;
		float dt = 1.0f / (endY - startY + 1.0f);
		for (i = startY; i <= endY; i++)
		{
			int posY = i;
			int posX = ((int)(startX + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				pixel_type pixel_color = pixel_type::blend(pixel_color1, pixel_color2, t);
				
				*movePointer<pixel_type>(ptr, posX, posY) = pixel_color;
				
				t += dt;
			}
			startX += slope;
		}
	}
}

template <typename pixel_type>
void PixelBuffer::drawLineAdditive(Point2DI p1, Point2DI p2, pixel_type pixel_color)
{
	pixel_type* ptr = buffer<pixel_type>();
	
	Point2DI size = getSizeInPixel();
	
	Point2DI dv(p2);
	dv -= p1;
	
	int dvx = (dv.x > 0) ? dv.x : -dv.x;
	int dvy = (dv.y > 0) ? dv.y : -dv.y;
	
	if (dvx > dvy) // horizontal
	{
		int startX = (int)(p1.x + 0.5f);
		int endX = (int)(p2.x + 0.5f);
		float startY = (float)p1.y;
		float slope = ((float)dv.y) / ((float)dv.x);
		if (dv.x < 0.0f)
		{
			//slope=-slope;
			int swap = startX;
			startX = endX;
			endX = swap;
			startY = (float)p2.y;
		}
		int i;
		for (i = startX; i <= endX; i++)
		{
			int posX = i;
			int posY = ((int)(startY + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				*movePointer<pixel_type>(ptr, posX, posY) += pixel_color;
			}
			startY += slope;
		}
	}
	else // vertical
	{
		int startY = (int)(p1.y + 0.5f);
		int endY = (int)(p2.y + 0.5f);
		float startX = (float)p1.x;
		float slope = ((float)dv.x) / ((float)dv.y);
		if (dv.y < 0.0f)
		{
			//slope=-slope;
			int swap = startY;
			startY = endY;
			endY = swap;
			startX = (float)p2.x;
		}
		int i;
		for (i = startY; i <= endY; i++)
		{
			int posY = i;
			int posX = ((int)(startX + 0.5f));
			if (posX >= 0 && posY >= 0 && posX < size.x && posY < size.y)
			{
				*movePointer<pixel_type>(ptr, posX, posY) += pixel_color;
			}
			startX += slope;
		}
	}
}

template <typename pixel_type, typename F>
void PixelBuffer::drawLineFunc(Point2DI p1, Point2DI p2, F f)
{
	pixel_type* ptr = buffer<pixel_type>();
	
	Point2DI size = getSizeInPixel();
	
	Point2DI dv(p2);
	dv -= p1;
	
	int dvx = (dv.x > 0) ? dv.x : -dv.x;
	int dvy = (dv.y > 0) ? dv.y : -dv.y;
	
	if (dvx>dvy) // horizontal
	{
		int startX = (int)(p1.x + 0.5f);
		int endX = (int)(p2.x + 0.5f);
		float startY = (float)p1.y;
		float slope = ((float)dv.y) / ((float)dv.x);
		if (dv.x<0.0f)
		{
			//slope=-slope;
			int swap = startX;
			startX = endX;
			endX = swap;
			startY = (float)p2.y;
		}
		int i;
		for (i = startX; i <= endX; i++)
		{
			int posX = i;
			int posY = ((int)(startY + 0.5f));
			if (posX >= 0 && posY >= 0 && posX<size.x && posY<size.y)
			{
				pixel_type* ptr2 = movePointer<pixel_type>(ptr, posX, posY);
				*ptr2 = f(*ptr2);
			}
			startY += slope;
		}
	}
	else // vertical
	{
		int startY = (int)(p1.y + 0.5f);
		int endY = (int)(p2.y + 0.5f);
		float startX = (float)p1.x;
		float slope = ((float)dv.x) / ((float)dv.y);
		if (dv.y<0.0f)
		{
			//slope=-slope;
			int swap = startY;
			startY = endY;
			endY = swap;
			startX = (float)p2.x;
		}
		int i;
		for (i = startY; i <= endY; i++)
		{
			int posY = i;
			int posX = ((int)(startX + 0.5f));
			if (posX >= 0 && posY >= 0 && posX<size.x && posY<size.y)
			{
				pixel_type* ptr2 = movePointer<pixel_type>(ptr, posX, posY);
				*ptr2 = f(*ptr2);
			}
			startX += slope;
		}
	}
}


template <typename pixel_type>
void PixelBuffer::drawRect(Point2DI p1, Point2DI p2, pixel_type pixel_color)
{
	Point2DI p12(p2.x, p1.y);
	Point2DI p21(p1.x, p2.y);
	drawLine<pixel_type>(p1, p12, pixel_color);
	drawLine<pixel_type>(p12, p2, pixel_color);
	drawLine<pixel_type>(p2, p21, pixel_color);
	drawLine<pixel_type>(p21, p1, pixel_color);
}


template <typename pixel_type>
void PixelBuffer::fillRect(Point2DI p1, Point2DI p2, pixel_type pixel_color)
{
	pixel_type* ptr = buffer<pixel_type>();
	for (int y = p1.y; y < p2.y; y++)
	{
		for (int x = p1.x; x < p2.x; x++)
		{
			*movePointer<pixel_type>(ptr, x, y) = pixel_color;
		}
	}
}

template <typename pixel_type>
void PixelBuffer::drawCircle(Point2DI c, int radius, pixel_type pixel_color)
{
	Point2DI size = getSizeInPixel();
	int l = (radius * 185363) >> 18;
	
	int x, y;
	int r2, y2, y2_new, ty;
	/* At x=0, y=radius */
	y = radius;
	
	r2 = y2 = y * y;
	ty = (2 * y) - 1;
	y2_new = r2 + 3;
	
	for (x = 0; x <= l; x++) {
		y2_new -= (2 * x) - 3;
		
		if ((y2 - y2_new) >= ty) {
			y2 -= ty;
			y -= 1;
			ty -= 2;
		}
		
		Point2DI pos;
		
		pos.Set(x, y); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(x, -y); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(-x, y); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(-x, -y); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		
		pos.Set(y, x); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(y, -x); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(-y, x); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
		pos.Set(-y, -x); pos += c;
		if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y) *getPixel<pixel_type>(pos.x, pos.y) = pixel_color;
	}
	
}

template <typename pixel_type>
void PixelBuffer::drawAndresCircle(Point2DI c, int radius, pixel_type pixel_color)
{
	int x = 0;
	int y = radius;
	int d = radius - 1;
	
	while (y >= x)
	{
		*getPixel<pixel_type>(c.x + x, c.y + y) = pixel_color;
		*getPixel<pixel_type>(c.x + y, c.y + x) = pixel_color;
		*getPixel<pixel_type>(c.x - x, c.y + y) = pixel_color;
		*getPixel<pixel_type>(c.x - y, c.y + x) = pixel_color;
		*getPixel<pixel_type>(c.x + x, c.y - y) = pixel_color;
		*getPixel<pixel_type>(c.x + y, c.y - x) = pixel_color;
		*getPixel<pixel_type>(c.x - x, c.y - y) = pixel_color;
		*getPixel<pixel_type>(c.x - y, c.y - x) = pixel_color;
		
		if (d >= 2 * x)
		{
			d -= 2 * x + 1;
			x++;
		}
		else if (d < 2 * (radius - y))
		{
			d += 2 * y - 1;
			y--;
		}
		else
		{
			d += 2 * (y - x - 1);
			y--;
			x++;
		}
	}
}

template <typename pixel_type>
void PixelBuffer::fillCircle(Point2DI c, int radius, pixel_type pixel_color)
{
	int r2 = radius * radius;
	int area = r2 << 2;
	int rr = radius << 1;
	
	for (int i = 0; i < area; i++)
	{
		int tx = (i % rr) - radius;
		int ty = (i / rr) - radius;
		
		if (tx * tx + ty * ty <= r2)
			*getPixel<pixel_type>(c.x + tx, c.y +ty) = pixel_color;
	}
}

template <typename pixel_type>
void PixelBuffer::fillAndresCircle(Point2DI c, int radius, pixel_type pixel_color)
{
	int x = 0;
	int y = radius;
	int d = radius - 1;
	
	while (y >= x)
	{
		drawLine(Point2DI(c.x + x, c.y + y), Point2DI(c.x + x, c.y - y), pixel_color);
		drawLine(Point2DI(c.x + y, c.y + x), Point2DI(c.x + y, c.y - x),pixel_color);
		drawLine(Point2DI(c.x - x, c.y + y), Point2DI(c.x - x, c.y - y),pixel_color);
		drawLine(Point2DI(c.x - y, c.y + x), Point2DI(c.x - y, c.y - x),pixel_color);
		
		if (d >= 2 * x)
		{
			d -= 2 * x + 1;
			x++;
		}
		else if (d < 2 * (radius - y))
		{
			d += 2 * y - 1;
			y--;
		}
		else
		{
			d += 2 * (y - x - 1);
			y--;
			x++;
		}
	}
}

template <typename pixel_type, typename buffer_type, typename func>
void PixelBuffer::drawRawBuffer(Point2DI pos, Point2DI size, buffer_type* raw, func f, bool flip, int repeat_size, int stride_in_byte)
{
	pixel_type* buf = buffer<pixel_type>() + pos.x;
	advancePointer(buf, pos.y);
	
	const int rstride = stride_in_byte!=0 ? stride_in_byte : size.x*sizeof(buffer_type);
	buffer_type* reader = raw;
	
	if(flip)
	{
		for (int y = size.y-1; y >= 0; --y)
		{
			reader = (buffer_type*)((unsigned char*)raw + (y*rstride));
			for (int x = 0; x < size.x; ++x)
			{
				pixel_type pix;
				if (f(reader[x], pix))
				{
					auto tmp = buf;
					for (int yy = 0; yy < repeat_size; ++yy)
					{
						for (int xx = 0; xx < repeat_size; ++xx)
						{
							tmp[x*repeat_size + xx] = pix;
						}
						advancePointer(tmp);
					}
				}
			}
			advancePointer(buf, repeat_size);
		}
	}
	else
	{
		for (int y = 0; y < size.y; ++y)
		{
			for (int x = 0; x < size.x; ++x)
			{
				pixel_type pix;
				if (f(raw[x], pix))
				{
					auto tmp = buf;
					for (int yy = 0; yy < repeat_size; ++yy)
					{
						for (int xx = 0; xx < repeat_size; ++xx)
						{
							tmp[x*repeat_size + xx] = pix;
						}
						advancePointer(tmp);
					}
				}
			}
			raw = (buffer_type*)((unsigned char*)raw + rstride);
			advancePointer(buf, repeat_size);
		}
	}
}
template <typename pixel_type, typename buffer_read_type, typename func>
void PixelBuffer::drawBuffer(Point2DI pos, PixelBuffer* other, func f, int repeat_size)
{
	pixel_type* buf = buffer<pixel_type>() + pos.x;
	advancePointer(buf, pos.y);
	auto size = other->getSizeInPixel();
	
	buffer_read_type* read = other->buffer<buffer_read_type>();
	
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			auto val = f(read[x]);
			auto tmp = buf;
			for (int yy = 0; yy < repeat_size; ++yy)
			{
				for (int xx = 0; xx < repeat_size; ++xx)
				{
					tmp[x*repeat_size + xx] = val;
				}
				
				advancePointer(tmp);
			}
		}
		advancePointer(buf, repeat_size);
		other->advancePointer(read);
	}
}


// helper class to access / maintain pixel buffer
template<int componentSize, int componentCount, int pixelPadding = 1, int lineBytePadding = 16>
class BasePixelBufferStruct final : public PixelBuffer
{
	using MyType = BasePixelBufferStruct<componentSize, componentCount, pixelPadding, lineBytePadding>;
	public:
	
	BasePixelBufferStruct() : myPixelRawBuffer(nullptr)
		, mySize(0, 0)
		, myXPadding(0)
	{
	}
	
	// Move constructor
	BasePixelBufferStruct(MyType&& other) : myPixelRawBuffer(nullptr)
	{
		*this = std::move(other);
	}
	// Move assignement operator
	MyType& operator=(MyType&& other)
	{
		clean();
		mySize = other.mySize;
		myXPadding = other.myXPadding;
		// Steal myPixelRawBuffer, no RefCount change;
		myPixelRawBuffer = other.myPixelRawBuffer;
		other.myPixelRawBuffer = nullptr;
		other.mySize.Set(0, 0);
		other.myXPadding = 0;
		return *this;
	}
	
	
	
	virtual ~BasePixelBufferStruct()
	{
		clean();
	}
	
	virtual PixelBuffer*	CreateSameType() const override
	{
		return new BasePixelBufferStruct<componentSize, componentCount, pixelPadding, lineBytePadding>();
	}
	
	
	
	inline int	getLineSizeInByte() const override
	{
		return mySize.x* componentCount * componentSize + myXPadding;
	}
	
	inline int	getXPaddingInByte() const override
	{
		return myXPadding;
	}
	
	inline int	getPixelSizeInByte() const override
	{
		return  componentCount * componentSize;
	}
	
	inline Point2DI	getSizeInPixel() const override
	{
		return  mySize;
	}
	
	inline int getComponentCount() const override
	{
		return componentCount;
	}
	
	inline int getComponentSize() const override
	{
		return componentSize;
	}
	
	using PixelBuffer::resize;
	inline void resize(int sx, int sy) override
	{
		checkForReinit(sx, sy);
	}
	
	unsigned char* char_buffer() override
	{
		return (unsigned char*)myPixelRawBuffer->buffer();
	}
	
	
	virtual std::vector<unsigned char> toRegularBuffer() override
	{
		std::vector<unsigned char> result(mySize.x*mySize.y*componentCount*componentSize);
		int writestride = mySize.x*componentCount*componentSize;
		
		unsigned char* dest = result.data();
		unsigned char* src = buffer();
		int readstride = getLineSizeInByte();
		
		for (int y = 0; y < mySize.y; ++y)
		{
			memcpy(dest, src, writestride);
			src += readstride;
			dest += writestride;
		}
		return result;
	}
	
	void exportToImage(const kstl::string& filename, TinyImage::ImageFileFormat format, bool flip=true)
	{
		if (componentSize == 1)
		{
			if (componentCount == 3)
				TinyImage::ExportImage(filename.c_str(), getXPaddingInByte() == 0 ? buffer() : toRegularBuffer().data(), mySize.x, mySize.y, TinyImage::RGB_24_888, format, flip);
			else if (componentCount == 1)
				TinyImage::ExportImage(filename.c_str(), getXPaddingInByte() == 0 ? buffer() : toRegularBuffer().data(), mySize.x, mySize.y, TinyImage::GREYSCALE, format, flip);
		}
	}
	
	unsigned int	getTypeID() const override
	{
		// byte padding on 7 bits
		// pixel padding on 7 bits
		// component count on 3 bits
		// component size on 3 bits
		
		unsigned int ID = (componentSize << 17) | (componentCount << 14) | (pixelPadding << 7) | (lineBytePadding);
		return ID;
	}
	
	protected:
	
	
	
	void clean()
	{
		if (myPixelRawBuffer)
		{
			myPixelRawBuffer->Destroy();
			myPixelRawBuffer = 0;
			mySize.Set(0, 0);
		}
	}
	
	int checkForReinit(int sx, int sy)
	{
		// sizex 
		Point2DI newsize(sx, sy);
		
		//int linexsizeinbyte = sx * componentCount * componentSize;
		
		// first compute needed padding for pixels
		int newpixelpadding = (sx - ((sx / pixelPadding) * pixelPadding));
		if (newpixelpadding)
		{
			newpixelpadding = pixelPadding - newpixelpadding;
		}
		
		// then needed padding for line size (in byte)
		int linexsizeinbytewithpixelpadding = (sx + newpixelpadding)*componentCount * componentSize;
		
		int newbytepadding = (linexsizeinbytewithpixelpadding - ((linexsizeinbytewithpixelpadding / lineBytePadding) * lineBytePadding));
		if (newbytepadding)
		{
			newbytepadding = lineBytePadding - newbytepadding;
		}
		int totalsize = (linexsizeinbytewithpixelpadding + newbytepadding)*newsize.y;
		
		int old_size = (mySize.x*componentCount*componentSize + myXPadding)*mySize.y;
		
		if (totalsize > old_size)
		{
			clean();
		}
		else
		{
			//memset(buffer(), 0, old_size);
		}
		
		mySize = newsize;
		// add a padding on x for each line (in byte)
		myXPadding = newpixelpadding *componentCount * componentSize + newbytepadding;
		
		
		if (myPixelRawBuffer == nullptr) {
			myPixelRawBuffer = new AlignedCoreRawBuffer<16, unsigned char>(totalsize, false);
			memset(myPixelRawBuffer->buffer(), 0, totalsize);
		}
		
		return totalsize;
	}
	
	AlignedCoreRawBuffer<16, unsigned char>*	myPixelRawBuffer;
	Point2DI									mySize;
	int											myXPadding;
};



typedef BasePixelBufferStruct<sizeof(int), 1> IntPixelBuffer;
typedef BasePixelBufferStruct<sizeof(char), 1> CharPixelBuffer;
typedef BasePixelBufferStruct<sizeof(short), 1> ShortPixelBuffer;
typedef BasePixelBufferStruct<sizeof(char), 3> RGB24PixelBuffer;
typedef BasePixelBufferStruct<sizeof(float), 1> FloatPixelBuffer;



class RawPixelBufferView : public PixelBuffer
{
	public:
	RawPixelBufferView() : myRawBuffer(nullptr), mySize(0,0), myLinesize(0) {}
	RawPixelBufferView(unsigned char* underlying_buffer, int linesize, Point2DI size)
		: myRawBuffer(underlying_buffer)
		, mySize(size)
		, myLinesize(linesize)
	{
		myCompCount = size.x != 0 ? myLinesize / size.x : 1;
	}
	
	void setTarget(unsigned char* underlying_buffer, int linesize, Point2DI size) 
	{
		myRawBuffer = underlying_buffer;
		mySize = size;
		myLinesize = linesize;
		myCompCount = size.x != 0 ? myLinesize / size.x : 1;
	}
	
	int getLineSizeInByte() const override { return myLinesize; }
	int getXPaddingInByte() const override { return 0; }
	int getPixelSizeInByte() const override { return myCompCount * sizeof(char); }
	int getComponentCount() const override { return myLinesize / mySize.x; }
	int getComponentSize() const override { return sizeof(char); }
	Point2DI getSizeInPixel() const override { return mySize; }
	unsigned char* char_buffer() override { return myRawBuffer; }
	void resize(int sx, int sy) override {};
	PixelBuffer* CreateSameType() const override { return new RGB24PixelBuffer(); };
	virtual std::vector<unsigned char> toRegularBuffer() override
	{
		std::vector<unsigned char> result(mySize.x*mySize.y*getComponentCount());
		memcpy(result.data(), myRawBuffer, result.size());
		return result;
	}
	protected:
	unsigned getTypeID() const override {
		return (sizeof(char) << 17) | (myCompCount << 14);
	}
	
	unsigned char* myRawBuffer;
	Point2DI mySize;
	int myLinesize;
	int myCompCount;
};


namespace ptr
{
	// advance ptr by N bytes (as if it was a char*)
	template<typename T>
		inline void add(T*& ptr, int bytes)
	{
		ptr = (T*)((char*)ptr + bytes);
	}
	
	// move ptr by x,y using stride (in bytes) to move between rows
	template<typename T>
		inline void move(T*& ptr, int stride, int x, int y)
	{
		ptr = (T*)((char*)(ptr + x) + stride*y);
	}
	
	// return a ptr that is offset x,y using stride (in bytes) to move between rows
	template<typename T>
		inline T* at(T* ptr, int stride, int x, int y)
	{
		move(ptr, stride, x, y);
		return ptr;
	}
	
	
	
}

#endif
