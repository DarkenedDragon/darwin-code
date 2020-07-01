/**
* @author Eli Jelesko
* @date 6/25/2020
*/ 

#ifndef COLOR_OUTPUT_H
#define COLOR_OUTPUT_H

#include <sstream>
#include <string>
#include <iostream>

using namespace std;

namespace ColorOutput {
    // syntax: $c{text} where c is the color code
    // r = red, b = blue, y = yellow, g = green, m = magenta
    // add a b infront of the color code for bold text
    const char* colorize(string str); // Adds ASCII color codes
}

#endif
