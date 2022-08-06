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

#include "libssh_esp32.h"
#include <libssh/libssh.h>

#include "sshclient_app.h"
#include "sshclient_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t *sshclient_app_main_tile = NULL;
lv_style_t sshclient_app_main_style;
lv_obj_t *sshclient_ip_textfield = NULL;
lv_obj_t *sshclient_user_textfield = NULL;
lv_obj_t *sshclient_pass_textfield = NULL;
lv_obj_t *sshclient_command_textfield = NULL;
lv_obj_t *win = NULL; 
lv_obj_t *txt = NULL; 
lv_obj_t *close_btn = NULL;

lv_task_t * _sshclient_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(sshclient_app_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_sshclient_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_sshclient_app_connect_event_cb( lv_obj_t * obj, lv_event_t event );
static void sshclient_ip_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void sshclient_user_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void sshclient_pass_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void sshclient_command_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
void sshclient_app_task( lv_task_t * task );
void ssh_task(void * pvParameters);

TaskHandle_t _ssh_Task = NULL;

/*
 * knownhosts.c
 * This file contains an example of how verify the identity of a
 * SSH server using libssh
 */

/*
Copyright 2003-2009 Aris Adamantiadis
This file is part of the SSH Library
You are free to copy this file, modify it in any way, consider it being public
domain. This does not apply to the rest of the library though, but it is
allowed to cut-and-paste working code from this file to any license of
program.
The goal is to show the API in action. It's not a reference on how terminal
clients must be made or how a client should react.
 */

#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libssh/priv.h"
#include <libssh/libssh.h>
#include "examples_common.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    //char buf[10];
    unsigned char *hash = NULL;
    size_t hlen;
    ssh_key srv_pubkey;
    int rc;

    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA256,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    state = ssh_session_is_known_server(session);

    switch(state) {
    case SSH_KNOWN_HOSTS_CHANGED:
        log_i("Host key for server changed : server's one is now :\n");
        ssh_print_hash(SSH_PUBLICKEY_HASH_SHA256, hash, hlen);
        ssh_clean_pubkey_hash(&hash);
        log_i("For security reason, connection will be stopped\n");
        return -1;
    case SSH_KNOWN_HOSTS_OTHER:
        log_i("The host key for this server was not found but an other type of key exists.\n");
        log_i("An attacker might change the default server key to confuse your client"
                "into thinking the key does not exist\n"
                "We advise you to rerun the client with -d or -r for more safety.\n");
        return -1;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        log_i("Could not find known host file. If you accept the host key here,\n");
        log_i("the file will be automatically created.\n");
        /* fallback to SSH_SERVER_NOT_KNOWN behavior */
        FALL_THROUGH;
    case SSH_SERVER_NOT_KNOWN:
        log_i(
                "The server is unknown. Do you trust the host key (yes/no)?\n");
        ssh_print_hash(SSH_PUBLICKEY_HASH_SHA256, hash, hlen);

        //if (fgets(buf, sizeof(buf), stdin) == NULL) {
        //    ssh_clean_pubkey_hash(&hash);
        //    return -1;
        //}
        //if(strncasecmp(buf,"yes",3)!=0){
        //    ssh_clean_pubkey_hash(&hash);
        //    return -1;
        //}
        log_i("This new key will be written on disk for further usage. do you agree ?\n");
        //if (fgets(buf, sizeof(buf), stdin) == NULL) {
        //    ssh_clean_pubkey_hash(&hash);
        //    return -1;
        //}
        //if(strncasecmp(buf,"yes",3)==0){
            rc = ssh_session_update_known_hosts(session);
            if (rc != SSH_OK) {
                ssh_clean_pubkey_hash(&hash);
                log_i( "error %s\n", strerror(errno));
                return -1;
            }
        //}

        break;
    case SSH_KNOWN_HOSTS_ERROR:
        ssh_clean_pubkey_hash(&hash);
        return -1;
    case SSH_KNOWN_HOSTS_OK:
        break; /* ok */
    }

    ssh_clean_pubkey_hash(&hash);

    return 0;
}

/*
 * authentication.c
 * This file contains an example of how to do an authentication to a
 * SSH server using libssh
 */

/*
Copyright 2003-2009 Aris Adamantiadis
This file is part of the SSH Library
You are free to copy this file, modify it in any way, consider it being public
domain. This does not apply to the rest of the library though, but it is
allowed to cut-and-paste working code from this file to any license of
program.
The goal is to show the API in action. It's not a reference on how terminal
clients must be made or how a client should react.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libssh/libssh.h>
#include "examples_common.h"

static int auth_keyfile(ssh_session session, char* keyfile)
{
    ssh_key key = NULL;
    char pubkey[132] = {0}; // +".pub"
    int rc;

    snprintf(pubkey, sizeof(pubkey), "%s.pub", keyfile);

    rc = ssh_pki_import_pubkey_file( pubkey, &key);

    if (rc != SSH_OK)
        return SSH_AUTH_DENIED;

    rc = ssh_userauth_try_publickey(session, NULL, key);

    ssh_key_free(key);

    if (rc!=SSH_AUTH_SUCCESS)
        return SSH_AUTH_DENIED;

    rc = ssh_pki_import_privkey_file(keyfile, NULL, NULL, NULL, &key);

    if (rc != SSH_OK)
        return SSH_AUTH_DENIED;

    rc = ssh_userauth_publickey(session, NULL, key);
    ssh_key_free(key);

    return rc;
}


static void error(ssh_session session)
{
    log_i("Authentication failed\n");
}

int authenticate_console(ssh_session session)
{
    int rc;
    int method;
    char *banner;

    banner = ssh_get_issue_banner(session);
    if (banner) {
        log_i("%s\n",banner);
        SSH_STRING_FREE_CHAR(banner);
    }

    // Try to authenticate with password
    log_i("trying password");
    rc = ssh_userauth_password(session, NULL, lv_textarea_get_text(sshclient_pass_textfield));
    log_i("ssh auth returns %d",rc);
    if (rc == SSH_AUTH_ERROR) {
        log_i("error with password auth");
        error(session);
        return rc;
    } else if (rc == SSH_AUTH_SUCCESS) {
        log_i("success in password auth");
        return rc;
    }

    return rc;
}

/*
 * connect_ssh.c
 * This file contains an example of how to connect to a
 * SSH server using libssh
 */

/*
Copyright 2009 Aris Adamantiadis
This file is part of the SSH Library
You are free to copy this file, modify it in any way, consider it being public
domain. This does not apply to the rest of the library though, but it is
allowed to cut-and-paste working code from this file to any license of
program.
The goal is to show the API in action. It's not a reference on how terminal
clients must be made or how a client should react.
 */

#include <libssh/libssh.h>
#include "examples_common.h"
#include <stdio.h>

ssh_session connect_ssh(const char *host, const char *user,int verbosity){
  ssh_session session;
  int auth=0;

  session=ssh_new();
  if (session == NULL) {
    log_i("couldn't create new session");
    return NULL;
  }

  if(user != NULL){
    if (ssh_options_set(session, SSH_OPTIONS_USER, user) < 0) { 
      log_i("couldn't set user in session");
      ssh_free(session);
      return NULL;
    }
  }

  if (ssh_options_set(session, SSH_OPTIONS_HOST, host) < 0) {
    log_i("couldn't set host in session");
    ssh_free(session);
    return NULL;
  }
  ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
  if(ssh_connect(session)){
    log_i("Connection failed\n");
    ssh_disconnect(session);
    ssh_free(session);
    return NULL;
  }
  //if(verify_knownhost(session)<0){
  //  log_i("knownhosts error in session");
  //  ssh_disconnect(session);
  //  ssh_free(session);
  //  return NULL;
  //}
  auth=authenticate_console(session);
  if(auth==SSH_AUTH_SUCCESS){
    return session;
  } else if(auth==SSH_AUTH_DENIED){
    log_i("Authentication failed\n");
  } else {
    log_i("Error while authenticating\n");
  }
  ssh_disconnect(session);
  ssh_free(session);
  return NULL;
}

int ex_main(){
    ssh_session session = NULL;
    ssh_channel channel = NULL;
    char buffer[512] = { 0 };
    int rbytes, wbytes, total = 0;
    int rc;

    memset(buffer, 0, sizeof(buffer)); 

    log_i("trying connection");
    log_i("%s",lv_textarea_get_text(sshclient_ip_textfield));
    log_i("%s",lv_textarea_get_text(sshclient_user_textfield));
    session = connect_ssh(lv_textarea_get_text(sshclient_ip_textfield), 
                                      lv_textarea_get_text(sshclient_user_textfield), 0);
    if (session == NULL) {
        log_i("connection error");
        if(win != NULL)
            lv_label_ins_text(txt, LV_LABEL_POS_LAST, "??? connect error");
        return 1;
    }

    channel = ssh_channel_new(session);
    if (channel == NULL) {
        log_i("channel error");
        if(win != NULL)
            lv_label_ins_text(txt, LV_LABEL_POS_LAST, "??? channel error");
        ssh_disconnect(session);
        ssh_free(session);
        return 1;
    }

    rc = ssh_channel_open_session(channel);
    if (rc < 0) {
        log_i("channel open error");
        goto failed;
    }

    rc = ssh_channel_request_exec(channel, lv_textarea_get_text(sshclient_command_textfield));
    if (rc < 0) {
        log_i("exec error");
        goto failed;
    }

    rbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    if (rbytes <= 0) {
        log_i("read error");
        goto failed;
    }

    do {
    log_i("ssh cmd: %s",buffer); 
    if(win != NULL)
        lv_label_ins_text(txt, LV_LABEL_POS_LAST, buffer);
    memset(buffer, 0, sizeof(buffer));  
    rbytes = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    total = 0;
    } while (rbytes > 0);

    if (rbytes < 0) {
        log_i("rbytes < 0");
        goto failed;
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    return 0;
failed:
    log_i("handling failed cleanup");
    if(win != NULL)
        lv_label_ins_text(txt, LV_LABEL_POS_LAST, "??? failed");
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    return 1;
}

void sshclient_app_main_setup( uint32_t tile_num ) {

    sshclient_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &sshclient_app_main_style, APP_STYLE );

    lv_obj_t * exit_btn = lv_imgbtn_create( sshclient_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &sshclient_app_main_style );
    lv_obj_align(exit_btn, sshclient_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_sshclient_app_main_event_cb );

    lv_obj_t * connect_btn = lv_imgbtn_create( sshclient_app_main_tile, NULL);
    lv_imgbtn_set_src(connect_btn, LV_BTN_STATE_RELEASED, &sshclient_app_32px);
    lv_imgbtn_set_src(connect_btn, LV_BTN_STATE_PRESSED, &sshclient_app_32px);
    lv_imgbtn_set_src(connect_btn, LV_BTN_STATE_CHECKED_RELEASED, &sshclient_app_32px);
    lv_imgbtn_set_src(connect_btn, LV_BTN_STATE_CHECKED_PRESSED, &sshclient_app_32px);
    lv_obj_add_style(connect_btn, LV_IMGBTN_PART_MAIN, &sshclient_app_main_style );
    lv_obj_align(connect_btn, sshclient_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( connect_btn, enter_sshclient_app_connect_event_cb );

    // text entry
    lv_obj_t *sshclient_ip_cont = lv_obj_create( sshclient_app_main_tile, NULL );
    lv_obj_set_size(sshclient_ip_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( sshclient_ip_cont, sshclient_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 20 );
    lv_obj_t *sshclient_ip_label = lv_label_create( sshclient_ip_cont, NULL);
    lv_label_set_text( sshclient_ip_label, "IP");
    lv_obj_align( sshclient_ip_label, sshclient_ip_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    sshclient_ip_textfield = lv_textarea_create( sshclient_ip_cont, NULL);
    lv_textarea_set_text( sshclient_ip_textfield, "192.168.0.1" );
    lv_textarea_set_accepted_chars(sshclient_ip_textfield, "0123456789.");
    lv_textarea_set_pwd_mode( sshclient_ip_textfield, false);
    lv_textarea_set_one_line( sshclient_ip_textfield, true);
    lv_textarea_set_cursor_hidden( sshclient_ip_textfield, true);
    lv_obj_set_width( sshclient_ip_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( sshclient_ip_textfield, sshclient_ip_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sshclient_ip_textfield, sshclient_ip_textarea_event_cb );

    // text entry
    lv_obj_t *sshclient_user_cont = lv_obj_create( sshclient_app_main_tile, NULL );
    lv_obj_set_size(sshclient_user_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( sshclient_user_cont, sshclient_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 60 );
    lv_obj_t *sshclient_user_label = lv_label_create( sshclient_user_cont, NULL);
    lv_label_set_text( sshclient_user_label, "User:");
    lv_obj_align( sshclient_user_label, sshclient_user_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    sshclient_user_textfield = lv_textarea_create( sshclient_user_cont, NULL);
    lv_textarea_set_text( sshclient_user_textfield, "root" );
    lv_textarea_set_pwd_mode( sshclient_user_textfield, false);
    lv_textarea_set_one_line( sshclient_user_textfield, true);
    lv_textarea_set_cursor_hidden( sshclient_user_textfield, true);
    lv_obj_set_width( sshclient_user_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( sshclient_user_textfield, sshclient_user_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sshclient_user_textfield, sshclient_user_textarea_event_cb );

    // text entry
    lv_obj_t *sshclient_pass_cont = lv_obj_create( sshclient_app_main_tile, NULL );
    lv_obj_set_size(sshclient_pass_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( sshclient_pass_cont, sshclient_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 100 );
    lv_obj_t *sshclient_pass_label = lv_label_create( sshclient_pass_cont, NULL);
    lv_label_set_text( sshclient_pass_label, "Pass:");
    lv_obj_align( sshclient_pass_label, sshclient_pass_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    sshclient_pass_textfield = lv_textarea_create( sshclient_pass_cont, NULL);
    lv_textarea_set_text( sshclient_pass_textfield, "*********" );
    lv_textarea_set_pwd_mode( sshclient_pass_textfield, true);
    lv_textarea_set_one_line( sshclient_pass_textfield, true);
    lv_textarea_set_cursor_hidden( sshclient_pass_textfield, true);
    lv_obj_set_width( sshclient_pass_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( sshclient_pass_textfield, sshclient_pass_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sshclient_pass_textfield, sshclient_pass_textarea_event_cb );

    // text entry
    lv_obj_t *sshclient_command_cont = lv_obj_create( sshclient_app_main_tile, NULL );
    lv_obj_set_size(sshclient_command_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( sshclient_command_cont, sshclient_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 140 );
    lv_obj_t *sshclient_command_label = lv_label_create( sshclient_command_cont, NULL);
    lv_label_set_text( sshclient_command_label, "Cmd:");
    lv_obj_align( sshclient_command_label, sshclient_command_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    sshclient_command_textfield = lv_textarea_create( sshclient_command_cont, NULL);
    lv_textarea_set_text( sshclient_command_textfield, "id" );
    lv_textarea_set_pwd_mode( sshclient_command_textfield, false);
    lv_textarea_set_one_line( sshclient_command_textfield, true);
    lv_textarea_set_cursor_hidden( sshclient_command_textfield, true);
    lv_obj_set_width( sshclient_command_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( sshclient_command_textfield, sshclient_command_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sshclient_command_textfield, sshclient_command_textarea_event_cb );

}

static void sshclient_ip_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}
static void sshclient_user_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}
static void sshclient_pass_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void sshclient_command_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

void ssh_task(void * pvParameters)
{
    log_i("init libssh");
    libssh_begin();
    vTaskDelay(100);
    int ex_rc = ex_main();
    vTaskDelay(100);
    ssh_finalize();
    vTaskDelay(100);
    vTaskDelete( NULL );
}

static void enter_sshclient_app_connect_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   win = lv_win_create(lv_scr_act(), NULL);
                                    lv_win_set_title(win, "Result");    
                                    close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);          
                                    lv_obj_set_event_cb(close_btn, lv_win_close_event_cb); 
                                    txt = lv_label_create(win, NULL);
                                    lv_label_set_long_mode(txt, LV_LABEL_LONG_BREAK);
                                    lv_obj_set_width( txt, LV_HOR_RES - 20); 
                                    lv_label_set_text(txt, "");
                                    lv_textarea_add_text(sshclient_ip_textfield,"");
                                    lv_textarea_add_text(sshclient_user_textfield,"");
                                    lv_textarea_add_text(sshclient_pass_textfield,"");
                                    lv_textarea_add_text(sshclient_command_textfield,"");
                                    xTaskCreate     (   ssh_task,                               /* Function to implement the task */
                                                        "SSH Task",                             /* Name of the task */
                                                        16000,                                  /* Stack size in words */
                                                        NULL,                                   /* Task input parameter */
                                                        1,                                      /* Priority of the task */
                                                        NULL);      
                                    break;
    }
}

static void exit_sshclient_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

