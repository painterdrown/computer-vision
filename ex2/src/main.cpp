//
//  main.cpp
//  MyCanny
//
//  Created by Raincome on 29/03/2018.
//  Copyright © 2018 painterdrown. All rights reserved.
//

// g++ -o bin/main src/main.cpp src/Canny.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11

#include "Canny.hpp"

int main(int argc, const char * argv[]) {
    // Canny bigben("img/bigben.jpg");
    Canny lena("img/lena.jpg");
    return 0;
}
