#if !defined(VERSION)
#define VERSION ""
#endif

#include <iostream>
#include <string>
#include <cstring>

using std::string;

#include "SDL/SDL.h"

#include "display.hpp"
#include "keyboard.hpp"

int main(int argc, char** argv)
{
    if (argc >= 2 && string(argv[1]) == "--version") {
        std::cout << VERSION;
        exit(0);
    }

    string initial_value = "";
    string title = "";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cerr << "Usage: kbinput -i [INITIAL VALUE] -t [SCREEN TITLE]" << std::endl;
            exit(0);
        }
        else if (strcmp(argv[i], "-i") == 0)
            initial_value = (string)(argv[++i]);
        else if (strcmp(argv[i], "-t") == 0)
            title = (string)(argv[++i]);
        else {
            std::cerr << "Unrecognized argument: " << argv[i] << std::endl;
            exit(0);
        }
    }

    int quit = 0;
    Display* display = new Display();
    Keyboard* kb = new Keyboard(display, initial_value, title);

    auto input_handler = [&kb](SDLKey key, Uint8 type, int repeating) {
        return kb->handleKeyPress(key, type, repeating);
    };

    auto frame_handler = [display, input_handler](void) {
        return display->onInputEvent(input_handler);
    };

    while (!quit) {
        quit = display->requestFrame(frame_handler);
    }

    delete display;

    if (!kb->cancelled) {
        std::cout << "\n\nRESULT:" << std::endl;
        std::cout << kb->getValue() << std::endl;
    }
    
    delete kb;

    return kb->cancelled;
}

/* 
entry point for the library version of kbinput
*/

extern "C" const char* launch_keyboard(const char* initial_value, const char* title) {
    Display* display = nullptr;
    Keyboard* kb = nullptr;
    char* return_value = nullptr;

    try {
        display = new Display();

        kb = new Keyboard(display, initial_value, title);

        display->lib_mode = true; // track libmode for the destructor

        int quit = 0;
        std::string result = "";

        auto input_handler = [&kb](SDLKey key, Uint8 type, int repeating) {
            return kb->handleKeyPress(key, type, repeating);
        };

        auto frame_handler = [display, input_handler](void) {
            return display->onInputEvent(input_handler);
        };

        while (!quit) {
            quit = display->requestFrame(frame_handler);
        }

        if (!kb->cancelled) {
            result = kb->getValue();
            std::cout << "\n\nRESULT:" << std::endl;
            std::cout << result << std::endl;
        }

        return_value = (char*)malloc(result.length() + 1);
        if (return_value != nullptr) {
            strcpy(return_value, result.c_str());
        }
    }
    catch (const std::exception& e) {
        std::cout << "\n\nERROR:" << std::endl;
        std::cout << "Exception caught in launch_keyboard: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "\n\nERROR:" << std::endl;
        std::cout << "Unknown exception caught in launch_keyboard" << std::endl;
    }

    delete display;
    delete kb;

    return return_value;
}
