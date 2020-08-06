//
// Created by Yaroslav on 28.07.2020.
//

#ifndef OPENGLTEST_WINDOW_HPP
#define OPENGLTEST_WINDOW_HPP


#include <string>

class Window
{
private:
    GLFWwindow *mainWindow;
public:
    static int _width, _height;
    Window(const char *title, int width, int height);
    ~Window();

    void startLoop();
    void makeContextCurrent();
};


#endif //OPENGLTEST_WINDOW_HPP
