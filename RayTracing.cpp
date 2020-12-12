#include <iostream>
#include <fstream>

float eps  = 0.0001;

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

	void normalize() {
		float norm = sqrt(x*x + y*y + z*z);
		x = x / norm;
		y = y / norm;
	    z = z / norm;
	}

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
public:
	Pos left;
	Pos right;
	float intense;
	int steps = 30;
	float portionOfIntense;
	Pos* lightPoses;

	LightSource(Pos _left, Pos _right, float _intense) {
		left = _left;
		right = _right;
		intense = _intense;
		portionOfIntense = intense / ((float)(steps));
		lightPoses = new Pos[steps];
		float t = 0.0f;
		for (int i = 0; i < steps; i++) {
			lightPoses[i] = left + (right - left) * t;
			t += 0.033333;
		}
	}

	LightSource(const LightSource& a) {
		left = a.left;
		right = a.right;
		intense = a.intense;
		steps = a.steps;
		portionOfIntense = a.portionOfIntense;
		lightPoses = new Pos[steps];
		for (int i = 0; i < steps; i++) {
			lightPoses[i] = a.lightPoses[i];
		}
	}

	~LightSource() {
		delete[] lightPoses;
	}

	Pos getLight(int i) {
		return lightPoses[i];
	}

	float getI() {
		return portionOfIntense;
	}
};

class GraphObject {
public:
	Color color;
	int specular = 0;
	float reflective = 0;

	GraphObject() {};

	virtual ~GraphObject() {};

	Color getColor() {
		return color;
	}

	virtual float intersect(Pos& camPos, Pos& ray) = 0;
	
	virtual Pos getNormal(Pos& onSurf) const = 0;

	int getSpecular() {
		return specular;
	}

	float getRef() {
		return reflective;
	}
};

class Sphere : public GraphObject {
protected:
	float radius;
	Pos centerPos;

public:
	Sphere(float r, int s, Color c, Pos p, float ref) {
		radius = r;
		specular = s;
		color = c;
		centerPos = p;
		reflective = ref;
	}

	Sphere() {
		radius = 0;
		specular = 0;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		Pos centerPos(0, 0, 0);
		reflective = 0;
	}

	Sphere(const Sphere& a) {
		radius = a.radius;
		specular = a.specular;
		color = a.color;
		centerPos = a.centerPos;
		reflective = a.reflective;
	}

	~Sphere() {};

	float getRadius() {
		return radius;
	}

	Pos getNormal(Pos& onSurf) const {
		return onSurf - centerPos;
	}

	float intersect(Pos& camPos, Pos& ray) {
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
protected:
	Pos normal;
	float distance;

public:
	Plane(Pos _normal, int _specular, Color _color, float _distance, float _reflective) {
		normal = _normal;
		specular = _specular;
		color = _color;
		distance = _distance;
		reflective = _reflective;
	}

	~Plane() {};

	Pos getNormal(Pos& onSurf) const {
		return normal;
	}

	float intersect(Pos& camPos, Pos& ray) {
		return -((distance + Scalar(camPos, normal)) / Scalar(ray, normal));
	}
};

Pos Cross(Pos a, Pos b) {
	Pos cross;
	cross.x = a.y * b.z - a.z * b.y;
	cross.y = a.z * b.x - a.x * b.z;
	cross.z = a.x * b.y - a.y * b.x;
	return cross;
}

class Triangle : public GraphObject {
protected:
	Pos* vertex;

public:
	Triangle(Pos vert1, Pos vert2, Pos vert3, int _specular, Color _color, float _reflective) {
		vertex = new Pos[3];
		vertex[0] = vert1;
		vertex[1] = vert2;
		vertex[2] = vert3;
		specular = _specular;
		color = _color;
		reflective = _reflective;
	}

	Triangle() {
		vertex = new Pos[3];
		specular = 0;
		Color color;
		reflective = 0.0f;
	};
	
	Triangle(const Triangle& a) {
		vertex = new Pos[3];
		for (int i = 0; i < 3; i++) {
			vertex[i] = a.vertex[i];
		}
		specular = a.specular;
		color = a.color;
		reflective = a.reflective;
	}

	~Triangle() {
		delete[] vertex;
	}

	Pos getNormal(Pos& onSurf) const {
		Pos vectorAB = vertex[0] - vertex[1];
		Pos vectorBC = vertex[1] - vertex[2];
		return Cross(vectorAB, vectorBC);
	}


	float intersect(Pos& camPos, Pos& ray) {
		Pos e1 = vertex[1] - vertex[0];
		Pos e2 = vertex[2] - vertex[0];
		Pos x = Cross(ray, e2);
		float det = Scalar(e1, x);

		if (det > - eps && det < eps) {
			return -1;
		}

		Pos s = camPos - vertex[0];
		float u = (1.0 / det) * Scalar(s, x);

		if (u < 0 || u > 1) {
			return -1;
		}

		Pos y = Cross(s, e1);
		float v = (1.0 / det) * Scalar(ray, y);

		if (v < 0 || v > 1 || u + v > 1) {
			return -1;
		}

		return (1.0 / det) * Scalar(e2, y);
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

float Length(const Pos a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

GraphObject* CheckForIntersect(float& t, GraphObject** array, Pos camPos, Pos ray, float tMin, float tMax) {
	GraphObject* closestObj = nullptr;
	float tempT;
	float closestT = 1000000;

	for (int i = 0; i < 3; i++) {
		tempT = array[i]->intersect(camPos, ray);
		if ((tempT - tMin > eps) && (tMax - tempT > eps) && (closestT - tempT > eps)) {
			closestT = tempT;
			closestObj = array[i];
		}
	}
	
	t = closestT;
	return closestObj;
}

Pos ReflectRay(Pos ray, Pos normal) {
	return normal * (2 * Scalar(normal, ray)) - ray;
}

float ComputeLight(LightSource& longLight, GraphObject** array, Pos onSurf, Pos normal, Pos minusRay, GraphObject* closestObj) {
	float n, r, intense = 0;
	float shadowT;
	Pos lightDirect, refection;
	Pos null;
	GraphObject* shadowObj = nullptr;

	for (int i = 0; i < longLight.steps; i++) {
		lightDirect = longLight.getLight(i) - onSurf;
		lightDirect.normalize();
		
		//shadow
		shadowObj = CheckForIntersect(shadowT, array, onSurf, lightDirect, 0.1, 1000000);
		if (shadowObj != nullptr) {
			continue;
		}
		
		//diffuse light
	    n = Scalar(normal, lightDirect);
		if (n > eps) {
			intense += longLight.getI() * n / (Length(normal) * Length(lightDirect));
		}

		//sparkling
	    if ((closestObj->getSpecular() != 0)) {
			Pos reflection = ReflectRay(lightDirect, normal); 
			reflection.normalize();
			r = Scalar(reflection, minusRay);
			if (r > eps) {
				intense += longLight.getI() * pow((r / (Length(reflection) * Length(minusRay))), closestObj->getSpecular());
			}
		}
	}

	if (intense > 0.8) {
		intense = 0.8;
	}

	return intense;
}

Color ColorToPut(float ambientLight, Pos& camPos, Pos& ray, GraphObject** array, LightSource& longLight, float depth) {
	float closestT;
	float i;
	Color black;
	Color tempColor, reflectedColor;
	Pos refRay;
	GraphObject* closestObj;
	Pos null;

	closestObj = CheckForIntersect(closestT, array, camPos, ray, 0.0, 1000000);

	if (closestObj == nullptr) {
		return black;
	}
	
	Pos onSurf = camPos + ray * closestT;
	Pos normal = closestObj->getNormal(onSurf);
	normal.normalize();

	tempColor = closestObj->getColor() * (ComputeLight(longLight, array, onSurf, normal, null - ray, closestObj) + ambientLight);

	if ((depth <= 0) || (closestObj->getRef() < 0)) {
		return tempColor;
	}
	
	refRay = ReflectRay(null - ray, normal);

	reflectedColor = ColorToPut(ambientLight, onSurf, refRay, array, longLight, depth - (float)(1));
	
	return tempColor * (1 - closestObj->getRef()) + reflectedColor * (closestObj->getRef());
}

void SimpleRender(Image& image, GraphObject** array, LightSource& longLight) {
	int pointer = 0;
	float anti = 0.3f;
	float ambientLight = 0.2f;
	Color totalColor, tempColor;
	float r, g, b;
	Pos camPos;
	float depth = 5.0f;

	float distance = (float)(image.getW() + image.getH()) / 2;

	for (int y = -(image.getH() / 2); y < image.getH() / 2; y++) {
		for (int x = -(image.getW() / 2); x < image.getW() / 2; x++) {
			
			r = 0;
			g = 0;
			b = 0;

			for (float newY = (float)(y); newY < (float)(y + 1); newY = newY + anti) {
				for (float newX = (float)(x); newX < (float)(x + 1); newX = newX + anti) {

					Pos newRay(newX, -newY, distance);
					newRay.normalize();
					tempColor = ColorToPut(ambientLight, camPos, newRay, array, longLight, depth);
					r += tempColor.r * anti*anti;
					g += tempColor.g * anti*anti;
					b += tempColor.b * anti*anti;
				}
			}
			
			totalColor.r = (int)(r);
			totalColor.g = (int)(g);
			totalColor.b = (int)(b);

			image.putPixel(totalColor, pointer);

			pointer++;
		}
	}
}

int main()
{
	Image image(1500, 1000);

	Color blue(175, 240, 240);
	Color white(255, 255, 255);
	Color purple(200, 162, 200);
	Color teal(0, 128, 128);

	Pos center1((float)(-500), (float)(-100), (float)(2500));
	Pos center2((float)(500), (float)(-100), (float)(2500));
	Pos normal(0, 1.0f, 0);
	Pos light1((float)(-300), (float)(500), (float)(1600));
	Pos light2(300.0, 500.0, 1600.0);

	Sphere blueSphere((float)(300), 1000, blue, center1, (float)(1.0));
	Sphere purpleSphere((float)(300), 1000, purple, center2, (float)(0.5));
	Plane whitePlane(normal, 1000, white, (float)(400), (float)(-1.0));


	GraphObject** array = new GraphObject * [3];
	array[0] = new Sphere(blueSphere);
	array[1] = new Sphere(purpleSphere);
	array[2] = new Plane(whitePlane);

	LightSource longLight(light1, light2, 0.5f);

	std::cout << "rendering...\n";
	
	SimpleRender(image, array, longLight);

	std::cout << "puting in file...\n";

	image.putInFile();

	std::cout << "done!\n";
	
    return 0;
}