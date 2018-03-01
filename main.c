#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_KEYBOARD_STATE ret_state;
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    float xres=640;
    float yres=480;
    display = al_create_display(xres, yres);
    if(!display) {
        fprintf(stderr, "failed to create display!\n");
        return -1;
    }
    event_queue = al_create_event_queue();
    if(!event_queue) {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        return -1;
    }
    if(!al_install_keyboard()) { // THIS IS NEEDED, IT'LL SEGFAULT WITHOUT THIS
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }
    if(!al_install_audio()) {
        fprintf(stderr, "failed to initialize the audio system!\n");
        return -1;
    }
    if(!al_reserve_samples(2)){
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }
    al_set_window_title(display, "redsPL's lame platformer");
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_clear_to_color(al_map_rgb(0,0,0));

    // INIT ADDONS
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_acodec_addon(); // loads audio codecs
    al_init_primitives_addon();


    // VARIABLES
    float a=10;
    float b=10;
    float c=0;
    int turn=2;
    float ammoX=a;
    float ammoY=b;
    float bullet=false;
    float hitX=0;
    float hitY=0;
    float ammoSpeed=1;
    int blockName=0;
    char *verify=(char*) malloc(sizeof(char)*64);
    int blockX=0;
    int blockY=0;
    int mapX=0;
    int mapY=0;
    int characterSpeed=3;
    int mapI=0;
    int **mapData;

    // LOAD_FILES (pls don't insert these into a while, it'll crash)
    //ALLEGRO_SAMPLE_ID id;
    //ALLEGRO_SAMPLE_ID id2;
    //ALLEGRO_SAMPLE *music = al_load_sample("music/1.ogg");
    //ALLEGRO_SAMPLE *music1 = al_load_sample("music/2.ogg");
    ALLEGRO_FONT *cavestory = al_load_ttf_font("fonts/Cave-Story.ttf", 56, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_BITMAP *char_front = al_load_bitmap("images/notmine_butused/char_front.png");
    ALLEGRO_BITMAP *char_back = al_load_bitmap("images/notmine_butused/char_back.png");
    ALLEGRO_BITMAP *char_left = al_load_bitmap("images/notmine_butused/char_left.png");
    ALLEGRO_BITMAP *char_right = al_load_bitmap("images/notmine_butused/char_right.png");
    ALLEGRO_BITMAP **textures;
    ALLEGRO_BITMAP *screen = al_create_bitmap(640,480);
    FILE *map;
    map = fopen("test.dat", "r");

    // uncomment below for music
    // ALLEGRO_SAMPLE_INSTANCE *SongInstance = al_create_sample_instance(music);
    // al_attach_sample_instance_to_mixer(SongInstance, al_get_default_mixer());
    ALLEGRO_EVENT ev;
    ALLEGRO_TIMEOUT timeout;
    fscanf(map, "%s", verify);
    if(strcmp(verify, "revy") == 0) {
                printf("revy file OK..\n");
                int bitmapsize=0;
                char *bitmappath=(char*) malloc(sizeof(char)*512);
                fscanf(map, "%d", &bitmapsize);
                printf("bitmapsize: %d\n", bitmapsize);
                textures=(ALLEGRO_BITMAP **) malloc(sizeof(ALLEGRO_BITMAP *) * bitmapsize);
                textures[0]=al_create_bitmap(1,1);
                for(int i=1; i<=bitmapsize; i++) {
                    fscanf(map, "%s\n", bitmappath);
                    printf("%s\n", bitmappath);
                    textures[i] = al_load_bitmap(bitmappath);
                }
                fscanf(map, "%d %d", &mapX, &mapY);
                printf("mapx: %d, mapy: %d\n", mapX, mapY);
                mapData = (int **) malloc(sizeof(int *) * mapX);
                for(int i = 0; i < mapX; i++){
                    mapData[i] = (int *) calloc(mapY, sizeof(int));

                    // Clearing map data
                    //for(int j = 0; j < mapY; j++){
                    //    mapData[i][j] = 0;
                    //}
                }

                while(1) {
                    blockX=0;
                    blockY=0;
                    blockName=0;
                    fscanf(map, "%d %d %d\n", &blockName, &blockX, &blockY);
                    //strncpy(&mapData[blockX][blockY], &blockName, 5);
                    mapData[blockX][blockY]=blockName;
                    //memcpy(blockName, mapData[blockX][blockY], sizeof(blockName));
                    mapI++;
                    if(blockName==0 && blockX == 0 && blockY == 0) {
                        break;
                    }
                }
            } else {
                printf("not a revy file, loading refused\n");
    }
    al_set_target_bitmap(screen);
    for(int i=0; i<mapY; i++) {
        for(int j=0; j<mapX; j++) {
            int coords=mapData[j][i];
            if(coords!=0) {
                printf("%i\n", coords);
            }
            al_draw_bitmap(textures[coords], j*8, i*8, 0);
        }
    }
    al_set_target_bitmap(al_get_backbuffer(display));
    while(1) {
        al_init_timeout(&timeout, 0.00); // sets the event timeout to 0.06s <-- no flicker at 0.01 and 0.00
        al_get_keyboard_state(&ret_state);
        bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);
        if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // Checks if you closed the window
            break;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_A)) {
            a=a-characterSpeed;
            turn=1;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_S)) {
            b=b+characterSpeed;
            turn=2;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_D)) {
            a=a+characterSpeed;
            turn=3;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_W)) {
            b=b-characterSpeed;
            turn=4;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_ESCAPE)) {
            break;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_X)) { // shoot primitive bullet.. doesn't work, fixme
            bullet=true;
            ammoX=a;
            ammoY=b;
        }
        if(al_key_down(&ret_state, ALLEGRO_KEY_H)) { // return character to screen
            a=100;
            b=100;
        }
        if(ammoX==xres) {
            ammoX=ammoX-ammoSpeed;
            hitX=1;
        } else {
            if(hitX==1) {
                ammoX=ammoX-ammoSpeed;
            } else {
                ammoX=ammoX+ammoSpeed;
            }
        }
        if(ammoY==yres) {
            ammoY=ammoY-ammoSpeed;
            hitY=1;
        } else {
            if(hitY==1) {
                ammoY=ammoY-ammoSpeed;
            } else {
                ammoY=ammoY+ammoSpeed;
            }
        }
        if(ammoY==0) {
            hitY=0;
        }
        if(ammoX==0) {
            hitX=0;
        }
        if(c!=40) {
            c=c+1;
        }
        if(bullet==true) {
            al_draw_line(ammoX, ammoY, ammoX+10, ammoY, al_map_rgb(255,0,0), 0); // Draw a line
        }
            al_draw_bitmap(screen, 1, 1, 0);
        if(turn==1) {
            al_draw_bitmap(char_left, a, b, 0);
        } else if(turn==2) {
            al_draw_bitmap(char_front, a, b, 0);
        } else if(turn==3) {
            al_draw_bitmap(char_right, a, b, 0);
        } else if(turn==4) {
            al_draw_bitmap(char_back, a, b, 0);
        }
        al_put_pixel(128, 128, al_map_rgb(255,0,0));
        al_draw_textf(cavestory, al_map_rgb(0,0,255), 10, 200, 0, "ReVY          %hhi", turn); // Display some text!
        al_flip_display(); // draws on the screen
        al_clear_to_color(al_map_rgb(20,150,20)); 
        //al_play_sample(music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, 0);
        }
    //al_destroy_sample_instance(SongInstance);
    //al_destroy_sample(music);
    //al_destroy_sample(music1);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    fclose(map);
    return 0;
}
