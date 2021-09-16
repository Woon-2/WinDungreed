#pragma once
#ifndef _xyutility
#define _xyutility
#include <windows.h>

const auto xytrans( const RECT& destrt, const RECT& srcrt, const POINT& pt )
{
	auto xdiff = pt.x - srcrt.left;
	auto ydiff = pt.y - srcrt.top;

	auto widthratio = ( static_cast< double >( destrt.right ) - destrt.left ) / ( static_cast< double >( srcrt.right ) - srcrt.left );
	auto heightratio = ( static_cast< double >( destrt.bottom ) - destrt.top ) / ( static_cast< double >( srcrt.bottom ) - srcrt.top );

	// windef.h의 RECT는 LONG을 통해 생성되고, double로부터 형변환이 불가능하다
	xdiff = static_cast< LONG >( xdiff * widthratio );
	ydiff = static_cast< LONG >( ydiff * heightratio );

	return POINT{ destrt.left + xdiff, destrt.top + ydiff };
}

class rect_builder
{
public:
	rect_builder& left(const int val)
	{
		_rect.left = val;
		flag |= LEFT;
		return *this;
	}
	rect_builder& top(const int val)
	{
		_rect.top = val; 
		flag |= TOP;
		return *this;
	}
	rect_builder& right(const int val)
	{ 
		_rect.right = val; 
		flag |= RIGHT;
		return *this;
	}
	rect_builder& bottom(const int val)
	{ 
		_rect.bottom = val; 
		flag |= BOTTOM;
		return *this;
	}
	rect_builder& center(const POINT val)
	{ 
		_center = val; 
		flag |= CENTER;
		return *this;
	}
	rect_builder& width(const int val)
	{ 
		_width = val; 
		flag |= WIDTH;
		return *this;
	}
	rect_builder& height(const int val)
	{ 
		_height = val;
		flag |= HEIGHT;
		return *this;
	}
	RECT build()
	{
		// left, top, right, bottom이 간접적으로 유도되는 경우를 처리한 뒤
		// RECT{ left, top, right, bottom }을 반환한다
		// 직접 대입이 일어나지 않았을 경우 간접 유도를 시도하고,
		// 불가능하면 오류를 던진다
		if (!(flag & LEFT))
			_rect.left = implicit_left();

		if (!(flag & TOP))
			_rect.top = implicit_top();

		if (!(flag & RIGHT))
			_rect.right = implicit_right();

		if (!(flag & BOTTOM))
			_rect.bottom = implicit_bottom();

		return _rect;
	}

	operator RECT()
	{
		return build();
	}

	rect_builder() : _rect{ -1, -1, -1, -1 }, _center{ -1, -1 },
		_width{ -1 }, _height{ -1 }, flag{ 0 } {}

	rect_builder(const rect_builder& other) : _rect{ other._rect },
		_center{ other._center }, _width{ other._width },
		_height{ other._height }, flag{ other.flag } {}

	rect_builder& operator=(const rect_builder& other)
	{
		_rect = other._rect;
		_center = other._center;
		_width = other._width;
		_height = other._height;
		flag = other.flag;
	}

private:
	RECT _rect;
	POINT _center;
	int _width;
	int _height;

	enum Flag { LEFT = 0x01, TOP = 0x02, RIGHT = 0x04, BOTTOM = 0x08,
		CENTER = 0x10, WIDTH = 0x20, HEIGHT = 0x40 };
	BYTE flag;

	HELPER const int implicit_left()
	{
		if (flag & WIDTH)
		{
			if (flag & RIGHT)
				return _rect.right - _width;
			else if (flag & CENTER)
				return _center.x - (_width >> 1);
			else
				throw "cannot make rect\n";
		}
		else
		{
			throw "cannot make rect\n";
		}
	}

	HELPER const int implicit_top()
	{
		if (flag & HEIGHT)
		{
			if (flag & BOTTOM)
				return _rect.bottom - _height;
			else if (flag & CENTER)
				return _center.y - (_height >> 1);
			else
				throw "cannot make rect\n";
		}
		else
		{
			throw "cannot make rect\n";
		}
	}

	HELPER const int implicit_right()
	{
		if (flag & WIDTH)
		{
			if (flag & LEFT)
				return _rect.left + _width;
			else if (flag & CENTER)
				return _center.x + (_width >> 1);
			else
				throw "cannot make rect\n";
		}
		else
		{
			throw "cannot make rect\n";
		}
	}

	HELPER const int implicit_bottom()
	{
		if (flag & HEIGHT)
		{
			if (flag & TOP)
				return _rect.top + _height;
			else if (flag & CENTER)
				return _center.y + (_height >> 1);
			else
				throw "cannot make rect\n";
		}
		else
		{
			throw "cannot make rect\n";
		}
	}
};

#include <iostream>
#include "randomvalue.h"
void test_rect_builder()
{
	for (int i = 0; i < 10; ++i)
	{
		auto rt = rect_builder()
			.left(random_value(100, 200))
			.top(random_value(100, 200))
			.right(random_value(300, 400))
			.bottom(random_value(300, 400))
			.build();
		auto rt2 = rect_builder()
			.center(POINT{ random_value(400, 600), random_value(400, 600) })
			.width(100)
			.height(100)
			.build();
		auto rt3 = rect_builder()
			.left(random_value(500, 600))
			.top(random_value(500, 600))
			.width(200)
			.height(300)
			.build();

		std::cout << "left in [100, 200], top in [100, 200], right in [300, 400], bottom in [300, 400]\n";
		std::cout << "{ " << rt.left << ", " << rt.top << ", " << rt.right << ", " << rt.bottom << " }\n\n";
		std::cout << "center.x in [400, 600], center.y in [400, 600], width 100, height 100\n";
		std::cout << "{ " << rt2.left << ", " << rt2.top << ", " << rt2.right << ", " << rt2.bottom << " }\n\n";
		std::cout << "left in [500, 600], top in [500, 600], width 200, height 300\n";
		std::cout << "{ " << rt3.left << ", " << rt3.top << ", " << rt3.right << ", " << rt3.bottom << " }\n\n";
	}
}
#endif