#include <iostream>
#include <fstream>

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

float Scalar(const Pos a, const Pos b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

class LightSource {
protected:
	int type; //0 for point light, 1 for directional light
	Pos lightPos;
	float intense;

public:
	LightSource(int _type, Pos _lightPos, float _intense) {
		type = _type;
		lightPos = _lightPos;
		intense = _intense;
	}

	LightSource() {
		type = 0;
		Pos lightPos;
		intense = 0;
	}

	LightSource(const LightSource& a) {
		type = a.type;
		lightPos = a.lightPos;
		intense = a.intense;
	}

	~LightSource() {};

	int getType() {
		return type;
	}

	Pos getPos() {
		return lightPos;
	}

	float getI() {
		return intense;
	}
};

class GraphObject {
public:
	Color color;

	GraphObject() {};

	virtual ~GraphObject() {};

	virtual Color getColor() const = 0;

	virtual float intersect(Pos& camPos, Pos& ray) const = 0;
};

class Sphere : public GraphObject {
protected:
	float radius;
	int specular;
	Pos centerPos;

public:
	Sphere(float r, int s, Color c, Pos p) {
		radius = r;
		specular = s;
		color = c;
		centerPos = p;
	}

	Sphere() {
		radius = 0;
		specular = 0;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		Pos centerPos(0, 0, 0);
	}

	Sphere(const Sphere& a) {
		radius = a.radius;
		specular = a.specular;
		color = a.color;
		centerPos = a.centerPos;
	}

	~Sphere() {};

	float getRadius() {
		return radius;
	}

	int getSpecular() {
		return specular;
	}

	Color getColor() const {
		return color;
	}

	Pos getCenter() {
		return centerPos;
	}

	float intersect(Pos& camPos, Pos& ray) const {
		Pos toCentre = camPos - centerPos;
		
		float c1 = Scalar(ray, ray);
		float c2 = (2 * Scalar(toCentre, ray));
		float c3 = Scalar(toCentre, toCentre) - radius * radius;
		float discr = c2 * c2 - 4 * c1 * c3;
		
		if (discr < 0) {
			return -1;
		}
		
		float t = (-c2 + sqrt(discr)) / (2 * c1);
		if (t > (-c2 - sqrt(discr)) / (2 * c1)) {
			t = (-c2 - sqrt(discr)) / (2 * c1);
		}
		
		return t;
	}
};

class Plane : public GraphObject {

};

class Triangle : public GraphObject {

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

float Length(const Pos a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Sphere* CheckForIntersect(float& t, Sphere* array, Pos camPos, Pos ray, float tMin, float tMax) {
	Sphere* closestSphere = nullptr;
	float tempT;
	float closestT = 100000;

	for (int i = 0; i < 4; i++) {
		tempT = array[i].intersect(camPos, ray);
		if (tempT != -1) {
			if ((tempT > tMin) && (tempT < tMax) && (tempT < closestT)) {
				closestT = tempT;
				closestSphere = &array[i];
			}
		}
	}

	t = closestT;
	return closestSphere;
}

float ComputeLight(LightSource* arrayL, Sphere* array, Pos onSurf, Pos normal, Pos minusRay, Sphere* closestSphere) {
	float n, r, intense = 0;
	float shadowT;
	Pos lightDirect, refection;
	Pos null;
	Sphere* shadowSphere = nullptr;

	for (int i = 0; i < 2; i++) {
		if (arrayL[i].getType() == 1) {
			//direct light
			lightDirect = arrayL[i].getPos();
		}
		else {
			//point light
			lightDirect = arrayL[i].getPos() - onSurf;
		}

		//shadow
		shadowSphere = CheckForIntersect(shadowT, array, onSurf, lightDirect, 0, 100000);
		if (shadowSphere != nullptr) {
			continue;
		}
		
		//diffuse light
	    n = Scalar(normal, lightDirect);
		if (n > 0) {
			intense += arrayL[i].getI() * n / (Length(normal) * Length(lightDirect));
		}

		//sparkling
		if ((closestSphere->getSpecular() != 0)) {
			Pos reflection = normal * (2 * n) - lightDirect;
			r = Scalar(reflection, minusRay);
			if (r > 0) {
				intense += arrayL[i].getI() * pow((r / (Length(reflection) * Length(minusRay))), closestSphere->getSpecular());
			}
		}
	}

	if (intense > 0.8) {
		intense = 0.8;
	}

	return intense;
}

Color ColorToPut(float ambientLight, Pos& camPos, Pos& ray, Sphere* array, LightSource* arrayL) {
	float closestT;
	Color black;
	Sphere* closestSphere;
	Pos null;

	closestSphere = CheckForIntersect(closestT, array, camPos, ray, 0, 100000);

	if (closestSphere == nullptr) {
		return black;
	}
	
	Pos onSurf = camPos + ray * closestT;
	Pos normal = onSurf - closestSphere->getCenter();
	normal = normal * (1 / Length(normal));

	return closestSphere->getColor() * (ComputeLight(arrayL, array, onSurf, normal, null - ray, closestSphere) + ambientLight);
}

void SimpleRender(Image& image, Sphere* array, LightSource* arrayL) {
	int pointer = 0;
	float ambientLight = 0.2f;
	Pos camPos;

	float distance = (float)(image.getW() + image.getH()) / 2;

	for (int y = -(image.getH() / 2); y < image.getH() / 2; y++) {
		for (int x = -(image.getW() / 2); x < image.getW() / 2; x++) {

			Pos newRay((float)(x), -(float)(y), distance);
			
			image.putPixel(ColorToPut(ambientLight, camPos, newRay, array, arrayL), pointer);

			pointer++;
		}
	}
}

int main()
{
	Image image(1920, 1080);

	Color blue(175, 240, 240);
	Color pink(255, 192, 203);
	Color yellow(255, 253, 208);
	Color white(255, 255, 255);

	Pos center1((float)(0), (float)(0), (float)(2000));
	Pos center2((float)(-700), (float)(50), (float)(1700));
	Pos center3((float)(400), (float)(-50), (float)(3000));
	Pos center4((float)(-100), (float)(-700), (float)(2200));

	Pos light1((float)(0), (float)(300), (float)(1600));
	Pos light2((float)(-300), (float)(300), (float)(2000));

	Sphere blueSphere((float)(300), 10, blue, center1);
	Sphere pinkSphere((float)(300), 500, pink, center2);
	Sphere yellowSphere((float)(300), 1000, yellow, center3);
	Sphere whiteSphere((float)(500), 1000, white, center4);

	LightSource rightPointLight(0, light1, (float)(0.6));
	LightSource leftPointLight(0, light2, (float)(0.2));

	Sphere* array = new Sphere[4];
	array[0] = blueSphere;
	array[1] = pinkSphere;
	array[2] = yellowSphere;
	array[3] = whiteSphere;

	LightSource* arrayOfLight = new LightSource[2];
	arrayOfLight[0] = rightPointLight;
	arrayOfLight[1] = leftPointLight;

	std::cout << "rendering...\n";
	
	SimpleRender(image, array, arrayOfLight);

	std::cout << "puting in file...\n";

	image.putInFile();

	std::cout << "done!\n";
	
    return 0;
}