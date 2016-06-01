#include "Help.h"

using namespace std;

namespace Help
{
	unsigned int GetFileLength(ifstream & file)
	{
		file.seekg(0, ios::end);
		unsigned int size = static_cast<unsigned int>(file.tellg());
		file.seekg(0, ios::beg);
		return size;
	}
	unique_ptr<vector<byte>> ReadFile(const string & fileName)
	{
		ifstream inFile;
		inFile.open(fileName, ios::binary);
		auto fileSize = GetFileLength(inFile);
		unique_ptr<vector<byte>> pFileData(new vector<byte>(fileSize));
		pFileData->reserve(fileSize);
		inFile.read(reinterpret_cast<char *>(pFileData->data()), fileSize);
		inFile.close();
		return pFileData;
	}
	bool WriteFile(const string & fileName, const unique_ptr<vector<byte>> & pFileData)
	{
		ofstream outFile;
		outFile.open(fileName, ios::binary);
		outFile.write(reinterpret_cast<char *>(pFileData->data()), pFileData->size());
		bool result = !(outFile.bad() | outFile.fail());
		outFile.close();
		return result;
	}

	bool wipAtof(const char ** str, double * const result)
	{
		if (**str != ' ' && **str != '-')
			return false;
		const char * c = *str + 1;
		*result = *c - '0';
		c+=2;
		*result += (*(c++) - '0') * 0.1;
		*result += (*(c++) - '0') * 0.01;
		*result += (*(c++) - '0') * 0.001;
		*result += (*(c++) - '0') * 0.0001;
		*result += (*(c++) - '0') * 0.00001;
		*result += (*(c++) - '0') * 0.000001;
		*result += (*(c++) - '0') * 0.0000001;
		*result += (*(c++) - '0') * 0.00000001;
		c++;
		double p = (*c == '-')? 0.1 : 10;
		c++;
		int n = *(c++) - '0';
		n = n * 10 + *(c++) - '0';
		n = n * 10 + *(c++) - '0';
		for (byte i = 0; i < n; ++i)
			*result *= p;
		*result = (**str == '-')? -*result : *result;
		*str += 17;
		return true;
	}
	vector<vector<vector<double>>> GetWight(const int r, const int rh, const double phi)
	{
		vector<vector<vector<double>>> weight(2 * r + 1, 
											  vector<vector<double>>(2 * r + 1, vector<double>(2 * rh + 1)));

		double a = 1 / (phi *sqrt(2 * pi));
		for (int i = -r; i <= r; ++i) {
			for (int j = -r; j <= r; ++j) {
				for (int k = -r * 5; k <= rh; ++k) {
					weight[r - i][r - j][rh - k] = a * exp(-(i * i + j * j + k *k )/ ( 2 * phi * phi));
				}
			}
		}
		// ¹éÒ»»¯
		double sum = 0.0f;
		for (int i = 0; i < 2 * r + 1; ++i) {
			for (int j = 0; j < 2 * r + 1; ++j) {
				for (int k = 0; k < 2 * rh + 1; ++k) {
					sum += weight[i][j][k];
				}
			}
		}
		for (int i = 0; i < 2 * r + 1; ++i) {
			for (int j = 0; j < 2 * r + 1; ++j) {
				for (int k = 0; k < 2 * r * 5 + 1; ++k) {
					weight[i][j][k] /= sum;
				}
			}
		}
		return weight;
	}
}
