// Eli Jelesko 6/25/2020

#include "ColorOutput.h"

const char* ColorOutput::colorize(string str) {
    /*
    %r = red
    %br = bold red
    etc
    */
    stringstream ss;
    int i = 0;
    
    while (i < str.size()) {
        if (str.at(i) == '$' && str.size() - i > 2 && (str.at(i + 2) == '{' || str.at(i + 3) == '{')) {
            i++;
            ss << "\033["; // start the color breakout
            // Check to see if bold or not
            if (str.at(i) == 'b') { 
                ss << "1;"; // denoting a bold color
                i++;
            } else {
                ss << "0;";
            }
           
            // determine the color
            switch (str.at(i)) {
                case 'r' :
                    ss << "31m";
                    break;
                case 'g':
                    ss << "32m";
                    break;
                case 'y': 
                    ss << "33m";
                    break;
                case 'b':
                    ss << "34m";
                    break;
                case 'm': 
                    ss << "35m";
                    break;
                case 'c': 
                    ss << "36m";
                    break;
                default: 
                    ss << "0m";
            }

            i++; // move to the next character
            
            // make sure that there are brackets
            if (str.at(i) == '{') {
                // add the text
                i++; // skip the {
                while (i < str.size() && str.at(i) != '}') {
                    ss << str.at(i++);
                }
            } else { // if there aren't brackes someone skrewed up
                cerr << "Error while parsing printc string" << endl;
            }
            // close the breakout
            ss << "\033[0m";
         } else {
            ss << str.at(i);
         }
         i++;
    }
    static string output;
    output.clear();
    output = ss.str();
    return output.c_str();
}

