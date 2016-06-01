#include "Bitmap.h"
#include "Help.h"

using namespace std;

Bitmap::Bitmap(const string & fileName) : Image(fileName) 
{
	bool bResult = TranslateFromHeader();
	assert(bResult);
	bResult = GetPixelData();
	assert(bResult);
}

Bitmap::Bitmap(const Bitmap & B) : Image(B)
{
	memcpy(&fileHeader, &B.fileHeader, sizeof(fileHeader));
	memcpy(&infoHeader, &B.infoHeader, sizeof(infoHeader));
	pPixelData = make_unique<ImageData<Pixel>>(*B.pPixelData);
}

double Bitmap::GetMSE(const Bitmap & bitmap)
{
	double result = 0.0f;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			result += (pPixelData->GetElem(i, j) - bitmap.pPixelData->GetElem(i, j)) * 
				(pPixelData->GetElem(i, j) - bitmap.pPixelData->GetElem(i, j));
		}
	}
	result /= (height * width);
	return result;
}

Bitmap::Bitmap(const ImageData<Pixel> & inImageData) : Image(inImageData.col, inImageData.row)
{
	memset(&fileHeader, 0, sizeof(fileHeader));
	memset(&infoHeader, 0, sizeof(infoHeader));
	pPixelData = make_unique<ImageData<Pixel>>(inImageData);
}

Bitmap::~Bitmap()
{
	pPixelData.release();
}

bool Bitmap::TranslateFromHeader()
{
	memcpy(&fileHeader, pFileData->data(), sizeof(fileHeader));
	memcpy(&infoHeader, pFileData->data() + sizeof(BITMAPFILEHEADER), sizeof(infoHeader));
	height = infoHeader.biHeight;
	width = infoHeader.biWidth;
	assert(infoHeader.biBitCount == 24);
	return true;
}

bool Bitmap::GetPixelData()
{
	unsigned int byteCount = infoHeader.biBitCount / 8;
	// 保证每行都是4的倍数进行对齐
	unsigned int rowSize = (width * byteCount + 3) / 4 * 4;
	pPixelData = make_unique<ImageData<Pixel>>(height, width);
	unsigned int dataOffset = fileHeader.bfOffBits;
	for (unsigned int i = 0; i < height; ++i) {
		for (unsigned int j = 0; j < width; ++j) {
			byte blue = (*pFileData)[dataOffset + i * rowSize + j * byteCount];
			byte green = (*pFileData)[dataOffset + i * rowSize + j * byteCount + 1];
			byte red = (*pFileData)[dataOffset + i * rowSize + j * byteCount + 2];
			pPixelData->GetElem(height - 1 - i, j) = RGB(red, green, blue);
		}
	}
	return true;
}

bool Bitmap::GetFileData()
{
	MakeHeader();
	assert(infoHeader.biBitCount == 24);
	pFileData = make_unique<vector<byte>>(fileHeader.bfSize);
	
	memcpy(pFileData->data(), &fileHeader, sizeof(fileHeader));
	memcpy(pFileData->data() + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(infoHeader));

	unsigned int byteCount = infoHeader.biBitCount / 8;
	// 保证每行都是4的倍数进行对齐
	unsigned int rowSize = (width * byteCount + 3) / 4 * 4;
	unsigned int dataOffset = fileHeader.bfOffBits;
	for (unsigned int i = 0; i < height; ++i) {
		for (unsigned int j = 0; j < width; ++j) {
			Pixel pixel = pPixelData->GetElem(height - 1 - i, j);
			(*pFileData)[dataOffset + i * rowSize + j * byteCount] = pixel.blue;
			(*pFileData)[dataOffset + i * rowSize + j * byteCount + 1] = pixel.green;
			(*pFileData)[dataOffset + i * rowSize + j * byteCount + 2] = pixel.red;
		}
	}
	return true;
}

bool Bitmap::MakeHeader()
{
	infoHeader.biBitCount = 24;
	infoHeader.biHeight = height;
	infoHeader.biWidth = width;
	infoHeader.biPlanes = 1;
	infoHeader.biSize = sizeof(infoHeader);
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = (width * infoHeader.biBitCount / 8 + 3) / 4 * 4 * height;
	infoHeader.biXPelsPerMeter = 0x00000ec3;
	infoHeader.biYPelsPerMeter = 0x00000ec3;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);
	fileHeader.bfSize = infoHeader.biSizeImage + fileHeader.bfOffBits;
	fileHeader.bfType = 0x4d42;
	return true;
}

Bitmap Bitmap::GetGrayScaleBMP()
{
	Bitmap grayScaleBMP(*this);
	for (unsigned int i = 0; i < height; ++i) {
		for (unsigned int j = 0; j < width; ++j) {
			unsigned int red = pPixelData->GetElem(i, j).red;
			unsigned int green = pPixelData->GetElem(i, j).green;
			unsigned int blue = pPixelData->GetElem(i, j).blue;
			unsigned int gray = (77 * red + 150 * green + 29 * blue + 128) >> 8;
			grayScaleBMP.pPixelData->GetElem(i, j) = Pixel(static_cast<byte>(gray));
		}
	}
	grayScaleBMP.GetFileData();
	return grayScaleBMP;
}


Bitmap & Bitmap::operator=(const Bitmap & B)
{
	Image::operator=(B);
	memcpy(&fileHeader, &B.fileHeader, sizeof(fileHeader));
	memcpy(&infoHeader, &B.infoHeader, sizeof(infoHeader));
	pPixelData.release();
	pPixelData = make_unique<ImageData<Pixel>>(*B.pPixelData);
	return *this;
}