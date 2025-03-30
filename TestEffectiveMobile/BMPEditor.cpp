#include "BMPEditor.h"

#include <cstdlib>
#include <iostream>
#include <fstream>

void BMPEditor::readBMP()
{
	std::string file_in;
	std::cout << "Enter input BMP file name: \n";
	getline(std::cin, file_in, '\n');
	
	std::ifstream in(file_in, std::ios::binary);
	if (!(in.is_open()))
	{
		throw std::runtime_error("Error opening file: " + file_in);
	}
	
	in.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));
	if (bmFileHeader.bfType != 0x4D42)
	{
		throw std::runtime_error("Error, file: " + file_in + " is not a bmp file");
	}
	dataOffset = bmFileHeader.bfOffBits;

	in.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));
	bitsPerPixel = bmInfoHeader.biBitCount;
	if (bitsPerPixel != 24 && bitsPerPixel != 32)
	{
		throw std::runtime_error("Error, incorrect color bit depth");
	}
	width = bmInfoHeader.biWidth;
	height = abs(bmInfoHeader.biHeight);
	heightSign = (bmInfoHeader.biHeight > 0) ? true : false;
	stridesSize = (bitsPerPixel * width + 31) / 32 * 4;
	pixelsData.resize(stridesSize * height);
	if (bitsPerPixel == 32)
	{
		in.read(reinterpret_cast<char*>(&bmInfoColors), sizeof(bmInfoColors));
	}
	in.seekg(dataOffset, std::ios::beg);
	in.read(reinterpret_cast<char*>(pixelsData.data()), pixelsData.size());
	
	in.close();

	isBlackAndWhite();
}

void BMPEditor::isBlackAndWhite()
{
	unsigned int index;
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			index = getPixelIndex(i, j);
			blue = pixelsData[index];
			green = pixelsData[index + 1];
			red = pixelsData[index + 2];

			if ((!(blue == 0 && green == 0 && red == 0) &&
				!(blue == 255 && green == 255 && red == 255)))
			{
				throw std::runtime_error("Error, the image is not black and white");
			}
		}
	}
}

unsigned int BMPEditor::getPixelIndex(int x, int y) const
{
	unsigned int index;
	if (heightSign)
	{
		index = y * stridesSize + x * bitsPerPixel / 8;
	}
	else
	{
		index = (height - 1 - y) * stridesSize + x * bitsPerPixel / 8;
	}
	
	return index;
}

void BMPEditor::setPixelColor(int x, int y, unsigned char red, unsigned char green, 
	unsigned char blue, unsigned char opacity)
{
	if (x >= width || y >= height)
	{
		throw std::runtime_error("Error, coordinates out of image range");
	}

	unsigned int index = getPixelIndex(x, y);
	pixelsData[index] = blue;
	pixelsData[index + 1] = green;
	pixelsData[index + 2] = red;
	if (bitsPerPixel == 32)
	{
		pixelsData[index + 3] = opacity;
	}
}

int BMPEditor::getWidth() const
{
	return width;
}

int BMPEditor::getHeight() const
{
	return height;
}

void BMPEditor::writeBMP() const
{
	unsigned int index;
	unsigned char blue;
	for (int j = height - 1; j > 0; j--)
	{
		for (int i = 0; i < width; i++)
		{
			index = getPixelIndex(i, j);
			blue = pixelsData[index];
			std::cout << (blue == 0 ? "#" : " ");
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void BMPEditor::drawLine(int x1, int y1, int x2, int y2)
{
	if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || 
		x1 >= width || y1 >= height || x2 >= width || y2 >= height)
	{
		throw std::runtime_error("Error, line coordinates out of image range");
	}

	int xDistance = x2 - x1;
	int yDistance = y2 - y1;

	int dx = ((xDistance > 0) ? 1 : -1);
	int dy = ((yDistance > 0) ? 1 : -1);
	int dx2;
	int dy2;

	int err = 0;
	int derr;
	int errcheck;

	if (abs(xDistance) >= abs(yDistance))
	{
		derr = abs(yDistance);
		errcheck = abs(xDistance);
		dx2 = dx;
		dy2 = 0;
	}
	else
	{
		derr = abs(xDistance);
		errcheck = abs(yDistance);
		dx2 = 0;
		dy2 = dy;
	}

	setPixelColor(x1, y1, 0, 0, 0, 255);
	int x = x1;
	int y = y1;
	while (x * dx < x2 * dx && y * dy < y2 * dy)
	{
		err += derr;
		if (2 * err > errcheck)
		{
			x += dx;
			y += dy;
			err -= errcheck;
		}
		else
		{
			x += dx2;
			y += dy2;
		}
		setPixelColor(x, y, 0, 0, 0, 255);
	}
	setPixelColor(x2, y2, 0, 0, 0, 255);
}

void BMPEditor::saveBMP() const
{
	std::string file_out;
	std::cout << "Enter output BMP file name: \n";
	getline(std::cin, file_out, '\n');
	
	std::ofstream out(file_out, std::ios::binary);
	if (!(out.is_open()))
	{
		throw std::runtime_error("Error opening file: " + file_out);
	}
	
	out.write(reinterpret_cast<const char*>(&bmFileHeader), sizeof(bmFileHeader));
	out.write(reinterpret_cast<const char*>(&bmInfoHeader), sizeof(bmInfoHeader));
	if (bitsPerPixel == 32)
	{
		out.write(reinterpret_cast<const char*>(&bmInfoColors), sizeof(bmInfoColors));
	}
	out.seekp(dataOffset, std::ios::beg);
	out.write(reinterpret_cast<const char*>(pixelsData.data()), pixelsData.size());

	out.close();
}

BMPEditor::BMPEditor()
{
	readBMP();
}

BMPEditor::~BMPEditor() {}
