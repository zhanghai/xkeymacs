/**
 * @file xkey.h
 * @author Zhang Hai
 */

#ifndef _XKEY_H_
#define _XKEY_H_

#include <X11/keysym.h>
#include <X11/Xlib.h>

#include "common.h"

extern unsigned int NumLockMask;
extern unsigned int ScrollLockMask;
extern unsigned int AltMask;

typedef BOOL (*xkey_handler_t)(XKeyEvent *event, KeySym key_sym,
        unsigned int modifiers);

void xkey_initialize();

void xkey_finalize();

void xkey_bind_key(KeySym key_sym, unsigned int modifiers,
        xkey_handler_t handler);

void xkey_send_key(Display *display, KeySym key_sym,
        unsigned int modifiers);

void xkey_loop();

#endif /* _XKEY_H_ */
