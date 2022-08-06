#include "config.h"
#include <TTGO.h>

#include "bluebox_app.h"
#include "bluebox_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"

uint32_t bluebox_app_main_tile_num;
icon_t *bluebox_app = NULL;

LV_IMG_DECLARE(bluebox_app_64px);
LV_IMG_DECLARE(info_1_16px);

static void enter_bluebox_app_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for bluebox app
 */
void bluebox_app_setup( void ) {
    bluebox_app_main_tile_num = mainbar_add_app_tile( 1, 1, "bluebox" );
    bluebox_app = app_register( "blue box", &bluebox_app_64px, enter_bluebox_app_event_cb );
    bluebox_app_main_setup( bluebox_app_main_tile_num );
}

/*
 *
 */
uint32_t bluebox_app_get_app_main_tile_num( void ) {
    return( bluebox_app_main_tile_num );
}

/*
 *
 */
static void enter_bluebox_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        app_hide_indicator( bluebox_app );
                                        mainbar_jump_to_tilenumber( bluebox_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

