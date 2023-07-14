#pragma once

#include <gui/view.h>
#include <toolbox/protocols/protocol_dict.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <toolbox/manchester_decoder.h>
#include <toolbox/stream/file_stream.h>

typedef enum {
    LfRfidReadAsk,
    LfRfidReadPsk,
    LfRfidReadAskOnly,
    LfRfidReadPskOnly
} LfRfidReadViewMode;

typedef struct LfRfidReadView LfRfidReadView;

LfRfidReadView* lfrfid_view_read_alloc();

void lfrfid_view_read_free(LfRfidReadView* read_view);

View* lfrfid_view_read_get_view(LfRfidReadView* read_view);

void lfrfid_view_read_set_read_mode(LfRfidReadView* read_view, LfRfidReadViewMode mode, ProtocolDict* dict, Storage* pstorage);

#include "lfrfid_view_read_i.h"
