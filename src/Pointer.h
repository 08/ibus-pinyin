#ifndef __PY_POINTER_H_
#define __PY_POINTER_H_

#include <glib-object.h>

namespace PY {

template<typename T>
struct Pointer {
public:
    Pointer (T *p = NULL) : m_p (NULL) {
        set (p);
    }

    ~Pointer (void) {
        set (NULL);
    }

    void set (T * p) {
        if (m_p) {
            g_object_unref (m_p);
        }

        m_p = p;
        if (p) {
	#if 0
            g_debug ("%s, floating = %d",G_OBJECT_TYPE_NAME (p), g_object_is_floating (p));
	#endif
            g_object_ref_sink (p);
        }
    }

    Pointer<T> &operator = (T *p) {
        set (p);
        return *this;
    }

    Pointer<T> &operator = (const Pointer<T> & p) {
        set (p.m_p);
        return *this;
    }

    const T * operator-> (void) const {
        return m_p;
    }

    T * operator-> (void) {
        return m_p;
    }

    operator T * (void) const {
        return m_p;
    }

    operator gboolean (void) const {
        return m_p != NULL;
    }

private:
    T *m_p;
};

};

#endif
