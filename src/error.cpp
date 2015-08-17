#include "error.hpp"

void Panic( const char * error ) {
    std::string error_data = error;
    error_data += " failure!";
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", error_data.c_str(), nullptr );
    exit( EXIT_FAILURE );
}

void Info( const char * info ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Info", info, nullptr );
}