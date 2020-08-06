# Develop AiTrack  

This guide is aimed at developers who want to contribute to the project.


## Development environment
- **Visual Studio 2019** (at least). Or MSBuild tools for C++.
- **Qt framework**: AITrack has been developed using version 5.9.9 and also tested with the most recent version at the moment (5.15), so it should work with the latest one.
- **cmake**: For setting up the project/dependencies.


## Setting up the project

1. Clone this repo: `git clone https://github.com/AIRLegend/aitracker`
2. Cd into the directory: `cd aitracker`.
3. Download and unpack dependencies: `cmake .`
4. Open `Camera.sln` with Visual Studio (or build it with `msbuild Camera.sln`).
5. Copy the `models/` dir to the same location as `Client.exe`.
6. Copy the necessary .dlls to the executable directory (`libusb.dll`, `onnxruntime.dll`, `opencv_world430.dll`). And then you can run `windeployqt <.exe path>` to let Qt auto-copy the remaining .dlls. 
7. You should be good to go!


## How to contribute

1. Create a fork of the repo and clone it on your local machine.
1. Once cloned, create a new branch for your feature/fix.
2. Try to document your code in order to debug/review it better.
3. Test your feature building and using the program.
4. If possible, squash all your commits into one with clear explanation of the changes.
5. Push your code to your fork.
6. Make a pull request.
    - To /master if you consider is a fully functional feature.
    - To /dev if it's part of something bigger that needs to be further developed.

You can start with anything (documentation, code cleaning...) and when you're more familiar with the codebase, try getting yout hands dirtier!

Thank you if you decide to contribute!