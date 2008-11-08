/*
*      wmfs.c
*      Copyright © 2008 Martin Duquesnoy <xorg62@gmail.com>
*      All rights reserved.
*
*      Redistribution and use in source and binary forms, with or without
*      modification, are permitted provided that the following conditions are
*      met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following disclaimer
*        in the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the  nor the names of its
*        contributors may be used to endorse or promote products derived from
*        this software without specific prior written permission.
*
*      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "wmfs.h"

void
init(void)
{
     /* First init */
     gc = DefaultGC (dpy, screen);
     screen = DefaultScreen (dpy);

     init_font();
     init_cursor();
     init_key();
     init_atom();
     init_root();
     infobar_init(&infobar);
     init_geometry();
     grabkeys();

     /* Warning about font */
     if(TBARH + BORDH < font->height)
          fprintf(stderr, "WMFS Warning: Font too big, can't draw any text in the titlebar.\n");

     return;
}

void
init_font(void)
{
     font = XftFontOpenName(dpy, screen, conf.font);
     if(!font)
     {
          fprintf(stderr, "WMFS Error: Cannot initialize font\n");
          font = XftFontOpenName(dpy, screen, "sans-10");
     }
}

void
init_cursor(void)
{
     cursor[CurNormal] = XCreateFontCursor(dpy, XC_left_ptr);
     cursor[CurResize] = XCreateFontCursor(dpy, XC_sizing);
     cursor[CurMove] = XCreateFontCursor(dpy, XC_fleur);

     return;
}

void
init_key(void)
{
     int i, j;
     XModifierKeymap *modmap = XGetModifierMapping(dpy);

     for(i = 0; i < 8; i++)
          for(j = 0; j < modmap->max_keypermod; ++j)
          {
               if(modmap->modifiermap[i * modmap->max_keypermod + j]
                  == XKeysymToKeycode(dpy, XK_Num_Lock))
                    numlockmask = (1 << i);
               if(modmap->modifiermap[i * modmap->max_keypermod + j]
                  == XKeysymToKeycode(dpy, XK_Scroll_Lock))
                    scrolllockmask = (1 << i);
          }
     XFreeModifiermap(modmap);

     return;
}

void
init_atom(void)
{
     wm_atom[WMState] = XInternAtom(dpy, "WM_STATE", False);
     wm_atom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
     wm_atom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
     wm_atom[WMName] = XInternAtom(dpy, "WM_NAME", False);
     net_atom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
     net_atom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
     XChangeProperty(dpy, root, net_atom[NetSupported], XA_ATOM, 32,
                     PropModeReplace, (unsigned char *) net_atom, NetLast);
     return;
}

void
init_root(void)
{
     XSetWindowAttributes at;

     root = RootWindow (dpy, screen);

     at.event_mask = KeyMask | ButtonPressMask | ButtonReleaseMask |
          SubstructureRedirectMask | SubstructureNotifyMask |
          EnterWindowMask | LeaveWindowMask | StructureNotifyMask ;
     at.cursor = cursor[CurNormal];
     XChangeWindowAttributes(dpy, root, CWEventMask | CWCursor, &at);
     if(conf.root.background_command)
          uicb_spawn(conf.root.background_command);

     return;
}

void
init_geometry(void)
{
     sgeo.x = BORDH;
     sgeo.y = infobar.geo.height;

     if(conf.bartop)
          sgeo.y += TBARH;
     else
          sgeo.y = TBARH;

     sgeo.width  = MAXW;
     sgeo.height = MAXH - infobar.geo.height - TBARH;

     return;
}