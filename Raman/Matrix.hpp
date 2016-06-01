#pragma once

#include "Util.h"
#include "Data.hpp"
#include "Structure.h"

template<class _ElemType>
class Matrix
{
public:
	Matrix(const unsigned int & inRow = 0, const unsigned int & inCol = 0) 
		: row(inRow), col(inCol), data(inRow * inCol) { }
	Matrix(const unsigned int & inRow, const unsigned int & inCol, const std::vector<_ElemType> dataVec)
		: row(inRow), col(inCol), data(inRow * inCol, dataVec) { }
	Matrix(const Matrix & B) : row(B.row), col(B.col), data(B.data) { }


	static Matrix RandomMatrix(const unsigned int & inRow, const unsigned int & inCol, const _ElemType & maxNumSubOne)
	{
		Matrix mat(inRow, inCol);
		for (unsigned int i = 0; i < mat.data.size(); ++i) 
			mat.data[i] = rand() % static_cast<int>(maxNumSubOne);
		return mat;
	}
	static Matrix IdentityMatrix(const unsigned int & dimension)
	{
		Matrix mat(dimension, dimension);
		for (unsigned int i = 0; i < dimension; ++i) 
			mat.GetElem(i, i) = 1;
		return mat;
	}

	inline _ElemType & GetElem(const unsigned int & inRow, const unsigned int & inCol)
	{
		//assert(inRow < row && inCol < col);
		return  *(data.pData + inRow * col + inCol);
	}
	inline const _ElemType & GetElem(const unsigned int & inRow, const unsigned int & inCol) const
	{
		//assert(inRow < row && inCol < col);
		return  *(data.pData + inRow * col + inCol);
	}
	inline _ElemType & GetElem(Subscrip2 subscrip)
	{
		//assert(subscrip.y < row && subscrip.x < col);
		return  data[subscrip.L];
	}
	inline const _ElemType & GetElem(Subscrip2 subscrip) const
	{
		//assert(subscrip.y < row && subscrip.x < col);
		return  data[subscrip.L];
	}

	void Print() const
	{
		for (unsigned int r = 0; r < row; ++r) {
			for (unsigned int c = 0; c < col; ++c) 
				std::cout << GetElem(r, c) << "\t";
			std::cout << "\n";
		}
		std::cout << std::endl;
	}
	void SetSubMatrixByB(const Matrix & B, const Subscrip2 & begin)
	{
		for (unsigned int i = 0; i < B.row; ++i) 
			for (unsigned int j = 0; j < B.col; ++j) 
				GetElem(begin.y + i, begin.x + j) = B.GetElem(i, j);
	}

	Matrix GetSubMatrix(const Subscrip2 & begin, const Subscrip2 & end)
	{
		assert(end.y >= begin.y && end.x >= begin.x);
		unsigned int resultRow = end.y - begin.y;
		unsigned int resultCol = end.x - begin.x;
		Matrix result(resultRow, resultCol);
		for (unsigned int i = 0; i < resultRow; ++i)
			for (unsigned int j = 0; j < resultCol; ++j)
				result.GetElem(i, j) = GetElem(begin.y + i, begin.x + j);
		return result;
	}

	Matrix MulByMatrixB(const Matrix & B) const
	{
		assert(col == B.row);
		Matrix result(row, B.col);
		for (unsigned int i = 0; i < row; ++i) {
			for (unsigned int k = 0; k < col; ++k) {
				const auto temp = *(data.pData + i * col + k);
				const auto beginA = result.data.pData + i * B.col;
				const auto beginB = B.data.pData + k * B.col;
				for (unsigned int j = 0; j < B.col; ++j) 
					*(beginA + j) += temp * *(beginB + j);
			}
		}
		return result;
	}
	Matrix MulByMatrix(const Matrix & B, const unsigned int parallelism = 4) const
	{
		assert(col == B.row);
		Matrix result(row, B.col);
		std::vector<thread> threads;
		const unsigned int trueParallelism = min(parallelism, row);
		for (unsigned int threadIndex = 0; threadIndex < trueParallelism; ++threadIndex) {
			threads.push_back(std::thread([=, &result, &B](const Matrix & A) {
				unsigned int len = A.row / trueParallelism;
				unsigned int begin = len * threadIndex;
				unsigned int end = (threadIndex == trueParallelism - 1) ? A.row : len * (threadIndex + 1);
				for (unsigned int i = begin; i < end; ++i) {
					for (unsigned int k = 0; k < A.col; ++k) {
						const auto temp = *(A.data.pData + i * A.col + k);
						const auto beginA = result.data.pData + i * B.col;
						const auto beginB = B.data.pData + k * B.col;
						for (unsigned int j = 0; j < B.col; ++j)
							*(beginA + j) += temp * *(beginB + j);
					}
				}
			}, *this));
		}
		for (auto & elem : threads)
			elem.join();
		return result;
	}
	Matrix MulByMatrixOneToOne(const Matrix & B) const
	{
		assert(row == B.row && col == B.col);
		Matrix result(row, B.col);
		for (unsigned int i = 0; i < row; ++i) 
			for (unsigned int j = 0; j < col; ++j) 
					result.GetElem(i, j) = GetElem(i, j) * B.GetElem(i, j);
		return result;
	}

	bool operator==(const Matrix & B) const
	{
		if (col != B.col || row != B.row)
			return false;
		for (unsigned int i = 0; i < row; ++i) 
			for (unsigned int j = 0; j < col; ++j) 
				if (this->GetElem(i, j) != B.GetElem(i, j))
					return false;
		return true;
	}
	bool operator!=(const Matrix & B) const { return !operator==(B); }

	Matrix operator+(const Matrix & B) const
	{
		assert(row == B.row && col == B.col);
		Matrix result(row, col);
		for (unsigned int i = 0; i < row; ++i) 
			for (unsigned int j = 0; j < col; ++j) 
				result.GetElem(i, j) = GetElem(i, j) + B.GetElem(i, j);
		return result;
	}

	Matrix & operator=(const Matrix & B)
	{
		row = B.row;
		col = B.col;
		data.resize(row * col);
		for (unsigned int i = 0; i < row; ++i) 
			for (unsigned int j = 0; j < col; ++j) 
				this->GetElem(i, j) = B.GetElem(i, j);
		return *this;
	}

	void ForEachElem(
					 Subscrip2 begin = Subscrip2(0, 0),
					 Subscrip2 end = Subscrip2(row, col),
					 std::function<void(const unsigned int i,
										const unsigned int j,
										_ElemType &)> prec  = [](0, 0, _ElemType &) { }
	) {
		for (unsigned int i = begin.y; i < end.y; ++i) {
			for (unsigned int j = begin.x; j < end.x; ++j) {
				prec(i, j, GetElem(i, j));
			}
		}
	}

	unsigned int row;
	unsigned int col;
	Data<_ElemType> data;
	//std::vector<_ElemType> data;
};