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

#include "paintatron.h"

paintatron::paintatron(int population)
{
    int i;

    sensors = 14, actuators = 13;
    this->population = population;
    run_steps = 1;
    rows = 4;
    columns = 10;
    connections_per_gene = 6;
    modules = 0;
    chromosomes=1;
    min_value = -1000;
    max_value = 1000;
    elitism = 4.0f / population;
    mutation_prob = 0.4f;
    use_crossover = 1;
    dropout_rate = 0.1f;
    random_seed = (unsigned int)time(NULL);
    integers_only = 0;
    sensor_names = (char**)malloc(sensors*sizeof(char*));
    for (i = 0; i < sensors; i++) {
        sensor_names[i] = (char*)malloc(32*sizeof(char));
    }
    actuator_names = (char**)malloc(actuators*sizeof(char*));
    for (i = 0; i < actuators; i++) {
        actuator_names[i] = (char*)malloc(32*sizeof(char));
    }

    // create an image used for previews
    preview_img_width = 64;
    preview_img_height = 64;
    preview_img = (unsigned char*)malloc(preview_img_width*preview_img_height*3*sizeof(unsigned char));

    no_of_instructions = gprcm_default_instruction_set((int*)instruction_set);

    /* create a population */
    gprcm_init_system(&sys, 1,
                      population,
                      rows, columns,
                      sensors, actuators,
                      connections_per_gene,
                      modules,
                      chromosomes,
                      min_value, max_value,
                      integers_only, &random_seed,
                      instruction_set, no_of_instructions);
}

paintatron::~paintatron()
{
    int i;

    for (i = 0; i < sensors; i++) {
        free(sensor_names[i]);
    }
    for (i = 0; i < actuators; i++) {
        free(actuator_names[i]);
    }
    free(sensor_names);
    free(actuator_names);
    free(preview_img);
}

/* an individual produces an artwork */
void paintatron::produce_art(int index,
                             unsigned char * img,
                             int img_width, int img_height,
                             QImage * source_images,
                             int no_of_sources,
                             char * filename)
{
    int x,y,x2,y2,n=0,itt,c, source_x, source_y, source_index,R,G,B,R2,G2,B2,actuator_offset,sensor_offset;
    float offset_x, offset_y, angle, radius;
    int cx = img_width/2;
    int cy = img_height/2;
    gprcm_population * pop = &sys.island[0];
    gprcm_function * f = &pop->individual[index];

    gprcm_clear_state(f, pop->rows,
                      pop->columns,
                      pop->sensors,
                      pop->actuators);

    /* for every image pixel */
    for (y = 0; y < img_height; y++) {
        for (x = 0; x < img_width; x++, n+=3) {
            if (no_of_sources > 0) {
                for (itt = 0; itt < 2; itt++) {
                    actuator_offset = itt*5;
                    sensor_offset = itt*5;
                    /* source image to use */
                    source_index =
                            (int)(fmod(fabs(gprcm_get_actuator(f, 3+actuator_offset,
                                                               pop->rows,
                                                               pop->columns,
                                                               pop->sensors)),1.0f)*no_of_sources);

                    if ((source_images[source_index].width() > 0) &&
                            (source_images[source_index].height() > 0)) {

                        angle =
                                fmod(fabs(gprcm_get_actuator(f, 4+actuator_offset,
                                                             pop->rows,
                                                             pop->columns,
                                                             pop->sensors)),1.0f)*2*3.1415927f;

                        radius =
                                fmod(fabs(gprcm_get_actuator(f, 5+actuator_offset,
                                                             pop->rows,
                                                             pop->columns,
                                                             pop->sensors)),1.0f)*img_width;

                        if (itt==0) {
                            x2 = x + (int)(radius*sin(angle));
                            y2 = y + (int)(radius*cos(angle));
                        }
                        else {
                            x2 = (img_width/2) + (int)(radius*sin(angle));
                            y2 = (img_height/2) + (int)(radius*cos(angle));
                        }

                        offset_x =
                                (fmod(fabs(gprcm_get_actuator(f, 6+actuator_offset,
                                                              pop->rows,
                                                              pop->columns,
                                                              pop->sensors)),1.0f)-0.5f)*4;
                        offset_y =
                                (fmod(fabs(gprcm_get_actuator(f, 7+actuator_offset,
                                                              pop->rows,
                                                              pop->columns,
                                                              pop->sensors)),1.0f)-0.5f)*4;

                        source_x = abs(x2*(int)(source_images[source_index].width()*offset_x)/img_width) % source_images[source_index].width();
                        source_y = abs(y2*(int)(source_images[source_index].height()*offset_y)/img_height) % source_images[source_index].height();

                        /* get the colour at this location */
                        QColor col = QColor::fromRgb (source_images[source_index].pixel(source_x,source_y));
                        col.getRgb(&R,&G,&B);

                        /* set sensors to the colour values */
                        gprcm_set_sensor(f, 4+sensor_offset, R/255.0f);
                        gprcm_set_sensor(f, 5+sensor_offset, G/255.0f);
                        gprcm_set_sensor(f, 6+sensor_offset, B/255.0f);

                        col = QColor::fromRgb (source_images[source_index].pixel((source_x+5) % source_images[source_index].width(),source_y));
                        col.getRgb(&R2,&G2,&B2);
                        gprcm_set_sensor(f, 7+sensor_offset, (abs(R-R2) + abs(G-G2) + abs(B-B2))/255.0f);
                        col = QColor::fromRgb (source_images[source_index].pixel(source_x,(source_y+5) % source_images[source_index].height()));
                        col.getRgb(&R2,&G2,&B2);
                        gprcm_set_sensor(f, 8+sensor_offset, (abs(R-R2) + abs(G-G2) + abs(B-B2))/255.0f);

                    }
                }
            }


            gprcm_set_sensor(f, 0,
                             (x*2/(float)img_width)-1.0f);

            gprcm_set_sensor(f, 1,
                             (y*2/(float)img_height)-1.0f);

            gprcm_set_sensor(f, 2,
                             n/(float)(img_width*img_height*3));

            gprcm_set_sensor(f, 3,
                             (float)sqrt(((x-cx)*(x-cx)) + ((y-cy)*(y-cy)))/img_width);

            for (itt = 0; itt < run_steps; itt++) {
                /* run the program */
                gprcm_run(f, pop, dropout_rate, 0, 0);
            }

            for (c = 0; c < 3; c++) {
                img[n + c] =
                    (unsigned char)(fmod(fabs(gprcm_get_actuator(f, c,
                                                                 pop->rows,
                                                                 pop->columns,
                                                                 pop->sensors)),1.0f)*255);
            }
        }
    }
    write_png_file(filename, img_width, img_height, img);
}

/* produce an artwork for each individual in the population */
void paintatron::produce_population_art(char * data_directory,
                                        QImage * source_images,
                                        int no_of_sources)
{
    int index;
    char filename[256];
    gprcm_population * pop = &sys.island[0];

    for (index = 0; index < pop->size; index++) {
        sprintf(filename, "%spic_%d.png", data_directory,index);
        produce_art(index, preview_img,
                    preview_img_width, preview_img_height,
                    source_images, no_of_sources,
                    filename);
        fflush(stdout);
    }
}

/* select the best individual to breed from */
void paintatron::select_best(int index)
{
    int i;

    for (i = 0; i < population; i++) {
        if (i != index) {
            if ((&sys.island[0])->fitness[i] > 20) {
                (&sys.island[0])->fitness[i] /= 2;
            }
            else {
                (&sys.island[0])->fitness[i] =
                    rand_num(&random_seed)%10000/100000.0f;
            }
        }
        else {
            (&sys.island[0])->fitness[i] = 99;
        }
    }
}

/* produce the next generation */
void paintatron::next_generation()
{
    gprcm_generation_system(&sys, 200,
                            elitism,
                            mutation_prob,
                            use_crossover, &random_seed,
                            instruction_set, no_of_instructions);
}
