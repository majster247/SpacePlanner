#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>

const int EMPTY = 0;
const int ROAD = 1;
const int FOREST = 2;
const int BUILDING_LOW = 3;
const int BUILDING_MEDIUM = 4;
const int BUILDING_HIGH = 5;
const int TRAM_TRACK = 6;
const int SCHOOL = 7;
const int SPORTS_FIELD = 8;
const int SHOPPING_CENTER = 9;
const int SIDEWALK = 10;
const int BIKE_PATH = 11;

// Funkcja do mapowania typów struktur na kolory
sf::Color get_color(int structure_type) {
    switch (structure_type) {
        case EMPTY: return sf::Color::White;
        case ROAD: return sf::Color::Black;
        case FOREST: return sf::Color::Green;
        case BUILDING_LOW: return sf::Color::Yellow;
        case BUILDING_MEDIUM: return sf::Color::Blue;
        case BUILDING_HIGH: return sf::Color::Red;
        case TRAM_TRACK: return sf::Color::Magenta;
        case SCHOOL: return sf::Color::Cyan;
        case SPORTS_FIELD: return sf::Color::White;
        case SHOPPING_CENTER: return sf::Color::Yellow;
        case SIDEWALK: return sf::Color::Magenta;
        case BIKE_PATH: return sf::Color::Cyan;
        default: return sf::Color::Transparent;
    }
}

// Funkcja do dodawania struktury do siatki
void add_structure(std::vector<std::vector<int>>& grid, int structure_type, int start_row, int start_col, int height, int width) {
    for (int i = start_row; i < start_row + height; ++i) {
        for (int j = start_col; j < start_col + width; ++j) {
            if (i >= 0 && i < grid.size() && j >= 0 && j < grid[0].size()) {
                grid[i][j] = structure_type;
            }
        }
    }
}

// Funkcja do animowanego wyświetlania statusu
void display_status(const std::string& status, int duration) {
    std::string animation = "...";
    for (int i = 0; i < duration; ++i) {
        std::cout << "\r" << status << animation.substr(0, i % 4);
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "\r" << status << "    " << std::endl; // Clear line after animation
}

// Funkcja do generowania siatki miejskiej
std::vector<std::vector<int>> generate_city_grid(float width_km, float height_km) {
    int width_m = width_km * 1000;
    int height_m = height_km * 1000;

    std::cout << "Width: " << width_m << " meters, Height: " << height_m << " meters\n";

    // Inicjalizacja pustej siatki
    std::vector<std::vector<int>> city_grid(height_m, std::vector<int>(width_m, EMPTY));

    std::cout << "Initialized city grid\n";


    // Dodawanie dróg głównych (co 500m)
for (int i = 0; i < height_m; i += 500) {
    if (i < height_m) {
        for (int j = 0; j < width_m; ++j) {
            if (j < width_m) {
                city_grid[i][j] = ROAD;
            }
        }
    }
}

    std::cout << "Added main roads\n";

    for (int j = 0; j < width_m; j += 500) {
        for (int i = 0; i < height_m; ++i) {
            city_grid[i][j] = ROAD;
        }
    }

    // Dodawanie torów tramwajowych wzdłuż głównych dróg
    for (int i = 0; i < height_m; i += 500) {
        for (int j = 0; j < width_m; ++j) {
            if (i + 1 < height_m) city_grid[i + 1][j] = TRAM_TRACK;
        }
    }
    for (int j = 0; j < width_m; j += 500) {
        for (int i = 0; i < height_m; ++i) {
            if (j + 1 < width_m) city_grid[i][j + 1] = TRAM_TRACK;
        }
    }

    // Dodawanie lasów (losowo rozmieszczone grupy 100x100m)
    /*srand(time(0));
    for (int n = 0; n < 10; ++n) {
        int start_row = rand() % (height_m - 100);
        int start_col = rand() % (width_m - 100);
        add_structure(city_grid, FOREST, start_row, start_col, 100, 100);
    }*/

    // Dodawanie niskiej zabudowy (losowo rozmieszczone grupy 50x50m)
    for (int n = 0; n < 50; ++n) {
        int start_row = rand() % (height_m - 50);
        int start_col = rand() % (width_m - 50);
        add_structure(city_grid, BUILDING_LOW, start_row, start_col, 50, 50);
    }

    // Dodawanie infrastruktury
    add_structure(city_grid, SCHOOL, 200, 200, 50, 50);
    add_structure(city_grid, SPORTS_FIELD, 300, 300, 60, 60);
    add_structure(city_grid, SHOPPING_CENTER, 400, 400, 70, 70);

    // Dodawanie chodników (równolegle do głównych dróg)
    for (int i = 0; i < height_m; i += 500) {
        for (int j = 0; j < width_m; ++j) {
            if (i - 1 >= 0) city_grid[i - 1][j] = SIDEWALK;
        }
    }
    for (int j = 0; j < width_m; j += 500) {
        for (int i = 0; i < height_m; ++i) {
            if (j - 1 >= 0) city_grid[i][j - 1] = SIDEWALK;
        }
    }

   // Dodawanie ścieżek rowerowych (wzdłuż głównych dróg)
for (int i = 0; i < height_m; i += 500) {
    for (int j = 0; j < width_m; ++j) {
        if (i + 2 < height_m && j + 2 < width_m) city_grid[i + 2][j] = BIKE_PATH;
    }
}
for (int j = 0; j < width_m; j += 500) {
    for (int i = 0; i < height_m; ++i) {
        if (i + 2 < height_m && j + 2 < width_m) city_grid[i][j + 2] = BIKE_PATH;
    }
}


    return city_grid;
}

// Funkcja do rysowania siatki jako pixel art
void draw_city_grid(const std::vector<std::vector<int>>& city_grid, int pixel_size) {
    int height = city_grid.size();
    int width = city_grid[0].size();

    sf::RenderWindow window(sf::VideoMode(width * pixel_size, height * pixel_size), "City Grid Pixel Art");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                sf::RectangleShape pixel(sf::Vector2f(pixel_size, pixel_size));
                pixel.setPosition(j * pixel_size, i * pixel_size);
                pixel.setFillColor(get_color(city_grid[i][j]));
                window.draw(pixel);
            }
        }

        window.display();
    }
}

int main() {
    // Parametry wejściowe od użytkownika
    float width_km = 0.1;  // Szerokość w kilometrach
    float height_km = 0.1; // Wysokość w kilometrach

    // Generowanie siatki miasta
    std::vector<std::vector<int>> city_grid = generate_city_grid(width_km, height_km);

    // Wyświetlanie statusu generowania świata
    display_status("Generating World", 10);

    // Rozmiar pojedynczego piksela
    int pixel_size = 2;  // Zmniejszony dla lepszej wydajności

    // Rysowanie siatki jako pixel art
    draw_city_grid(city_grid, pixel_size);

    return 0;
}


/*TODO: 

-Generowanie siatki miasta z poziomu oceny oczekiwań
-Implementacja Tensorflow lokalnie
-Poprawka funkcji generacji lasów
-Implementacja funkcji generacji dróg głównych, torów tramwajowych, niskiej zabudowy, infrastruktury, chodników, ścieżek rowerowych
-Implementacja funkcji rysowania siatki jako pixel art oraz optymalizacja generacji końcowego obrazu w celu stworzenia przejrzystego i czytelnego obrazu

-Memory managment
-Opymalizacja i użycie wątków oraz GPU(?) do generacji siatki
-Implementacja funkcji do zapisu i odczytu siatki z pliku

-Wersja 3D z możliwością obracania kamery
-Przekierowanie do modelu 3D miasta w przeglądarce jako obiektu mapy 3D
-Implementacja funkcji do generacji 3D miasta w opaciu o modele 3D budynków, dróg, lasów, itp. (kilka patternów do wyboru po każdym)
-Implementacja funkcji do rysowania miasta w 3D w oparciu o wybrane modele 3D
-Implementacja funkcji do zapisu i odczytu miasta 3D z pliku

-Wytrenowanie modelu na podstawie wygenerowanych miast 2D i 3D
-Wytrenowanie modelu na podstawie danych z OpenStreetMap
-Wytrenowanie modelu na podstawie danych z Google Maps
-Wytrenowanie modelu na podstawie danych z Bing Maps
-Wytrenowanie modelu na podstawie danych z Here Maps
-Wytrenowanie modelu na podstawie danych ze zbiorów akademickich o planowaniu przestrzennym miast

*/