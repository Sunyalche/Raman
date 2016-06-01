#pragma once
template<class _ElemType>
struct Data
{
	Data(const unsigned int & inSize) : _size(inSize)
	{
		pData = new _ElemType[_size];
	};
	Data(const unsigned int & inSize, const _ElemType & data) : _size(inSize)
	{
		pData = new _ElemType[_size];
		for (unsigned int i = 0; i < _size; ++i) {
			pData[i] = data;
		}
	};
	Data(const std::vector<_ElemType> & dataVec) : _size(data.size())
	{
		pData = new _ElemType[_size];
		memcpy(pData, dataVec.data(), _size * sizeof(pData[0]));
	}
	Data(const unsigned int inSize, const std::vector<_ElemType> & dataVec) : _size(inSize)
	{
		pData = new _ElemType[_size];
		INT32 delta = _size - dataVec.size();
		if (delta <= 0) {
			memcpy(pData, dataVec.data(), _size * sizeof(pData[0]));
		}  else {
			memcpy(pData, dataVec.data(), dataVec.size() * sizeof(pData[0]));
			memset(pData + dataVec.size(), 0, delta * sizeof(pData[0]));
		}
	}
	Data(const Data & B)
	{
		_size = B._size;
		pData = new _ElemType[_size];
		memcpy(pData, B.pData, _size * sizeof(pData[0]));
	}

	~Data()
	{
		delete[] pData;
	}

	inline _ElemType & operator[](const int & i) { return pData[i]; }
	inline const _ElemType & operator[](const int & i) const { return pData[i]; }

	void resize(const unsigned int & newSize) { _size = newSize; delete[] pData; pData = new _ElemType[_size]; }
	const unsigned int & size() const { return _size; }

	Data & operator=(const Data & B)
	{
		_size = B._size;
		pData = new _ElemType[_size];
		memcpy(pData, B.pData, _size * sizeof(pData[0]));
		return *this;
	}

	_ElemType * pData;
	unsigned int _size;
};

