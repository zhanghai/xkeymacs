/**
 * @file keymacs.c
 * @author Zhang Hai
 */

#include "keymacs.h"

#include "log.h"
#include "xkey.h"

static BOOL control_x_pressed = FALSE;
static unsigned int selection_mask = 0;

static BOOL simple_handler(XKeyEvent *key_event, KeySym key_sym,
        unsigned int modifiers) {

    BOOL is_press = key_event->type == KeyPress;
    Display *display = key_event->display;
    log_info("simple_handler: Handling key sym=0x%x, modifiers=0x%x, press=%d",
            key_sym, modifiers, is_press);

    if (is_press) {
        // Quit
        if (key_sym == XK_G && modifiers == ControlMask) {
            control_x_pressed = FALSE;
            // TODO: Clear selection
            selection_mask = 0;
        }
        if (control_x_pressed) {
            // C-x mode
            control_x_pressed = FALSE;
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
            // C-X
            if (key_sym == XK_X && modifiers == ControlMask) {
                control_x_pressed = TRUE;
                log_info("simple_handler: C-X pressed=%d",
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
                log_info("simple_handler: Selection=%d",
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

void keymacs_on_bind_key() {
    // C-X
    xkey_bind_key(XK_X, ControlMask, simple_handler);
    // Navigation
    xkey_bind_key(XK_F, ControlMask, simple_handler);
    xkey_bind_key(XK_B, ControlMask, simple_handler);
    xkey_bind_key(XK_P, ControlMask, simple_handler);
    xkey_bind_key(XK_N, ControlMask, simple_handler);
    xkey_bind_key(XK_A, ControlMask, simple_handler);
    xkey_bind_key(XK_E, ControlMask, simple_handler);
    xkey_bind_key(XK_V, ControlMask, simple_handler);
    xkey_bind_key(XK_V, AltMask, simple_handler);
    xkey_bind_key(XK_comma, AltMask | ShiftMask, simple_handler);
    xkey_bind_key(XK_period, AltMask | ShiftMask, simple_handler);
    xkey_bind_key(XK_F, AltMask, simple_handler);
    xkey_bind_key(XK_B, AltMask, simple_handler);
    // Edit
    xkey_bind_key(XK_space, ControlMask, simple_handler);
    xkey_bind_key(XK_H, 0, simple_handler);
    xkey_bind_key(XK_W, ControlMask, simple_handler);
    xkey_bind_key(XK_W, AltMask, simple_handler);
    xkey_bind_key(XK_Y, ControlMask, simple_handler);
    xkey_bind_key(XK_D, ControlMask, simple_handler);
    xkey_bind_key(XK_D, AltMask, simple_handler);
    xkey_bind_key(XK_slash, ControlMask, simple_handler);
    // Search
    xkey_bind_key(XK_S, ControlMask, simple_handler);
    xkey_bind_key(XK_R, ControlMask, simple_handler);
    // Frame
    xkey_bind_key(XK_K, 0, simple_handler);
    xkey_bind_key(XK_C, ControlMask, simple_handler);
    // Misc
    xkey_bind_key(XK_S, ControlMask, simple_handler);
    // Quit
    xkey_bind_key(XK_G, ControlMask, simple_handler);
}
