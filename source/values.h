//! \file values.h Zmienne globalne i struktury.

#pragma once
#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

//! Wysokość okna.
uint32_t window_height;
//! Długość okna.
uint32_t window_width;

//! Nazwy obrazów.
extern ALLEGRO_USTR** image_names;
//! Obecny obraz.
extern uint32_t current_image;
//! Wybrany algorytm.
extern uint32_t current_algorithm;

//! Główna czcionka używana w prawym panelu.
extern ALLEGRO_FONT* main_font;
//! Dodatkowa czcionka używana w prawym panelu.
extern ALLEGRO_FONT* hint_font;

//! Zmienna do zarządzania trybem wykonania algorytmów.
extern bool visualisation_mode;
//! Dla dużych obrazów przyjmuje wartość false.
bool visualization_mode_available;

//! Nazwy algorytmów.
typedef enum algorithm_t
{
	STACK_BASED_RECURSIVE_FOUR_WAY,
	STACK_BASED_RECURSIVE_EIGHT_WAY,
	QUEUE_BASED_FOUR_WAY,
	SCANLINE_RECURSIVE
} algorithm_t;

//! Ilość algorytmów
extern uint32_t ALGORITHM_AMOUNT;


//! Wartosci mierzone podczas wykonywania algorytmow
typedef struct MeasureValues {
	uint64_t recursion_count;
	uint64_t clock_cycle_count;
	uint64_t duration;
	uint64_t max_stack_height;
	uint64_t current_stack_height;
} MeasureValues;
extern MeasureValues measure_values;

//! Struktura odpowiedzialna za kolor, alpha nie jest wczytywana przez stbi_load dla plików BMP w systemie Windows.
typedef struct Color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Color_t;

/*!
* Struktura przechowująca wszystkie informacje dotyczące badanego zdjęcia.
* Zawiera ścieżkę do pliku, plik Bitmapy do wyświetlenia, tablicę char*(składowe kolorów pikseli),
* wymiary zdjęcia, współczynnik skalowania zdjęcia do poprawnego wyświetlenia 
* oraz zmienne potrzebne do biblioteki stb.
*/
typedef struct Image {
	uint8_t* path;
	uint8_t* as_array;
	ALLEGRO_BITMAP* image;
	uint32_t width;
	uint32_t height;
	uint32_t stb_x;
	uint32_t stb_y;
	uint32_t stb_comp;
	double scale; //! mnożnik, który ustawia wielkość pojedynczego piksela, tak żeby obraz nie wychodził za ekran
} Image;

//! Kolor używany do wypełniania.
extern Color_t replacement_color;