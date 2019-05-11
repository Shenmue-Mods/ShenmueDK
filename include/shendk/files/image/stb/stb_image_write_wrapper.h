#pragma once
#include "shendk/files/image/stb/stb_image_write.h"

#include <ostream>

static void writeStbToStream(void *context, void *data, int size) {
    std::ostream *stream = reinterpret_cast<std::ostream*>(context);
    stream->write(reinterpret_cast<char*>(data), size);
}
