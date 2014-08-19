/**
 * @file keymacs.c
 * @author Zhang Hai
 */

#include "keymacs.h"

#include "log.h"
#include "xkey.h"

static BOOL key_handler(XKeyEvent *key_event, KeySym key_sym,
        unsigned int modifiers);

static BOOL alt_x_pressed = FALSE;
static BOOL control_x_pressed = FALSE;
static unsigned int selection_mask = 0;

void keymacs_on_bind_key() {
    // M-x
    xkey_bind_key(XK_X, AltMask, key_handler);
    // C-x
    xkey_bind_key(XK_X, ControlMask, key_handler);
    // Navigation
    xkey_bind_key(XK_F, ControlMask, key_handler);
    xkey_bind_key(XK_B, ControlMask, key_handler);
    xkey_bind_key(XK_P, ControlMask, key_handler);
    xkey_bind_key(XK_N, ControlMask, key_handler);
    xkey_bind_key(XK_A, ControlMask, key_handler);
    xkey_bind_key(XK_E, ControlMask, key_handler);
    xkey_bind_key(XK_V, ControlMask, key_handler);
    xkey_bind_key(XK_V, AltMask, key_handler);
    xkey_bind_key(XK_comma, AltMask | ShiftMask, key_handler);
    xkey_bind_key(XK_period, AltMask | ShiftMask, key_handler);
    xkey_bind_key(XK_F, AltMask, key_handler);
    xkey_bind_key(XK_B, AltMask, key_handler);
    // Edit
    xkey_bind_key(XK_space, ControlMask, key_handler);
    xkey_bind_key(XK_H, 0, key_handler);
    xkey_bind_key(XK_W, ControlMask, key_handler);
    xkey_bind_key(XK_W, AltMask, key_handler);
    xkey_bind_key(XK_Y, ControlMask, key_handler);
    xkey_bind_key(XK_D, ControlMask, key_handler);
    xkey_bind_key(XK_D, AltMask, key_handler);
    xkey_bind_key(XK_slash, ControlMask, key_handler);
    // Search
    xkey_bind_key(XK_S, ControlMask, key_handler);
    xkey_bind_key(XK_R, ControlMask, key_handler);
    // Frame
    xkey_bind_key(XK_K, 0, key_handler);
    xkey_bind_key(XK_C, ControlMask, key_handler);
    // Misc
    xkey_bind_key(XK_S, ControlMask, key_handler);
    // Quit
    xkey_bind_key(XK_G, ControlMask, key_handler);
}

static BOOL key_handler(XKeyEvent *key_event, KeySym key_sym,
        unsigned int modifiers) {

    BOOL is_press = key_event->type == KeyPress;
    Display *display = key_event->display;
    log_info("key_handler: Handling key sym=0x%x, modifiers=0x%x, press=%d",
            key_sym, modifiers, is_press);

    if (is_press) {
        // Quit
        if (key_sym == XK_G && modifiers == ControlMask) {
            control_x_pressed = FALSE;
            // TODO: Clear selection
            selection_mask = 0;
        }
        if (alt_x_pressed) {
            // M-x mode
            alt_x_pressed = FALSE;
            log_info("key_handler: M-X pressed=%d", alt_x_pressed);
            return FALSE;
        } else if (control_x_pressed) {
            // C-x mode
            control_x_pressed = FALSE;
            log_info("key_handler: C-X pressed=%d",
                    control_x_pressed);
            // Edit
            if (key_sym == XK_H && modifiers == 0) {
                xkey_send_key(key_event->display, XK_A, ControlMask);
            // Frame
            } else if (key_sym == XK_K && modifiers == 0) {
                // FIXME: Partially broken: switches to tty
                xkey_send_key(key_event->display, XK_F4, ControlMask);
            } else if (key_sym == XK_C && modifiers == ControlMask) {
                // FIXME: Partially broken: switches to tty
                xkey_send_key(key_event->display, XK_F4, AltMask);
            // Misc
            } else if (key_sym == XK_S && modifiers == ControlMask) {
                // FIXME: Broken
                xkey_send_key(key_event->display, XK_S, ControlMask);
            } else {
                // TODO: Ring a bell
            }
        } else {
            // Normal mode
            // M-X
            if (key_sym == XK_X && modifiers == AltMask) {
                alt_x_pressed = TRUE;
                log_info("key_handler: M-X pressed=%d",
                        alt_x_pressed);
            // C-X
            } else if (key_sym == XK_X && modifiers == ControlMask) {
                control_x_pressed = TRUE;
                log_info("key_handler: C-X pressed=%d",
                        control_x_pressed);
            // Navigation
            } else if (key_sym == XK_F && modifiers == ControlMask) {
                xkey_send_key(display, XK_Right, selection_mask);
            } else if (key_sym == XK_B && modifiers == ControlMask) {
                xkey_send_key(display, XK_Left, selection_mask);
            } else if (key_sym == XK_P && modifiers == ControlMask) {
                xkey_send_key(display, XK_Up, selection_mask);
            } else if (key_sym == XK_N && modifiers == ControlMask) {
                xkey_send_key(display, XK_Down, selection_mask);
            } else if (key_sym == XK_A && modifiers == ControlMask) {
                xkey_send_key(display, XK_Home, selection_mask);
            } else if (key_sym == XK_E && modifiers == ControlMask) {
                xkey_send_key(display, XK_End, selection_mask);
            } else if (key_sym == XK_V && modifiers == ControlMask) {
                xkey_send_key(display, XK_Page_Down, selection_mask);
            } else if (key_sym == XK_V && modifiers == AltMask) {
                xkey_send_key(display, XK_Page_Up, selection_mask);
            } else if (key_sym == XK_comma && modifiers == (AltMask | ShiftMask)) {
                xkey_send_key(display, XK_Home, ControlMask | selection_mask);
            } else if (key_sym == XK_period && modifiers == (AltMask | ShiftMask)) {
                xkey_send_key(display, XK_End, ControlMask | selection_mask);
            } else if (key_sym == XK_F && modifiers == AltMask) {
                xkey_send_key(display, XK_Right, ControlMask | selection_mask);
            } else if (key_sym == XK_B && modifiers == AltMask) {
                xkey_send_key(display, XK_Left, ControlMask | selection_mask);
            // Edit
            } else if (key_sym == XK_space && modifiers == ControlMask) {
                selection_mask ^= ShiftMask;
                log_info("key_handler: C-Space, selection=%d",
                        selection_mask == ShiftMask);
            } else if (key_sym == XK_W && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_X, ControlMask);
            } else if (key_sym == XK_W && modifiers == AltMask) {
                xkey_send_key(key_event->display, XK_C, ControlMask);
            } else if (key_sym == XK_Y && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_V, ControlMask);
            } else if (key_sym == XK_D && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_Delete, 0);
            } else if (key_sym == XK_D && modifiers == AltMask) {
                xkey_send_key(key_event->display, XK_Delete, ControlMask);
            } else if (key_sym == XK_K && modifiers == ControlMask) {
                if (selection_mask == 0) {
                    xkey_send_key(key_event->display, XK_End, ShiftMask);
                    xkey_send_key(key_event->display, XK_Delete, 0);
                } else {
                    xkey_send_key(key_event->display, XK_X, ControlMask);
                }
            } else if (key_sym == XK_slash && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_Z, ControlMask);
            // Search
            } else if (key_sym == XK_S && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_F3, 0);
            } else if (key_sym == XK_R && modifiers == ControlMask) {
                xkey_send_key(key_event->display, XK_F3, ShiftMask);
            } else {
                // Pass through keys bound for C-x in normal mode
                return FALSE;
            }
        }
    }
    return TRUE;
}
