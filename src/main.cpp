#include <iostream>
#include <Factory.h>
#include <SceneManager.h>

int main(int argc, char *argv[]) {
    // Load configuration
    TWS::setConfigFile("config.ini");

    // Create a scene manager instance
    TWS::SceneManager *sceneManager = new TWS::SceneManager();

    // If we can initialize the lesson
    if (sceneManager->init()) sceneManager->run();
    else std::cerr << "TWS::SceneManager initializiation failed!" << std::endl;

    delete sceneManager;
    return 0;
}
