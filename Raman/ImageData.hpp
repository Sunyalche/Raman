#pragma once

#include "Util.h"
#include "Matrix.hpp"
#include "Structure.h"

template<class _PixelType>
class ImageData : public Matrix<_PixelType>
{
public:
	enum exMode
	{
		EM_CLAMP
	};

	ImageData(const unsigned int & inHeight = 0, const unsigned int & inWidth = 0)
		: Matrix(inHeight, inWidth), realBegin(0, 0), realEnd(inHeight, inWidth){ }
	ImageData(const ImageData & B) : Matrix(B), realBegin(B.realBegin), realEnd(B.realEnd) { }

	ImageData GetExpandImage(unsigned int exHeight, 
							 unsigned int exWidth, 
							 exMode exMode = EM_CLAMP, 
							 std::function<void(_PixelType &)> precForEachElem = [](_PixelType &) { } )
	{
		ImageData expandImage(Height() + 2 * exHeight, Width() + 2 * exWidth);
		expandImage.SetSubMatrixByB(*this, Subscrip2(exHeight, exWidth));
		expandImage.realBegin = Subscrip2(exHeight, exWidth);
		expandImage.realEnd = Subscrip2(Height() + exHeight, Width() + exWidth);
		expandImage.FillExpandedArea(exMode, precForEachElem);
		return expandImage;
	}

	void FillExpandedArea(exMode exMode, std::function<void(_PixelType &)> precForEachElem)
	{
		for (unsigned int i = RealBeginY(); i < RealEndY(); ++i) {
			for (unsigned int j = RealBeginX(); j < RealEndX(); ++j) {
				precForEachElem(GetElem(i, j));
			}
		}

		switch (exMode) {
		case EM_CLAMP:{
			FillExpandClamp();
			break;
		}
		default:
			assert(0);
			break;
		}
	}

	ImageData Blur2(const unsigned int & radius)
	{
		ImageData expandImage(GetExpandImage(radius, radius));
		ImageData * pData = &expandImage;
		ImageData resultHori(expandImage);
		ImageData result(RealHeight(), RealWidth());
		pData->ForEachElem(Subscrip2(0, pData->RealBeginX()), Subscrip2(pData->Height(), pData->RealEndX()),
						   [&](const unsigned int & i, const unsigned int & j, Pixel & pixel) {
			double sum = 0;
			for (unsigned int m = j - radius, end = j + radius; m < end; ++m) {
				sum += pData->GetElem(i, m);
			}
			sum = sum / (radius * 2 + 1);
			resultHori.GetElem(i, j) = _PixelType(sum);
		});
		pData = &resultHori;
		pData->ForEachElem(pData->realBegin, pData->realEnd,
						   [&](const unsigned int & i, const unsigned int & j, Pixel & pixel) {
			double sum = 0;
			for (unsigned int n = i - radius, end = i + radius; n < end; ++n) {
				sum += pData->GetElem(n, j);
			}
			sum = sum / (radius * 2 + 1);
			result.GetElem(i - radius, j - radius) = _PixelType(sum);
		});
		return result;
	}

	_PixelType * GetDataMemAddr() { return data.pData; }

	ImageData & operator=(const ImageData & B)
	{
		row = B.row;
		col = B.col;
		data.resize(row * col);
		for (unsigned int i = 0; i < row; ++i)
			for (unsigned int j = 0; j < col; ++j)
				this->GetElem(i, j) = B.GetElem(i, j);
		return *this;
	}

private:
	void FillRect(Subscrip2 begin, 
				  Subscrip2 end,
				  std::function<_PixelType(const unsigned int &, const unsigned int &)> Func)
	{
		for (unsigned int i = begin.y; i < end.y; ++i) {
			for (unsigned int j = begin.x; j < end.x; ++j) {
				GetElem(i, j) = Func(i, j);
			}
		}
	}

	void FillExpandClamp()
	{
		FillRect(Subscrip2(0, RealBeginX()), Subscrip2(RealBeginY(), RealEndX()),
				 [&](const unsigned int &, const unsigned int & j)->_PixelType {
			return GetElem(RealBeginY(), j);
		});
		FillRect(Subscrip2(RealEndY(), RealBeginX()), Subscrip2(Height(), RealEndX()),
				 [&](const unsigned int &, const unsigned int & j)->_PixelType {
			return GetElem(RealEndY() - 1, j);
		});
		FillRect(Subscrip2(0, 0), Subscrip2(Height(), RealBeginX()),
				 [&](const unsigned int & i, const unsigned int &)->_PixelType {
			return GetElem(i, RealBeginX());
		});
		FillRect(Subscrip2(0, RealEndX()), Subscrip2(Height(), Width()),
				 [&](const unsigned int & i, const unsigned int &)->_PixelType {
			return GetElem(i, RealEndX() - 1);
		});
	}
public:
	inline const unsigned int Height() { return row; }
	inline const unsigned int Width() { return col; }
	inline const unsigned int RealBeginX() { return realBegin.x; }
	inline const unsigned int RealBeginY() { return realBegin.y; }
	inline const unsigned int RealEndX() { return realEnd.x; }
	inline const unsigned int RealEndY() { return realEnd.y; }
	inline const unsigned int RealHeight() { return realEnd.y - realBegin.y; }
	inline const unsigned int RealWidth() { return realEnd.x - realBegin.x; }
	Subscrip2 realBegin;
	Subscrip2 realEnd;
};