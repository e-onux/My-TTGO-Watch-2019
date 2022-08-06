/****************************************************************************
 *   linuxthor 2020
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
#include <lwip/sockets.h>

#include "netscan_app.h"
#include "netscan_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t * result_cont = NULL;
lv_obj_t *netscan_app_main_tile = NULL;
lv_style_t netscan_app_main_style;
lv_obj_t *netscan_ip_textfield = NULL;

lv_task_t * _netscan_app_task;

LV_IMG_DECLARE(next_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(netscan_app_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_netscan_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void netscan_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_netscan_app_next_event_cb( lv_obj_t * obj, lv_event_t event );
void netscan_app_task( lv_task_t * task );

void netscan_app_main_scanner_try( int port )
{
       lv_obj_t * label;

       log_i("connect start");
       int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
       if(sock < 0)
	  log_e("socket error");
       struct sockaddr_in serverAddress;
       serverAddress.sin_family = AF_INET;
       inet_pton(AF_INET, lv_textarea_get_text(netscan_ip_textfield), &serverAddress.sin_addr.s_addr);
       serverAddress.sin_port = htons(port);
       int rc = connect(sock, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));
       if(rc == 0)
       {
	   label = lv_label_create(result_cont, NULL);
	   lv_label_set_text_fmt(label, "%d: Open", port);
	   log_i("connect ok");
       }
       else
       {
	   label = lv_label_create(result_cont, NULL);
	   lv_label_set_text_fmt(label, "%d: Closed", port);
	   log_i("connect err");
       }
       rc = close(sock);
}

void netscan_app_main_setup( uint32_t tile_num ) {

    netscan_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &netscan_app_main_style, APP_STYLE );

    lv_obj_t * exit_btn = lv_imgbtn_create( netscan_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &netscan_app_main_style );
    lv_obj_align(exit_btn, netscan_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_netscan_app_main_event_cb );

    lv_obj_t * next_btn = lv_imgbtn_create( netscan_app_main_tile, NULL);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_RELEASED, &netscan_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_PRESSED, &netscan_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_RELEASED, &netscan_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_PRESSED, &netscan_app_32px);
    lv_obj_add_style(next_btn, LV_IMGBTN_PART_MAIN, &netscan_app_main_style );
    lv_obj_align(next_btn, netscan_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, (LV_HOR_RES / 2) -15 , -10 );
    lv_obj_set_event_cb( next_btn, enter_netscan_app_next_event_cb );

    // text entry 
    lv_obj_t *netscan_ip_cont = lv_obj_create( netscan_app_main_tile, NULL );
    lv_obj_set_size(netscan_ip_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( netscan_ip_cont, netscan_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 20 );
    lv_obj_t *netscan_ip_label = lv_label_create( netscan_ip_cont, NULL);
    lv_label_set_text( netscan_ip_label, "IP");
    lv_obj_align( netscan_ip_label, netscan_ip_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    netscan_ip_textfield = lv_textarea_create( netscan_ip_cont, NULL);
    lv_textarea_set_text( netscan_ip_textfield, "127.0.0.1" );
    lv_textarea_set_accepted_chars(netscan_ip_textfield, "0123456789.");
    lv_textarea_set_pwd_mode( netscan_ip_textfield, false);
    lv_textarea_set_one_line( netscan_ip_textfield, true);
    lv_textarea_set_cursor_hidden( netscan_ip_textfield, true);
    lv_obj_set_width( netscan_ip_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( netscan_ip_textfield, netscan_ip_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( netscan_ip_textfield, netscan_textarea_event_cb );
}

static void netscan_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void enter_netscan_app_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      result_cont = lv_cont_create( netscan_app_main_tile, NULL);
                                       lv_obj_set_auto_realign(result_cont, true);                    
                                       lv_obj_align_origo(result_cont, NULL, LV_ALIGN_CENTER, 0, 0);  
                                       lv_cont_set_fit(result_cont, LV_FIT_TIGHT);
                                       lv_cont_set_layout(result_cont, LV_LAYOUT_COLUMN_MID);
                                       netscan_app_main_scanner_try(21);
                                       netscan_app_main_scanner_try(22);
                                       netscan_app_main_scanner_try(23);
                                       netscan_app_main_scanner_try(80);
                                       netscan_app_main_scanner_try(443);
                                       break;
    }
}

static void exit_netscan_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       if(result_cont != NULL)
                                        {
                                            lv_obj_del( result_cont );      
                                            result_cont = NULL;   
                                        }            
                                        mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void netscan_app_task( lv_task_t * task ) {
    // put your code her
}
