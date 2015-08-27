#include "error.hpp"

void Panic( const char * error ) {
    std::string errorData = error;
    errorData += " failure!";
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", errorData.c_str(),
                              nullptr );
    exit( EXIT_FAILURE );
}

void Info( const char * info ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Info", info, nullptr );
}
