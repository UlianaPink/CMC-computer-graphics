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

	Color(const Color& a) {
		r = a.r;
		g = a.g;
		b = a.b;
	}

	~Color() {};

	const Color operator+(const Color& a) const {
		Color sum;
		sum.r = r + a.r;
		sum.g = g + a.g;
		sum.b = b + a.b;
		return sum;
	}

	const Color operator*(float f) const {
		Color mult;
		mult.r = r * f;
		mult.g = g * f;
		mult.b = b * f;
		return mult;
	}
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

	const Pos operator+(const Pos& a) const {
		Pos sum;
		sum.x = x + a.x;
		sum.y = y + a.y;
		sum.z = z + a.z;
		return sum;
	}

	const Pos operator-(const Pos& a) const {
		Pos dec;
		dec.x = x - a.x;
		dec.y = y - a.y;
		dec.z = z - a.z;
		return dec;
	}

	const Pos operator*(float f) const {
		Pos mult;
		mult.x = x * f;
		mult.y = y * f;
		mult.z = z * f;
		return mult;
	}
};

class PointLight {
protected:
	float intensity;
	Pos pos;

public:
	PointLight(float i, Pos position) {
		intensity = i;
		pos = position;
	}

	PointLight() {
		intensity = 0;
		Pos pos(0, 0, 0);
	}

	PointLight(const PointLight& a) {
		intensity = a.intensity;
		pos = a.pos;
	}

	~PointLight() {};

	float getI() {
		return intensity;
	}

	Pos getPos() {
		return pos;
	}
};

class Sphere {
protected:
	float radius;
	float sparkling;
	Color color;
	Pos centerPos;

public:
	Sphere(float r, float s,  Color c, Pos p) {
		radius = r;
		sparkling = s;
		color = c;
		centerPos = p;
	}

	Sphere() {
		radius = 0;
		sparkling = 0;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		Pos centerPos(0, 0, 0);
	}

	Sphere(const Sphere& a) {
		radius = a.radius;
		sparkling = a.sparkling;
		color = a.color;
		centerPos = a.centerPos;
	}

	~Sphere() {};

	float getRad() {
		return radius;
	}

	float getS() {
		return sparkling;
	}

	Color getColor() {
		return color;
	}

	Pos getPos() {
		return centerPos;
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

float Length(const Pos a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

void SimpleRender(Image& image, Sphere& sphere, PointLight& light) {
	int pointer = 0;
	Pos null;
	Pos camPos;
	Pos canvasPos;
	Pos onSurfacePos;
	Pos normToSphere;
	Pos lightDirect;
	Pos toCentre;
	Pos vectorV;
	Pos reflection;
	float distance = (float)(image.getW() + image.getH()) / 2;
	float c1, c2, c3, t, discr, intense, n, r;
	Color black;
	Color onSurfColor;

	toCentre = camPos - sphere.getPos();

	for (int y = -(image.getH() / 2); y < image.getH() / 2; y++) {
		for (int x = -(image.getW() / 2); x < image.getW() / 2; x++) {
			canvasPos.x = (float)(x);
			canvasPos.y = (float)(y);
			canvasPos.z = distance;

			c1 = Scalar(canvasPos, canvasPos);
			c2 = 2 * Scalar(toCentre, canvasPos);
			c3 = Scalar(toCentre, toCentre) - sphere.getRad() * sphere.getRad();
			discr = c2 * c2 - 4 * c1 * c3;

			if (discr < 0) {
				image.putPixel(black, pointer);
			}
			else { //ITERSECTION
				t = (-c2 - sqrt(discr)) / (2 * c1);
				onSurfacePos = camPos + canvasPos * t;
				normToSphere = onSurfacePos - sphere.getPos();
				normToSphere = normToSphere * (1 / Length(normToSphere));

				intense = 0;
				lightDirect = light.getPos() - onSurfacePos;
				n = Scalar(normToSphere, lightDirect);
				if (n > 0) {
					intense += light.getI() * n / (Length(normToSphere) * Length(lightDirect));
				}

				if (sphere.getS() != -1) {
					vectorV = null - canvasPos;
					reflection = (normToSphere * (2 * n) - lightDirect);
					r = Scalar(reflection, vectorV);
					if (r > 0) {
						intense += light.getI() * pow(r / (Length(reflection) * Length(vectorV)), sphere.getS());
					}
				}

				intense += 0.03;

				if (intense > 1) {
					intense = 1;
				}

				onSurfColor = sphere.getColor() * intense;
				image.putPixel(onSurfColor, pointer);
			}

			pointer++;
		}
	}
}

int main()
{
	Image image(1920, 1080);

	Color blue(175, 240, 240);
	Pos center((float)(0), (float)(0), (float)(2000));
	Sphere sphere((float)(300), (float)(300), blue, center);
	Pos lightPos((float)(1000), (float)(-50), (float)(1500));
	PointLight light(0.9, lightPos);

	std::cout << "rendering...\n";

	SimpleRender(image, sphere, light);

	std::cout << "puting in file...\n";

	image.putInFile();

	std::cout << "done!\n";
	
    return 0;
}