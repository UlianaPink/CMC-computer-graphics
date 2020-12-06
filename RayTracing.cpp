#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>

class Color {
public:
	int r;
	int g;
	int b;

	Color(int red, int green, int blue) {
		r = red;
		g = green;
		b = blue;
	}

	Color() {
		r = 0;
		g = 0;
		b = 0;
	}

	~Color() {};
};

class Pos {
public:
	float x;
	float y;
	float z;
	
	Pos(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Pos() {
		x = 0;
		y = 0;
		z = 0;
	}

	Pos(const Pos& a) {
		x = a.x;
		y = a.y;
		z = a.z;
	}

	~Pos() {};
};

class Sphere {
protected:
	float radius;
	Color color;
	Pos centerPos;

public:
	Sphere(float r, Color c, Pos p) {
		radius = r;
		color = c;
		centerPos = p;
	}

	Sphere() {
		radius = 0;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		Pos centerPos(0, 0, 0);
	}

	Sphere(const Sphere& a) {
		radius = a.radius;
		color = a.color;
		centerPos = a.centerPos;
	}

	~Sphere() {};

	Color getColor() {
		return color;
	}

	Pos getPos() {
		return centerPos;
	}

	float getRad() {
		return radius;
	}
};

class Image {
protected:
	int width;
	int height;
	Color* pixelData;

public:
	Image(int w, int h) {
		width = w;
		height = h;
		int bufSize = w * h;
		pixelData = new Color[bufSize];
		Color white(255, 255, 255);
		for (int i = 0; i < bufSize; i++) {
			pixelData[i] = white;
		}
	}

	Image(const Image& a) {
		width = a.width;
		height = a.height;
		int bufSize = width * height;
		pixelData = new Color[bufSize];
		for (int i = 0; i < bufSize; i++) {
			pixelData[i] = a.pixelData[i];
		}
	}

	~Image() {
		delete[] pixelData;
	}

	int getW() {
		return width;
	}

	int getH() {
		return height;
	}

	void putPixel(Color color, int pointer) {
		pixelData[pointer] = color;
	}

	void putPixel(int red, int green, int blue, int pointer) {
		pixelData[pointer].r = red;
		pixelData[pointer].g = green;
		pixelData[pointer].b = blue;
	}

	void putInFile() {
		std::ofstream imageFile("picture.ppm");

		// add the header
		imageFile << "P3" << std::endl;
		imageFile << width << " " << height << std::endl;
		imageFile << "255" << std::endl;

		for (int i = 0; i < width * height; i++) {
			imageFile << pixelData[i].r << " "
				<< pixelData[i].g << " "
				<< pixelData[i].b << std::endl;
		}

		imageFile.close();
	}
};

float Scalar(const Pos a, const Pos b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void SimpleRender(Image& image, Sphere& sphere) {
	int pointer = 0;
	Pos camPos(0, 0, 0);
	Pos canvasPos(0, 0, 0);
	float distance = (float)(image.getW() + image.getH()) / 2;
	float c1, c2, c3, discr;
	Color black;

	for (int y = -(image.getH() / 2); y < image.getH() / 2; y++) {
		for (int x = -(image.getW() / 2); x < image.getW() / 2; x++) {
			canvasPos.x = (float)(x);
			canvasPos.y = (float)(y);
			canvasPos.z = distance;

			c1 = Scalar(canvasPos, canvasPos);
			c2 = 2 * Scalar(sphere.getPos(), canvasPos);
			c3 = Scalar(sphere.getPos(), sphere.getPos()) - sphere.getRad() * sphere.getRad();

			discr = c2 * c2 - 4 * c1 * c3;

			if (discr < 0) {
				image.putPixel(black, pointer);
			}
			else {
				image.putPixel(sphere.getColor(), pointer);
			}
			pointer++;
		}
	}
}

int main()
{
	Image image(1920, 1080);
	
	//create simple scene
	Color blue(175, 240, 240);
	Pos center((float)(0), float(50), (float)(2000));
	Sphere sphere((float)(300), blue, center);

	SimpleRender(image, sphere);

	image.putInFile();

    return 0;
}