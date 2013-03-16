#include "paintatron.h"

paintatron::paintatron(int population)
{
    int i;

    sensors = 5, actuators = 6;
    this->population = population;
    run_steps = 1;
    rows = 6;
    columns = 16;
    connections_per_gene = 3;
    modules = 0;
    chromosomes=1;
    min_value = -100;
    max_value = 100;
    elitism = 1.0f / population;
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
    preview_img_width = 128;
    preview_img_height = 128;
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
    int x,y,n=0,itt,c, source_x, source_y, source_index,R,G,B;
    float offset_x, offset_y;
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
                /* source image to use */
                source_index =
                            (int)(fmod(fabs(gprcm_get_actuator(f, 3,
                                                                pop->rows,
                                                                pop->columns,
                                                                pop->sensors)),1.0f)*no_of_sources);

                if ((source_images[source_index].width() > 0) &&
                    (source_images[source_index].height() > 0)) {

                    offset_x =
                        fmod(fabs(gprcm_get_actuator(f, 4,
                                                     pop->rows,
                                                     pop->columns,
                                                     pop->sensors)),2.0f)-1.0f;
                    offset_y =
                        fmod(fabs(gprcm_get_actuator(f, 5,
                                                     pop->rows,
                                                     pop->columns,
                                                     pop->sensors)),2.0f)-1.0f;

                    source_x = abs(x*(int)(source_images[source_index].width()*offset_x)/img_width) % source_images[source_index].width();
                    source_y = abs(y*(int)(source_images[source_index].height()*offset_y)/img_height) % source_images[source_index].height();

                    /* get the colour at this location */
                    QColor col = QColor::fromRgb (source_images[source_index].pixel(source_x,source_y));
                    col.getRgb(&R,&G,&B);

                    /* set sensors to the colour values */
                    gprcm_set_sensor(f, 2, R/255.0f);
                    gprcm_set_sensor(f, 3, G/255.0f);
                    gprcm_set_sensor(f, 4, B/255.0f);
                }
            }


            gprcm_set_sensor(f, 0,
                             (x*2/(float)img_width)-0.5f);

            gprcm_set_sensor(f, 1,
                             (y*2/(float)img_height)-0.5f);

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
            (&sys.island[0])->fitness[i] =
                rand_num(&random_seed)%10000/100000.0f;
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
