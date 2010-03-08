#include "Config.h"
#include "FullPinyinEditor.h"

namespace PY {

FullPinyinEditor::FullPinyinEditor (PinyinProperties & props)
    : PinyinEditor (props)
{
}

FullPinyinEditor::~FullPinyinEditor (void)
{
}

void
FullPinyinEditor::reset (void)
{
    gboolean retval = FALSE;
    if (m_cursor != 0) {
        m_cursor = 0;
        retval = TRUE;
    }

    if (m_text.length () != 0) {
        m_text.truncate (0);
        retval = TRUE;
    }

    if (retval) {
        updatePinyin ();
        update ();
    }
}

gboolean
FullPinyinEditor::insert (gint ch)
{
    /* is full */
    if (G_UNLIKELY (m_text.length () >= MAX_PINYIN_LEN))
        return FALSE;

    m_text.insert (m_cursor++, ch);

    if (G_UNLIKELY ((Config::option () & PINYIN_INCOMPLETE_PINYIN) == 0)) {
        updatePinyin ();
    }
    else {
        if (G_LIKELY ((m_cursor - 1 == m_pinyin_len) ||
                      (m_cursor - 2 == m_pinyin_len &&
                       m_text[m_pinyin_len] == '\''))) {
            updatePinyin ();
        }
    }
    update ();
    return TRUE;
}

gboolean
FullPinyinEditor::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    m_text.erase (m_cursor, 1);

    updatePinyin ();
    update ();

    return TRUE;
}

gboolean
FullPinyinEditor::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_text.erase (m_cursor, 1);
    update ();

    return TRUE;
}

gboolean
FullPinyinEditor::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    guint cursor;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        cursor = m_pinyin_len;
    }
    else {
        const Pinyin & p = *m_pinyin.back ();
        cursor = m_cursor - p.len;
        m_pinyin_len -= p.len;
        m_pinyin.pop ();
    }

    m_text.erase (cursor, m_cursor - cursor);
    m_cursor = cursor;
    updatePhraseEditor ();
    update ();
    return TRUE;
}

gboolean
FullPinyinEditor::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_text.erase (m_cursor, -1);
    update ();
    return TRUE;
}

gboolean
FullPinyinEditor::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    updatePinyin ();
    update ();
    return TRUE;
}

gboolean
FullPinyinEditor::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_cursor ++;
    updatePinyin ();
    update ();

    return TRUE;
}

gboolean
FullPinyinEditor::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        return TRUE;
    }

    const Pinyin & p = *m_pinyin.back ();
    m_cursor -= p.len;
    m_pinyin_len -= p.len;
    m_pinyin.pop ();
    updatePhraseEditor ();
    update ();

    return TRUE;
}

gboolean
FullPinyinEditor::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

gboolean
FullPinyinEditor::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor = 0;
    m_pinyin.removeAll ();
    m_pinyin_len = 0;
    updatePhraseEditor ();
    update ();

    return TRUE;
}

gboolean
FullPinyinEditor::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return FALSE;

    m_cursor = m_text.length ();
    updatePinyin  ();
    update ();

    return TRUE;
}

void
FullPinyinEditor::updatePinyin (void)
{
    if (G_UNLIKELY (m_text.isEmpty ())) {
        m_pinyin.removeAll ();
        m_pinyin_len = 0;
    }
    else {
        m_pinyin_len = m_parser.parse (m_text,              // text
                                       m_cursor,            // text length
                                       Config::option (),   // option
                                       m_pinyin,            // result
                                       MAX_PHRASE_LEN);     // max result length
    }

    updatePhraseEditor ();
}

};
