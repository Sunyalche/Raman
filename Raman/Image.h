#pragma once

#include "UTil.h"
#include "Matrix.hpp"

class Image
{
public:		
	Image(const unsigned int & inWidth = 0, const unsigned int & inHeight = 0) 
		: width(inWidth), height(inHeight) { }
	Image(const std::string & inFileName);
	Image(const Image & B);
	virtual ~Image();
	
	virtual bool TranslateFromHeader() = 0;
	virtual bool GetPixelData() = 0;
	virtual bool GetFileData() = 0;
	virtual bool MakeHeader() = 0;
	virtual bool SaveAs(const std::string & newFileName);

	unsigned int width;
	unsigned int height;

	Image & operator=(const Image & B);

	std::string fileName;
	std::unique_ptr<std::vector<byte>> pFileData;
};

