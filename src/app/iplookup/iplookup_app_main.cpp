/****************************************************************************
 *   linuxthor 2020 ip-api lookup tool
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the musty tentacles of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include "HTTPClient.h"

#include <TTGO.h>
#include <lwip/sockets.h>

#include "iplookup_app.h"
#include "iplookup_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/json_psram_allocator.h"

lv_obj_t * iplookup_result_cont = NULL;
lv_obj_t *iplookup_app_main_tile = NULL;
lv_style_t iplookup_app_main_style;
lv_obj_t *iplookup_ip_textfield = NULL;

lv_task_t * _iplookup_app_task;

LV_IMG_DECLARE(next_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(iplookup_app_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_iplookup_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void iplookup_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_iplookup_app_next_event_cb( lv_obj_t * obj, lv_event_t event );
void iplookup_app_task( lv_task_t * task );

void fetch_ip_info( void ) {
    char url[128]="";
    char status[16];
    char cnt[64];
    char reg[72];
    char cit[72];
    char asn[128];
    int httpcode = -1;
    lv_obj_t * label;
    
    snprintf( url, sizeof( url ), "http://ip-api.com/json/%s", lv_textarea_get_text(iplookup_ip_textfield));
    log_i("request for %s", url);

    HTTPClient today_client;

    today_client.begin( url );
    httpcode = today_client.GET();

    if ( httpcode != 200 ) {
        log_e("HTTPClient error %d", httpcode, url );
        today_client.end();
        label = lv_label_create(iplookup_result_cont, NULL);
        lv_label_set_text_fmt(label, "HTTP Err %d", httpcode);
        return;
    }

    SpiRamJsonDocument doc( 1024 );

    DeserializationError error = deserializeJson( doc, today_client.getStream() );
    if (error) {
        log_e("iplookup deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        today_client.end();
        label = lv_label_create(iplookup_result_cont, NULL);
        lv_label_set_text(label, "JSON error");
        return;  
    }

    today_client.end();

    label = lv_label_create(iplookup_result_cont, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, lv_disp_get_hor_res(NULL)); 
    strncpy(status, doc["status"], 15);
    if(strcmp(status, "fail") == 0)
    {
        // may be rfc1918 or localhost or something
        lv_label_set_text(label, "No data for query");
        return; 
    }
    strncpy(cnt, doc["country"], 63);
    strncpy(reg, doc["regionName"], 71);
    strncpy(cit, doc["city"],71); 
    strncpy(asn, doc["as"], 125); 
    lv_label_set_text_fmt(label, "%s\n"
                                 "%s\n"
                                 "%s\n"
                                 "%s\n",
                                 cnt,reg,cit,asn);
    log_i("country: %s", cnt);
    log_i("region: %s", reg);
    log_i("city: %s", cit);
    log_i("asn: %s", asn);
    doc.clear();
    return;
}

void iplookup_app_main_setup( uint32_t tile_num ) {

    iplookup_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &iplookup_app_main_style, APP_STYLE );

    lv_obj_t * exit_btn = lv_imgbtn_create( iplookup_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &iplookup_app_main_style );
    lv_obj_align(exit_btn, iplookup_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_iplookup_app_main_event_cb );

    lv_obj_t * next_btn = lv_imgbtn_create( iplookup_app_main_tile, NULL);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_RELEASED, &iplookup_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_PRESSED, &iplookup_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_RELEASED, &iplookup_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_PRESSED, &iplookup_app_32px);
    lv_obj_add_style(next_btn, LV_IMGBTN_PART_MAIN, &iplookup_app_main_style );
    lv_obj_align(next_btn, iplookup_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, (LV_HOR_RES / 2) -15 , -10 );
    lv_obj_set_event_cb( next_btn, enter_iplookup_app_next_event_cb );

    // text entry 
    lv_obj_t *iplookup_ip_cont = lv_obj_create( iplookup_app_main_tile, NULL );
    lv_obj_set_size(iplookup_ip_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( iplookup_ip_cont, iplookup_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 20 );
    lv_obj_t *iplookup_ip_label = lv_label_create( iplookup_ip_cont, NULL);
    lv_label_set_text( iplookup_ip_label, "IP");
    lv_obj_align( iplookup_ip_label, iplookup_ip_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    iplookup_ip_textfield = lv_textarea_create( iplookup_ip_cont, NULL);
    lv_textarea_set_text( iplookup_ip_textfield, "1.1.1.1" );
    lv_textarea_set_accepted_chars(iplookup_ip_textfield, "0123456789.");
    lv_textarea_set_pwd_mode( iplookup_ip_textfield, false);
    lv_textarea_set_one_line( iplookup_ip_textfield, true);
    lv_textarea_set_cursor_hidden( iplookup_ip_textfield, true);
    lv_obj_set_width( iplookup_ip_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( iplookup_ip_textfield, iplookup_ip_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( iplookup_ip_textfield, iplookup_textarea_event_cb );
}

static void iplookup_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void enter_iplookup_app_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      if(iplookup_result_cont != NULL)
                                       {
                                           lv_obj_del( iplookup_result_cont );      
                                           iplookup_result_cont = NULL;   
                                       }            
                                       iplookup_result_cont = lv_cont_create( iplookup_app_main_tile, NULL);
                                       lv_obj_set_auto_realign(iplookup_result_cont, true);                    
                                       lv_obj_align_origo(iplookup_result_cont, NULL, LV_ALIGN_CENTER, 0, 0);  
                                       lv_cont_set_fit(iplookup_result_cont, LV_FIT_TIGHT);
                                       lv_cont_set_layout(iplookup_result_cont, LV_LAYOUT_COLUMN_MID);
                                       fetch_ip_info();
                                       break;
    }
}

static void exit_iplookup_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       if(iplookup_result_cont != NULL)
                                        {
                                            lv_obj_del( iplookup_result_cont );      
                                            iplookup_result_cont = NULL;   
                                        }            
                                        mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

