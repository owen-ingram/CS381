1. To fetch git submodules, `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git` Then clone the dependencies for the submodule `git submodule init` and `git submodule update --init --recursive`

2. Inside the AS4 file run the command `rm -rf build`, then make a new build folder `mkdir build`, change into this build folder `cd build`, inside the build folder, run `cmake ..` and `make` to compile the libraries. To run the program, use the command, `./as4`. 

3. W to increase speed, A to move left, D to move right, S to reduce speed, space to stop velocity. Q to increase elevation, E to decrease elevation

Conenado, a game about a flying tornado cone. Added some background music and some ascii art.