#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sensors/sensors.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <x86_adapt.h>

int main(int argc, char **argv)
{
    char *label;

    pid_t pid = fork();
    if(pid == 0)
    {
        raise(SIGSTOP);
        execvp(argv[1], &argv[1]);
        return 0;
    }
    x86_adapt_init();

     int fds[32];
     for(int cpu = 0; cpu < 32; cpu++)
     {
         fds[cpu] = x86_adapt_get_device_ro(X86_ADAPT_CPU, cpu);
     }
        const sensors_feature *feature;
        sensors_init(NULL);
        const sensors_chip_name *chip;
        int chip_nr = 0;
        int breaking = 0;
        while((chip = sensors_get_detected_chips(NULL, &chip_nr)))
        {
            int i = 0;
            while((feature = sensors_get_features(chip, &i)))
            {
                label = sensors_get_label(chip, feature);

                if(!strcmp("SVI2_Core", label))
                {
                    breaking = 1;
                    break;
                }
            }
            if(breaking)
                break;
        }
        const sensors_subfeature *sf;
        int j = 0;
        sf = sensors_get_all_subfeatures(chip, feature, &j);

        struct timespec tp;
        tp.tv_sec = 0;
        tp.tv_nsec = 5e8;
        kill(pid, SIGCONT);
        double val;
        double count = 1;
        double avg;

	 uint64_t results[32];
        uint64_t cpu_avg[32];
     for (int cpu  = 0; cpu < 32; cpu++)
     {
         x86_adapt_get_setting(fds[cpu], 14, &(cpu_avg[cpu]));
     }
        sensors_get_value(chip, sf->number, &avg);
        do
        {
            count++;
            for (int cpu  = 0; cpu < 32; cpu++)
            {
                x86_adapt_get_setting(fds[cpu], 14, &(results[cpu]));
                cpu_avg[cpu] = cpu_avg[cpu] * ((count-1)/count) + results[cpu]/count;
            }
            sensors_get_value(chip, sf->number, &val);
            nanosleep(&tp, NULL);
            avg = avg * ((count-1)/count) + val/count;
        }while(!waitpid(pid, NULL, WNOHANG));
        printf("Whole: %f\n", avg);
        
        for(int cpu = 0; cpu < 32; cpu++)
        {
            printf("PerCpuVolt: %lu\n", cpu_avg[cpu]);
        }
        return 0;
}
