#include <time.h>
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    char* rand_in_sec = "rand_nums.txt";
    char* rand_in_even_sec = "even_rand_nums.txt";

    FILE *rand_file;
    FILE *even_rand_file;

    int nums_count = 0;

    srand(time(NULL));

    long int start_period = time(NULL);
    while (1) 
    {
        long int cur_period = time(NULL);
        if (cur_period - start_period >= 1) 
        {
            rand_file = fopen(rand_in_sec, "a");
            start_period = cur_period;
            int rand_num = rand();
            char buf[255];
            sprintf(buf, "%d\n", rand_num);
            fprintf(rand_file, buf);
            fclose(rand_file);

            if (!(cur_period % 2))
            {
                // Каждое одиннадцатое значение будет затирать все предыдущие
                if (nums_count != 10)
                {
                    even_rand_file = fopen(rand_in_even_sec, "a");
                }
                else 
                {
                    even_rand_file = fopen(rand_in_even_sec, "w");
                    nums_count = 0;
                }
                fprintf(even_rand_file, buf);
                fclose(even_rand_file);
                nums_count++;
            }
        }
    }
    return 0;
}