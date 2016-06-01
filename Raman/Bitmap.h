#pragma once

#include "Util.h"
#include "Image.h"
#include "ImageData.hpp"

class Bitmap : public Image
{
public:
	Bitmap() : Image() { }
	Bitmap(const std::string & fileName);
	Bitmap(const Bitmap & B);
	Bitmap(const ImageData<Pixel> & inImageData);
	~Bitmap();

	virtual bool TranslateFromHeader();
	virtual bool GetPixelData();
	virtual bool GetFileData();
	virtual bool MakeHeader();

	Bitmap GetGrayScaleBMP();

	double GetMSE(const Bitmap & bitmap);

	Bitmap & operator=(const Bitmap & B);

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	std::unique_ptr<ImageData<Pixel>> pPixelData;
};

