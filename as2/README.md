1. To fetch git submodules, `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git` Then clone the dependencies for the submodule `git submodule init` and `git submodule update --init --recursive`

2. Inside the AS1 file run the command `rm -rf build`, then make a new build folder `mkdir build`, change into this build folder `cd build`, inside the build folder, run `cmake ..` and `make` to compile the libraries. To run the program, use the command, `./as2`. 

3. not applicable

The point of the DrawBoundedModel function is to draw 3d models after transformations have been applied to them. This function can be used to set transformations when a model is loaded. Yes it can transform a model relative to the parent.
