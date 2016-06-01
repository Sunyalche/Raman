#pragma once

#include "Util.h"

namespace Help
{
	static const double pi = 3.1415926;

	unsigned int GetFileLength(std::ifstream & file);
	std::unique_ptr<std::vector<byte>> ReadFile(const std::string & fileName);
	bool WriteFile(const std::string &fileName, 
				   const std::unique_ptr<std::vector<byte>> & pFileData);

	bool wipAtof(const char ** str, double * const result);
	std::vector<std::vector<std::vector<double>>>  GetWight(const int r, const int rh, const double phi);
}

