1. To fetch git submodules, `git submodule add https://github.com/owen-ingram/cs381.git` Then clone the dependencies for the submodule `git submodule init` and `git submodule update --init --recursive`

2. Inside the build folder, run `cmake ..` and `make` to compile the libraries. To run the program, use the command, `./as1`. 

3. not applicable

4. A speaker is able to create audio by taking the input of an audio file which is made up of numbers representing sound waves which is the transfered into analog signals that the speaker can read. raylib::AudioDevice is necessary because it handles controlling the audio for the app. It will initialize the audio that is being played by the program and will handle the data from the audio files being put through the program.