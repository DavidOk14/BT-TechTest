#pragma once

// C++ Includes
#include <string>
#include <vector>

namespace lve
{
	class LvePipeline
	{
	public:
		LvePipeline(const std::string& vertFilePath, const std::string& fragFilePath);
		//~LvePipeline();

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}
