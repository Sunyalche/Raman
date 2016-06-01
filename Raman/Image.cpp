#include "Image.h"
#include "Help.h"

using namespace std;

Image::Image(const string & inFileName) : width(0), height(0), fileName(inFileName)
{
	pFileData = Help::ReadFile(fileName);
}

Image::Image(const Image & B) : width(B.width), height(B.height)
{
	fileName = B.fileName;
	pFileData = make_unique<vector<byte>>(*B.pFileData);
}

bool Image::SaveAs(const std::string & newFileName)
{
	GetFileData();
	return Help::WriteFile(newFileName, pFileData);
}

Image::~Image()
{
	pFileData.release();
}

Image & Image::operator=(const Image & B)
{
	width = B.width;
	height = B.height;
	return *this;
}
