/****************************************************************************
 *   TTGO Watch 2020 Bluebox 
 *                 linuxthor   
 *      RIP Onkel Dittemeyer
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include <TTGO.h>

#include "bluebox_app.h"
#include "bluebox_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/sound.h"


lv_obj_t *bluebox_app_main_tile = NULL;
lv_style_t bluebox_app_main_style;

lv_task_t * _bluebox_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(keypad_whistle_64px);
LV_IMG_DECLARE(keypad_0_64px);
LV_IMG_DECLARE(keypad_1_64px);
LV_IMG_DECLARE(keypad_2_64px);
LV_IMG_DECLARE(keypad_3_64px);
LV_IMG_DECLARE(keypad_4_64px);
LV_IMG_DECLARE(keypad_5_64px);
LV_IMG_DECLARE(keypad_6_64px);
LV_IMG_DECLARE(keypad_7_64px);
LV_IMG_DECLARE(keypad_8_64px);
LV_IMG_DECLARE(keypad_9_64px);
LV_IMG_DECLARE(keypad_star_64px);
LV_IMG_DECLARE(keypad_hash_64px);
LV_FONT_DECLARE(Ubuntu_72px);

// buttons
static void enter_bluebox_app_whistle_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_zero_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_one_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_two_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_three_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_four_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_five_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_six_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_seven_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_eight_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_nine_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluebox_app_star_event_cb( lv_obj_t * obj, lv_event_t event );      // kp
static void enter_bluebox_app_hash_event_cb( lv_obj_t * obj, lv_event_t event );      // st

static void exit_bluebox_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

void bluebox_app_main_setup( uint32_t tile_num ) {

    bluebox_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &bluebox_app_main_style, APP_STYLE );

    // whistle
    lv_obj_t * whistle_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(whistle_btn, LV_BTN_STATE_RELEASED, &keypad_whistle_64px);
    lv_imgbtn_set_src(whistle_btn, LV_BTN_STATE_PRESSED, &keypad_whistle_64px);
    lv_imgbtn_set_src(whistle_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_whistle_64px);
    lv_imgbtn_set_src(whistle_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_whistle_64px);
    lv_obj_add_style(whistle_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(whistle_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 192, 0 );
    lv_obj_set_event_cb( whistle_btn, enter_bluebox_app_whistle_event_cb );
 
    // 0
    lv_obj_t * zero_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(zero_btn, LV_BTN_STATE_RELEASED, &keypad_0_64px);
    lv_imgbtn_set_src(zero_btn, LV_BTN_STATE_PRESSED, &keypad_0_64px);
    lv_imgbtn_set_src(zero_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_0_64px);
    lv_imgbtn_set_src(zero_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_0_64px);
    lv_obj_add_style(zero_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(zero_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 64, 192 );
    lv_obj_set_event_cb( zero_btn, enter_bluebox_app_zero_event_cb );
 
    // 1
    lv_obj_t * one_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(one_btn, LV_BTN_STATE_RELEASED, &keypad_1_64px);
    lv_imgbtn_set_src(one_btn, LV_BTN_STATE_PRESSED, &keypad_1_64px);
    lv_imgbtn_set_src(one_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_1_64px);
    lv_imgbtn_set_src(one_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_1_64px);
    lv_obj_add_style(one_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(one_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( one_btn, enter_bluebox_app_one_event_cb );

    // 2
    lv_obj_t * two_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(two_btn, LV_BTN_STATE_RELEASED, &keypad_2_64px);
    lv_imgbtn_set_src(two_btn, LV_BTN_STATE_PRESSED, &keypad_2_64px);
    lv_imgbtn_set_src(two_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_2_64px);
    lv_imgbtn_set_src(two_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_2_64px);
    lv_obj_add_style(two_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(two_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 64, 0 );
    lv_obj_set_event_cb( two_btn, enter_bluebox_app_two_event_cb );

    // 3 
    lv_obj_t * three_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(three_btn, LV_BTN_STATE_RELEASED, &keypad_3_64px);
    lv_imgbtn_set_src(three_btn, LV_BTN_STATE_PRESSED, &keypad_3_64px);
    lv_imgbtn_set_src(three_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_3_64px);
    lv_imgbtn_set_src(three_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_3_64px);
    lv_obj_add_style(three_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(three_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 128, 0 );
    lv_obj_set_event_cb( three_btn, enter_bluebox_app_three_event_cb );

    // 4 
    lv_obj_t * four_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(four_btn, LV_BTN_STATE_RELEASED, &keypad_4_64px);
    lv_imgbtn_set_src(four_btn, LV_BTN_STATE_PRESSED, &keypad_4_64px);
    lv_imgbtn_set_src(four_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_4_64px);
    lv_imgbtn_set_src(four_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_4_64px);
    lv_obj_add_style(four_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(four_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 64 );
    lv_obj_set_event_cb( four_btn, enter_bluebox_app_four_event_cb );

    // 5 
    lv_obj_t * five_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(five_btn, LV_BTN_STATE_RELEASED, &keypad_5_64px);
    lv_imgbtn_set_src(five_btn, LV_BTN_STATE_PRESSED, &keypad_5_64px);
    lv_imgbtn_set_src(five_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_5_64px);
    lv_imgbtn_set_src(five_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_5_64px);
    lv_obj_add_style(five_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(five_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 64, 64 );
    lv_obj_set_event_cb( five_btn, enter_bluebox_app_five_event_cb );
 
    // 6
    lv_obj_t * six_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(six_btn, LV_BTN_STATE_RELEASED, &keypad_6_64px);
    lv_imgbtn_set_src(six_btn, LV_BTN_STATE_PRESSED, &keypad_6_64px);
    lv_imgbtn_set_src(six_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_6_64px);
    lv_imgbtn_set_src(six_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_6_64px);
    lv_obj_add_style(six_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(six_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 128, 64 );
    lv_obj_set_event_cb( six_btn, enter_bluebox_app_six_event_cb );

    // 7 
    lv_obj_t * seven_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(seven_btn, LV_BTN_STATE_RELEASED, &keypad_7_64px);
    lv_imgbtn_set_src(seven_btn, LV_BTN_STATE_PRESSED, &keypad_7_64px);
    lv_imgbtn_set_src(seven_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_7_64px);
    lv_imgbtn_set_src(seven_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_7_64px);
    lv_obj_add_style(seven_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(seven_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 128 );
    lv_obj_set_event_cb( seven_btn, enter_bluebox_app_seven_event_cb );
 
    // 8
    lv_obj_t * eight_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(eight_btn, LV_BTN_STATE_RELEASED, &keypad_8_64px);
    lv_imgbtn_set_src(eight_btn, LV_BTN_STATE_PRESSED, &keypad_8_64px);
    lv_imgbtn_set_src(eight_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_8_64px);
    lv_imgbtn_set_src(eight_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_8_64px);
    lv_obj_add_style(eight_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(eight_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 64, 128 );
    lv_obj_set_event_cb( eight_btn, enter_bluebox_app_eight_event_cb );

    // 9 
    lv_obj_t * nine_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(nine_btn, LV_BTN_STATE_RELEASED, &keypad_9_64px);
    lv_imgbtn_set_src(nine_btn, LV_BTN_STATE_PRESSED, &keypad_9_64px);
    lv_imgbtn_set_src(nine_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_9_64px);
    lv_imgbtn_set_src(nine_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_9_64px);
    lv_obj_add_style(nine_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(nine_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 128, 128 );
    lv_obj_set_event_cb( nine_btn, enter_bluebox_app_nine_event_cb );

    // star 
    lv_obj_t * star_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(star_btn, LV_BTN_STATE_RELEASED, &keypad_star_64px);
    lv_imgbtn_set_src(star_btn, LV_BTN_STATE_PRESSED, &keypad_star_64px);
    lv_imgbtn_set_src(star_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_star_64px);
    lv_imgbtn_set_src(star_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_star_64px);
    lv_obj_add_style(star_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(star_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 192 );
    lv_obj_set_event_cb( star_btn, enter_bluebox_app_star_event_cb );
 
    // hash
    lv_obj_t * hash_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(hash_btn, LV_BTN_STATE_RELEASED, &keypad_hash_64px);
    lv_imgbtn_set_src(hash_btn, LV_BTN_STATE_PRESSED, &keypad_hash_64px);
    lv_imgbtn_set_src(hash_btn, LV_BTN_STATE_CHECKED_RELEASED, &keypad_hash_64px);
    lv_imgbtn_set_src(hash_btn, LV_BTN_STATE_CHECKED_PRESSED, &keypad_hash_64px);
    lv_obj_add_style(hash_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(hash_btn, bluebox_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 128, 192 );
    lv_obj_set_event_cb( hash_btn, enter_bluebox_app_hash_event_cb );
 
    lv_obj_t * exit_btn = lv_imgbtn_create( bluebox_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &bluebox_app_main_style );
    lv_obj_align(exit_btn, bluebox_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_bluebox_app_main_event_cb );
}

void play_out(const char *filename)
{
    sound_play_spiffs_mp3(filename);
}

static void enter_bluebox_app_whistle_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/2600.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_zero_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/0.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_one_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/1.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_two_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/2.mp3" ); 
                                       break;
    }
}

static void enter_bluebox_app_three_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/3.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_four_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/4.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_five_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):     play_out( "/5.mp3" );
                                      break;
    }
}

static void enter_bluebox_app_six_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):     play_out( "/6.mp3" );
                                      break;
    }
}

static void enter_bluebox_app_seven_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/7.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_eight_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/8.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_nine_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      play_out( "/9.mp3" );
                                       break;
    }
}

static void enter_bluebox_app_star_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       play_out( "/ST.mp3" );
                                        break;
    }
}

static void enter_bluebox_app_hash_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       play_out( "/KP.mp3" );
                                        break;
    }
}

static void exit_bluebox_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}


