#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pokemon.h"
#include "display.h"

#define COLORSCRIPTS_PATH "/pokemonc/assets/colorscripts/"

char *get_colorscripts_path(const char *slug, const char *form, int shiny) {
    char *home_dir = getenv("HOME");
    if (!home_dir) {
        printf("Unable to get user home directory path.\n");
        return NULL;
    }

    char *art_path = malloc(strlen(home_dir) + strlen(COLORSCRIPTS_PATH) + strlen(slug) + strlen(form) + 50);
    if (!art_path) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    if (strcmp(form, "regular") == 0) {
        snprintf(art_path, 256, "%s%s%s/%s", home_dir, COLORSCRIPTS_PATH, shiny ? "shiny" : "regular", slug);
    } else {
        snprintf(art_path, 256, "%s%s%s/%s-%s", home_dir, COLORSCRIPTS_PATH, shiny ? "shiny" : "regular", slug, form);
    }

    // printf("Art path: %s\n", art_path);
    if (access(art_path, F_OK) != 0) {
        printf("Art file not found: %s\n", art_path);
        free(art_path);
        return NULL;
    }

    return art_path;
}

void list_all_pokemon(Pokemon *pokemon_list, int count) {
    for (int i = 0; i < count; i++) {
        if (pokemon_list[i].slug) {
            printf("%s\n", pokemon_list[i].slug);
        } else {
            printf("The name of the Pokémon was not found.\n");
        }
    }
}

void display_pokemon(Pokemon *pokemon_list, int count, const char *name, const char *form, int shiny) {
    for (int i = 0; i < count; i++) {
        if (pokemon_list[i].slug && strcmp(pokemon_list[i].slug, name) == 0) {
            char *art_path = get_colorscripts_path(pokemon_list[i].slug, form, shiny);
            if (!art_path) {
                printf("Unable to read art file for Pokémon '%s'.\n", name);
                return;
            }

            FILE *art_file = fopen(art_path, "r");
            free(art_path);
            if (!art_file) {
                printf("Unable to open art file for Pokémon '%s'.\n", name);
                return;
            }

            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), art_file) != NULL) {
                printf("%s", buffer);
            }

            fclose(art_file);
            return;
        }
    }
    printf("Pokémon %s not found.\n", name);
}

void display_random_pokemon(Pokemon *pokemon_list, int count, int shiny, int no_title, int no_mega, int no_gmax, int no_regional, int info, int gen_min, int gen_max, int *gen_list, int gen_count) {
    int index;
    const char *form;
    int valid_pokemon_found = 0;

    do {
        index = rand() % count;
        form = pokemon_list[index].form_count > 0 && pokemon_list[index].forms[0] ? pokemon_list[index].forms[0] : "regular";
        int pokemon_gen = pokemon_list[index].gen;
        
        int in_gen_list = 0;

        // Check if the Pokémon's generation is in the specified gen_list
        if (gen_count > 0) {
            for (int i = 0; i < gen_count; i++) {
                if (pokemon_gen == gen_list[i]) {
                    in_gen_list = 1;
                    break;
                }
            }
        } else {
            // If no gen_list is provided, use gen_min and gen_max
            in_gen_list = (pokemon_gen >= gen_min && pokemon_gen <= gen_max);
        }

        // Ensure the Pokémon meets all filter criteria
        if (in_gen_list &&
            !(no_mega && (strstr(form, "mega") != NULL || strstr(form, "mega-X") != NULL || strstr(form, "mega-Y") != NULL)) &&
            !(no_gmax && strstr(form, "gmax") != NULL) &&
            !(no_regional && (strstr(form, "alola") != NULL || strstr(form, "galar") != NULL || strstr(form, "hisui") != NULL || strstr(form, "paldea") != NULL))) {
            valid_pokemon_found = 1;
        }

    } while (!valid_pokemon_found); // Keep looping until a valid Pokémon is found

    const char *name = pokemon_list[index].slug;

    if (!no_title) {
        printf("%s (%s)\n", name, form);
    }

    display_pokemon(pokemon_list, count, name, form, shiny);

    if (info && pokemon_list[index].desc) {
        printf("\n%s\n", pokemon_list[index].desc);
    }
}
