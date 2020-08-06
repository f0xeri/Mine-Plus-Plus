#include <iostream>
#include <GLFW/glfw3.h>
#include <thread>
#include "Logger.hpp"
#include "Window/Window.hpp"


int main()
{
    Window window("Mine++", 1920, 1080);
    window.makeContextCurrent();
    window.startLoop();
    glfwTerminate();
    return 0;
}
