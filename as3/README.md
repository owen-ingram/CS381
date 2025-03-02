1. To fetch git submodules, `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git` Then clone the dependencies for the submodule `git submodule init` and `git submodule update --init --recursive`

2. Inside the AS1 file run the command `rm -rf build`, then make a new build folder `mkdir build`, change into this build folder `cd build`, inside the build folder, run `cmake ..` and `make` to compile the libraries. To run the program, use the command, `./as3`. 

3. W to increase speed, A to move left, D to move right, S to reduce speed, Z to stop velocity.

DT is the time that is taken between frames. DT is important because the game will rely on real time rather than frame time. This solves the problem of computers running games at different frame rates. If a computer were to have a low frame rate, the object would take a longer time to travel where a higher end computer with a higher frame rate would take a shorter time to travel. We are able to get delta time using GetFrameTime();

Extra Credit:
Camera movement implemented
Car can now fly, Q to fly up, E to fly down