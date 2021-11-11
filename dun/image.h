#ifndef _image
#define _image

#include <atlimage.h>
#include <tchar.h>
#include <memory>
#include "doublebuffer.h"
#include <numeric>
#include <cmath>
#include "TMP.h"

struct ClipArea
{
	int x;
	int y;
	int w;
	int h;
};

class Image : private CImage
{
public:
	int flag;
	float radian;
	unsigned char alpha;

	enum class Flag { v = 0x01, h = 0x02, r = 0x04, a = 0x08 };

	const auto width() const
	{
		return GetWidth();
	}

	const auto height() const
	{
		return GetHeight();
	}

	void load( const TCHAR* file_path )
	{
		CImage::Load( file_path );
	}

	void draw( HDC dc, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh )
	{
		doublebuffer source{ dc, sw, sh };

		Draw( source.buf_dc, 0, 0, sw, sh, sx, sy, sw, sh );

		if ( flag & etoi(Flag::v) )
		{
			dy += dh;
			dh *= -1;
		}

		if ( flag & etoi(Flag::h) )
		{
			dx += dw;
			dw *= -1;
		}

		if ( flag & etoi( Flag::a ) )
		{
			doublebuffer temp{ dc, sw, sh };

			BLENDFUNCTION bf;
			bf.AlphaFormat = 0;
			bf.BlendFlags = 0;
			bf.BlendOp = AC_SRC_OVER;
			bf.SourceConstantAlpha = alpha;

			::AlphaBlend( temp.buf_dc, 0, 0, sw, sh, source.buf_dc, 0, 0, sw, sh, bf );
			::BitBlt( source.buf_dc, 0, 0, sw, sh, temp.buf_dc, 0, 0, SRCCOPY );
		}

		if ( flag & etoi( Flag::r ) )
		{
			doublebuffer temp{ dc, sw, sh };
			SetGraphicsMode( temp.buf_dc, GM_ADVANCED );

			XFORM R;
			R.eM11 = std::cos( radian );	R.eM12 = std::sin( radian );	R.eDx = sw / 2.f;
			R.eM21 = -std::sin( radian );	R.eM22 = std::cos( radian );	R.eDy = sh / 2.f;

			SetWorldTransform( temp.buf_dc, &R );
			::BitBlt( temp.buf_dc, -sw / 2, -sh / 2, sw, sh, source.buf_dc, 0, 0, SRCCOPY );

			SetTransparentColor( RGB( 0, 0, 0 ) );
			::BitBlt( source.buf_dc, 0, 0, sw, sh, temp.buf_dc, -sw / 2, -sh / 2, SRCCOPY );
		}

		::TransparentBlt( dc, dx, dy, dw, dh, source.buf_dc, 0, 0, sw, sh, SRCCOPY );
	}

	static auto make( const TCHAR* file_path )
	{
		return std::shared_ptr< Image >{ new Image{ file_path } };
	}

	Image( const Image& ) = delete;
	Image& operator=( const Image& ) = delete;

private:
	Image() : flag{ 0 }, radian{ 0.f }, alpha{ 0xff } {}

	Image( const TCHAR* file_path ) : flag{ 0 }, radian{ 0.f }
	{
		load( file_path );
	}
};

using image_ptr = std::shared_ptr< Image >;

#endif