#include "glib.h"
#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#define FILESLIM  1024
#define FILENAMELIM  256
char files[FILESLIM][FILENAMELIM];
int total_files = 0;
GtkWidget *results_list;
//--------------------------------extractFiles----------------------------
void extractFiles(){
	const char *path = "/usr/share/applications/" ;
	DIR* d = opendir(path);
	struct dirent *dir;
	if(d==NULL){
		    fprintf(stderr, "Error opening dir: %s\n",path);
		return;
	}

	while ((dir = readdir(d)) != NULL) {

		if (total_files >= FILESLIM) {
            fprintf(stderr, "Warning: Too many files! Limit reached.\n");
            break; 
        }
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || strstr(dir->d_name ,".desktop") == NULL) {
            continue;
        }
		

		if(strcmp(strstr(dir->d_name ,".desktop" ),".desktop")==0) {
			strcpy(files[total_files],dir->d_name);
			files[total_files][FILENAMELIM - 1] = '\0';
		total_files++;
		}

	}
	closedir(d);
}

//------------------search------------------------------------
  static void search(GtkWidget *input,gpointer data){
	const char *text = gtk_entry_get_text(GTK_ENTRY(input));
	GList *children = gtk_container_get_children(GTK_CONTAINER(results_list));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);



    for (int i = 0; i < total_files; i++) {
        char *filename = files[i];
        
        if (strlen(text) == 0 || strstr(filename, text) != NULL) {
            
            GtkWidget *row = gtk_label_new(filename);
            gtk_widget_set_halign(row, GTK_ALIGN_START); // align left
            gtk_widget_set_margin_start(row, 10);        // add padding
            gtk_widget_set_margin_top(row, 5);
            gtk_widget_set_margin_bottom(row, 5);

            gtk_list_box_insert(GTK_LIST_BOX(results_list), row, -1);
        }
    }
	gtk_widget_show_all(results_list);
}
//--------------------------quit------------------------------------------
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    // Check if the key pressed is Escape
    if (event->keyval == GDK_KEY_Escape) {
        // Quit the application attached to this window
        GApplication *app = g_application_get_default();
        g_application_quit(app);
        return TRUE;     }

    return FALSE; }
//----------------------------UI/window handeling-------------------------
static void activate (GtkApplication *app,gpointer user_data){
  GtkWidget *window;
  GtkWidget *input;
  GtkWidget *vbox;


  window = gtk_application_window_new (app);
	              //learn:we use GTK_WINDOW(window) for casting window pointer to a GtkWindow 
	//we need this to render the window as a layer on hyprland so it doesnt tille and instead floats and centers it self 
	gtk_layer_init_for_window(GTK_WINDOW(window));

	gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_OVERLAY);
//anchor to nothing to center it 
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, FALSE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, FALSE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, FALSE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, FALSE);
	//sets the keyboard focus on it (to stop this you need to make an exit button)
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE);


  gtk_window_set_title (GTK_WINDOW (window), "peak");
  gtk_window_set_default_size (GTK_WINDOW (window), 1200, 1000);
 //learn: this creates a virtical box with 0 spacing in between items 
	extractFiles();
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(vbox, "main-container");
gtk_container_add(GTK_CONTAINER(window), vbox);
	//learn: user input
  input = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(input), "looking for femboys kido?");	
g_signal_connect(input, "activate", G_CALLBACK(search), window);
gtk_box_pack_start(GTK_BOX(vbox), input, FALSE, TRUE, 5);
//css  
	GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    results_list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled), results_list);
	gtk_widget_set_name(scrolled, "scroll");
	gtk_widget_set_name(results_list, "res");


g_signal_connect(input, "changed", G_CALLBACK(search), NULL);
	search(input, NULL);

GtkCssProvider *provider = gtk_css_provider_new();
GError *error = NULL;
    gtk_css_provider_load_from_path(provider,"style.css", &error);
	if (error) {
        g_warning("Failed to load CSS: %s", error->message);
        g_clear_error(&error);
    }
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);

gtk_widget_show_all(window);
	//learn: renders the whole thing (deleting it still works idk why )
  gtk_window_present (GTK_WINDOW (window));
}
//------------------------main--------------------------------
int main (int    argc, char **argv){
  GtkApplication *app;
  int status;
  char AppId[]= "org.Peak.peakMenu";

  app = gtk_application_new (AppId, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}


