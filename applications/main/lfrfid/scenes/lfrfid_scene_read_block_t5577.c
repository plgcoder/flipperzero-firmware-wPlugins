#include "../lfrfid_i.h"

static void lfrfid_read_block_t5577_do(LfRfid* app) {
    Popup* popup = app->popup;
    char curr_buf[50] = {};
    //TODO: use .txt file in resources for passwords.

    uint8_t* pass_p = app->password;
    uint32_t current_password =  pass_p[0]<<24 | pass_p[1]<<16 | pass_p[2]<<8 | pass_p[3];
    uint8_t page = app->read_page;
    uint8_t block = app->read_block;

    popup_set_header(popup, "Reading\nblock", 102, 10, AlignCenter, AlignCenter);
    //popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    popup_set_text(popup, curr_buf, /*92*/66, 33, AlignCenter, AlignCenter);
    snprintf(curr_buf, sizeof(curr_buf), "Page %u  Block %u", page, block);
    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    //DEBUG delay
    //furi_delay_ms(1000);
    notification_message(app->notifications, &sequence_blink_start_magenta);
    {
	//furi_delay_ms(500);
	bool use_password =  app->extra_options & LfRfidUsePassword;

	if (use_password)
	    t5577_send_read_command(page, block, true, current_password);
	else
	    t5577_send_read_command(page, block, false, 0);
    }
    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);
}

void lfrfid_scene_read_block_t5577_on_enter(void* context) {
    LfRfid* app = context;
    //Popup* popup = app->popup;

    // NEEDED?
    furi_string_reset(app->file_name);
    app->protocol_id = PROTOCOL_NO;
    app->read_type = LFRFIDWorkerReadTypeAuto;


    lfrfid_read_block_t5577_do(app);

    //notification_message(app->notifications, &sequence_success);
    
    //popup_set_header(popup, "Done!", 102, 10, AlignCenter, AlignTop);
    //popup_set_header(popup, "Fix code, doesn't work yet!", 60, 10, AlignCenter, AlignTop);
    //popup_set_icon(popup, 0, 7, &I_RFIDDolphinSuccess_108x57);
    //popup_set_context(popup, app);
    //popup_set_callback(popup, lfrfid_popup_timeout_callback);
    //popup_set_timeout(popup, 1500);
    //popup_enable_timeout(popup);
    //view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);

    notification_message_block(app->notifications, &sequence_set_green_255);
    
    // refactory
    // needs some tweaks?
    //scene_manager_set_scene_state(app->scene_manager, LfRfidSceneStart, SubmenuIndexRead);
    scene_manager_next_scene(app->scene_manager, LfRfidSceneRead);
    
    //Does work?
    // some code
    //LFRFIDWorker* worker = malloc sizeof LFRFIDWorker ;
    //worker->read_cb = NULL;
    //worker->read_type = LFRFIDWorkerReadTypeASKOnly;
    //LFRFIDFeature feature = LFRFIDFeatureASK;
    //ProtocolId * read_result = malloc sizeof ProtocolId ;
    //LFRFIDWorkerReadState state = lfrfid_worker_read_ttf(worker, feature, LFRFID_WORKER_WRITE_VERIFY_TIME_MS, read_result);
}

bool lfrfid_scene_read_block_t5577_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true; // Ignore Back button presses
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, LfRfidSceneExtraActions);
    } else if(event.type == SceneManagerEventTypeCustom && event.event == LfRfidEventPopupClosed) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, LfRfidSceneExtraActions);
        consumed = true;
    }
    return consumed;
}

void lfrfid_scene_read_block_t5577_on_exit(void* context) {
    LfRfid* app = context;
    popup_reset(app->popup);
    notification_message_block(app->notifications, &sequence_reset_green);
}
