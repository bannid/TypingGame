/* date = May 24th 2023 9:35 am */

#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#define DEBUG_LOG(Msg)
#define DEBUG_LOG_TIMED_BLOCK(ExecutionTime, Name) \
printf("%s took: %f ms\n", Name, ExecutionTime)
#define TIMED_BLOCK(Name) timed_block TimedBlock##__COUNTER__(Name)

struct timed_block
{
    const char *Name;
    float StartTime;
    timed_block(const char *Name)
    {
        this->Name = Name;
        this->StartTime = glfwGetTime();
    }
    ~timed_block()
    {
        float TimeElapsed = glfwGetTime() - this->StartTime;
        DEBUG_LOG_TIMED_BLOCK(TimeElapsed * 1000.0f, this->Name);
    }
};

#endif //DEBUG_H
