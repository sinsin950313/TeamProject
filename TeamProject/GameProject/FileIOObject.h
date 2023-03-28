#pragma once

#include <string>

namespace SSB
{
	struct ReadScriptInfo
	{
		int BufferSize;
		char* Pointer;
	};

	class ObjectScriptIO
	{
	private:
		char* _str = nullptr;

	public:
		~ObjectScriptIO();

	private:
		void NewBuffer(int size);

	public:
		ReadScriptInfo Read(std::string fileName);
		void Write(std::string fileName, std::string str);
	};
}
