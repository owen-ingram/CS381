1. To fetch git submodules, `git submodule add https://github.com/joshuadahlunr/raylib-cpp.git` Then clone the dependencies for the submodule `git submodule init` and `git submodule update --init --recursive`

2. Inside the AS5 file run the command `rm -rf build`, then make a new build folder `mkdir build`, change into this build folder `cd build`, inside the build folder, run `cmake ..` and `make` to compile the libraries. To run the program, use the command, `./as5`. 

3. W to increase speed, S to decrease speed. A/D to rotate. Tab to change entity.

The selection index works by cycling through the vector when tab is pressed. When the entity is selected, a bounding box will be drawn around it to highlight that the entity is selected. Monolithic Entities is used for small projects where the entities are all similar in scale. Ad Hoc would be for larger projects where entities are very different and cannot be repeated. I can see the efficiency of monolithic but I would still prefer Ad Hoc because there are less restraints.