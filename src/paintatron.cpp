/*
  Paintatron
  Copyright (C) 2013 Bob Mottram
  bob@robotics.uk.to

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

    sensors = 21, actuators = 25;
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
    data_size=5;
    data_fields=1;
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
                      integers_only,
                      data_size, data_fields,
                      &random_seed,
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
                             int no_of_source_images,
                             QString * source_texts,
                             int no_of_source_texts,
                             char * filename)
{
    int x,y,x2,y2,n=0,itt,c, source_x, source_y, source_index,R,G,B,R2,G2,B2,actuator_offset=0,sensor_offset;
    int source_char_index, max_itt;
    float scale_x, scale_y, angle, radius, radius_scale=img_width;
    int centre_x=img_width/2, cx = img_width/2;
    int centre_y=img_height/2, cy = img_height/2;
    const int image_itterations = 3;
    const int initial_actuator = 6;
    const int actuator_image_inputs = 5;
    //const int actuator_inputs = actuator_image_inputs;
    const int sensor_image_inputs = 5;
    const char * texts[128];
    //const int sensor_inputs = sensor_image_inputs;
    gprcm_population * pop = &sys.island[0];
    gprcm_function * f = &pop->individual[index];

    gprcm_clear_state(f, pop->rows,
                      pop->columns,
                      pop->sensors,
                      pop->actuators);

    for (itt = 0; itt < no_of_source_texts; itt++) {
        texts[itt] = source_texts[itt].toStdString().c_str();
    }

    /* for every image pixel */
    for (y = 0; y < img_height; y++) {
        for (x = 0; x < img_width; x++, n+=3) {
            if (no_of_source_images > 0) {

                /* number of itterations at this pixel */
                max_itt = 1 +
                    (int)(fmod(fabs(gprcm_get_actuator(f, initial_actuator+1+actuator_offset,
                                                       pop->rows,
                                                       pop->columns,
                                                       pop->sensors)),1.0f)*image_itterations);

                for (itt = 0; itt < max_itt; itt++) {
                    actuator_offset = itt*actuator_image_inputs;
                    sensor_offset = itt*sensor_image_inputs;
                    /* source image to use */
                    source_index = itt%no_of_source_images;
                    /*
                      source_index =
                      (int)(fmod(fabs(gprcm_get_actuator(f, initial_actuator+actuator_offset,
                      pop->rows,
                      pop->columns,
                      pop->sensors)),1.0f)*no_of_source_images);
                    */

                    if ((source_images[source_index].width() > 0) &&
                        (source_images[source_index].height() > 0)) {

                        angle =
                            fmod(fabs(gprcm_get_actuator(f, initial_actuator+1+actuator_offset,
                                                         pop->rows,
                                                         pop->columns,
                                                         pop->sensors)),1.0f)*2*3.1415927f;

                        radius =
                            fmod(fabs(gprcm_get_actuator(f, initial_actuator+2+actuator_offset,
                                                         pop->rows,
                                                         pop->columns,
                                                         pop->sensors)),1.0f)*radius_scale;

                        scale_x =
                            ((fmod(fabs(gprcm_get_actuator(f, initial_actuator+3+actuator_offset,
                                                           pop->rows,
                                                           pop->columns,
                                                           pop->sensors)),1.0f)-0.5f)*4);
                        scale_y =
                            ((fmod(fabs(gprcm_get_actuator(f, initial_actuator+4+actuator_offset,
                                                           pop->rows,
                                                           pop->columns,
                                                           pop->sensors)),1.0f)-0.5f)*4);

                        if (itt==0) {
                            x2 = x + (int)(radius*sin(angle));
                            y2 = y + (int)(radius*cos(angle));
                        }
                        else {
                            x2 = centre_x + (int)(radius*sin(angle));
                            y2 = centre_y + (int)(radius*cos(angle));
                        }

                        source_x = abs(x2*(int)(source_images[source_index].width())*scale_x/img_width) % source_images[source_index].width();
                        source_y = abs(y2*(int)(source_images[source_index].height())*scale_y/img_height) % source_images[source_index].height();

                        /* get the colour at this location */
                        QColor col = QColor::fromRgb (source_images[source_index].pixel(source_x,source_y));
                        col.getRgb(&R,&G,&B);

                        /* set sensors to the colour values */
                        gprcm_set_sensor_complex(f, 5+sensor_offset,
                                                 R/255.0f, G/255.0f,
                                                 pop->sensors, pop->actuators,
                                                 pop->rows, pop->columns);
                        gprcm_set_sensor_complex(f, 6+sensor_offset,
                                                 G/255.0f, B/255.0f,
                                                 pop->sensors, pop->actuators,
                                                 pop->rows, pop->columns);
                        gprcm_set_sensor_complex(f, 7+sensor_offset,
                                                 B/255.0f, R/255.0f,
                                                 pop->sensors, pop->actuators,
                                                 pop->rows, pop->columns);

                        col = QColor::fromRgb (source_images[source_index].pixel((source_x+5) % source_images[source_index].width(),source_y));
                        col.getRgb(&R2,&G2,&B2);
                        gprcm_set_sensor(f, 8+sensor_offset, (abs(R-R2) + abs(G-G2) + abs(B-B2))/255.0f);
                        col = QColor::fromRgb (source_images[source_index].pixel(source_x,(source_y+5) % source_images[source_index].height()));
                        col.getRgb(&R2,&G2,&B2);
                        gprcm_set_sensor(f, 9+sensor_offset, (abs(R-R2) + abs(G-G2) + abs(B-B2))/255.0f);
                    }
                }
            }

            if (no_of_source_texts > 0) {
                sensor_offset = image_itterations*sensor_image_inputs;
                actuator_offset = image_itterations*actuator_image_inputs;

                source_index =
                    (int)(fmod(fabs(gprcm_get_actuator(f, initial_actuator+actuator_offset,
                                                       pop->rows,
                                                       pop->columns,
                                                       pop->sensors)),1.0f)*no_of_source_texts);
                source_char_index =
                    (int)(fmod(fabs(gprcm_get_actuator(f, initial_actuator+1+actuator_offset,
                                                       pop->rows,
                                                       pop->columns,
                                                       pop->sensors)),1.0f)*(source_texts[source_index].length()-1));
                gprcm_set_sensor(f, 10+sensor_offset, texts[source_index][source_char_index]/128.0f);
            }

            centre_x =
                cx + (int)((fmod(fabs(gprcm_get_actuator(f, 4,
                                                         pop->rows,
                                                         pop->columns,
                                                         pop->sensors)),1.0f)-0.5f)*img_width);
            centre_y =
                cy + (int)((fmod(fabs(gprcm_get_actuator(f, 5,
                                                         pop->rows,
                                                         pop->columns,
                                                         pop->sensors)),1.0f)-0.5f)*img_height);

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

            radius_scale =
                img_width - (fmod(fabs(gprcm_get_actuator(f, 3,
                                                          pop->rows,
                                                          pop->columns,
                                                          pop->sensors)),1.0f)*img_width);

            gprcm_set_sensor(f, 4,radius_scale/img_width);

        }
    }
    write_png_file(filename, img_width, img_height, img);
}

/* produce an artwork for each individual in the population */
void paintatron::produce_population_art(char * data_directory,
                                        QImage * source_images,
                                        int no_of_source_images,
                                        QString * source_texts,
                                        int no_of_source_texts)
{
    int index;
    char filename[256];
    gprcm_population * pop = &sys.island[0];

    for (index = 0; index < pop->size; index++) {
        sprintf(filename, "%spic_%d.png", data_directory,index);
        produce_art(index, preview_img,
                    preview_img_width, preview_img_height,
                    source_images, no_of_source_images,
                    source_texts, no_of_source_texts,
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
