#ifndef paintatron_H
#define paintatron_H

#include <stdio.h>
#include <time.h>
extern "C" {
#include "libgpr/globals.h"
#include "libgpr/gprcm.h"
#include "libgpr/pnglite.h"
}

class paintatron
{
protected:
    gprcm_system sys;
    unsigned int random_seed;

public:
    int population;
    int rows, columns, connections_per_gene;
    int modules, chromosomes;
    float min_value, max_value;
    float elitism, mutation_prob;
    int use_crossover;
    int sensors, actuators, integers_only;
    int instruction_set[64], no_of_instructions;
    char ** sensor_names, ** actuator_names;
    unsigned char * preview_img;
    int preview_img_width, preview_img_height;
    float dropout_rate;
    int run_steps;

    void produce_population_art(char * data_directory);
    void produce_art(int index,
                     unsigned char * img,
                     int img_width, int img_height,
                     char * filename);
    void next_generation();
    void select_best(int index);

    paintatron(int population);
    ~paintatron();
};



#endif // paintatron_H
