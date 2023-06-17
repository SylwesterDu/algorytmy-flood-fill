//! \file right_panel.c Funkcja odpowialna za wyświetlanie panelu z informacjami.

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdint.h>
#include "values.h"

/*!
* Wyświetlanie prawego panelu z informacjami. 
* Funkcja na podstawie argumentu algorithm podświetla obecnie wykorzystywany algorytm.
* Argument show_measure_result decyduje, czy wypisać zmienne ze struktury MeasureValues.
* \param algorithm_t algorithm Wybrany algorytm wypełniania.
* \param bool show_measure_result Czy pokazujemy wyniki działania algorytmu.
*/ 
void show_right_panel(algorithm_t algorithm, bool show_measure_result) {

	ALLEGRO_USTR_INFO info;

	//! Nazwy algorytmów
	static uint8_t* algorithm_names[] = {
		"STACK_BASED_RECURSIVE_FOUR_WAY",
		"STACK_BASED_RECURSIVE_EIGHT_WAY",
		"QUEUE_BASED_FOUR_WAY",
		"RECURSIVE_SCANLINE"
	};

	al_draw_text(
		hint_font,
		al_map_rgb(150, 150, 150),
		window_width * 0.6,
		window_height * 0.12,
		0,
		"ZMIENIAJ TRYB -SPACJA-"
	);

	al_draw_ustr(
		main_font,
		al_map_rgb(100, 170, 100),
		window_width * 0.6,
		window_height * 0.15,
		0,
		al_ref_cstr(&info, visualization_mode_available
			? visualisation_mode
				? "WIZUALIZACJA"
				: "POMIAR CZASU"
			: "OBRAZ ZBYT DUŻY, TYLKO POMIAR CZASU" 
			)
	);

	al_draw_line(
		window_width * 0.55,
		window_height * 0.22,
		window_width * 0.95,
		window_height * 0.22,
		al_map_rgb(70, 70, 100),
		1
	);

	al_draw_text(
		hint_font,
		al_map_rgb(150, 150, 150),
		window_width * 0.6,
		window_height * 0.26,
		0,
		"ZMIENIAJ ZDJĘCIE -TAB-"
	);

	al_draw_ustr(
		main_font,
		al_map_rgb(140, 140, 180),
		window_width * 0.6,
		window_height * 0.29,
		0,
		image_names[current_image]
	);

	al_draw_line(
		window_width * 0.55,
		window_height * 0.37,
		window_width * 0.95,
		window_height * 0.37,
		al_map_rgb(70, 70, 100),
		1
	);

	al_draw_ustr(
		hint_font,
		al_map_rgb(150, 150, 150),
		window_width * 0.6,
		window_height * 0.42,
		0,
		al_ref_cstr(&info, "ZMIENIAJ ALGORYTMY STRZAŁKAMI")
	);

	al_draw_filled_rectangle( // Podœwietlenie obecnego algorytmu
		window_width * 0.59,
		window_height * 0.397 + window_height * (algorithm + 1) * 0.05,
		window_width * 0.9,
		window_height * 0.4 + window_height * (algorithm + 1) * 0.05 + window_height / 30, // punkt pocz¹tkowy + wysokoœæ czcionki
		al_map_rgb(100, 100, 150)
	);

	for (uint32_t i = 0; i < ALGORITHM_AMOUNT; i++) { // Wyœwietlenie wszystkich algorytmów
		al_draw_textf(
			main_font,
			al_map_rgb(200, 200, 200),
			window_width * 0.6,
			window_height * 0.4 + window_height * (i + 1) * 0.05,
			0,
			"%s",
			algorithm_names[i]
		);
	}

	al_draw_line(
		window_width * 0.55,
		window_height * 0.64,
		window_width * 0.95,
		window_height * 0.64,
		al_map_rgb(70, 70, 100),
		1
	);

	if (show_measure_result) 
	{
		ALLEGRO_USTR* time_spent_in_function = al_ustr_newf(
			visualisation_mode ? "CZAS DZIAŁANIA FUNKCJI: --" : "CZAS DZIAŁANIA FUNKCJI: %llu ms", 
			measure_values.duration
		);
		al_draw_ustr(
			main_font,
			al_map_rgb(200, 200, 200),
			window_width * 0.6,
			window_height * 0.7,
			0,
			time_spent_in_function
		);
		al_ustr_free(time_spent_in_function);

		ALLEGRO_USTR* clock_cycles_amount = al_ustr_newf(
			visualisation_mode ? "ILOŚĆ CYKLI ZEGARA: --" : "ILOŚĆ CYKLI ZEGARA: %llu", 
			measure_values.clock_cycle_count
		);
		al_draw_ustr(
			main_font,
			al_map_rgb(200, 200, 200),
			window_width * 0.6,
			window_height * 0.75,
			0,
			clock_cycles_amount
		);
		al_ustr_free(clock_cycles_amount);

		ALLEGRO_USTR* function_call_amount = al_ustr_newf("ILOŚĆ WYWOLAŃ FUNKCJI: %llu", measure_values.recursion_count);
		al_draw_ustr(
			main_font,
			al_map_rgb(200, 200, 200),
			window_width * 0.6,
			window_height * 0.8,
			0,
			function_call_amount
		);
		al_ustr_free(function_call_amount);

		//Pomijamy liczenie wysokosci stosu w algorytmie, który go nie wykorzystuje
		if (algorithm != QUEUE_BASED_FOUR_WAY) {
			ALLEGRO_USTR* max_stack_height = al_ustr_newf("MAKSYMALNA WYSOKOŚĆ STOSU: %llu", measure_values.max_stack_height);
			al_draw_ustr(
				main_font,
				al_map_rgb(200, 200, 200),
				window_width * 0.6,
				window_height * 0.85,
				0,
				max_stack_height
			);
			al_ustr_free(max_stack_height);

			ALLEGRO_USTR* current_stack_height = al_ustr_newf("OBECNA WYSOKOŚĆ STOSU: %llu", measure_values.current_stack_height);
			if (visualisation_mode) {
				al_draw_ustr(
					main_font,
					al_map_rgb(200, 200, 200),
					window_width * 0.6,
					window_height * 0.9,
					0,
					al_ustr_newf("OBECNA WYSOKOŚĆ STOSU: %llu", measure_values.current_stack_height)
				);
			}
			al_ustr_free(current_stack_height);
		}
	}
}