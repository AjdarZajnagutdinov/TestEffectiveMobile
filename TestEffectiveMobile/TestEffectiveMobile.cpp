#include "BMPEditor.h"

#include <iostream>

int main()
{
	try
	{
		BMPEditor bmpimage;

		bmpimage.writeBMP();

		int width = bmpimage.getWidth();
		int height = bmpimage.getHeight();
		bmpimage.drawLine(0, 0, width - 1, height - 1);
		bmpimage.drawLine(0, height - 1, width - 1, 0);

		bmpimage.writeBMP();

		bmpimage.saveBMP();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}