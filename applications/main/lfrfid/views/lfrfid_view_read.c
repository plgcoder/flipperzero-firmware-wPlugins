#include "lfrfid_view_read.h"
#include <gui/elements.h>
#include <assets_icons.h>

#include <toolbox/stream/file_stream.h>

#define TEMP_STR_LEN 128

struct LfRfidReadView {
    View* view;
};

typedef struct {
    IconAnimation* icon;
    LfRfidReadViewMode read_mode;
    ProtocolDict* dict;
    Storage* storage;
    //Stream* stream;
    //bool stream_result;
} LfRfidReadViewModel;

static void lfrfid_view_read_draw_callback(Canvas* canvas, void* _model) {
    LfRfidReadViewModel* model = _model;
    canvas_set_color(canvas, ColorBlack);

    Stream* stream = NULL;
    bool result = false;
    if (model->storage)
        stream = file_stream_alloc(model->storage);
    if (stream)
        result = file_stream_open(stream, "/ext/lfrfid/read_saved.txt", FSAM_READ_WRITE, FSOM_OPEN_APPEND);
 
    //canvas_draw_icon(canvas, 0, 8, &I_NFC_manual_60x50);
    canvas_draw_icon(canvas, 0, 3, &I_NFC_manual_60x50);

    canvas_set_font(canvas, FontPrimary);

    if(model->read_mode == LfRfidReadAsk) {
        canvas_draw_str(canvas, 70, 16, "Reading 1/2");

        canvas_draw_str(canvas, 77, 29, "ASK");
        canvas_draw_icon(canvas, 70, 22, &I_ButtonRight_4x7);
        canvas_draw_icon_animation(canvas, 102, 21, model->icon);

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 77, 43, "PSK");
    } else if(model->read_mode == LfRfidReadPsk) {
        canvas_draw_str(canvas, 70, 16, "Reading 2/2");

        canvas_draw_str(canvas, 77, 43, "PSK");
        canvas_draw_icon(canvas, 70, 36, &I_ButtonRight_4x7);
        canvas_draw_icon_animation(canvas, 102, 35, model->icon);

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 77, 29, "ASK");
    } else {
        canvas_draw_str(canvas, 72, 16, "Reading");

        if(model->read_mode == LfRfidReadAskOnly) {
            canvas_draw_str(canvas, 77, 35, "ASK");
        } else {
            canvas_draw_str(canvas, 77, 35, "PSK");
        }
        canvas_draw_icon_animation(canvas, 102, 27, model->icon);
    }

    canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str(canvas, 61, 54, "Don't move card");
    int proto = protocol_dict_get_current_read_protocol(model->dict);
    if (proto != PROTOCOL_NO)
        canvas_draw_str(canvas, 81, 54, protocol_dict_get_name(model->dict, proto));

    FuriString* info_str = malloc(30);
    ProtocolEM4100* p_em4100 = NULL;
    //ProtocolBase* proto_p = NULL;
    if (model->dict) {
	//int proto = *model->proto_id;
        //proto_p = model->dict->data[app->protocol];
        //if(proto_p && proto_p->encoded_data) {
        p_em4100 = model->dict->data[LFRFIDProtocolEM4100];

        if(p_em4100 && p_em4100->encoded_data) {
            uint64_t enc_data = p_em4100->encoded_data;
            //FIX
            if(enc_data>>32)	        // first byte not null
                //furi_string_printf(info_str, "%016llX", enc_data);
                furi_string_printf(info_str, "%04X %04X  %04X %04X", 
                        (uint16_t)(enc_data>>48), 
                        (uint16_t)((enc_data>>32)&0xFFFF),
                        (uint16_t)((enc_data>>16)&0xFFFF),
                        (uint16_t)(enc_data&0xFFFF) );
            else    // first byte null
                //furi_string_printf(info_str, "        %08lX", (uint32_t)enc_data);
                furi_string_printf(info_str, "%04X %04X",
                        (uint16_t)((enc_data>>16)&0xFFFF),
                        (uint16_t)(enc_data&0xFFFF)  );
        }
	canvas_draw_str(canvas, 0, 63, furi_string_get_cstr(info_str));

	if (result)
	    stream_write_format(stream, "%s\n", furi_string_get_cstr(info_str));
    }
    if (stream)
        stream_free(stream);
}

void lfrfid_view_read_enter(void* context) {
    LfRfidReadView* read_view = context;
    with_view_model(
        read_view->view, LfRfidReadViewModel * model, { icon_animation_start(model->icon); }, true);
}

void lfrfid_view_read_exit(void* context) {
    LfRfidReadView* read_view = context;
    with_view_model(
        read_view->view, LfRfidReadViewModel * model, { icon_animation_stop(model->icon); }, false);
}

LfRfidReadView* lfrfid_view_read_alloc() {
    LfRfidReadView* read_view = malloc(sizeof(LfRfidReadView));
    read_view->view = view_alloc();
    view_set_context(read_view->view, read_view);
    view_allocate_model(read_view->view, ViewModelTypeLocking, sizeof(LfRfidReadViewModel));

    with_view_model(
        read_view->view,
        LfRfidReadViewModel * model,
        {
            model->icon = icon_animation_alloc(&A_Round_loader_8x8);
            view_tie_icon_animation(read_view->view, model->icon);
            //model->stream = file_stream_alloc(storage);
            //model->stream_result = file_stream_open(stream, "/ext/lfrfid/read_saved.txt", FSAM_READ_WRITE, FSOM_OPEN_APPEND);
        },
        false);

    view_set_draw_callback(read_view->view, lfrfid_view_read_draw_callback);
    view_set_enter_callback(read_view->view, lfrfid_view_read_enter);
    view_set_exit_callback(read_view->view, lfrfid_view_read_exit);

    return read_view;
}

void lfrfid_view_read_free(LfRfidReadView* read_view) {
    with_view_model(
        read_view->view, LfRfidReadViewModel * model, { icon_animation_free(model->icon); }, false);

    view_free(read_view->view);
    free(read_view);

    //stream_free(model->stream);
    //furi_record_close(RECORD_STORAGE);
}

View* lfrfid_view_read_get_view(LfRfidReadView* read_view) {
    return read_view->view;
}

void lfrfid_view_read_set_read_mode(LfRfidReadView* read_view, LfRfidReadViewMode mode, ProtocolDict* pdict, Storage* pstorage) {
    with_view_model(
        read_view->view,
        LfRfidReadViewModel * model,
        {
            icon_animation_stop(model->icon);
            icon_animation_start(model->icon);
            model->read_mode = mode;
	    model->dict = pdict;
            model->storage = pstorage;
        },
        true);
}
