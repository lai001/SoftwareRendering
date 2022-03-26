#pragma once
#include <string>

#include "FrameBuffer.hpp"

class PPM
{
public:
	static void writePxielsToFile(const FrameBuffer& buffer, std::string filename);
	static void writeZBufferToFile(const FrameBuffer& buffer, std::string filename);
};
