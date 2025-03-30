#ifndef BMPEditor_H
#define BMPEditor_H

#include <windows.h>
#include <vector>
#include <string>

class BMPEditor
{
private:
	BITMAPFILEHEADER bmFileHeader;
	BITMAPINFOHEADER bmInfoHeader;
	RGBQUAD bmInfoColors[4];
	
	int dataOffset;
	int width;
	int height;
	bool heightSign;
	int bitsPerPixel;
	int stridesSize;
	std::vector<unsigned char> pixelsData;

	void readBMP();
	void isBlackAndWhite();
	unsigned int getPixelIndex(int x, int y) const;
	void setPixelColor(int x, int y, unsigned char red, unsigned char green, 
		unsigned char blue, unsigned char opacity);

public:
	int getWidth() const;
	int getHeight() const;
	void writeBMP() const;
	void drawLine(int x1, int y1, int x2, int y2);
	void saveBMP() const;
	
	BMPEditor();
	~BMPEditor();
};

#endif BMPEditor_H