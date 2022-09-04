#pragma once

#include <onnxruntime_cxx_api.h>

// This class is a workaround  since, for some reason we cant have several ONNX Environments
// created. When instantiating a second one, it'll crash. With this we ensure only one is created 
// and shared among models.
class SessionSingleton
{
public:

    std::shared_ptr<Ort::Env> enviro;

    static SessionSingleton& getInstance() {
        static SessionSingleton INSTANCE;
        return INSTANCE;
    }
    ~SessionSingleton() {
        enviro->release();
    }
private:

    SessionSingleton() {
        enviro = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "env");
        enviro->DisableTelemetryEvents();
    }

};