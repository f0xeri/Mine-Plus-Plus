#include <GLFW/glfw3.h>
#include "Window/Window.hpp"


int main()
{
    Window window("Mine++", 1920, 1080);
    window.makeContextCurrent();
    window.startLoop();
    return 0;
}
