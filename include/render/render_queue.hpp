#ifndef __INC_RENDER_QUEUE_HPP
#define __INC_RENDER_QUEUE_HPP

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include "mesh_system.hpp"
#include "render_command.hpp"

namespace ModernBoy
{
    template<typename Mesh>
    class RenderQueue{
    private:
        using Command = RenderCommand<Mesh>;
        using Commands = std::vector<Command>;

        Commands commands[2];
        uint32_t currentIndex = 0;

        const MeshSystem<Mesh>& meshSystem;

        std::thread genThread;
        bool running = true;
        std::mutex mutex;

    public:
        RenderQueue(const MeshSystem<Mesh>& ms)
        :meshSystem(ms), genThread(&queueLoop, this){}
        ~RenderQueue(){
            running = false;
            if(genThread.joinable()) genThread.join();
        }

        Commands& swapBuffer(){
            std::lock_guard<std::mutex> lock(mutex);
            auto idx = currentIndex;

            currentIndex = (currentIndex + 1) % 2;
            commands[currentIndex].clear();

            return commands[idx];
        }

    private:
        void push(const Command& cmd){
            std::lock_guard<std::mutex> lock(mutex);
            commands[currentIndex].push_back(cmd);
        }

        void queueLoop(void){
            while(running){
                auto components = meshSystem.getAllVisible();

                push(StartCommand{});

                for(const auto& comp: components){
                    push(DrawCommand<Mesh>{
                        .handle = comp.get().meshHandle
                    });
                }

                push(ClearCommand{});
            }
        }
    };
} // namespace ModernBoy

#endif // __INC_RENDER_QUEUE_HPP