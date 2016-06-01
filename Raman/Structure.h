#pragma once

#include "Util.h"

struct Subscrip2
{
	Subscrip2(const unsigned int & inY, const unsigned int & inX, const unsigned int inLen = 0)
		: y(inY), x(inX), rowLen(inLen)
	{
		L = y * rowLen + x;
	};
	unsigned int L;
	unsigned int y;
	unsigned int x;
	unsigned int rowLen;
};

struct Pixel
{
	Pixel(COLORREF inColor = 0)
		: red(GetRValue(inColor)), green(GetGValue(inColor)), blue(GetBValue(inColor)), alpha(255) { }
	Pixel(const Pixel & B) : red(B.red), green(B.green), blue(B.blue), alpha(B.alpha) { }
	Pixel(const byte & grayScale) : red(grayScale), green(grayScale), blue(grayScale), alpha(255) { }
	Pixel(const double & grayScale)
	{
		byte color = static_cast<byte>(min(abs(grayScale), 255.0f));
		red = green = blue = color;
		alpha = 255;
	}

	Pixel operator+(const Pixel & B)
	{
		Pixel result;
		unsigned int temp = static_cast<unsigned int>(red) + B.red;
		result.red = min(temp, 255);
		temp = static_cast<unsigned int>(green) + B.green;
		result.green = min(temp, 255);
		temp = static_cast<unsigned int>(blue) + B.blue;
		result.blue = min(temp, 255);
		temp = static_cast<unsigned int>(alpha) + B.alpha;
		result.alpha = min(temp, 255);
		return result;
	}
	Pixel & operator+=(const Pixel & B)
	{
		unsigned int temp = static_cast<unsigned int>(red) + B.red;
		red = min(temp, 255);
		temp = static_cast<unsigned int>(green) + B.green;
		green = min(temp, 255);
		temp = static_cast<unsigned int>(blue) + B.blue;
		blue = min(temp, 255);
		temp = static_cast<unsigned int>(alpha) + B.alpha;
		alpha = min(temp, 255);
		return *this;
	}
	Pixel operator-(const Pixel & B)
	{
		Pixel result;
		unsigned int temp = static_cast<unsigned int>(red)-B.red;
		result.red = min(temp, 255);
		temp = static_cast<unsigned int>(green)-B.green;
		result.green = min(temp, 255);
		temp = static_cast<unsigned int>(blue)-B.blue;
		result.blue = min(temp, 255);
		temp = static_cast<unsigned int>(alpha)-B.alpha;
		result.alpha = min(temp, 255);
		return result;
	}
	Pixel & operator-=(const Pixel & B)
	{
		unsigned int temp = static_cast<unsigned int>(red)-B.red;
		red = min(temp, 255);
		temp = static_cast<unsigned int>(green)-B.green;
		green = min(temp, 255);
		temp = static_cast<unsigned int>(blue)-B.blue;
		blue = min(temp, 255);
		temp = static_cast<unsigned int>(alpha)-B.alpha;
		alpha = min(temp, 255);
		return *this;
	}
	Pixel operator*(const Pixel & B)
	{
		Pixel result;
		unsigned int temp = static_cast<unsigned int>(red) * B.red;
		result.red = min(temp, 255);
		temp = static_cast<unsigned int>(green) * B.green;
		result.green = min(temp, 255);
		temp = static_cast<unsigned int>(blue) * B.blue;
		result.blue = min(temp, 255);
		temp = static_cast<unsigned int>(alpha) * B.alpha;
		result.alpha = min(temp, 255);
		return result;
	}
	Pixel & operator=(const Pixel & B)
	{
		red = B.red;
		green = B.green;
		blue = B.blue;
		alpha = B.alpha;
		return *this;
	}
	Pixel MulByDouble(const double & A, const Pixel & B)
	{
		Pixel result(B);
		result.blue = static_cast<unsigned int>(static_cast<double>(result.blue) * A);
		result.green = static_cast<unsigned int>(static_cast<double>(result.green) * A);
		result.red = static_cast<unsigned int>(static_cast<double>(result.red) * A);
		return result;
	}

	friend double operator*(const double & A, const Pixel & B)
	{
		return A * static_cast<double>(B.blue);
	}
	friend double & operator*=(double & A, const Pixel & B)
	{
		A *= static_cast<double>(B.blue);
		return A;
	}
	friend double operator+(const double & A, const Pixel & B)
	{
		return A + static_cast<double>(B.blue);
	}
	friend double & operator+=(double & A, const Pixel & B)
	{
		A += static_cast<double>(B.blue);
		return A;
	}

	COLORREF GetColor() { return RGB( red, green, blue ); }

	byte blue;
	byte green;
	byte red;
	byte alpha;
};

struct Spectrum
{
	std::vector<double> ccdVec;
	double partSum;

	Spectrum & operator=(const Spectrum & B)
	{
		this->ccdVec = B.ccdVec;
		this->partSum = B.partSum;
		return *this;
	}
};

struct ItemData
{
	double begin;
	double end;
	int red, green, blue;
};