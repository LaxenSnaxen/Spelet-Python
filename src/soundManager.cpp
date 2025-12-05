#include <miniaudio.h>
#include <iostream>

// Great Suck

// Hej hej

class soundManager
{
private:
    ma_result result;
    ma_engine engine;
    ma_engine_config engineConfig;

    soundManager(){};
public:

    static soundManager& get() {
        static soundManager instance;
        return instance;
    }

    void init();
    void play(const char*);
    void stop();
};

void soundManager::init(){
    engineConfig = ma_engine_config_init();

    result = ma_engine_init(&engineConfig, &engine);
    if(result != MA_SUCCESS){
        std::cout << "Error initialising sound manager" << std::endl;
    }
}

void soundManager::play(const char* filepath){
    ma_engine_play_sound(&engine, filepath, NULL);
}

void soundManager::stop(){
    ma_engine_stop(&engine);
}
