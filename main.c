#include <stdio.h>
#include <stdbool.h>
#include <gtk-3.0/gtk/gtk.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


// if the border of your window manager is getting into the img capture just
// offset it here
// this is for my i3 wm
static const int x_offset_left = 2;
static const int x_offset_right = 0;
static const int y_offset_top = 0;
static const int y_offset_bottom = 0;



GtkWidget* window;
GtkWidget* box;
GtkWidget* top_box;


static const char* css_fp = "style.css";
static bool is_recording = false;


void load_css() {
    // load css to use it for styling
    // gtk uses css for some reason
    
    GtkCssProvider* css_provider;
    css_provider = gtk_css_provider_new();

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),css_provider,GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    //TODO: i should take the error and handle it, instead of NULL there
    gtk_css_provider_load_from_file(css_provider,g_file_new_for_path(css_fp),NULL);
    g_object_unref(css_provider);
}

void on_record(GtkButton* widget ,GtkEntry* entry) {
    if(!is_recording) {
        gtk_button_set_label(widget,"Recording...");
        gtk_widget_set_name(widget, "btn_recording");

    } else {
        gtk_button_set_label(widget,"Record!");
        gtk_widget_set_name(widget, "btn_not_recording");
    }

    is_recording = !is_recording;
    int top_box_height = gtk_widget_get_allocated_height(top_box);

    int w , h;
    w = gtk_widget_get_allocated_width(window) - x_offset_right;
    h = gtk_widget_get_allocated_height(window) - y_offset_bottom - top_box_height;

    int x , y;
    gtk_window_get_position((GtkWindow*)window,&x,&y);
    y += top_box_height + y_offset_top;
    x += x_offset_left;


    GdkPixbuf* buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,true,8,w,h);
    buf = gdk_pixbuf_get_from_window(gdk_get_default_root_window(),x,y,w,h);
    gdk_pixbuf_save(buf,"img.png","png",NULL,NULL);
}
void on_draw(GtkWidget* widget,cairo_t* cr,gpointer* data) {
}

int main(int argc, char** argv) {
    gtk_init(&argc,&argv);

    load_css();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size((GtkWindow*)window,800,600);
    gtk_window_set_resizable((GtkWindow*)window,false);

    GdkScreen *screen;
    GdkVisual *visual;
    gtk_widget_set_app_paintable(window, true);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if(visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(window,visual);
    } else {
        printf("Failed to init, no composition");
        return 0;
    }


    g_signal_connect(GTK_WIDGET(window),"destory",G_CALLBACK(gtk_main_quit),NULL);



    // global container for layout stuff
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);    
    gtk_container_add(GTK_CONTAINER(window),box);

    // top container
    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_widget_set_name(top_box, "top_bar");



    GtkWidget* button = gtk_button_new_with_label("Record!");


    g_signal_connect(GTK_WIDGET(button),"clicked",G_CALLBACK(on_record),NULL);
    gtk_box_pack_start((GtkBox*)top_box,button,false,true,0);



    gtk_box_pack_start((GtkBox*)box,top_box,false,true,0);

    GtkWidget* canvas = gtk_drawing_area_new();
    gtk_widget_set_app_paintable(canvas, true);

    g_signal_connect(GTK_WIDGET(canvas),"draw",G_CALLBACK(on_draw),NULL);
    gtk_box_pack_start((GtkBox*)box,canvas,true,true,0);




    
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}