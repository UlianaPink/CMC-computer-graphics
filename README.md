# ТРАССИРОВКА ЛУЧЕЙ

КАК СОБРАТЬ ПРОЕКТ:

Весь код написан на С++ и находится в файле RayTracing.cpp в папке bin. Программа использует только две стандартные библиотеки - iostream и fstream.   
Ничего дополнительно устанавливать не нужно. Чтобы собрать проект, достаточно скомпилировать код из RayTracing.cpp.  
Программа сохраняет изображение в формате .ppm там же, где находится файл .exe.   
Можно указать удобный путь для сохранения файла примерно в 330-ой строке кода (метод putInFile класса Image).  
Файл формата .ppm можно открыть в программах Gimp или Adobe Photoshop.  
В корневой папке есть файл с готовым рендером в формате png (final_render.png).  

РЕАЛИЗОВАННЫЕ ЭФФЕКТЫ:

Отбрасывание тени на другие объекты  
Зеркальные грани  
Антиалиасинг  
Попиксельный расчёт освещённости по стандартным моделям  
Объёмные или протяжённые источники освещения  
Нечёткие тени  
Нечёткие отражения  

ПРОИЗВОДИТЕЛЬНОСТЬ:

Если Вы хотите собрать проект, но не хотите ждать полного рендера, измените локальную переменную anti в функции SimpleRender следующим образом:  
float anti = 1.0f;  
Это отключит антиалиасинг и значительно ускорит работу программы.  

ПОСЛЕДОВАТЕЛЬНАЯ РАБОТА:

Этапы работы над сценой можно посмотреть в этом же репозитории, в ветках Update_1, Update_2 и Update_3  
(они выкладывались с 8 декабря)  

ГОТОВЫЙ РЕНДЕР:

![Image alt](https://github.com/UlianaPink/CMC-computer-graphics/raw/master/final_render.png)
