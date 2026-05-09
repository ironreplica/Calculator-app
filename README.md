# Calculator app

A calculator for windows written in C++ for fun

## Prerequisites
Install Oat++, CMake and docker.
Additionally, make sure docker desktop is running.

# Oat Usage

To build the application, cd into your src/ folder, and run the following command

`docker build -f dockerfile -t my-oatpp-app .`

If the build succeeds, now run this command to run the app

`docker run -p 8000:8000 my-oatpp-app`