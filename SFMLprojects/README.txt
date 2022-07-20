To make the usage easier assets are at the moment small enough to be included in the versioning.
Current assets remain almost same size after archiving so they are not archived.

Compilation
C++ 17 or newer is required at the moment
This project uses SFML library for window management(version 2.5.1#12).
I used vckpg to install it and have it integrated in the VS project(https://vcpkg.io/en/)
Installation using vcpkg requires to run:
    vcpkg install SFML
    vcpkg integrate install
Be sure to check that SFML version and target architecture matches
After that all SFML includes should be working. If the includes fail, try to create a new project.
If you have issues with assets loading check the working directory of the application. Relative path to assets has to be: "..\Assets"
If the problems persist open an issue.