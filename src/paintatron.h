/*
  Paintatron
  Copyright (C) 2013 Bob Mottram
  bob@sluggish.dyndns.org

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef paintatron_H
#define paintatron_H

#include <stdio.h>
#include <time.h>
#include <QImage>
#include <QColor>
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

    void produce_population_art(char * data_directory,
                                QImage * source_images, int no_of_source_images,
                                QString * source_texts, int no_of_source_texts);
    void produce_art(int index,
                     unsigned char * img,
                     int img_width, int img_height,
                     QImage * source_images,
                     int no_of_source_images,
                     QString * source_texts,
                     int no_of_source_texts,
                     char * filename);
    void next_generation();
    void select_best(int index);

    paintatron(int population);
    ~paintatron();
};



#endif // paintatron_H
