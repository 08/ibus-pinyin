#include "Config.h"
#include "DoublePinyinEditor.h"

namespace PY {

#include "DoublePinyinTable.h"

DoublePinyinEditor::DoublePinyinEditor (PinyinProperties & props)
    : PinyinEditor (props)
{
}

static inline gint
char_to_id (gint ch)
{
    switch (ch) {
    case IBUS_a ... IBUS_z:
        return ch - IBUS_a;
    case IBUS_semicolon:
        return 26;
    default:
        return -1;
    }
}

inline const Pinyin *
DoublePinyinEditor::isPinyin (gchar i, gchar j)
{
    const Pinyin *pinyin;
    gint schema = Config::doublePinyinSchema ();
    gint sheng = double_pinyin_map[schema].sheng[char_to_id(i)];
    const gint *yun = double_pinyin_map[schema].yun[char_to_id(j)];

    if (sheng == PINYIN_ID_VOID || yun[0] == PINYIN_ID_VOID)
        return NULL;

    if (sheng == PINYIN_ID_ZERO && yun[0] == PINYIN_ID_ZERO)
        return NULL;

    pinyin = m_parser.isPinyin (sheng, yun[0], Config::option () & PINYIN_FUZZY_ALL);
    if (pinyin == NULL && yun[1] != PINYIN_ID_VOID)
        pinyin = m_parser.isPinyin (sheng, yun[1], Config::option () & PINYIN_FUZZY_ALL);
    return pinyin;
}

gboolean
DoublePinyinEditor::insert (gint ch)
{
    /* is full */
    if (G_UNLIKELY (m_buffer.length () >= MAX_PINYIN_LEN))
        return FALSE;

    gint id = char_to_id (ch);
    if (id < 0)
        return FALSE;

    m_buffer.insert (m_cursor++, ch);

    if (m_cursor != m_pinyin_len + 2)
        return TRUE;

    const Pinyin *pinyin = isPinyin (m_buffer[m_cursor - 2], ch);
    if (pinyin == NULL)
        return TRUE;
    m_pinyin.append (pinyin, m_pinyin_len, 2);
    m_pinyin_len += 2;
    return TRUE;
}

gboolean
DoublePinyinEditor::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    m_buffer.erase (m_cursor, 1);

    if (m_cursor < m_pinyin_len) {
        m_pinyin.pop ();
        m_pinyin_len -= 2;
    }

    return TRUE;
}

gboolean
DoublePinyinEditor::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_buffer.length ()))
        return FALSE;

    m_buffer.erase (m_cursor, 1);

    return TRUE;
}

gboolean
DoublePinyinEditor::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    guint cursor;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        cursor = m_pinyin_len;
    }
    else {
        m_pinyin.pop ();
        cursor = m_cursor - 2;
        m_pinyin_len -= 2;
    }

    m_buffer.erase (cursor, m_cursor - cursor);
    m_cursor = cursor;
    return TRUE;
}

gboolean
DoublePinyinEditor::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_buffer.length ()))
        return FALSE;

    m_buffer.erase (m_cursor, -1);
    return TRUE;
}

gboolean
DoublePinyinEditor::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;
    m_cursor --;

    if (m_cursor >= m_pinyin_len)
        return TRUE;

    m_pinyin.pop ();
    m_pinyin_len -= 2;
    return TRUE;
}

gboolean
DoublePinyinEditor::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_buffer.length ()))
        return FALSE;

    m_cursor ++;
    updatePinyin ();

    return TRUE;
}

gboolean
DoublePinyinEditor::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        return TRUE;
    }

    m_pinyin.pop ();
    m_cursor -= 2;
    m_pinyin_len -= 2;
    return TRUE;
}

gboolean
DoublePinyinEditor::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

gboolean
DoublePinyinEditor::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor = 0;
    m_pinyin.removeAll ();
    m_pinyin_len = 0;

    return TRUE;
}

gboolean
DoublePinyinEditor::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_buffer.length ()))
        return FALSE;

    m_cursor = m_buffer.length ();
    updatePinyin  ();

    return TRUE;
}

void
DoublePinyinEditor::reset (void)
{
    gboolean retval = FALSE;
    if (m_cursor != 0) {
        m_cursor = 0;
        retval = TRUE;
    }

    if (m_buffer.length () != 0) {
        m_buffer.truncate (0);
        retval = TRUE;
    }

    if (retval)
        updatePinyin ();
}


void
DoublePinyinEditor::updatePinyin (void)
{
    if (G_UNLIKELY (m_buffer.isEmpty ())) {
        m_pinyin.removeAll ();
        m_pinyin_len = 0;
        return;
    }

    m_pinyin.removeAll ();
    m_pinyin_len = 0;
    for (guint i = 0; i + 1 < m_cursor; i+= 2) {
        const Pinyin *pinyin = isPinyin (m_buffer[i], m_buffer[i + 1]);
        if (pinyin == NULL)
            break;
        m_pinyin.append (pinyin, m_pinyin_len, 2);
        m_pinyin_len += 2;
    }
}

#define CMSHM_MASK              \
        (IBUS_CONTROL_MASK |    \
         IBUS_MOD1_MASK |       \
         IBUS_SUPER_MASK |      \
         IBUS_HYPER_MASK |      \
         IBUS_META_MASK)

#define CMSHM_FILTER(modifiers)  \
    (modifiers & (CMSHM_MASK))

gboolean
DoublePinyinEditor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    /* handle ';' key */
    if (G_UNLIKELY (keyval == IBUS_semicolon)) {
        if (CMSHM_FILTER (modifiers) == 0) {
            if (insert (keyval))
                return TRUE;
        }
    }

    return PinyinEditor::processKeyEvent (keyval, keycode, modifiers);
}

};


