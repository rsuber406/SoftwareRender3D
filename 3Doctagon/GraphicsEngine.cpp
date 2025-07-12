// 3Doctagon.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Window.h"
#include <thread>
#include <condition_variable>
#include <mutex>

struct MainThreadData {
    std::mutex* mainMutex = nullptr;
    std::condition_variable* mainCond = nullptr;
    bool* keepRunning = nullptr;
    Window* window = nullptr;

};

void ThreadEntryPoint(MainThreadData* threadData) 
{
    threadData->window->UpdateLoop();
    *threadData->keepRunning = false;
    threadData->mainCond->notify_all();
}


int main()
{
    bool keepRunning = true;
    std::condition_variable mainCond;
    std::mutex mainMutex;

    MainThreadData threadInfo;
    threadInfo.keepRunning = &keepRunning;
    threadInfo.mainCond = &mainCond;
    threadInfo.mainMutex = &mainMutex;
    Window window;
    threadInfo.window = &window;
    std::thread(ThreadEntryPoint, &threadInfo).detach();

    std::cout << "Press enter to kill app\n";
    std::cin.get();
    window.KillWindow();
    {
        std::unique_lock<std::mutex> lock(mainMutex);
        mainCond.wait(lock, [&] {return !keepRunning; });
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
