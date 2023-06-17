/*! \file main.c Plik programu z pętlą główną. */

#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // do łaczenia nazw ścieżek i nazw plikow
#include <time.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include "values.h"
#include "right_panel.h"
#include "fill_algorithms.h"
#include "image_management.h"

void check_init(bool checked_function);
void init_allegro(ALLEGRO_EVENT_QUEUE**, ALLEGRO_DISPLAY**);
void get_image_names();
void load_fonts();
void main_loop(ALLEGRO_EVENT_QUEUE*, ALLEGRO_DISPLAY*);
void clean_up_allegro(ALLEGRO_EVENT_QUEUE**, ALLEGRO_DISPLAY**);
void quantize_mouse_position(uint32_t, uint32_t*, uint32_t*);

void fill_with_color(Image*, algorithm_t, uint32_t, uint32_t);
bool check_if_clicked_on_image(ALLEGRO_MOUSE_STATE, Image);

MeasureValues measure_values;
uint32_t IMAGE_AMOUNT;
ALLEGRO_USTR** image_names;
bool visualisation_mode = false;
uint32_t ALGORITHM_AMOUNT = 4;

Color_t replacement_color = { 128, 128, 255 };

ALLEGRO_FONT* main_font;
ALLEGRO_FONT* hint_font;

uint32_t current_image = 0;
uint32_t current_algorithm = STACK_BASED_RECURSIVE_FOUR_WAY;

int main()
{
	ALLEGRO_EVENT_QUEUE* queue = NULL;
	ALLEGRO_DISPLAY* display = NULL;
	init_allegro(&queue, &display);
	get_image_names();
	load_fonts();

	main_loop(queue, display);

	clean_up_allegro(&queue, &display);
	return EXIT_SUCCESS;
}

//! Sprawdza inicjalizacje pierwszych funkcji w init_allegro(), w przypadku błędu kończy działanie aplikacji.
//! \param bool checked_function Sprawdzana funkcja z Allegro5.
void check_init(bool checked_function)
{
	if (!checked_function)
	{
		printf("%s", "Error initializing function, refer to debugger.");
		exit(EXIT_FAILURE);
	}
}

/*!
* Wczytuje wszystkie zdjęcia z rozszerzeniem .bmp z folderu Images. Odczytuje zawartość folderu dwa razy.
* Pierwszy raz wyznacza ilość obrazów, drugi raz dynamicznie przydziela pamięć na ścieżki dla plików zdjęć.
*/
void get_image_names()
{
	const char* current_dir = al_get_current_directory();
	strcat_s(current_dir, strlen(current_dir) + 2, "\\");
	ALLEGRO_PATH* path = al_create_path(current_dir);
	al_append_path_component(path, "Images");
	puts(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));

	ALLEGRO_FS_ENTRY* images_directory = al_create_fs_entry(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
	al_open_directory(images_directory);

	uint32_t image_amount = 0;
	ALLEGRO_FS_ENTRY* image = NULL;
	while (true)
	{
		image = al_read_directory(images_directory);
		if (!image)
		{
			break;
		}

		uint32_t fs_flags = al_get_fs_entry_mode(image);
		if (fs_flags & ALLEGRO_FILEMODE_ISFILE)
		{
			image_amount++;
		}

	}

	ALLEGRO_PATH* result_image_path = path;
	al_append_path_component(result_image_path, "Result.bmp");
	if (al_filename_exists(al_path_cstr(result_image_path, ALLEGRO_NATIVE_PATH_SEP)))
	{
		image_amount--;
	}

	al_close_directory(images_directory);
	image_names = malloc(sizeof(ALLEGRO_USTR*) * image_amount);
	al_open_directory(images_directory);

	for (uint32_t i = 0; i < image_amount; i++)
	{
		image = al_read_directory(images_directory);
		if (
			!strcmp(al_get_path_filename(al_create_path(al_get_fs_entry_name(image))), "Result.bmp")) {
			i--;
			continue;
		}

		if (strcmp(al_get_path_extension(al_create_path(al_get_fs_entry_name(image))), ".bmp"))
		{
			image_amount--;
			i--;
			continue;
		}
		const char* image_path = al_get_fs_entry_name(image);
		const char* image_name = al_get_path_filename(al_create_path(image_path));
		const char image_folder[128] = "Images/";
		strcat_s(image_folder, strlen(image_folder) + strlen(image_name) + 1, image_name);

		image_names[i] = al_ustr_new(image_folder);
	}

	IMAGE_AMOUNT = image_amount;

	al_destroy_fs_entry(image);
	al_close_directory(images_directory);
	al_destroy_fs_entry(images_directory);
	al_destroy_path(path);
}

/*!
* Funkcja wczytująca czcionki. Nie przyjmuje argumentów, gdyż czcionki są zmiennymi globalnymi.
*/
void load_fonts() {
	//! Główna, większa czcionka.
	main_font = al_load_ttf_font("Fonts/Hack-Regular.ttf", window_height / 40, 0);

	//! Mniejsza czcionka używana do wyświetlania wskazówek.
	hint_font = al_load_ttf_font("Fonts/Hack-Regular.ttf", window_height / 60, 0);
}

/*!
* Sprząta po bibliotece Allegro, zwalniając czcionki, nazwy zdjęć, kolejkę zdarzeń oraz okno.
* \param ALLEGRO_EVENT_QUEUE** queue Zwalniana kolejka zdarzeń.
* \param ALLEGRO_DISPLAY** display Zwalniane okno.
*/
void clean_up_allegro(ALLEGRO_EVENT_QUEUE** queue, ALLEGRO_DISPLAY** display)
{
	al_destroy_font(main_font);
	al_destroy_font(hint_font);

	al_destroy_event_queue(*queue);
	al_destroy_display(*display);
	for (uint32_t i = 0; i < IMAGE_AMOUNT; i++) {
		al_ustr_free(image_names[i]);
	}
}

/*!
* Zamienia pozycje współrzędne x i y (przekazywane jako wskaźnik) na wartosci odpowiadajace współrzędnym klikniętego piksela.
* Proporcje piksela są stałe(1:1), więc wykorzystujemy tylko jeden wymiar zdjęcia(image_width).
* \param uint32_t image_width Długość grafiki.
* \param uint32_t* mouse_x Zmieniana pozycja myszy na osi X.
* \param uint32_t* mouse_y Zmieniana pozycja myszy na osi Y.
*/
void quantize_mouse_position(uint32_t image_width, uint32_t* mouse_x, uint32_t* mouse_y)
{
	*mouse_x /= (float)window_height / image_width;
	*mouse_y /= (float)window_height / image_width;
}

/*!
* Zwraca true, jeśli uzytkownik kliknął na obraz.
* Obliczenia wykonuje na podstawie pozycji wskażnika, wymiarów obrazu oraz współczynnika skalowania obrazu.
* \param ALLEGRO_MOUSE_STATE state Stan myszy opisywany przez Allegro
* \param Image image Obraz, dla którego sprawdzamy, czy na niego kliknęliśmy.
* \returns To czy kliknęliśmy, jako wartość typu bool.
*/
bool check_if_clicked_on_image(ALLEGRO_MOUSE_STATE state, Image image) {
	if (
		state.x / image.scale < image.width
		&& state.y / image.scale < image.height
		) return true;
	return false;
}

//! Inicjalizuje obiekty powiazane z Allegro, wyznacza wielkość okna, ustawia wejścia myszy i klawiatury.
//! \param ALLEGRO_EVENT_QUEUE** queue Inicjalizowana kolejka zdarzeń.
//! \param ALLEGRO_DISPLAY** display Inicjalizowane okno.
void init_allegro(ALLEGRO_EVENT_QUEUE** queue, ALLEGRO_DISPLAY** display)
{


	check_init(al_init());
	check_init(al_init_image_addon());
	check_init(al_install_mouse());
	check_init(al_install_keyboard());
	check_init(al_init_font_addon());
	check_init(al_init_ttf_addon());
	check_init(al_init_primitives_addon());

	ALLEGRO_MONITOR_INFO monitor_info;
	al_get_monitor_info(0, &monitor_info);

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	window_width = monitor_info.x2 - monitor_info.x2 / 8;
	window_height = window_width / 2;

	*display = al_create_display(window_width, window_height);
	*queue = al_create_event_queue();

	al_set_target_backbuffer(*display);
	int window_middle_x = monitor_info.x2 / 2 - window_width / 2;
	al_set_window_position(*display, window_middle_x, 0);

	al_register_event_source(*queue, al_get_keyboard_event_source());
	al_register_event_source(*queue, al_get_mouse_event_source());
	al_register_event_source(*queue, al_get_display_event_source(*display));

}

/*!
* Główna pętla odpowiadająca za wyświetlenie obrazu, odczytywania kliknięcia myszy, oraz naciśnięcia klawiatury.
* Spacja:	zmiana trybu wyświetlania
* Tab:		zmiana zdjęcia
* R:		reset zdjęcia
* Strzałki: zmiana algorytmu
* \param ALLEGRO_EVENT_QUEUE* queue Kolejka zdarzeń.
* \param ALLEGRO_DISPLAY* display Okno.
*/
void main_loop(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* display)
{
	Image image = { 0 };

	//! Początkowe wczytanie pierwszego obrazu
	load_image(&image, image_names[current_image]);

	bool show_measure_result = false;

	al_flip_display();

	ALLEGRO_EVENT event;
	ALLEGRO_MOUSE_STATE mouse_state;

	bool break_loop = false;

	while (!break_loop)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		//! Wyświetlenie obrazu skalowanego tak, żeby wypełniał odpowiednio duzy obszar
		al_draw_scaled_bitmap(
			image.image,
			0,
			0,
			image.width,
			image.height,
			0,
			0,
			image.width * image.scale,
			image.height * image.scale,
			0
		);

		//! Wyświetlenie prawego panelu
		show_right_panel(current_algorithm, show_measure_result);
		al_flip_display();



		al_wait_for_event(queue, &event);
		switch (event.type)
		{

			//! Obsługa kliknięcia
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			switch (event.mouse.button)
			{

				//! Został naciśnięty lewy przycisk myszy
			case 1:

				//! Wczytujemy niezmieniona pozycje kursora wzgledem okna.
				al_get_mouse_state(&mouse_state);

				//! Ignorujemy, jeśli nie kliknęliśmy na obraz.
				if (!check_if_clicked_on_image(mouse_state, image)) continue;

				// Jeśli kliknięte na obraz, rozpoczynamy wypełnianie.
				fill_with_color(&image, current_algorithm, mouse_state.x, mouse_state.y);

				/*!
				* Dla poprawnego odwzorowania wypełniania w trybie wizualizacji działania
				* konieczne jest poczekanie po ostatnim wykonaniu funkcji wypełniania.
				*/
				if (visualisation_mode) al_rest(0.1);
				//! Po wypełnianiu zdjęcie zapisujemy na dysku.
				save_image_to_bmp("Images/Result.bmp", image.as_array, image.stb_x, image.stb_y, image.stb_comp);

				//! Zapisane zdjęcie ponownie wczytujemy do wyświetlenia.
				load_image(&image, al_ustr_new("Images/Result.bmp")); // wczytujemy od nowa

				//! Po udanym wypełnieniu prawy panel ma wyświetlić wyniki
				show_measure_result = true;
				break;
			}
			break;


			//! Obsluga naciśnięcia klawiszy klawiatury
		case ALLEGRO_EVENT_KEY_UP:
			switch (event.keyboard.keycode)
			{

				//! W przypadku strzałki w górę zmieniamy algorytm
			case ALLEGRO_KEY_UP:
				if (current_algorithm > 0) {
					show_measure_result = false;
					current_algorithm--;
				}
				break;

				//! W przypadku strzałki w dół zmieniamy algorytm
			case ALLEGRO_KEY_DOWN:
				if (current_algorithm < ALGORITHM_AMOUNT - 1) {
					show_measure_result = false;
					current_algorithm++;
				}
				break;

				//! W przypadku klawisza TAB zmieniamy zdjęcie
			case ALLEGRO_KEY_TAB:
				show_measure_result = false;
				current_image = (current_image + 1) % IMAGE_AMOUNT;
				load_image(&image, image_names[current_image]);
				break;

				//! W przypadku klawisza R odświeżamy zdjęcie(ponownie wczytujemy z dysku)
			case ALLEGRO_KEY_R:
				show_measure_result = false;
				load_image(&image, image_names[current_image]);
				break;

				//! W przypadku spacji przełączamy tryby natychmiastowego działania oraz wizualizacji
			case ALLEGRO_KEY_SPACE:
				show_measure_result = false;
				//! Wyłączamy tryb wizualizacji
				if (visualisation_mode) {
					visualisation_mode = false;
				}
				//! Włączamy tryb wizualizacji, jeśli jest to możliwe
				else {
					if (visualization_mode_available) visualisation_mode = true;
				}
				break;
				//! W przypadku klawisza escape pętla zostaje przerwana, a instrukcje w funkcji main() poprawnie zakończą działanie aplikacji
			case ALLEGRO_KEY_ESCAPE:
				break_loop = true; // przerwanie pętli w obecnej funkcji
				break;
			}
			break;


			//! W przypadku wyłączenia okna  pętla zostaje przerwana, a instrukcje w funkcji main() poprawnie zakończą działanie aplikacji
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			break_loop = true;
			break;
		}
	}

	//! W przypadku przerwania pętli usuwamy zdjęcia z pamięci przed zakończeniem działania aplikacji
	clean_up_image(&image);
}


/*!
* Wypełnia kliknięty obszar kolorem.
* Przed wypełnieniem zeruje wartości mierzone przy wypełnianiu.
* Po wypełnianiu oblicza wartości i ustawia w strukturze MeasureValues
* \param Image* image Wypełniany obraz.
* \param algorithm_t algorithm Rodzaj algorytmu wybrany przez użytkownika.
* \param uint32_t mouse_x Pozycja myszy w koordynatach monitora na osi X.
* \param uint32_t mouse_y Pozycja myszy w koordynatach monitora na osi y.
*/
void fill_with_color(Image* image, algorithm_t algorithm, uint32_t mouse_x, uint32_t mouse_y) {

	//! Zerujmy liczniki przed wykonaniem algorytmu wypełniania
	measure_values.recursion_count = 0;
	measure_values.clock_cycle_count = 0;
	measure_values.duration = 0;
	measure_values.current_stack_height = 0;
	measure_values.max_stack_height = 0;

	//! Zmieniamy pozycje kursora wzgledem okna na koordynaty obrazu.
	quantize_mouse_position(image->width, &mouse_x, &mouse_y);

	//! Odczytujemy jaki jest kolor klikniętego piksela
	Color_t current_color = { 0 };
	get_pixel_color(&current_color, mouse_x, mouse_y, image);

	//! Zapamiętujemy obecny stan czasu i ilość cykli zegara
	clock_t time_start = clock();
	uint64_t clock_start = __rdtsc();
	flood_fill(algorithm, mouse_x, mouse_y, image, current_color);
	//! Zapamiętujemy stan czasu i cykli zegara po zakończeniu algorytmu wypełniania
	uint64_t clock_end = __rdtsc();
	clock_t time_end = clock();

	//! W trybie wizualizacji liczenie czasu i cykli zegara pomijamy
	if (visualisation_mode) return;

	//! Czas działania i ilość cykli zegara liczymy odejmując wartości przed i po wypełnieniu algorytmem
	measure_values.duration = time_end - time_start;
	measure_values.clock_cycle_count = clock_end - clock_start;
}
