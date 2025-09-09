#ifndef _HEAD_FPSLIMIT_HPP
#define _HEAD_FPSLIMIT_HPP
#include <chrono>
#include <thread>
namespace fps_func {
class FPS_Limiter {
private:
    const std::chrono::duration< double,std::ratio<1,1> > targetFrameTime;
    std::chrono::high_resolution_clock::time_point start,end;
    long long targetDelayMS;
public:
    FPS_Limiter(const double targetFPS):
        targetFrameTime(1.0/targetFPS),
        start(std::chrono::high_resolution_clock::now()),
        targetDelayMS(std::chrono::duration_cast<std::chrono::milliseconds>(targetFrameTime).count())
    {}
    void limit(){
        end=std::chrono::high_resolution_clock::now();
        std::chrono::duration< double,std::ratio<1,1> > eplased(end-start);
        if(eplased<targetFrameTime) std::this_thread::sleep_for(targetFrameTime-eplased);
        start=std::chrono::high_resolution_clock::now();
        return;
    }
};
}
#endif