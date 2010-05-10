#ifndef __PY_CONFIG_H_
#define __PY_CONFIG_H_

#include <string>
#include <glib.h>
#include <glib-object.h>
#include <ibus.h>
#include "Object.h"
#include "Bus.h"

namespace PY {

class Config : Object {
public:
    Config (Bus & bus) : Object (ibus_bus_get_config (bus)) {
        readDefaultValues ();
        g_signal_connect (get<IBusConfig> (), "value-changed", G_CALLBACK (valueChangedCallback), this);
    }

    static guint option (void) { return m_option & m_option_mask; }
    static guint orientation (void) { return m_orientation; }
    static guint pageSize (void) { return m_page_size; }
    static gboolean shiftSelectCandidate (void) { return m_shift_select_candidate; }
    static gboolean minusEqualPage (void) { return m_minus_equal_page; }
    static gboolean commaPeriodPage (void) { return m_comma_period_page; }
    static gboolean autoCommit (void) { return m_auto_commit; }
    static gboolean doublePinyin (void) { return m_double_pinyin; }
    static gint doublePinyinSchema (void) { return m_double_pinyin_schema; }
    static gboolean doublePinyinShowRaw (void) { return m_double_pinyin_show_raw; }
    static gboolean initChinese (void) { return m_init_chinese; }
    static gboolean initFull (void) { return m_init_full; }
    static gboolean initFullPunct (void) { return m_init_full_punct; }
    static gboolean initSimpChinese (void) { return m_init_simp_chinese; }
    static gboolean tradCandidate (void) { return m_trad_candidate; }
    static gboolean specialPhrases (void) { return m_special_phrases; }

private:
    bool read (const std::string & section, const std::string & name, bool defval);
    int read (const std::string & section, const std::string & name, int defval);
    void readDefaultValues (void);
    static void valueChangedCallback (IBusConfig    *config,
                                      const gchar   *section,
                                      const gchar   *name,
                                      const GValue  *value,
                                      Config        *self);

private:
    static guint m_option;
    static guint m_option_mask;

    static gint m_orientation;
    static guint m_page_size;
    static gboolean m_shift_select_candidate;
    static gboolean m_minus_equal_page;
    static gboolean m_comma_period_page;
    static gboolean m_auto_commit;

    static gboolean m_double_pinyin;
    static gint m_double_pinyin_schema;
    static gboolean m_double_pinyin_show_raw;

    static gboolean m_init_chinese;
    static gboolean m_init_full;
    static gboolean m_init_full_punct;
    static gboolean m_init_simp_chinese;
    static gboolean m_trad_candidate;
    static gboolean m_special_phrases;
};

};
#endif
