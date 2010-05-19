/* vim:set et sts=4: */
#ifndef __PY_BOPOMOFO_ENGINE_H__
#define __PY_BOPOMOFO_ENGINE_H__

#include "Engine.h"
#include "PinyinProperties.h"

namespace PY {

class BopomofoEngine : public Engine {
public:
    BopomofoEngine (IBusEngine *engine);
    ~BopomofoEngine (void);

    gboolean processKeyEvent (guint keyval, guint keycode, guint modifiers);
    void focusIn (void);
    void focusOut (void) {
        reset ();
    }

    void reset (void) {
        m_prev_pressed_key = IBUS_VoidSymbol;
        m_input_mode = MODE_INIT;
        for (gint i = 0; i < MODE_LAST; i++) {
            m_editors[i]->reset ();
        }
        m_fallback_editor->reset ();
    }


    void enable (void) {}
    void disable (void) {}
    void pageUp (void);
    void pageDown (void);
    void cursorUp (void);
    void cursorDown (void);

    gboolean propertyActivate (const gchar *prop_name, guint prop_state);
    void candidateClicked (guint index, guint button, guint state);

private:
    gboolean processPunct (guint keyval, guint keycode, guint modifiers);

private:
    void showSetupDialog (void);
    void connectEditorSignals (EditorPtr editor);

private:
    void slotCommitText (Text & text);
    void slotUpdatePreeditText (Text & text, guint cursor, gboolean visible);
    void slotShowPreeditText (void);
    void slotHidePreeditText (void);
    void slotUpdateAuxiliaryText (Text & text, gboolean visible);
    void slotShowAuxiliaryText (void);
    void slotHideAuxiliaryText (void);
    void slotUpdateLookupTable (LookupTable &table, gboolean visible);
    void slotUpdateLookupTableFast (LookupTable &table, gboolean visible);
    void slotShowLookupTable (void);
    void slotHideLookupTable (void);
    void slotUpdateProperty (Property & prop);

private:
    PinyinProperties m_props;

    guint    m_prev_pressed_key;

    enum {
        MODE_INIT = 0,          // init mode
        MODE_PUNCT,             // punct mode
        MODE_RAW,               // raw mode
    #if 0
        MODE_ENGLISH,           // press v into English input mode
        MODE_STROKE,            // press u into stroke input mode
    #endif
        MODE_EXTENSION,         // press i into extension input mode
        MODE_LAST,
    } m_input_mode;

    EditorPtr m_editors[MODE_LAST];
    EditorPtr m_fallback_editor;
};

};

#endif
