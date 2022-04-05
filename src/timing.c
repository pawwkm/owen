#include "compiler.h"

#ifdef _WIN64
// TODO: Check if I can get away with using clock_t instead.
// It would be really nice to not have any Windows ties.
// I would be satisfied by only using milliseconds if 
// that means that I can use clock_t. clock_t is just a number
// os some size. Use CLOCKS_PER_SEC and then I should be able to 
// have the time units I have at the moment while being 
// portable.
// I would argue that any real workload takes more time than 
// were sub-millisecond resolution matters. It is whole passes,
// so far, that is being measured and not a single function.
#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)
#endif

static uint64_t timers[Timer_file_writing + 1];
static size_t current_timer;
static uint64_t last_time_stamp;
static uint64_t ticks_per_second;

static uint64_t read_current_time(void)
{
#ifdef _WIN64
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    
    return (uint64_t)current_time.QuadPart;
#else
#error This target platform is not supported.
#endif
}

void start_timer(Timer timer)
{
    uint64_t current_time = read_current_time();
    if (last_time_stamp > 0)
        timers[current_timer] += current_time - last_time_stamp;
    
    last_time_stamp = current_time;
    current_timer = timer;
}

static void print_timing(char* name, uint64_t ticks)
{
    printf("%s", name);
    
    double seconds = (double)ticks / ticks_per_second;
    if (seconds >= 1)
    {
        printf("%.6gs\n", seconds);
        return;
    }
    
    double milliseconds = (double)ticks / (ticks_per_second / 1000);
    if (milliseconds >= 1)
    {
        printf("%.6gms\n", milliseconds);
        return;
    }

    double microseconds = (double)ticks / (ticks_per_second / 1000 / 1000);
    if (microseconds >= 1)
    {
        printf("%.6gμs\n", microseconds);
        return;
    }
    
    double nanoseconds = ticks ? (double)ticks / (ticks_per_second / 1000 / 1000 / 1000) : 0;
    printf("%.6gns\n", nanoseconds);
}

void stop_current_timer(void)
{
    if (last_time_stamp)
        timers[current_timer] += read_current_time() - last_time_stamp;
}

void print_timings(void)
{
#ifdef _WIN64
    LARGE_INTEGER capture_frequency;
    QueryPerformanceFrequency(&capture_frequency);
    
    ticks_per_second = (uint64_t)capture_frequency.QuadPart;
#else
#error This target platform is not supported.
#endif
    
    printf("Timer\n");
    print_timing("File reading    ", timers[Timer_file_reading]);
    print_timing("Lexing          ", timers[Timer_lexing]);
    print_timing("Parsing         ", timers[Timer_parsing]);
    print_timing("Analysis        ", timers[Timer_analysis]);
    print_timing("Optimization    ", timers[Timer_optimization]);
    print_timing("Code generation ", timers[Timer_code_generation]);
    print_timing("File writing    ", timers[Timer_file_writing]);
    
    uint64_t total = 0;
    for (size_t i = 0; i < Timer_code_generation; i++)
        total += timers[i];
    
    print_timing("Total           ", total);
}

