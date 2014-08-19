/**
 * @file xkey.c
 * @author Zhang Hai
 */

#include "xkey.h"

#include <string.h>

#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>

#include "log.h"

#define XKEY_BOUND_KEYS_MAX 256

#define XKEY_NORMALIZE_MODIFIERS(modifiers) (modifiers & ~(LockMask | NumLockMask | ScrollLockMask))

static void initialize_modifier_masks();
static void initialize_modifier_states();
static void grab_key(KeyCode key_code, unsigned int modifiers);
static void update_modifier_states();

unsigned int NumLockMask;
unsigned int ScrollLockMask;
unsigned int AltMask;

static Display *display;
static Window window;

static KeyCode control_l_key_code;
static KeyCode control_r_key_code;
static KeyCode alt_l_key_code;
static KeyCode alt_r_key_code;
static KeyCode shift_l_key_code;
static KeyCode shift_r_key_code;
static BOOL control_l_pressed;
static BOOL control_r_pressed;
static BOOL alt_l_pressed;
static BOOL alt_r_pressed;
static BOOL shift_l_pressed;
static BOOL shift_r_pressed;

static struct {
    KeySym key_sym;
    KeyCode key_code;
    unsigned int modifiers;
    xkey_handler_t handler;
} bound_keys[XKEY_BOUND_KEYS_MAX];
static unsigned int bound_keys_count = 0;

void xkey_initialize() {

    //int screen_number;
    BOOL supported;

    display= XOpenDisplay(NULL);
    if (display == NULL) {
        // TODO: Print critical and exit.
    }
    window = DefaultRootWindow(display);

    // Seems unnecessary
    /*
    for (screen_number = 0; screen_number < ScreenCount(display);
            ++screen_number) {
        XSelectInput(display, RootWindow(display, screen_number),
                KeyPressMask | KeyReleaseMask);
    }
    */

    XkbSetDetectableAutoRepeat(display, True, &supported);
    if (!supported) {
        log_error("xkey_initialize: Detectable auto repeat not supported");
    }

    initialize_modifier_masks();

    initialize_modifier_states();
}

static void initialize_modifier_masks() {

    static unsigned int mask_table[8] = {
            ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask,
            Mod3Mask, Mod4Mask, Mod5Mask
    };

    XModifierKeymap *modifier_keymap;
    KeyCode num_lock_code, scroll_lock_code, alt_l_code;
    int i, key_count;

    num_lock_code = XKeysymToKeycode(display, XK_Num_Lock);
    scroll_lock_code = XKeysymToKeycode(display, XK_Scroll_Lock);
    alt_l_code = XKeysymToKeycode(display, XK_Alt_L);

    modifier_keymap = XGetModifierMapping(display);
    if (modifier_keymap == NULL) {
        // Handle critical error.
    }

    key_count = 8 * modifier_keymap->max_keypermod;
    for (i = 0; i < key_count; ++i) {
        if (modifier_keymap->modifiermap[i] == num_lock_code) {
            NumLockMask = mask_table[i / modifier_keymap->max_keypermod];
        } else if (modifier_keymap->modifiermap[i] == scroll_lock_code) {
            ScrollLockMask = mask_table[i / modifier_keymap->max_keypermod];
        } else if (modifier_keymap->modifiermap[i] == alt_l_code) {
            AltMask = mask_table[i / modifier_keymap->max_keypermod];
        }
    }

    XFreeModifiermap(modifier_keymap);
}

static void initialize_modifier_states() {
    control_l_key_code = XKeysymToKeycode(display, XK_Control_L);
    control_r_key_code = XKeysymToKeycode(display, XK_Control_R);
    alt_l_key_code = XKeysymToKeycode(display, XK_Alt_L);
    alt_r_key_code = XKeysymToKeycode(display, XK_Alt_R);
    shift_l_key_code = XKeysymToKeycode(display, XK_Shift_L);
    shift_r_key_code = XKeysymToKeycode(display, XK_Shift_R);
}

void xkey_finalize() {
    XUngrabKey(display, AnyKey, AnyModifier, window);
    XUngrabKeyboard(display, CurrentTime);
    // The following line causes application to hang, since we are
    // exiting we just ignore it.
    //XCloseDisplay(display);
}

void xkey_bind_key(KeySym key_sym, unsigned int modifiers,
        xkey_handler_t handler) {

    KeyCode key_code = XKeysymToKeycode(display, key_sym);
    modifiers = XKEY_NORMALIZE_MODIFIERS(modifiers);

    grab_key(key_code, modifiers);

    bound_keys[bound_keys_count].key_sym = key_sym;
    bound_keys[bound_keys_count].key_code = key_code;
    bound_keys[bound_keys_count].modifiers = modifiers;
    bound_keys[bound_keys_count].handler = handler;
    ++bound_keys_count;
}

static void grab_key(KeyCode key_code, unsigned int modifiers) {
    XGrabKey(display, key_code, modifiers, window, False,
            GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | LockMask, window, False,
            GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | NumLockMask, window,
            False, GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | LockMask | NumLockMask,
            window, False, GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | ScrollLockMask, window,
            False, GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | LockMask | ScrollLockMask,
            window, False, GrabModeSync, GrabModeSync);
    XGrabKey(display, key_code, modifiers | NumLockMask
            | ScrollLockMask, window, False, GrabModeSync,
            GrabModeSync);
    XGrabKey(display, key_code, modifiers | LockMask | NumLockMask
            | ScrollLockMask, window, False, GrabModeSync,
            GrabModeSync);
}

static void update_modifier_states() {

    char keys[32];

    XQueryKeymap(display, keys);

#define XKEY_IS_PRESSED(key_code) ((keys[key_code / 8] & (1 << (key_code % 8))) != 0)
    control_l_pressed = XKEY_IS_PRESSED(control_l_key_code);
    control_r_pressed = XKEY_IS_PRESSED(control_r_key_code);
    alt_l_pressed = XKEY_IS_PRESSED(alt_l_key_code);
    alt_r_pressed = XKEY_IS_PRESSED(alt_r_key_code);
    shift_l_pressed = XKEY_IS_PRESSED(shift_l_key_code);
    shift_r_pressed = XKEY_IS_PRESSED(shift_r_key_code);
#undef XKEY_IS_PRESSED
}

/**
 * Calls UngrabKeyboard() to avoid being grabbed again by ourselves,
 * however if there is any currently grabbed key, a KeyRelease event
 * will be sent to target instead of to us.
 */
void xkey_send_key(Display *display, KeySym key_sym,
        unsigned int modifiers) {

    KeyCode key_code;
    BOOL need_control, has_control, need_alt, has_alt, need_shift,
            has_shift;

    key_code = XKeysymToKeycode(display, key_sym);
    log_info("xkey_send_key: Sending key code=0x%x, modifiers=0x%x",
            key_code, modifiers);

    update_modifier_states();

    need_control = (modifiers & ControlMask) != 0;
    has_control = control_l_pressed || control_r_pressed;
    need_alt = (modifiers & AltMask) != 0;
    has_alt = alt_l_pressed || alt_r_pressed;
    need_shift = (modifiers & ShiftMask) != 0;
    has_shift = shift_l_pressed || shift_r_pressed;
    log_info("xkey_send_key: Control needed=%d, left=%d, right=%d",
            need_control, control_l_pressed, control_r_pressed);
    log_info("xkey_send_key: Alt needed=%d, left=%d, right=%d",
            need_alt, alt_l_pressed, alt_r_pressed);
    log_info("xkey_send_key: Shift needed=%d, left=%d, right=%d",
            need_shift, shift_l_pressed, shift_r_pressed);

    XUngrabKeyboard(display, CurrentTime);

    // TODO: Is this needed?
    //XTestGrabControl(display, True);

    if (need_control && !has_control) {
        XTestFakeKeyEvent(display, control_l_key_code, True,
                CurrentTime);
    } else if (!need_control && has_control) {
        if (control_l_pressed) {
            XTestFakeKeyEvent(display, control_l_key_code, False,
                CurrentTime);
        }
        if (control_r_pressed) {
            XTestFakeKeyEvent(display, control_r_key_code, False,
                    CurrentTime);
        }
    }
    if (need_alt && !has_alt) {
        XTestFakeKeyEvent(display, alt_l_key_code, True, CurrentTime);
    } else if (!need_alt && has_alt) {
        if (alt_l_pressed) {
            XTestFakeKeyEvent(display, alt_l_key_code, False,
                    CurrentTime);
        }
        if (alt_r_pressed) {
            XTestFakeKeyEvent(display, alt_r_key_code, False,
                    CurrentTime);
        }
    }
    if (need_shift && !has_shift) {
        XTestFakeKeyEvent(display, shift_l_key_code, True,
                CurrentTime);
    } else if (!need_shift && has_shift) {
        if (shift_l_pressed) {
            XTestFakeKeyEvent(display, shift_l_key_code, False,
                    CurrentTime);
        }
        if (shift_r_pressed) {
            XTestFakeKeyEvent(display, shift_r_key_code, False,
                    CurrentTime);
        }
    }

    XTestFakeKeyEvent(display, key_code, True, CurrentTime);
    XTestFakeKeyEvent(display, key_code, False, CurrentTime);

    if (need_shift && !has_shift) {
        XTestFakeKeyEvent(display, shift_l_key_code, False,
                CurrentTime);
    } else if (!need_shift && has_shift) {
        if (shift_r_pressed) {
            XTestFakeKeyEvent(display, shift_r_key_code, True,
                    CurrentTime);
        }
        if (shift_l_pressed) {
            XTestFakeKeyEvent(display, shift_l_key_code, True,
                    CurrentTime);
        }
    }
    if (need_alt && !has_alt) {
        XTestFakeKeyEvent(display, alt_l_key_code, False,
                CurrentTime);
    } else if (!need_alt && has_alt) {
        if (alt_r_pressed) {
            XTestFakeKeyEvent(display, alt_r_key_code, True,
                    CurrentTime);
        }
        if (alt_l_pressed) {
            XTestFakeKeyEvent(display, alt_l_key_code, True,
                    CurrentTime);
        }
    }
    if (need_control && !has_control) {
        XTestFakeKeyEvent(display, control_l_key_code, False,
                CurrentTime);
    } else if (!need_control && has_control) {
        if (control_r_pressed) {
            XTestFakeKeyEvent(display, control_r_key_code, True,
                    CurrentTime);
        }
        if (control_l_pressed) {
            XTestFakeKeyEvent(display, control_l_key_code, True,
                CurrentTime);
        }
    }

    //XTestGrabControl(display, False);
}

void xkey_loop() {

    XEvent event;
    XKeyEvent *key_event;
    unsigned int modifiers;
    BOOL handled;
    int i;

    while (TRUE) {

        XNextEvent(display, &event);
        if (!(event.type == KeyPress || event.type == KeyRelease)) {
            continue;
        }

        key_event = &event.xkey;
        modifiers = XKEY_NORMALIZE_MODIFIERS(key_event->state);
        log_info("xkey_loop: Handling key code=0x%x, modifiers=0x%x, press=%d",
                key_event->keycode, modifiers,
                key_event->type == KeyPress);

        handled = FALSE;
        for (i = 0; i < bound_keys_count; ++i) {
            if (key_event->keycode == bound_keys[i].key_code
                    && modifiers == bound_keys[i].modifiers) {
                if (bound_keys[i].handler(key_event,
                        bound_keys[i].key_sym,
                        bound_keys[i].modifiers)) {
                    log_info("xkey_loop: Syncing");
                    XAllowEvents(display, SyncKeyboard, CurrentTime);
                } else {
                    log_info("xkey_loop: Replaying");
                    XAllowEvents(display, ReplayKeyboard,
                            CurrentTime);
                    XFlush(display);
                }
                handled = TRUE;
            }
        }
        if (!handled) {
            log_warn("xkey_loop: Unhandled, replaying: key code=0x%x, modifiers=0x%x, press=%d",
                    key_event->keycode, modifiers,
                    key_event->type == KeyPress);
            XAllowEvents(display, ReplayKeyboard,
                    CurrentTime);
            XFlush(display);
        }
    }
}
