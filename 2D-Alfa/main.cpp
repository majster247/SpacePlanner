#include <iostream>
#include <cstdlib> // for rand()
#include <ctime>   // for time()

using namespace std;

int width, height;

class Terrain {
    // 2D array of size width x height filled with 0
    int **arr;

public:
    // Constructor to initialize the array
    Terrain(int h, int w) {
        height = h;
        width = w;
        arr = new int*[width];
        for (int i = 0; i < width; i++) {
            arr[i] = new int[height];
            for (int j = 0; j < height; j++) {
                arr[i][j] = 0;
            }
        }
    }

    // Destructor to clean up the array
    ~Terrain() {
        for (int i = 0; i < width; i++) {
            delete[] arr[i];
        }
        delete[] arr;
    }

    // Function to create a random terrain near sea
    void seaTerrain() {
        srand(time(0)); // Seed for random number generation
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (i == 0 && j == 0) {
                    arr[i][j] = -8;
                } else if (i == 0) {
                    arr[i][j] = arr[i][j - 1] + rand() % 5;
                } else if (j == 0) {
                    arr[i][j] = arr[i - 1][j] + rand() % 5;
                } else {
                    arr[i][j] = arr[i - 1][j] + rand() % 5;
                }

                // Ensure values stay within sea limit
            }
        }
    }

    // Function to print the map
    void printMap() {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                cout << arr[i][j] << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    // Get the width and height
    cin >> width >> height;
    Terrain map(height, width);
    map.seaTerrain();
    map.printMap();

    return 0;
}
