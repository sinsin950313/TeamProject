#pragma once

#include <vector>
#include <string>
#include "SerializeableDataType.h"
#include "TMath.h"

namespace SSB
{
	class Serializeable
	{
	public:
		virtual std::string Serialize() = 0;
		virtual void Deserialize(const char* buffer, int size, int& offset) = 0;
	};
}