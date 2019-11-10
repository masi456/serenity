// https://github.com/nothings/stb/blob/master/stb_truetype.h
// https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c
// https://github.com/nothings/stb/issues/450

#include "TrueType.h"

#include <AK/String.h>
#include <AK/kmalloc.h>
#include <AK/ByteBuffer.h>
#include <AK/StringBuilder.h>
#include <LibCore/CFile.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#pragma GCC diagnostic pop


// static String format_bytebuffer(const ByteBuffer &buffer, int width, int height) 
// {
//     ASSERT(buffer.size() == width * height);

//     StringBuilder string_builder;
//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             string_builder.appendf("%x ", buffer[y * width + x]);
//         }
//         string_builder.append('\n');
//     }

//     return string_builder.to_string();
// }

bool TrueTypeFont::load_from_file(const String &path)
{
    dbg() << "Loading TrueType font from " << path;
    auto file = CFile::construct();
    file->set_filename(path);
    if (!file->open(CIODevice::ReadOnly)) {
        dbg() << "Could not open file";
    }

    m_buffer = file->read_all();
    dbg() << "Buffer is " << m_buffer.size() << " bytes big";

    auto init_font_result = stbtt_InitFont(&m_fontinfo, m_buffer.data(), 0);
    if (init_font_result == 0) {
        dbg() << "InitFont failed";
    }

    dbg() << "InitFont success";
    return true;
}

const TrueTypeGlyph* TrueTypeFont::glyph(int codepoint)
{
    if (m_glyph_cache.contains(codepoint)) {
        auto map_entry = m_glyph_cache.get(codepoint);
        ASSERT(map_entry.has_value());
        
        auto ptr = map_entry.value();
        dbg() << String::format(
            "TrueTypeFont::glyph: Codepoint %c in buffer.\n  Data size: %d\n  Size: %s", 
            (char) codepoint, ptr->data()->size(), ptr->size().to_string());
        return ptr;
    }

    dbg() << "Trying to make Codepoint bitmap for codepoint " << (char) codepoint;

    int bitmap_width = 0;
    int bitmap_height = 0;
    auto buffer = ByteBuffer();

    if (codepoint == (int) ' ') {
        // FIXME: Take values from another char here.
        bitmap_width = 20;
        bitmap_height = 38;
        buffer = ByteBuffer::create_zeroed(bitmap_width * bitmap_height);
    } else {
        float scale = stbtt_ScaleForPixelHeight(&m_fontinfo, 64);
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(
            &m_fontinfo, codepoint, scale, scale,
            &x0, &y0, &x1, &y1
        );

        dbg() << String::format("Bitmap must be (%d, %d, %d, %d) big",
            x0, y0, x1, y1);

        bitmap_width = x1 - x0;
        bitmap_height = y1 - y0;
        dbg() << String::format("Bitmap width = %d, Bitmap height = %d", bitmap_width, bitmap_height);

        buffer = ByteBuffer::create_uninitialized(bitmap_width * bitmap_height);
        stbtt_MakeCodepointBitmap(
            &m_fontinfo, buffer.data(),
            bitmap_width, bitmap_height, bitmap_width,
            scale, scale, codepoint
        );
        // dbg() << format_bytebuffer(buffer, bitmap_width, bitmap_height);
    }

    auto ret = make<TrueTypeGlyph>(
        buffer,
        Size(bitmap_width, bitmap_height)
    );
    m_glyph_cache.set(codepoint, move(ret));

    return glyph(codepoint);
}
