/* TODO: add mouse down event */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "glk.h"
#include "garglk.h"
#include "garglk-plug.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

GtkWidget *frame;
static GtkWidget *canvas;
static GtkWidget *filedlog;
static char *filename;

static int timerid = -1;
static int timeouts = 0;

static int timeout(void *data)
{
	timeouts ++;
	return TRUE;
}

void glk_request_timer_events(glui32 millisecs)
{
	if (timerid != -1)
	{
		gtk_timeout_remove(timerid);
		timerid = -1;
	}

	if (millisecs)
	{
		timerid = gtk_timeout_add(millisecs, timeout, NULL);
	}
}

void winabort(const char *fmt, ...)
{
	va_list ap;
	char buf[256];
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	// XXX MessageBoxA(NULL, buf, "Fatal error", MB_ICONERROR);
	fprintf(stderr, "fatal: %s\n", buf);
	fflush(stderr);
	abort();
}

static void onokay(GtkFileSelection *widget, void *data)
{
	strcpy(filename, gtk_file_selection_get_filename(GTK_FILE_SELECTION(filedlog)));
	gtk_widget_destroy(filedlog);
	filedlog = NULL;
	gtk_main_quit(); /* un-recurse back to normal loop */
}

static void oncancel(GtkFileSelection *widget, void *data)
{
	strcpy(filename, "");
	gtk_widget_destroy(filedlog);
	filedlog = NULL;
	gtk_main_quit(); /* un-recurse back to normal loop */
}

void winopenfile(char *prompt, char *buf, int len)
{
    /* Return if in protected mode */
    if(garglk_plug_get_protected(GARGLK_PLUG(frame)))
        return;
    
	char realprompt[256];
	sprintf(realprompt, "Open: %s", prompt);
	filedlog = gtk_file_selection_new(realprompt);
	if (strlen(buf))
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(filedlog), buf);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filedlog));
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(filedlog)->ok_button),
		"clicked", (GCallback)onokay, NULL);
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(filedlog)->cancel_button),
		"clicked", (GCallback)oncancel, NULL);
	filename = buf;
	gtk_widget_show(filedlog);
	gtk_main(); /* recurse... */
}

void winsavefile(char *prompt, char *buf, int len)
{
    /* Return if in protected mode */
    if(garglk_plug_get_protected(GARGLK_PLUG(frame)))
        return;
    
	char realprompt[256];
	sprintf(realprompt, "Save: %s", prompt);
	filedlog = gtk_file_selection_new(realprompt);
	if (strlen(buf))
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(filedlog), buf);
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(filedlog)->ok_button),
		"clicked", (GCallback)onokay, NULL);
	g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(filedlog)->cancel_button),
		"clicked", (GCallback)oncancel, NULL);
	filename = buf;
	gtk_widget_show(filedlog);
	gtk_main(); /* recurse... */
}

static void onresize(GtkWidget *widget, GtkAllocation *event, void *data)
{
	int newwid = event->width;
	int newhgt = event->height;

	if (newwid == gli_image_w && newhgt == gli_image_h)
		return;

	gli_image_w = newwid;
	gli_image_h = newhgt;

	gli_image_s = ((gli_image_w * 3 + 3) / 4) * 4;
	if (gli_image_rgb)
		free(gli_image_rgb);
	gli_image_rgb = malloc(gli_image_s * gli_image_h);

	gli_force_redraw = 1;

	gli_windows_size_change();
}

static void onexpose(GtkWidget *widget, GdkEventExpose *event, void *data)
{
	int x0 = event->area.x;
	int y0 = event->area.y;
	int w = event->area.width;
	int h = event->area.height;

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x0 + w > gli_image_w) w = gli_image_w - x0;
	if (y0 + h > gli_image_h) h = gli_image_h - y0;
	if (w < 0) return;
	if (h < 0) return;

	gli_windows_redraw();

	gdk_draw_rgb_image(canvas->window, canvas->style->black_gc,
		x0, y0, w, h,
		GDK_RGB_DITHER_NONE,
		gli_image_rgb + y0 * gli_image_s + x0 * 3,
		gli_image_s);
}

static void onbutton(GtkWidget *widget, GdkEventButton *event, void *data)
{
    gtk_widget_grab_focus(gtk_bin_get_child(GTK_BIN(widget)));
	gli_input_handle_click(event->x, event->y);
}

static void onkeypress(GtkWidget *widget, GdkEventKey *event, void *data)
{
	int key = event->keyval;

	switch (key)
	{
	case GDK_Return: gli_input_handle_key(keycode_Return); break;
	case GDK_BackSpace: gli_input_handle_key(keycode_Delete); break;
	case GDK_Tab: gli_input_handle_key(keycode_Tab); break;
	case GDK_Prior: gli_input_handle_key(keycode_PageUp); break;
	case GDK_Next: gli_input_handle_key(keycode_PageDown); break;
	case GDK_Home: gli_input_handle_key(keycode_Home); break;
	case GDK_End: gli_input_handle_key(keycode_End); break;
	case GDK_Left: gli_input_handle_key(keycode_Left); break;
	case GDK_Right: gli_input_handle_key(keycode_Right); break;
	case GDK_Up: gli_input_handle_key(keycode_Up); break;
	case GDK_Down: gli_input_handle_key(keycode_Down); break;
	case GDK_Escape: gli_input_handle_key(keycode_Escape); break;
	case GDK_F1: gli_input_handle_key(keycode_Func1); break;
	case GDK_F2: gli_input_handle_key(keycode_Func2); break;
	case GDK_F3: gli_input_handle_key(keycode_Func3); break;
	case GDK_F4: gli_input_handle_key(keycode_Func4); break;
	case GDK_F5: gli_input_handle_key(keycode_Func5); break;
	case GDK_F6: gli_input_handle_key(keycode_Func6); break;
	case GDK_F7: gli_input_handle_key(keycode_Func7); break;
	case GDK_F8: gli_input_handle_key(keycode_Func8); break;
	case GDK_F9: gli_input_handle_key(keycode_Func9); break;
	case GDK_F10: gli_input_handle_key(keycode_Func10); break;
	case GDK_F11: gli_input_handle_key(keycode_Func11); break;
	case GDK_F12: gli_input_handle_key(keycode_Func12); break;
	default:
		if (key >= 32 && key <= 255)
			gli_input_handle_key(key);
	}
}

static void onquit(GtkWidget *widget, void *data)
{
	/* forced exit by wm */
	exit(0);
}

void wininit(int *argc, char **argv)
{
	gtk_init(argc, &argv);
	gtk_widget_set_default_colormap(gdk_rgb_get_cmap());
	gtk_widget_set_default_visual(gdk_rgb_get_visual());
}

void winopen(void)
{
	int defw;
	int defh;

	defw = gli_wmarginx * 2 + gli_cellw * gli_cols;
	defh = gli_wmarginy * 2 + gli_cellh * gli_rows;

	frame = garglk_plug_new(0);
	GTK_WIDGET_SET_FLAGS(frame, GTK_CAN_FOCUS);
	gtk_widget_set_events(frame, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(frame), "button_press_event", (GCallback)onbutton, NULL);
	g_signal_connect(G_OBJECT(frame), "key_press_event", (GCallback)onkeypress, NULL);
	g_signal_connect(G_OBJECT(frame), "destroy", (GCallback)onquit, "WM destroy");

    canvas = gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(canvas), "size_allocate", (GCallback)onresize, NULL);
	g_signal_connect(G_OBJECT(canvas), "expose_event", (GCallback)onexpose, NULL);
	gtk_container_add(GTK_CONTAINER(frame), canvas);

	wintitle();

	gtk_widget_set_size_request(GTK_WIDGET(frame), defw, defh);

	gtk_widget_show(canvas);
	gtk_widget_show(frame);
    
    GTK_WIDGET_SET_FLAGS(canvas, GTK_CAN_FOCUS);
	gtk_widget_grab_focus(canvas);
}

void wintitle(void)
{
    garglk_plug_send_story_title(GARGLK_PLUG(frame), gli_story_name);
}

void winrepaint(int x0, int y0, int x1, int y1)
{
	/* and pray that gtk+ is smart enough to coalesce... */
	gtk_widget_queue_draw_area(canvas, x0, y0, x1-x0, y1-y0);
}

void gli_select(event_t *event, int block)
{
    gli_curevent = event;
    gli_event_clearevent(event);

    gli_input_guess_focus();

    if (block)
    {
	while (gli_curevent->type == evtype_None && !timeouts)
	    gtk_main_iteration();
    }

    else
    {
	while (gtk_events_pending() && !timeouts)
	    gtk_main_iteration();
    }

    if (gli_curevent->type == evtype_None && timeouts)
    {
	gli_event_store(evtype_Timer, NULL, 0, 0);
	timeouts = 0;
    }

    gli_curevent = NULL;
}
