pragma once
#include "Util.h"
#include "Timer.h"
#include "Matrix.hpp"

template<class _ElemType>
class Test
{
public:
	Test(const unsigned int & inTimes = 10) : timer(), timesNum(inTimes) { srand(static_cast<unsigned int>(time(NULL))); }

	template<class _PrePrec, class _Prec, class _SucPrec> 
	void Run(_PrePrec _prePrec, std::vector<_Prec> _precs, _SucPrec _sucPrec)
	{
		testNum = _precs.size();
		Test::Inil();

		for (timesIndex = 0; timesIndex < runningTimeList[0].size(); ++timesIndex) {
			_prePrec();
			unsigned int eachTimeBegin = timesIndex * testNum;
			for (testIndex = 0; testIndex < runningTimeList.size(); ++testIndex) {
				timer.ReStart(); 
				_precs[testIndex](); 
				timer.Stop();
				printf("%4u %c: %fs\n", timesIndex, testIndex + 'A',
						runningTimeList[testIndex][timesIndex] = timer.GetDeltaBySec());

				if (!results.empty() && results[eachTimeBegin + testIndex] != results[eachTimeBegin]) {
					printf("\t%c is wrong answer!\n", 'A' + testIndex);
				}
			}
			_sucPrec();
			std::cout << endl;
		}
	}
	
	void Inil()
	{
		runningTimeList.resize(testNum);
		for (auto & elem : runningTimeList)
			elem.resize(timesNum);
	}
	void DisplayResult()
	{
		unsigned int index = 1;
		for (unsigned int i = 0; i < runningTimeList.size(); ++i) {
			double sum = 0.0f;
			for (const auto & elem : runningTimeList[i]) {
				sum += elem;
			}
			printf("average %c : %lf\n", 'A' + i, sum / timesNum);
		}
	}

	Timer timer;
	unsigned int timesNum;
	unsigned int testNum;
	unsigned int timesIndex;
	unsigned int testIndex;
	std::vector<std::vector<double> > runningTimeList;
	std::vector<_ElemType> operands;
	std::vector<_ElemType> results;
};

