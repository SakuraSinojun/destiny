
#pragma once


class Scene;
class Game {
public:
    ~Game();
    static Game* Get();

    void Start(void);

private:
    Game();

    void GenHero(void);
    Scene* scene;
};

