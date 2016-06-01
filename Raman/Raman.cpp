#include "Raman.h"
#include "Help.h"

using namespace std;

Raman::Raman(const string & fileName, 
			 const unsigned int inHeight, 
			 const unsigned int inWidth,
			 const double inBeginLambda,
			 const double inEndLambda) 
	: Image(fileName), beginLambda(inBeginLambda), endLambda(inEndLambda)
{
	height = inHeight;
	width = inWidth;
	bool bResult = TranslateFromHeader();
	assert(bResult);
	bResult = GetPixelData();
	assert(bResult);
}
Raman::Raman(const Raman & B) : Image(B)
{
	pPixelData = make_unique<ImageData<Spectrum>>(*B.pPixelData);
}
Raman::Raman(const ImageData<Spectrum> & inImageData) : Image(inImageData.col, inImageData.row)
{
	beginLambda = endLambda = 0.0f;
	pPixelData = make_unique<ImageData<Spectrum>>(inImageData);
}
Raman::~Raman()
{ 
	pPixelData.release();
}

bool Raman::TranslateFromHeader()
{
	return true;
}
bool Raman::GetPixelData()
{
	vector<double> bufferVec;
	double doubleTemp;
	// 快速读取文件中的浮点数
	const char *strFileData = reinterpret_cast<char *>(pFileData->data());
	while (Help::wipAtof(&strFileData, &doubleTemp)) {
		bufferVec.push_back(doubleTemp);
	}
	assert(bufferVec.size() % (height * width) == 0);
	// 计算每个光谱的数据规模
	spectrumSize = bufferVec.size() / (height * width);
	auto iter = bufferVec.begin();
	// 创建height * width个光谱数组
	pPixelData = make_unique<ImageData<Spectrum>>(height, width);
	// 将数据按序存入数组中
	for (unsigned int i = 0; i < width; ++i) {
		for (unsigned int j = 0; j < height; ++j) {
			auto & elem = pPixelData->GetElem(i, j);
			elem.ccdVec.clear();
			elem.ccdVec.reserve(spectrumSize);
			for (unsigned int k = 0; k < spectrumSize; ++k) {
				elem.ccdVec.push_back(*iter);
				iter++;
			}
		}
	}
	return true;
}
bool Raman::GetFileData()
{ 
	return true;
}
bool Raman::MakeHeader()
{
	return true;
}

Bitmap Raman::CreateBitmap(const unsigned int begin, const unsigned int end, Pixel & inPixel)
{
	ImageData<Pixel> pixelData(height, width);
	double maxSum = -999999999.9f;
	double minSum = 999999999.9f;
	// 对begin~end之间的光谱进行积分，并找出最大和最小峰面积
	pPixelData->ForEachElem(Subscrip2(0, 0), Subscrip2(height, width),
							[&](const unsigned int &, const unsigned &, Spectrum & elem) {
		elem.partSum = 0;
		for (unsigned int n = begin; n < end; ++n) {
			elem.partSum += elem.ccdVec[n];
		}
		maxSum = max(elem.partSum, maxSum);
		minSum = min(elem.partSum, minSum);
	});
	// 最大最小归一化处理
	double delta = maxSum - minSum;
	pixelData.ForEachElem(Subscrip2(0, 0), Subscrip2(height, width),
						  [&](const unsigned int & i, const unsigned int &j, Pixel & pixel) {
		double grayScale = (pPixelData->GetElem(i, j).partSum - minSum) / delta;
		pixel = inPixel.MulByDouble(grayScale, inPixel);
	});
	return Bitmap(pixelData);
}

Bitmap Raman::BlendBitmap(const unsigned int begin, const unsigned int end, Pixel & inPixel, Bitmap & oldBitmap)
{
	ImageData<Pixel> pixelData(*oldBitmap.pPixelData);
	double maxSum = -999999999.9f;
	double minSum = 999999999.9f;
	// 对begin~end之间的光谱进行积分，并找出最大和最小峰面积
	pPixelData->ForEachElem(Subscrip2(0, 0), Subscrip2(height, width),
							[&](const unsigned int &, const unsigned &, Spectrum & elem) {
		elem.partSum = 0;
		for (unsigned int n = begin; n < end; ++n) {
			elem.partSum += elem.ccdVec[n];
		}
		maxSum = max(elem.partSum, maxSum);
		minSum = min(elem.partSum, minSum);
	});
	// 最大最小归一化处理
	double delta = maxSum - minSum;
	pixelData.ForEachElem(Subscrip2(0, 0), Subscrip2(height, width),
						  [&](const unsigned int & i, const unsigned int &j, Pixel & pixel) {
		double grayScale = (pPixelData->GetElem(i, j).partSum - minSum) / delta;
		pixel += inPixel.MulByDouble(grayScale, inPixel);
	});
	return Bitmap(pixelData);
}

void Raman::DoNLM(const int d, const int sr, const int h)
{
	ImageData<Spectrum> oriData;
	ImageData<Spectrum> newData;
	newData = *pPixelData;
	// 对原图进行扩展，使边界上的像素点的处理保持一致
	int exR = d + sr;
	oriData = pPixelData->GetExpandImage(exR, exR, ImageData<Spectrum>::exMode::EM_CLAMP,
										 [=](Spectrum & spectrum) {
		// 对每个光谱进行扩展
		Spectrum exSepctrum;
		exSepctrum.ccdVec.resize(spectrum.ccdVec.size() + 2 * (exR * 5));
		for (int i = 0; i < exR; ++i) {
			exSepctrum.ccdVec[i] = spectrum.ccdVec[0];
		}
		for (int i = 0; i < (int)spectrumSize; ++i) {
			exSepctrum.ccdVec[i + exR * 5] = spectrum.ccdVec[i];
		}
		for (int i = 0; i < exR * 5; ++i) {
			exSepctrum.ccdVec[exR * 5 + (int)spectrumSize + i] 
				= spectrum.ccdVec[(int)spectrumSize - 1];
		}
		spectrum = exSepctrum;
	});
	// 对xy平面的扩展
	for (int i = 0; i < (int)width; ++i) {
		for (int j = 0; j < (int)height; ++j) {
			for (int k = 0; k < (int)spectrumSize; ++k) {
				newData.GetElem(i, j).ccdVec[k] = 0.0f;
			}
		}
	}
	int dh = d * 5;
	double phi = 4.0f;
	// 计算高斯权重矩阵
	auto GSWeight = Help::GetWight(d, dh, phi);
	// 开始NLM算法
	// 对width * height * spectrumSize中的所有像素点进行计算
	for (int i = exR; i < (int)width + exR; ++i) {
		for (int j = exR; j < (int)height + exR; ++j) {
			for (int k = exR * 5; k < (int)spectrumSize + exR * 5; ++k) {
				// 在2*sr+1 * 2*sr+1 * (2*sr+1)*5 的窗口进行搜索
				double sumWeight = 0.0f;
				for (int si = i - sr; si <= i + sr; ++si) {
					for (int sj = j - sr; sj <= j + sr; ++sj) {
						for (int sk = k - sr * 5; sk <= k + sr * 5; ++sk) {
							// 计算大小为2*d+1 * 2*d+1 * (2*d+1)*5的图像柱所占权重
							double w = 0.0f;
							for (int di = -d; di <= d; ++di) {
								for (int dj = -d; dj <= d; ++dj) {
									for (int dk = -d * 5; dk <= d * 5; ++dk) {
										double delta = oriData.GetElem(i - di, j - dj).ccdVec[k - dk]
											- oriData.GetElem(si - di, sj - dj).ccdVec[sk - dk];
										w += GSWeight[di + d][dj + d][dk + d * 5]
											* delta * delta;
									}
								}
							}
							// 获得最后的高斯加权距离
							w = exp(-w / (h * h));
							// 计算权重和以便归一化
							sumWeight += w;
							newData.GetElem(i - exR, j - exR).ccdVec[k - exR * 5] += w * oriData.GetElem(si, sj).ccdVec[sk];
						}
					}
				} // end of all search point
				if (sumWeight > 0.0f) {
					// 归一化
					newData.GetElem(i - exR, j - exR).ccdVec[k - exR * 5] /= sumWeight;
				}
			}
			// cout << i - exR + 1 << " " << j - exR + 1 << endl;
		}
	}
	for (int i = 0; i < (int)width; ++i) {
		for (int j = 0; j < (int)height; ++j) {
			for (int k = 0; k < (int)spectrumSize; ++k) {
				pPixelData->GetElem(i, j).ccdVec[k] = newData.GetElem(i, j).ccdVec[k];
			}
		}
	}
}



Raman & Raman::operator=(const Raman & B)
{
	Image::operator=(B);
	pPixelData = make_unique<ImageData<Spectrum>>(*B.pPixelData);
	beginLambda = B.beginLambda;
	endLambda = B.endLambda;
	return *this;
}
