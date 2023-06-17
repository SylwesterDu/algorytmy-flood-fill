//! \file image_management.c Zarządzanie obrazami.

#include<allegro5/allegro.h>
#include <stdint.h>
#include "values.h"

#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


/*!
* Funkcja wczytująca plik .bmp do struktury Image, przekazywanej jako wskaźnik.
* W przypadku, gdy wcześniej był już wczytywany plik, usuwa go z pamięci.
* Tworzy ALLEGRO_BITMAP oraz wczytuje obraz do tablicy pikseli (dokładniej do tablicy składowych kolorów).
* Ustawia zmienne odpowiedzialne za wysokość, szerokość zdjęcia oraz współczynnik skalowania przy wyświetlaniu.
* Wczytuje zmienne potrzebne do biblioteki stb(stb_x, stb_y, stb_comp) w celu zapisywania zdjęcia.
* Określa, czy zdjęcie jest na tyle małe, żeby można było wykorzystać go w trybie wizualizacji.
* \param Image* image Wczytywany obraz.
* \param ALLEGRO_USTR* image_name Struktura Allegro5 pozwalająca na przekazanie nazwy obrazu ze znakami UTF-8.
*/
void load_image(Image* image, ALLEGRO_USTR* image_name) {
	if (image->image) al_destroy_bitmap(image->image);
	if (image->as_array) stbi_image_free(image->as_array);

	image->path = al_cstr(image_name);
	printf("Image: %s\n", image->path);

	uint32_t x;
	uint32_t y;
	uint32_t comp;

	image->as_array = stbi_load(image->path, &x, &y, &comp, 0);
	image->image = al_load_bitmap(image->path);
	if (image->image == NULL) puts("error when load bitmap\n");
	image->width = al_get_bitmap_width(image->image);
	image->height = al_get_bitmap_height(image->image);
	image->stb_comp = comp;
	image->stb_x = x;
	image->stb_y = y;

	if (image->width > image->height)
	{
		image->scale = (window_width - window_width / 2) / (double)image->width;
	}
	else if (image->width < image->height)
	{
		image->scale = window_height / (double)image->height;
	}
	else
	{
		image->scale = (window_width - window_width / 2) / (double)image->width;
	}

	if (image->height * image->width <= 2500) {
		visualization_mode_available = true;
	}
	else {
		visualization_mode_available = false;
		visualisation_mode = false;
	}
}

/*!
* Funkcja zapisująca obraz do pliku .bmp. Jako argumenty przyjmuje nazwę pliku, tablicę składowych kolorów pikseli
* oraz zmienne związane z biblioteką STB
* \param uint8_t* name Nazwa obrazu.
* \param uint8_t* as_array Obraz reprezentowany jako tablica uint8_t w pamięci.
* \param uint32_t x Długość obrazu.
* \param uint32_t y Szerokość obrazu.
* \param uint32_t comp Ilość kanałów w obrazie.
*/
void save_image_to_bmp(uint8_t* name, uint8_t* as_array, uint32_t x, uint32_t y, uint32_t comp) {
	stbi_write_bmp(name, x, y, comp, as_array);
}


/*!
* Funkcja czyszcząca pamięć po wczytanym zdjęciu. Jako argument przyjmuje wskaźnik na strukturę Image.
* Ta struktura przechowuje obiekty do usunięcia.
* \param Image* image Czyszczone zdjęcie.
*/
void clean_up_image(Image* image) {
	if (image->image) al_destroy_bitmap(image->image);
	if (image->as_array) stbi_image_free(image->as_array);
}

/*!
* Funkcja umieszczająca kolor badanego piksela w strukturze Color_t, która jest modyfikowana w tej funkcji, 
* dlatego musi byæ przekazana jako wskaźnik. Do odczytania koloru potrzebuje współrzędnych piksela oraz struktury Image.
* \param Color_t* current_color Poprzedni kolor piksela, który będzie nadpisywany.
* \param uint32_t mouse_x Koordynat X badanego piksela.
* \param uint32_t mouse_y Koordynat Y badanego piksela.
* \param Image* image Obraz na którym wykonujemy funkcję.
*/
void get_pixel_color(Color_t* current_color, uint32_t mouse_x, uint32_t mouse_y, Image* image) {

	if (mouse_x < 0 || mouse_x > image->width || mouse_y < 0 || mouse_y > image->height)
	{
		return;
	}

	current_color->r = image->as_array[mouse_x * 3 + mouse_y * image->width * 3];
	current_color->g = image->as_array[mouse_x * 3 + mouse_y * image->width * 3 + 1];
	current_color->b = image->as_array[mouse_x * 3 + mouse_y * image->width * 3 + 2];
}

/*!
* Funkcja, która zamienia kolor (są to trzy kolejne unsigned chary) określonego pola.
* Modyfikacja zachodzi w tablicy pikseli w strukturze Image, więc jest ona przekazywana jako wskaźnik.
* W przypadku wyjścia poza obszar zdjęcia funkcja przerywa swoje działanie, nie robiąc nic.
* \param Image* image Obraz w którym zmieniamy kolor piksela.
* \param uint32_t mouse_x Koordynat X zamienianego piksela.
* \param uint32_t mouse_y Koordynat Y zamienianego piksela.
*/
void swap_color(Image* image, uint32_t mouse_x, uint32_t mouse_y) {

	if (mouse_x < 0 || mouse_x > image->width || mouse_y < 0 || mouse_y > image->height)
	{
		return;
	}

	image->as_array[mouse_x * 3 + mouse_y * image->width * 3] = replacement_color.r;
	image->as_array[mouse_x * 3 + 1 + mouse_y * image->width * 3] = replacement_color.g;
	image->as_array[mouse_x * 3 + 2 + mouse_y * image->width * 3] = replacement_color.b;
}
