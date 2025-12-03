#include "App.h"

int main(int argc, char** argv) {
    App app(1280, 720);
    if(!app.init()) return -1;

    app.run();
    app.shutdown();

    return 0;
}
