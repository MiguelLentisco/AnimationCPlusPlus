#pragma once

class Application
{
private:
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

public:
    Application() {}
    virtual ~Application() = default;

    virtual void Initialize() {}
    virtual void Update(float inDeltaTime) {}
    virtual void Render(float inAspectRatio) {}
    virtual void Shutdown() {}
    
}; // Application
