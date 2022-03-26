#include "PPM.hpp"
#include <fstream> 

void PPM::writePxielsToFile(const FrameBuffer & buffer, std::string filename)
{
	std::ofstream f(filename);
	const int width = buffer.getWidth();
	const int height = buffer.getHeight();
	const unsigned char*  data = buffer.getData();
	f << "P3" << std::endl;
	f << std::to_string(width) << " " << std::to_string(height) << std::endl;
	f << "255" << std::endl;
	
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int idx = i * width + j;
			idx = idx * 3;
			const unsigned char r = data[idx];
			const unsigned char g = data[idx+1];
			const unsigned char b = data[idx+2];

			f << std::to_string(r) << " ";
			f << std::to_string(g) << " ";
			f << std::to_string(b) << " ";
		}
		f << std::endl;
	}
}

void PPM::writeZBufferToFile(const FrameBuffer & buffer, std::string filename)
{
	std::ofstream f(filename);
	const int width = buffer.getWidth();
	const int height = buffer.getHeight();
	const double* zBuffer = buffer.getZBuffer();
	f << "P3" << std::endl;
	f << std::to_string(width) << " " << std::to_string(height) << std::endl;
	f << "255" << std::endl;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int idx = i * width + j;
			const unsigned char z = static_cast<const unsigned char>(zBuffer[idx] * 255.0);
			f << std::to_string(z) << " ";
			f << std::to_string(z) << " ";
			f << std::to_string(z) << " ";
		}
		f << std::endl;
	}
}
