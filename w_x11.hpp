#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cstring>
#include <iostream>
#include "common.hpp"

namespace spw {
  static Display *display;
  static XFontStruct *body_font;
  static XFontStruct *heading_font;
  static Window base_window, text_window, title_window, buttons_window;
  static int screen_num;
  static XColor fg, bg, mg;
  static GC     gc;
  unsigned int padding;

  void createTextArea() {
    unsigned long m;
    XSetWindowAttributes a;

    m = CWBackPixel | CWBorderPixel | CWEventMask;
    a.event_mask = ExposureMask;
    a.border_pixel = mg.pixel;
    a.background_pixel = bg.pixel;
    text_window = XCreateWindow(display, base_window,
        padding, padding, 140, 100, 2,
        DefaultDepth(display, screen_num), InputOutput,
        DefaultVisual(display, screen_num), m, &a);
    XClearWindow(display, text_window);
  }
  void fitTextArea() {
    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;

    int text_x, text_y;
    unsigned int text_width, text_height;

    XGetGeometry(display, base_window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
    std::cout << "y ret" << y_return << "x" << height_return;

    text_width = width_return - padding*2 - border_width_return*2;
    text_height = height_return - height_return / 8;

    // Adjust for title.
    XGetGeometry(display, title_window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
    text_x = padding;
    text_y = y_return + height_return;
    std::cout << "y ret" << y_return << "x" << height_return;
    std::cout << "Making text @ " << text_x << "x" << text_y << std::endl;

    // TODO: Adjust for buttons.
    XMoveResizeWindow(display, text_window, text_x, text_y, text_width, text_height);
  }
  void createButtons() {
  }
  void fitButtons() {
    // TODO: position after text_window
  }

  void createTitle(std::string title) {
    unsigned long m;
    XSetWindowAttributes a;

    m = CWEventMask;
    a.event_mask = ExposureMask;
    //a.border_pixel = mg.pixel;
    //a.background_pixel = bg.pixel;
   
    int direction_return;
    int font_ascent_return, font_descent_return;
    XCharStruct overall_return;
    XQueryTextExtents(display, heading_font->fid, (char*)title.c_str(), title.length(), &direction_return, &font_ascent_return, &font_descent_return, &overall_return);

    title_window = XCreateWindow(display, base_window,
        padding, padding, overall_return.width, overall_return.ascent - overall_return.descent, 0,
        DefaultDepth(display, screen_num), InputOutput,
        DefaultVisual(display, screen_num), m, &a);
    XClearWindow(display, title_window);
    XDrawImageString(display, title_window, gc, 0, 0, (char*)title.c_str(), title.length());
  }
  void fitTitle(std::string title) {
    int direction_return;
    int font_ascent_return, font_descent_return;
    XCharStruct overall_return;
    XQueryTextExtents(display, body_font->fid, (char*)title.c_str(), title.length(), &direction_return, &font_ascent_return, &font_descent_return, &overall_return);

    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;
    XGetGeometry(display, base_window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);

    XResizeWindow(display, title_window, width_return - padding*2 - border_width_return*2, overall_return.ascent - overall_return.descent);

    XDrawImageString(display, title_window, gc, 0, 0, (char*)title.c_str(), title.length());
  }

  bool createX11Window(spw::Level level, const std::string title, const std::string body) {
    Window slider_window, trough_window;
    XSetWindowAttributes  attrs;
    XSizeHints            wmsize;
    XWMHints              wmhints;
    XTextProperty         window_name, icon_name;
    XEvent                base_event;
    XGCValues             gc_values;
    Pixmap                buffer;
    int                   display_width, display_height;
    int                   window_x, window_y, window_width, window_height;

    char *win_name = (char*)title.c_str();
    char *ico_name = "spw";

    bool done = false;
    int i ,y, new_end, old_end;
    uint64_t mask;
    padding = 2;

    // Open X11 server connection.
    display = XOpenDisplay("");

    // Get necessary variables.
    screen_num = DefaultScreen(display);
    fg.pixel = 0xe3e3e3;
    bg.pixel = 0x232323;
    mg.pixel = 0xa3a3a3;
    display_width = XDisplayWidth(display, screen_num);
    display_height = XDisplayHeight(display, screen_num);
    //
    window_width = 600;
    window_height = 600;
    if (window_width > display_width) {
      window_width = display_width / 2;
    }
    if (window_height > display_height) {
      window_height = display_height / 2;
    }
    window_x = display_width / 2 - window_width / 2;
    window_y = display_height / 2 - window_height / 2;

    // Set attributes and mask.
    attrs.border_pixel = mg.pixel;
    attrs.background_pixel = bg.pixel;
    attrs.event_mask = ExposureMask;
    mask = CWBackPixel | CWBorderPixel | CWEventMask;

    // Create main window.
    base_window = XCreateWindow(display, RootWindow(display, screen_num),
        window_x, window_y, window_width, window_height, 2,
        DefaultDepth(display, screen_num), InputOutput,
        DefaultVisual(display, screen_num),
        mask, &attrs);
    XClearWindow(display, base_window);

    // Set WM hints.
    wmsize.flags = PSize;
    wmsize.width = window_width;
    wmsize.height = window_height;
    XSetWMNormalHints(display, base_window, &wmsize);
    wmhints.initial_state = NormalState;
    wmhints.flags = StateHint;
    XSetWMHints(display, base_window, &wmhints);

    XStringListToTextProperty(&win_name, 1, &window_name);
    XSetWMName(display, base_window, &window_name);

    XStringListToTextProperty(&ico_name, 1, &icon_name);
    XSetWMIconName(display, base_window, &icon_name);

    // Load our fonts
    body_font = XLoadQueryFont(display, "fixed");
    if (body_font == NULL) {
      std::cout << "OH NO" << std::endl;
    }

    heading_font = XLoadQueryFont(display, "10x20");
    if (heading_font == NULL) {
      std::cout << "OH NO" << std::endl;
    }

    // Get window resources.
    gc_values.background = mg.pixel;
    gc_values.foreground = fg.pixel;
    gc_values.font = body_font->fid;
    mask = GCForeground | GCBackground | GCFont;
    gc = XCreateGC(display, base_window, mask, &gc_values);

    XSelectInput(display, base_window,
        ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask
    );

    // Set our needed atoms.
    Atom atom_delete    = XInternAtom(display, "WM_DELETE_WINDOW", true);
    Atom atom_win_type  = XInternAtom(display, "_NET_WM_WINDOW_TYPE", true);
    Atom atom_dialog    = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", true);
    XSetWMProtocols(display, base_window, &atom_delete, 1);
    XSetWMProtocols(display, base_window, &atom_dialog, false);
    unsigned char data[sizeof(long)];
    memcpy(&data, &atom_dialog, sizeof(long));
    XChangeProperty(display, base_window, atom_win_type, XA_ATOM, 32, PropModeReplace, data, 1);

    // Create sub windows.
    // TODO
    createTextArea();
    createTitle(title);

    // Map windows.
    XMapWindow(display, base_window);
    XMapWindow(display, text_window);
    XMapWindow(display, title_window);
    
    //
    fitTitle(title);
    fitTextArea();

    // Event loop!
    while(!done) {
      XNextEvent(display, &base_event);
      switch(base_event.type) {
        case Expose:
          // UPDATE
          std::cout << "Expose" << std::endl;
          break;
        case ButtonPress:
          std::cout << "ButtonPress" << std::endl;
          break;
        case ButtonRelease:
          std::cout << "ButtonRelease" << std::endl;
          done = true;
          break;
        case ConfigureNotify:
          fitButtons();
          fitTitle(title);
          fitTextArea();
          std::cout << "ConfigureNotify: " << base_event.xconfigure.type << ": " << base_event.xconfigure.x << "x" << base_event.xconfigure.height << ": " << base_event.xconfigure.width << "x" << base_event.xconfigure.height << std::endl;
          break;
        case MotionNotify:
          // TODO
          break;
        case DestroyNotify:
          std::cout << "XDestroyWindowEvent" << std::endl;
          done = true;
          break;
        case ClientMessage:
          if ((Atom)base_event.xclient.data.l[0] == atom_delete) {
            done = true;
          }
          break;
      }
    }

    // Cleanup.
    XUnmapWindow(display, base_window);
    XFreeFont(display, body_font);
    XFreeFont(display, heading_font);
    return true;
  }
}
