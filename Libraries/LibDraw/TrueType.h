
#include <AK/String.h>
#include <AK/ByteBuffer.h>
#include <AK/NonnullRefPtr.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/HashMap.h>

#include <LibDraw/Size.h>

#include "stb_truetype.h"

struct TrueTypeGlyph
{
public:
    TrueTypeGlyph(ByteBuffer data, Size size)
        : m_data(data), m_size(size) {}

    Size size() const { return m_size; }
    const ByteBuffer *data() const { return &m_data; }

private:
    ByteBuffer m_data;
    Size m_size;
};


class TrueTypeFont
{
public:
    bool load_from_file(const String& path);
    // FIXME: Replace with NonnullRefPtr
    const TrueTypeGlyph* glyph(int codepoint);
private:
    ByteBuffer m_buffer;
    stbtt_fontinfo m_fontinfo;
    HashMap<int, NonnullOwnPtr<TrueTypeGlyph>> m_glyph_cache;
};