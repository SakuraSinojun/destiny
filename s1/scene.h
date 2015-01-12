
#pragma once

class Scene {
public:
    Scene();
    virtual ~Scene();

    virtual bool run(void) = 0;

};

