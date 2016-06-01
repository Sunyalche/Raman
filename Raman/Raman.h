#pragma once
#include "Util.h"
#include "Structure.h"
#include "Image.h"
#include "ImageData.hpp"
#include "Bitmap.h"


class Raman : public Image
{
public:
	Raman() : Image() { }
	Raman(const std::string & fileName,
		  const unsigned int inHeight = 32,
		  const unsigned int inWidth = 32,
		  const double inBeginLambda = 532.268249511719f,
		  const double inEndLambda = 665.941528320313f);
	Raman(const Raman & B);
	Raman(const ImageData<Spectrum> & inImageData);
	~Raman();

	virtual bool TranslateFromHeader();
	virtual bool GetPixelData();
	virtual bool GetFileData();
	virtual bool MakeHeader();

	void DoNLM(const int d, const int sr, const int h);

	Bitmap CreateBitmap(const unsigned int begin, const unsigned int end, Pixel & inPixel);
	Bitmap BlendBitmap(const unsigned int begin, const unsigned int end, Pixel & inPixel, Bitmap & oldBitmap);

	Raman & operator=(const Raman & B);

	double beginLambda;
	double endLambda;
	unsigned int spectrumSize;

	std::unique_ptr<ImageData<Spectrum>> pPixelData;
};

