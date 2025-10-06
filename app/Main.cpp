#include <iostream>
#include <conio.h>

#include "Logger/Logger.hpp"
#include "OpenGLUtils/Window.hpp"

int main() {
    Logs::Logger::getInstance().setMinimumLogLevel(Logs::LogLevel::Debug);

    try {
        LOG_DEBUG("Creating the window ...");
        OpenGLUtils::Window window(720, 480, "Hello window !");
        LOG_DEBUG("done");
    } catch (std::exception e) {
        LOG_FATAL("Could not create the window: {}", e.what());
        return 84;
    }
    return 0;
}