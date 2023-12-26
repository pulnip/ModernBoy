#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

class Game
{
  public:
    struct Factory
    {
        template<typename Concrete>
            requires std::derived_from<Concrete, Game>
        static std::shared_ptr<Concrete> make() noexcept
        {
            struct make_shared_enabler : public Concrete
            {
                make_shared_enabler()
                  : Concrete()
                {
                }
            };
            std::shared_ptr<Game> game = std::make_shared<make_shared_enabler>();
            game->load(game);
            return std::static_pointer_cast<Concrete>(game);
        }
        Factory() = delete;
        ~Factory() = delete;
    };
    ~Game() = default;

    // 게임이 끝나기 전까지 게임 루프를 실행
    void runLoop() noexcept;
    // 게임 종료
    void shutDown() noexcept;

    void quit() noexcept { isRunning = false; }
    class SDL_Texture* getTexture(const char* fileName) noexcept;
    void appendActor(const std::shared_ptr<class Actor> actor) noexcept;
    void appendDrawable(const std::weak_ptr<class DrawComponent> drawable) noexcept;
    void removeDrawable(const std::weak_ptr<class DrawComponent> drawable) noexcept;

  protected:
    Game() noexcept;

  private:
    // 게임 세계의 액터를 로드
    virtual void load(const std::weak_ptr<Game> self) noexcept = 0;

    // 게임 루프를 위한 헬퍼 함수
    void processInput() noexcept;
    void updateGame() noexcept;
    void generateOutput() noexcept;

    // 이미지 로딩 과정 캡슐화
    class SDL_Texture* loadTexture(const char* fileName) noexcept;

  private:
    const uint8_t* keyState;
    // SDL이 생성한 윈도우
    class SDL_Window* window;
    class SDL_Renderer* renderer;
    std::map<const std::string, class SDL_Texture*> textures;

    // 게임이 계속 실행돼야 하는지를 판단
    bool isRunning = false;
    uint32_t ticksCount = 0;

    std::list<std::shared_ptr<class Actor>> actors;
    std::list<std::shared_ptr<class Actor>> pendingActors;
    bool isUpdatingActors = false;
    std::vector<std::weak_ptr<class DrawComponent>> drawables;
};

class p1pingpong : public Game
{
  public:
    p1pingpong() noexcept = default;

  private:
    void load(const std::weak_ptr<Game> self) noexcept override;
};

class spaceShip : public Game
{
  public:
    spaceShip() noexcept = default;

  private:
    void load(const std::weak_ptr<Game> self) noexcept override;
};