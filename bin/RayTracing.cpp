#include <iostream>
#include <fstream>

float eps = 0.0001f;

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
		mult.r = (int)((float)(r) * f);
		mult.g = (int)((float)(g) * f);
		mult.b = (int)((float)(b) * f);
		return mult;
	}
};

class Pos { //вектор или точка в пространстве
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

	void normalize() { //нормирование вектора
		float norm = sqrt(x * x + y * y + z * z);
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

float Scalar(const Pos a, const Pos b) { //скалярное произведение
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//протяжённый источник освещения, представлен отрезком
class LightSource { 
public:
	Pos left; //начало источника
	Pos right; //конец источника
	float intense; //интенсивность света 
	int steps = 25; //количество разбиений сетки источника
	float portionOfIntense; //количество интенсивности в одной точке источника
	Pos* lightPoses; //массив для точек из сетки

	LightSource(Pos _left, Pos _right, float _intense) {
		left = _left;
		right = _right;
		intense = _intense;
		portionOfIntense = intense / ((float)(steps));
		lightPoses = new Pos[steps];
		float t = 0.0f;
		for (int i = 0; i < steps; i++) {
			lightPoses[i] = left + (right - left) * t;
			t += 0.04f;
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

class GraphObject { //родитель всех графических объектов
public:
	Color color;
	int specular = 0; //коэффициент бликов
	float reflective = 0; //коэффициент отражения
	float gloss = 0; //коэффициент шероховатости (для нечётких отражений)

	GraphObject() {};

	virtual ~GraphObject() {};

	Color getColor() {
		return color;
	}

	float getGloss() {
		return gloss;
	}

	//проверка на пересечение с объектом. Возвращает параметр t для уравнения луча, чтобы найти точку пересечения.
	//camPos - позиция камеры (начальный вектор для луча), ray - направление луча
	virtual float intersect(Pos& camPos, Pos& ray) = 0; 

	//возвращает нормаль к поверхности объекта в заданной точке onSurf
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
	Pos centerPos; //центр сферы

public:
	Sphere(float r, int s, Color c, Pos p, float ref, float _gloss) {
		radius = r;
		specular = s;
		color = c;
		centerPos = p;
		reflective = ref;
		gloss = _gloss;
	}

	~Sphere() {};

	float getRadius() {
		return radius;
	}

	Pos getNormal(Pos& onSurf) const {
		return onSurf - centerPos;
	}

	//проверка на пересечение луча со сферой - решение квадратного уравнения
	float intersect(Pos& camPos, Pos& ray) {
		Pos toCentre = camPos - centerPos;

		float c1 = Scalar(ray, ray);
		float c2 = (2 * Scalar(toCentre, ray));
		float c3 = Scalar(toCentre, toCentre) - radius * radius;
		float discr = c2 * c2 - 4 * c1 * c3;

		//Если из любой функции intersect вернуть -1, то пересечения нет
		if (discr < 0) {
			return -1;
		}

		//нам нужно вернуть только меньшее из значений t
		float t = (-c2 + sqrt(discr)) / (2 * c1);
		if (t > (-c2 - sqrt(discr)) / (2 * c1)) {
			t = (-c2 - sqrt(discr)) / (2 * c1);
		}

		return t;
	}
};

class Plane : public GraphObject {
protected:
	Pos normal; //нормаль к плоскости
	float distance; //расстояние от плоскости до начала координат вдоль положительного направления нормали

public:
	Plane(Pos _normal, int _specular, Color _color, float _distance, float _reflective, float _gloss) {
		normal = _normal;
		specular = _specular;
		color = _color;
		distance = _distance;
		reflective = _reflective;
		gloss = _gloss;
	}

	~Plane() {};

	Pos getNormal(Pos& onSurf) const {
		return normal;
	}

	float intersect(Pos& camPos, Pos& ray) {
		return -((distance + Scalar(camPos, normal)) / Scalar(ray, normal));
	}
};

Pos Cross(Pos a, Pos b) { //векторное произведение
	Pos cross;
	cross.x = a.y * b.z - a.z * b.y;
	cross.y = a.z * b.x - a.x * b.z;
	cross.z = a.x * b.y - a.y * b.x;
	return cross;
}

class Image {
protected:
	int width;
	int height;
	Color* pixelData; //указатель на массив пикселей

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

	//запись в файл
	void putInFile() {
		//вставьте удобный Вам путь, чтобы сохранить файл
		std::ofstream imageFile("picture.ppm");

		//добавляем в файл заголовок для формата .ppm
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

float Length(const Pos a) { //длина вектора
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

//проверка для каждого объекта в сцене, есть ли с ним пересчение.
//если пересечение есть, то возвращаем указатель на объект,
//который ближе всего
GraphObject* CheckForIntersect(float& t, GraphObject** array, Pos camPos, Pos ray, float tMin, float tMax) {
	GraphObject* closestObj = nullptr;
	float tempT;
	float closestT = 1000000;

	for (int i = 0; i < 6; i++) {
		tempT = array[i]->intersect(camPos, ray);
		if ((tempT - tMin > eps) && (tMax - tempT > eps) && (closestT - tempT > eps)) {
			closestT = tempT;
			closestObj = array[i];
		}
	}

	t = closestT;
	return closestObj;
}

Pos ReflectRay(Pos ray, Pos normal) { //отражает луч вдоль нормали
	return normal * (2 * Scalar(normal, ray)) - ray;
}

//расчёт освещённости для пикселя, возвращает интенсивность отражённого в точке onSurf света в переменной intense
float ComputeLight(LightSource& longLight, GraphObject** array, Pos onSurf, Pos normal, Pos minusRay, GraphObject* closestObj) {
	float n, r, intense = 0;
	float shadowT;
	Pos lightDirect, refection;
	Pos null;
	GraphObject* shadowObj = nullptr;

	//расчёт освещённости идёт для каждой позиции в сетке, на которую разбит протяжённый источник света
	for (int i = 0; i < longLight.steps; i++) {
		lightDirect = longLight.getLight(i) - onSurf; //направление света от одной точки
		lightDirect.normalize();

		//проверка, есть ли тень
		shadowObj = CheckForIntersect(shadowT, array, onSurf, lightDirect, 0.1f, 1000000.0f);
		if (shadowObj != nullptr) {
			continue;
		}

		//расчёт, сколько света падает в точку
		n = Scalar(normal, lightDirect);
		if (n > eps) {
			intense += longLight.getI() * n / (Length(normal) * Length(lightDirect));
		}

		//расчёт блеска в зависимости от свойств объекта
		if ((closestObj->getSpecular() != 0)) {
			Pos reflection = ReflectRay(lightDirect, normal);
			reflection.normalize();
			r = Scalar(reflection, minusRay);
			if (r > eps) {
				intense += longLight.getI() * pow((r / (Length(reflection) * Length(minusRay))), closestObj->getSpecular());
			}
		}
	}

	//если значение intense окажется слишком большим,
	//то появятся засвеченные области красного цвета
	if (intense > 0.8) {
		intense = 0.8f;
	}

	return intense;
}

float RandFloat() {
	return rand() / (float)(RAND_MAX);
}

Pos RandomVector() { //случайные единичные векторы
	Pos temp(RandFloat(), RandFloat(), RandFloat());
	temp.normalize();
	return temp;
}

//функция для расчёта цвета в точке, направление луча передаётся в функцию
Color ColorToPut(float ambientLight, Pos& camPos, Pos& ray, GraphObject** array, LightSource& longLight, float depth) {
	float closestT;
	Color black;
	int rays = 3; //количество лучей, для которых считаем среднее значение цвета.
	              //Это создаёт нечёткие отражения.
	Color tempColor, reflectedColor, totalReflectedColor, totalColor;
	Pos refRay;
	GraphObject* closestObj;
	Pos null;

	//ищем ближайший объект, с которым есть пересчение
	closestObj = CheckForIntersect(closestT, array, camPos, ray, 0.0f, 1000000.0f);

	//если луч ни с чем не пересекается, возвращаем цвет фона
	if (closestObj == nullptr) {
		return black;
	}

	//нормаль для работы с отражениями
	Pos onSurf = camPos + ray * closestT;
	Pos normal = closestObj->getNormal(onSurf);
	normal.normalize();
	
	//расчёт света в вызове ComputeLight, запоминаем цвет
	tempColor = closestObj->getColor() * (ComputeLight(longLight, array, onSurf, normal, null - ray, closestObj) + ambientLight);

	//depth - рекурсивный параметр. Проверяем его, чтобы понять, когда нужная глубина
	//отражений достигнута, и можно больше не вызывать рекурсию
	if ((depth <= 0) || (closestObj->getRef() < 0)) {
		return tempColor;
	}

	//расчитываем цвет отражения, если коэффициент отражения > 0
	if (closestObj->getRef() != 0.0f) {
		for (int i = 0; i < rays; i++) {
			refRay = ReflectRay(null - ray, normal) + RandomVector() * closestObj->getGloss();

			//рекурсивный вызов
			reflectedColor = ColorToPut(ambientLight, onSurf, refRay, array, longLight, depth - (float)(1));

			//считаем среднее значение цвета
			totalReflectedColor.r += (int)(reflectedColor.r * 1 / ((float)(rays)));
			totalReflectedColor.g += (int)(reflectedColor.g * 1 / ((float)(rays)));
			totalReflectedColor.b += (int)(reflectedColor.b * 1 / ((float)(rays)));
		}
	}

	totalColor = tempColor * (1 - closestObj->getRef()) + totalReflectedColor * (closestObj->getRef());

	if (totalColor.r > 255) {
		totalColor.r = 255;
	}
	if (totalColor.g > 255) {
		totalColor.g = 255;
	}
	if (totalColor.b > 255) {
		totalColor.b = 255;
	}

	return totalColor;
}

//основная функция, которая вызывает рендер и записывает результат в изображение
void SimpleRender(Image& image, GraphObject** array, LightSource& longLight) {
	int pointer = 0;
	int counter = 1;
	float anti = 0.25f; //параметр антиалиасинга. Очень сильно влияет на производительность
	float ambientLight = 0.2f; //интенсивность окружающего света
	Color totalColor, tempColor;
	float r, g, b;
	Pos camPos; //позиция камеры
	float depth = 3.0f; //глубина отражений (сколько раз отражаем луч)

	//расстояние от камеры до воображаемого холста
	float distance = (float)(image.getW() + image.getH()) / 2;

	//начало рендера, шаг - один пиксель
	for (int y = -(image.getH() / 2); y < image.getH() / 2; y++) {

		if (y % 100 == 0) {
			std::cout << "\n\n" << counter << " / " << image.getH() / 100.0f << " is rendereding:\n\n";
			counter++;
		}
		else {
			if (y % 10 == 0) {
				std::cout << " * ";
			}
		}

		for (int x = -(image.getW() / 2); x < image.getW() / 2; x++) {

			r = 0;
			g = 0;
			b = 0;

			//антиалиасинг. Дробим координаты на сетку, в каждой точке которой считаем луч.
			//итоговый цвет пикселя = среднее значение всех собранных цветов
			//anti = 0.25f считает 16 лучей на пиксель
			for (float newY = (float)(y); newY < (float)(y + 1); newY = newY + anti) {
				for (float newX = (float)(x); newX < (float)(x + 1); newX = newX + anti) {

					Pos newRay(newX, -newY, distance);
					newRay.normalize();
					tempColor = ColorToPut(ambientLight, camPos, newRay, array, longLight, depth);
					r += tempColor.r * anti * anti;
					g += tempColor.g * anti * anti;
					b += tempColor.b * anti * anti;
				}
			}

			totalColor.r = (int)(r);
			totalColor.g = (int)(g);
			totalColor.b = (int)(b);

			//запись в массив пикселей
			image.putPixel(totalColor, pointer);

			pointer++;
		}
	}
}

int main()
{
	Image image(1500, 1000);

	Color white(255, 255, 255);
	Color darkBlue(0, 0, 139);
	Color blue(15, 82, 166);
	Color lightBlue(137, 207, 240);
	Color purple(152, 141, 242);

	Pos center1(-200.0f, -150.0f, 2200.0f);
	Pos center2(100.0f, 100.0f, 1625.0f);
	Pos center3(300.0f, 300.0f, 1600.0f);
	Pos center4(-500.0f, 300.0f, 2300.0f);
	Pos center5(200.0f, -250.0f, 1700.0f);
	Pos normal(0.0f, 1.0f, 0.0f);
	Pos light1(600.0f, 500.0f, 1200.0f);
	Pos light2(700.0f, 500.0f, 1200.0f);

	Sphere darkBlueSphere(250.0f, 1000, darkBlue, center1, 0.5f, 0.1f);
	Sphere blueSphere(70.0f, 10, blue, center2, 0.0f, 0.0f);
	Sphere lightBlueSphere(150.0f, 1000, lightBlue, center3, 0.1f, 0.0f);
	Sphere purpleSphere(150.0f, 1000, purple, center4, 0.0f, 0.0f);
	Sphere mirrorSphere(150.0f, 1000, lightBlue, center5, 0.9f, 0.0f);
	Plane whitePlane(normal, 20, white, 400.0f, 0.0f, 0.0f);

	LightSource longLight(light1, light2, 0.8f);

	GraphObject** array = new GraphObject * [6];
	array[0] = new Sphere(darkBlueSphere);
	array[1] = new Sphere(blueSphere);
	array[2] = new Sphere(lightBlueSphere);
	array[3] = new Sphere(purpleSphere);
	array[4] = new Sphere(mirrorSphere);
	array[5] = new Plane(whitePlane);

	std::cout << "rendering...\n";

	SimpleRender(image, array, longLight);

	std::cout << "\nputing in file...\n";

	image.putInFile();

	std::cout << "\ndone!\n";

	return 0;
}
