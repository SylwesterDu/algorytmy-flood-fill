//! \file fill_algorithms.c Algorytmy wypełniania.

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "right_panel.h"
#include "queue.h"
#include "values.h"
#include "image_management.h"

void stack_based_recursive_four_way(uint32_t, uint32_t, Image*, Color_t);
void stack_based_recursive_eight_way(uint32_t, uint32_t, Image*, Color_t);
void queue_based_four_way(uint32_t, uint32_t, Image*, Color_t);
void scanline_recursive(uint32_t, uint32_t, Image*, Color_t);

void stack_based_recursive_four_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void stack_based_recursive_eight_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void queue_based_four_way_visualize(uint32_t, uint32_t, Image*, Color_t);
void scanline_recursive_visualize(uint32_t, uint32_t, Image*, Color_t);

/*!
* Funkcja wywołująca wypełnienie na podstawie obecnego algorytmu przekazywanego jako argument.
* Na podstawie zmiennej globalnej visualization_mode określa, czy funkcje wywołać w trybie wizualizacji czy szybkiego działania.
* \param algorithm_t algorithm Wybrany algorytm.
* \param uint32_t mouse_x Zmieniony przez quantize_mouse_position koordynat na osi X.
* \param uint32_t mouse_y Zmieniony przez quantize_mouse_position koordynat na osi Y.
* \param Image* image Zmieniany w pamięci obraz.
* \param Color_t current_color Kolor klikniętego piksela.
*/
void flood_fill(algorithm_t algorithm, uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {
	switch (algorithm) {
	case STACK_BASED_RECURSIVE_FOUR_WAY:
		visualisation_mode
			? stack_based_recursive_four_way_visualize(mouse_x, mouse_y, image, current_color)
			: stack_based_recursive_four_way(mouse_x, mouse_y, image, current_color);
		break;
	case STACK_BASED_RECURSIVE_EIGHT_WAY:
		visualisation_mode
			? stack_based_recursive_eight_way_visualize(mouse_x, mouse_y, image, current_color)
			: stack_based_recursive_eight_way(mouse_x, mouse_y, image, current_color);
		break;
	case QUEUE_BASED_FOUR_WAY:
		visualisation_mode
			? queue_based_four_way_visualize(mouse_x, mouse_y, image, current_color)
			: queue_based_four_way(mouse_x, mouse_y, image, current_color);
		break;
	case SCANLINE_RECURSIVE:
		visualisation_mode
			? scanline_recursive_visualize(mouse_x, mouse_y, image, current_color)
			: scanline_recursive(mouse_x, mouse_y, image, current_color);
		break;
	default:
		break;
	}
}

/*!
* Algorytm wypełniający powierzchnię.
* Funkcja zmienia kolor obecnego piksela, a następnie wywołuje samą siebie dla sąsiednich pikseli.
* Wywołuje się dla pikseli na górze, dole, po prawej i po lewej.
* Przed zmianą koloru sprawdza, czy piksel jest w takim samym kolorze jak ten kliknięty.
* Przerywa w przypadku kliknięcia na taki sam kolor, jak kolor wypełniania.
* Może doprowadzić do przepełnienia stosu.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void stack_based_recursive_four_way(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	//! Sprawdzamy czy jestesmy poza obszarem zdjęcia.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Odczytujemy kolor badanego piksela
	Color_t current_pixel_color = { 0 };
	get_pixel_color(&current_pixel_color, mouse_x, mouse_y, image);

	//! Jeśli kolor jest taki sam, jak początkowy kolor piksela, od którego zaczynaliśmy wypełnianie zmieniamy go.
	if (
		current_pixel_color.r == current_color.r
		&& current_pixel_color.g == current_color.g
		&& current_pixel_color.b == current_color.b
		) {
		swap_color(image, mouse_x, mouse_y);
	}
	//! Kiedy kolor jest inny, kończymy działanie obecnej funkcji, zmiejszając zmienną liczącą wysokość stosu.
	else {
		measure_values.current_stack_height--;
		return;
	}

	//! Rekursywnie wykonujemy algorytm na pikselach na górze, dole i po bokach.
	stack_based_recursive_four_way(mouse_x + 1, mouse_y, image, current_color);
	stack_based_recursive_four_way(mouse_x - 1, mouse_y, image, current_color);
	stack_based_recursive_four_way(mouse_x, mouse_y + 1, image, current_color);
	stack_based_recursive_four_way(mouse_x, mouse_y - 1, image, current_color);

	//! Po zakończeniu funkcji zmniejszamy zmienną liczącą wysokość stosu
	measure_values.current_stack_height--;
}

/*!
* Algorytm wypełniający powierzchnię.
* Zmienia kolor obecnego piksela, a następnie wywołuje samą siebie dla sąsiednich pikseli.
* Wywołuje się dla pikseli na górze, dole, po prawej i po lewej oraz po skosach.
* Przed zmianą koloru sprawdza, czy piksel jest w takim samym kolorze jak ten kliknięty.
* Przerywa w przypadku kliknięcia na taki sam kolor, jak kolor wypełniania.
* Może doprowadzić do przepełnienia stosu.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void stack_based_recursive_eight_way(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count++;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	//! Sprawdzamy czy jestesmy poza obszarem zdjęcia.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		// Jesli tak, to wychodzimy z funkcji.
		measure_values.current_stack_height--;
		return;
	}

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Odczytujemy kolor badanego piksela
	Color_t current_pixel_color = { 0 };
	get_pixel_color(&current_pixel_color, mouse_x, mouse_y, image);

	//! Jeśli kolor jest taki sam, jak początkowy kolor piksela, od którego zaczynaliśmy wypełnianie zmieniamy go.
	if (
		current_pixel_color.r == current_color.r
		&& current_pixel_color.g == current_color.g
		&& current_pixel_color.b == current_color.b
		) {
		swap_color(image, mouse_x, mouse_y);
	}
	//! Kiedy kolor jest inny, kończymy działanie obecnej funkcji, zmiejszając zmienną liczącą wysokość stosu.
	else {
		measure_values.current_stack_height--;
		return;
	}

	//! Rekursywnie wykonujemy algorytm na pikselach na górze, dole po bokach i skosach.
	stack_based_recursive_eight_way(mouse_x + 1, mouse_y, image, current_color);
	stack_based_recursive_eight_way(mouse_x - 1, mouse_y, image, current_color);
	stack_based_recursive_eight_way(mouse_x, mouse_y + 1, image, current_color);
	stack_based_recursive_eight_way(mouse_x, mouse_y - 1, image, current_color);
	stack_based_recursive_eight_way(mouse_x + 1, mouse_y - 1, image, current_color);
	stack_based_recursive_eight_way(mouse_x + 1, mouse_y + 1, image, current_color);
	stack_based_recursive_eight_way(mouse_x - 1, mouse_y - 1, image, current_color);
	stack_based_recursive_eight_way(mouse_x - 1, mouse_y + 1, image, current_color);

	//! Po zakończeniu funkcji zmniejszamy zmienną liczącą wysokość stosu
	measure_values.current_stack_height--;
}
/*!
* Algorytm wypełniania oparty na kolejce.
* Zamienia kolor bierzącego piksela na kolor wypełnienia, a następnie sąsiednie piksele wstawia do kolejki.
* Każdy piksel z kolejki jest sprawdzany, czy jest konieczność zmiany jego koloru. Jeśli tak,
* zmienia się jego kolor, a sąsiednie dla niego piksele zostają wstawione do kolejki.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void queue_based_four_way(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (
		replacement_color.r == current_color.r
		&& replacement_color.g == current_color.g
		&& replacement_color.b == current_color.b) {
		return;
	}

	//! Tworzymy kolejkę przechowującą współrzędne badanych pikseli
	QueuePointers queue = { NULL, NULL };

	//! Dodajemy kliknięty piksel do kolejki
	enqueue(&queue, mouse_x, mouse_y);

	//! Zmienne przechowujące współrzędne badanych pikseli
	uint32_t position_x;
	uint32_t position_y;

	//! Pętla badająca każde współrzędne piksela w kolejce aż do momentu zwolnienia kolejki 
	while (queue.head != NULL) {

		//! Wczytujemy nową pozycje piksela
		dequeue(&queue, &position_x, &position_y);

		//! Odczytujemy kolor badanego piksela
		Color_t current_pixel_color = { 0 };
		get_pixel_color(&current_pixel_color, position_x, position_y, image);

		//! Jeśli kolor jest taki sam, jak kolor na który klikneliśmy, zmieniamy jego kolor. Do kolejki dodajemy jego sąsiednie piksele.
		if (
			current_color.r == current_pixel_color.r
			&& current_color.g == current_pixel_color.g
			&& current_color.b == current_pixel_color.b
			) {
			swap_color(image, position_x, position_y);
			if (position_x > 0) {
				enqueue(&queue, position_x - 1, position_y); // lewo
			}
			if (position_y > 0) {
				enqueue(&queue, position_x, position_y - 1); // gora
			}
			if (position_x < image->width - 1) {
				enqueue(&queue, position_x + 1, position_y); // prawo
			}
			if (position_y < image->height - 1) {
				enqueue(&queue, position_x, position_y + 1); // dol
			}
		}

	}
}

/*!
* Algorytm wypełniania powierzchni. Działa w sposób rekurencyjny.
* Wypełnia poziomą linie na kolor wypełnienia. Przy każdym zmieniamym pikselu wywołuje samą siebie dla piksela wyżej i piksela niżej.
* Przed zmianą koloru kolejnych pikseli sprawdza, czy jest on taki sam, jak kolor, na który klikneliśmy
* \param int mouse_x Pozycja piksela na osi X.
* \param int mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void scanline_recursive(int mouse_x, int mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	//! Sprawdzamy czy jestesmy poza widocznym obszarem.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	Color_t current_pixel_color = { 0 };

	//! Sprawdzamy piksele na prawo od badanego piksela, jeśli kolor jest taki sam jak ten, na który kliknęliśmy, zmieniamy go.
	uint32_t right_x = 0;
	for (right_x = mouse_x; right_x < image->width; ++right_x) {

		get_pixel_color(&current_pixel_color, right_x, mouse_y, image);

		if (current_pixel_color.r != current_color.r
			|| current_pixel_color.g != current_color.g
			|| current_pixel_color.b != current_color.b
			) {
			break;
		}
		else {
			swap_color(image, right_x, mouse_y);
		}
	}

	//! Sprawdzamy piksele na lewo od badanego piksela, jeśli kolor jest taki sam jak ten, na który kliknęliśmy, zmieniamy go.
	uint32_t left_x = mouse_x - 1;
	for (left_x; left_x > 0 && left_x < image->width; --left_x) {

		get_pixel_color(&current_pixel_color, left_x, mouse_y, image);

		if (current_pixel_color.r != current_color.r
			|| current_pixel_color.g != current_color.g
			|| current_pixel_color.b != current_color.b
			) {
			break;
		}
		else {
			swap_color(image, left_x, mouse_y);
		}
	}

	if (left_x != 0) {
		++left_x;
	}

	//! Sprawdzamy piksele od lewej strony wypełnionego paska do prawej.
	//! Jeśli wykryjemy powyżej lub poniżej nich wypełniany kolor, rekursywnie wykonujemy scanline_recursive().
	for (left_x; left_x < right_x; ++left_x) {

		get_pixel_color(&current_pixel_color, left_x, mouse_y - 1, image);
		if (
			current_pixel_color.r == current_color.r
			&& current_pixel_color.g == current_color.g
			&& current_pixel_color.b == current_color.b
			) {
			scanline_recursive(left_x, mouse_y - 1, image, current_color);
		}

		get_pixel_color(&current_pixel_color, left_x, mouse_y + 1, image);
		if (
			current_pixel_color.r == current_color.r
			&& current_pixel_color.g == current_color.g
			&& current_pixel_color.b == current_color.b
			) {
			scanline_recursive(left_x, mouse_y + 1, image, current_color);
		}
	}

	measure_values.current_stack_height--;
}

/*!
* Algorytm wypełniający powierzchnię.
* Zmienia kolor obecnego piksela, a następnie wywołuje samą siebie dla sąsiednich pikseli.
* Wywołuje się dla pikseli na górze, dole, po prawej i po lewej.
* Przed zmianą koloru sprawdza, czy piksel jest w takim samym kolorze jak ten kliknięty.
* Przerywa w przypadku kliknięcia na taki sam kolor, jak kolor wypełniania.
* Co każdy zamalowany piksel zapisuje zdjęcie, wczytuje je na nowo i wyświetla, czekając 100ms
* Może doprowadzić do przepełnienia stosu.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void stack_based_recursive_four_way_visualize(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	//! Sprawdzamy czy jestesmy poza obszarem zdjęcia.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		measure_values.current_stack_height--;
		return;
	}

	//! Odczytujemy kolor badanego piksela
	Color_t current_pixel_color = { 0 };
	get_pixel_color(&current_pixel_color, mouse_x, mouse_y, image);

	//! Jeśli kolor jest taki sam, jak początkowy kolor piksela, od którego zaczynaliśmy wypełnianie zmieniamy go.
	if (
		current_pixel_color.r == current_color.r
		&& current_pixel_color.g == current_color.g
		&& current_pixel_color.b == current_color.b
		) {
		swap_color(image, mouse_x, mouse_y);

		/*
		* Po zmianie koloru piksela zapisujemy zdjęcie, czekamy chwilę(żeby poprawnie pokazać wypełnianie krok po kroku),
		* ponownie ładujemy zdjęcie i wyświetlamy je oraz prawy panel z wynikami
		*/
		save_image_to_bmp("Images/Result.bmp", image->as_array, image->stb_x, image->stb_y, image->stb_comp);
		al_rest(0.1);
		al_destroy_bitmap(image->image);
		image->image = al_load_bitmap("Images/Result.bmp");
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_scaled_bitmap(
			image->image,
			0,
			0,
			image->width,
			image->height,
			0,
			0,
			image->width * image->scale,
			image->height * image->scale,
			0
		);
		show_right_panel(current_algorithm, true);
		al_flip_display();
	}
	//! Kiedy kolor jest inny, kończymy działanie obecnej funkcji, zmiejszając zmienną liczącą wysokość stosu.
	else {
		measure_values.current_stack_height--;
		return;
	}

	//! Rekursywnie wykonujemy algorytm na pikselach na górze, dole i po bokach.
	stack_based_recursive_four_way_visualize(mouse_x + 1, mouse_y, image, current_color);
	stack_based_recursive_four_way_visualize(mouse_x - 1, mouse_y, image, current_color);
	stack_based_recursive_four_way_visualize(mouse_x, mouse_y + 1, image, current_color);
	stack_based_recursive_four_way_visualize(mouse_x, mouse_y - 1, image, current_color);

	//! Po zakończeniu funkcji zmniejszamy zmienną liczącą wysokość stosu
	measure_values.current_stack_height--;
}

/*!
* Algorytm wypełniający powierzchnię.
* Zmienia kolor obecnego piksela, a następnie wywołuje samą siebie dla sąsiednich pikseli.
* Wywołuje się dla pikseli na górze, dole, po prawej i po lewej oraz po skosach.
* Przed zmianą koloru sprawdza, czy piksel jest w takim samym kolorze jak ten kliknięty.
* Przerywa w przypadku kliknięcia na taki sam kolor, jak kolor wypełniania.
* Co każdy zamalowany piksel zapisuje zdjęcie, wczytuje je na nowo i wyświetla, czekając 100ms.
* Może doprowadzić do przepełnienia stosu.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void stack_based_recursive_eight_way_visualize(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	//! Sprawdzamy czy jestesmy poza obszarem zdjęcia.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		//! Jesli tak, to wychodzimy z funkcji, stos rekurencyjnych wywołań zmniejsza się o 1.
		measure_values.current_stack_height--;
		return;
	}

	//! Odczytujemy kolor badanego piksela

	Color_t current_pixel_color = { 0 };
	get_pixel_color(&current_pixel_color, mouse_x, mouse_y, image);

	/*
	* Po zmianie koloru piksela zapisujemy zdjęcie, czekamy chwilę(żeby poprawnie pokazać wypełnianie krok po kroku),
	* ponownie ładujemy zdjęcie i wyświetlamy je oraz prawy panel z wynikami
	*/
	if (
		current_pixel_color.r == current_color.r
		&& current_pixel_color.g == current_color.g
		&& current_pixel_color.b == current_color.b
		) {
		swap_color(image, mouse_x, mouse_y);
		save_image_to_bmp("Images/Result.bmp", image->as_array, image->stb_x, image->stb_y, image->stb_comp);
		al_rest(0.1);
		al_destroy_bitmap(image->image);
		image->image = al_load_bitmap("Images/Result.bmp");
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_scaled_bitmap(
			image->image,
			0,
			0,
			image->width,
			image->height,
			0,
			0,
			image->width * image->scale,
			image->height * image->scale,
			0
		);
		show_right_panel(current_algorithm, true);
		al_flip_display();
	}
	//! Kiedy kolor jest inny, kończymy działanie obecnej funkcji, zmiejszając zmienną liczącą wysokość stosu.
	else {
		measure_values.current_stack_height--;
		return;
	}

	//! Rekursywnie wykonujemy algorytm na pikselach na górze, dole, po bokach oraz po skosach.
	stack_based_recursive_eight_way_visualize(mouse_x + 1, mouse_y, image, current_color); // prawo
	stack_based_recursive_eight_way_visualize(mouse_x - 1, mouse_y, image, current_color); // lewo
	stack_based_recursive_eight_way_visualize(mouse_x, mouse_y + 1, image, current_color); // dół
	stack_based_recursive_eight_way_visualize(mouse_x, mouse_y - 1, image, current_color); // góra
	stack_based_recursive_eight_way_visualize(mouse_x + 1, mouse_y - 1, image, current_color); // prawo-góra
	stack_based_recursive_eight_way_visualize(mouse_x + 1, mouse_y + 1, image, current_color); // prawo-dół
	stack_based_recursive_eight_way_visualize(mouse_x - 1, mouse_y - 1, image, current_color); // lewo-góra
	stack_based_recursive_eight_way_visualize(mouse_x - 1, mouse_y + 1, image, current_color); // lewo-dół

	//! Po zakończeniu funkcji zmniejszamy zmienną liczącą wysokość stosu
	measure_values.current_stack_height--;
}

/*!
* Algorytm wypełniania oparty na kolejce.
* Zamienia kolor bierzącego piksela na kolor wypełnienia, a następnie sąsiednie piksele wstawia do kolejki.
* Każdy piksel z kolejki jest sprawdzany, czy jest konieczność zmiany jego koloru. Jeśli tak,
* zmienia się jego kolor, a sąsiednie dla niego piksele zostają wstawione do kolejki.
* Co każdy zamalowany piksel zapisuje zdjęcie, wczytuje je na nowo i wyświetla, czekając 100ms.
* \param uint32_t mouse_x Pozycja piksela na osi X.
* \param uint32_t mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void queue_based_four_way_visualize(uint32_t mouse_x, uint32_t mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;

	//! Sprawdzamy czy weszliśmy na kolor taki sam jakim malujemy, jeśli tak, kończymy działąnie funkcji
	if (
		replacement_color.r == current_color.r
		&& replacement_color.g == current_color.g
		&& replacement_color.b == current_color.b) {
		return;
	}

	//! Tworzymy kolejkę przechowującą współrzędne badanych pikseli
	QueuePointers queue = { NULL, NULL };

	//! Dodajemy kliknięty piksel do kolejki
	enqueue(&queue, mouse_x, mouse_y);

	//! Zmienne przechowujące współrzędne badanych pikseli
	uint32_t position_x;
	uint32_t position_y;

	//! Pętla badająca każde współrzędne piksela w kolejce aż do momentu zwolnienia kolejki
	while (queue.head != NULL) {

		//! Wczytujemy nową pozycje piksela
		dequeue(&queue, &position_x, &position_y);

		//! Odczytujemy kolor badanego piksela
		Color_t current_pixel_color = { 0 };
		get_pixel_color(&current_pixel_color, position_x, position_y, image);

		/*!
		* Jeśli kolor jest taki sam, jak kolor na który klikneliśmy, zmieniamy jego kolor.
		* Do kolejki dodajemy jego sąsiednie piksele.
		* Za każdym razem zdjęcie jest zapisywane na dysk, chwila przerwy i ponownie wczytywane w celu
		* zwizualizowania działania algorytmu.
		*/
		if (
			current_color.r == current_pixel_color.r
			&& current_color.g == current_pixel_color.g
			&& current_color.b == current_pixel_color.b
			) {
			swap_color(image, position_x, position_y);
			save_image_to_bmp("Images/Result.bmp", image->as_array, image->stb_x, image->stb_y, image->stb_comp);
			al_rest(0.1);
			al_destroy_bitmap(image->image);
			image->image = al_load_bitmap("Images/Result.bmp");
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_scaled_bitmap(
				image->image,
				0,
				0,
				image->width,
				image->height,
				0,
				0,
				image->width * image->scale,
				image->height * image->scale,
				0
			);
			show_right_panel(current_algorithm, true);
			al_flip_display();

			//! Jeśli nie wyszliśmy poza obszar zdjęcia, dodajemy sąsiednie piksele do kolejki.
			if (position_x > 0) {
				enqueue(&queue, position_x - 1, position_y);
			}
			if (position_y > 0) {
				enqueue(&queue, position_x, position_y - 1);
			}
			if (position_x < image->width - 1) {
				enqueue(&queue, position_x + 1, position_y);
			}
			if (position_y < image->height - 1) {
				enqueue(&queue, position_x, position_y + 1);
			}
		}

	}
}

/*!
* Algorytm wypełniania powierzchni. Działa w sposób rekurencyjny.
* Wypełnia poziomą linie na kolor wypełnienia. Przy każdym zmieniamym pikselu wywołuje samą siebie dla piksela wyżej i piksela niżej.
* Przed zmianą koloru kolejnych pikseli sprawdza, czy jest on taki sam, jak kolor, na który klikneliśmy.
* Co każdą zamalowaną linię zapisuje zdjęcie, wczytuje je i wyświetla na nowo, czekając 100ms.
* \param int mouse_x Pozycja piksela na osi X.
* \param int mouse_y Pozycja piksela na osi Y.
* \param Image* image Modyfikowany obraz.
* \param Color_t current_color Kolor obecnego piksela.
*/
void scanline_recursive_visualize(int mouse_x, int mouse_y, Image* image, Color_t current_color) {

	//! Zwiększamy o 1 ilość wywołań funkcji
	measure_values.recursion_count += 1;
	measure_values.current_stack_height++;
	if (measure_values.max_stack_height < measure_values.current_stack_height) {
		measure_values.max_stack_height = measure_values.current_stack_height;
	}

	// Sprawdzamy czy jestesmy poza widocznym obszarem.
	if (mouse_x >= image->width || mouse_x < 0 || mouse_y >= image->height || mouse_y < 0) {
		// Jesli tak, to wychodzimy z funkcji.
		measure_values.current_stack_height--;
		return;
	}

	//Sprawdzamy czy weszliœmy na kolor taki sam jakim malujemy
	if (replacement_color.r == current_color.r && replacement_color.g == current_color.g && replacement_color.b == current_color.b) {
		measure_values.current_stack_height--;
		return;
	}


	Color_t current_pixel_color = { 0 };


	if (
		replacement_color.r == current_color.r
		&& replacement_color.g == current_color.g
		&& replacement_color.b == current_color.b) {
		measure_values.current_stack_height--;
		return;
	}

	uint32_t right_x = 0;
	for (right_x = mouse_x; right_x < image->width; ++right_x) {

		get_pixel_color(&current_pixel_color, right_x, mouse_y, image);

		if (current_pixel_color.r != current_color.r
			|| current_pixel_color.g != current_color.g
			|| current_pixel_color.b != current_color.b
			) {
			break;
		}
		else {
			swap_color(image, right_x, mouse_y);
		}
	}
	uint32_t left_x = mouse_x - 1;
	for (left_x; left_x >= 0 && left_x < image->width; --left_x) {

		get_pixel_color(&current_pixel_color, left_x, mouse_y, image);

		if (current_pixel_color.r != current_color.r
			|| current_pixel_color.g != current_color.g
			|| current_pixel_color.b != current_color.b
			) {
			break;
		}
		else {
			swap_color(image, left_x, mouse_y);
		}
	}


	if (left_x != 0) {
		++left_x;
	}

	//! Sprawdzamy piksele od lewej strony wypełnionego paska do prawej.
	//! Jeśli wykryjemy powyżej lub poniżej nich wypełniany kolor, rekursywnie wykonujemy scanline_recursive().
	for (left_x; left_x < right_x; ++left_x) {

		get_pixel_color(&current_pixel_color, left_x, mouse_y - 1, image);

		if (
			current_pixel_color.r == current_color.r
			&& current_pixel_color.g == current_color.g
			&& current_pixel_color.b == current_color.b
			) {
			//------------------------////
			save_image_to_bmp("Images/Result.bmp", image->as_array, image->stb_x, image->stb_y, image->stb_comp);
			al_rest(0.1);
			al_destroy_bitmap(image->image);
			image->image = al_load_bitmap("Images/Result.bmp");
			al_clear_to_color(al_map_rgb(0, 0, 0));
			// wyswietlenie obrazu i panelu bocznego
			al_draw_scaled_bitmap(
				image->image,
				0,
				0,
				image->width,
				image->height,
				0,
				0,
				image->width * image->scale,
				image->height * image->scale,
				0
			);
			show_right_panel(current_algorithm, true);
			al_flip_display();
			//------------------------////
			scanline_recursive_visualize(left_x, mouse_y - 1, image, current_color);
		}

		get_pixel_color(&current_pixel_color, left_x, mouse_y + 1, image);
		if (
			current_pixel_color.r == current_color.r
			&& current_pixel_color.g == current_color.g
			&& current_pixel_color.b == current_color.b
			) {
			//------------------------////
			save_image_to_bmp("Images/Result.bmp", image->as_array, image->stb_x, image->stb_y, image->stb_comp);
			al_rest(0.1);
			al_destroy_bitmap(image->image);
			image->image = al_load_bitmap("Images/Result.bmp");
			al_clear_to_color(al_map_rgb(0, 0, 0));
			// wyswietlenie obrazu i panelu bocznego
			al_draw_scaled_bitmap(
				image->image,
				0,
				0,
				image->width,
				image->height,
				0,
				0,
				image->width * image->scale,
				image->height * image->scale,
				0
			);
			show_right_panel(current_algorithm, true);
			al_flip_display();
			//------------------------////

			scanline_recursive_visualize(left_x, mouse_y + 1, image, current_color);
		}
	}

	measure_values.current_stack_height--;
}