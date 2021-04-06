// usage: ./compute [cpuid] [cpuid] ...
// for example ./compute 0 1 3  <- 3 threads on pinned on cores 0,1,3

#include <vector>
#include <thread>
#include <chrono>

#include <pthread.h>
#include <sched.h>

#define VEC_SIZE 1024
#define RUNTIME 10

bool start = false;
std::chrono::high_resolution_clock::time_point until;
void compute_kernel(std::vector<double>& A, std::vector<double>&B, std::vector<double>& C, std::size_t repeat)
{
    double m = C[0];

    for(std::size_t i = 0; i < repeat; i++)
    {
        for(uint64_t i = 0; i < VEC_SIZE; i++)
        {
            m += B[i] * A[i];
        }
        C[0] = m;
    }
}

void per_thread()
{
    std::vector<double> vec_A(1024);
    std::vector<double> vec_B(1024);
    std::vector<double> vec_C(1024);

    for (std::size_t i; i < vec_A.size(); ++i)
    {
        vec_A[i] = static_cast<double>(i) * 0.3;
        vec_B[i] = static_cast<double>(i) * 0.2;
        vec_C[i] = static_cast<double>(i) * 0.7;
    }

    while(!start);

    std::size_t loops;

    while(std::chrono::high_resolution_clock::now() <= until)
    {
        if (vec_C[0] == 123.12345)
            vec_A[0] += 1.0;
        compute_kernel(vec_A, vec_B, vec_C, 32);
        loops++;
    }

    volatile int dd = 0;
    if (vec_C[0] == 42.0)
        dd++;
}
int main (int argc, char **argv)
{
    std::vector<std::thread> t_set;

    cpu_set_t set;
    for(int c = 1; c < argc; c++)
    {
        CPU_ZERO(&set);
        CPU_SET(atoi(argv[c]), &set);

        t_set.emplace_back(per_thread);
        pthread_setaffinity_np(t_set.back().native_handle(), sizeof(cpu_set_t), &set);
    }
    until = std::chrono::high_resolution_clock::now() + std::chrono::seconds(RUNTIME);
    start = true;
    for(auto& t :t_set)
    {
        t.join();
    }
    return 0;
}
