#define _GNU_SOURCE
#include <sys/stat.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#ifndef CSS_INSTALL_PATH
#define CSS_INSTALL_PATH "style.css"
#endif

#define FILESLIM  1024
#define LINELIM  512
typedef struct {
    char name[LINELIM];     
    char exec[LINELIM]; 
    char icon[LINELIM]; 
    char filename[LINELIM];
    int is_terminal;
}app;
int total_files = 0;
app apps[FILESLIM];


GtkWidget *results_list;
//----------------------------func def-------------------------------------------------
  static void search(GtkWidget *input,gpointer data);
int  fillFiles(const char *path, app *app,const char *fname );
static void extractFiles(const char * folder);
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
static void launch(app *app);
const char* get_terminal();
void scanInit();




void scanInit() {
    total_files = 0;
    extractFiles("/usr/share/applications");

    char user_path[LINELIM];
    const char *home = getenv("HOME");
    if (home) {
        snprintf(user_path, sizeof(user_path), "%s/.local/share/applications", home);
        extractFiles(user_path);
    }


}
//--------------------------------extractFiles----------------------------
static void extractFiles(const char *folder){
	char path[LINELIM];
	DIR* d = opendir(folder);
	struct dirent *dir;
	if(d==NULL){
		    fprintf(stderr, "Error opening dir: %s\n",folder);
		return;
	}
	while ((dir = readdir(d)) != NULL) {
		unsigned char type= dir->d_type;

		if (total_files >= FILESLIM) {
            fprintf(stderr, "Warning: Too many files! Limit reached.\n");
            break; 
        }

		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 ) {
            continue;
        }

			snprintf(path, sizeof(path), "%s/%s", folder, dir->d_name);
// FALLBACK: If type is unknown, we must ask the OS explicitly using stat()
    if (type == DT_UNKNOWN || type == DT_LNK) {
        struct stat st;
			
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) type = DT_DIR;
            else if (S_ISREG(st.st_mode)) type = DT_REG;
        }
    }

		if(type == DT_DIR){
			extractFiles(path);
		}
		else if (type == DT_REG && strcasestr(dir->d_name ,".desktop") != NULL){
				printf("%s\n",dir->d_name);
                    if(fillFiles(path, &apps[total_files], dir->d_name) == 1 ){
		total_files++;
		}
	}
	}
	closedir(d);
}

//---------------------name + icon + display fill -----------------------
int  fillFiles(const char *path, app *app,const char *fname ){
	FILE *fp = fopen(path, "r");
    if (!fp) return 0 ;

	char line[LINELIM];
	strcpy(app->filename , fname);
	app->name[0] = '\0';
        app->exec[0] = '\0';
        app->icon[0] = '\0';
        app->is_terminal = 0;
	while(fgets(line,sizeof(line),fp)){
		line[strcspn(line, "\n")] = 0;
 	if(strncmp(line, "NoDisplay=true", 14)==0){
		return 0;
	}else if (strncmp(line,"Name=",5)==0 && app->name[0] =='\0'){
			strncpy(app->name, line +5,LINELIM-1);
			if (app->name[0] =='\0'){return 0;}
	 }else if (strncmp(line,"Exec=",5)==0 && app->exec[0] =='\0'){
			char *cmd = line + 5;
            
            // CLEANUP: Remove field codes like %u, %F, %U
            // We look for the " %" pattern and cut the string there
            char *args = strstr(cmd, " %");
            if (args) *args = '\0'; 
            
            strncpy(app->exec, cmd, LINELIM - 1);
			
			if (app->exec[0] =='\0'){return 0;}
	 }else if (strncmp(line,"Terminal=true",13)==0){
			app->is_terminal=1;
			
	 }else if (strncmp(line,"Icon=",5)==0 && app->icon[0] =='\0'){
			strncpy(app->icon, line +5,LINELIM-1);
			
	 }
}
return 1;
	}

//------------------search-----------------------------------------------
  static void search(GtkWidget *input,gpointer data){
	const char *text = gtk_entry_get_text(GTK_ENTRY(input));
	GList *children = gtk_container_get_children(GTK_CONTAINER(results_list));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);



    for (int i = 0; i < total_files; i++) {
        char *name = apps[i].name;
        
        if (strlen(text) == 0 || strcasestr(name, text) != NULL) {
            
            GtkWidget *row = gtk_label_new(name);
            gtk_widget_set_halign(row, GTK_ALIGN_START); // align left
            gtk_widget_set_margin_start(row, 10);        // add padding
            gtk_widget_set_margin_top(row, 5);
            gtk_widget_set_margin_bottom(row, 5);

            g_object_set_data(G_OBJECT(row), "app_ptr", &apps[i]);
            gtk_list_box_insert(GTK_LIST_BOX(results_list), row, -1);
        }
    }
	gtk_widget_show_all(results_list);
}

//--------------------selected app for launch-------------------------------
void on_row_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
    
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));

    app *data = (app *)g_object_get_data(G_OBJECT(label), "app_ptr");

    if (data != NULL) {
        launch(data);
    }
}
void on_entry_activate(GtkEntry *entry, gpointer user_data) {
    GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(results_list), 0);
    
    if (row) {
        on_row_activated(GTK_LIST_BOX(results_list), row, NULL);
    }
}

//----------------------------------launch-------------------------------
static void launch(app *app){
GError *error = NULL;
    char command[LINELIM*2];

    if (app->is_terminal) {
		const char * terminal = get_terminal();
        snprintf(command, sizeof(command), "%s -e %s",terminal ,app->exec);
    } else {
        snprintf(command, sizeof(command), "%s", app->exec);
    }

    printf("Launching: %s\n", command); 
    if (!g_spawn_command_line_async(command, &error)) {
        fprintf(stderr, "Failed to launch: %s\n", error->message);
        g_error_free(error);

    } 
        GApplication *application = g_application_get_default();
        g_application_quit(application);
}
//-----------------get terminal (just a helper for launch)----------------
const char* get_terminal() {
    const char *terminals[] = {
        "kitty",
        "alacritty",
        "gnome-terminal",
        "konsole",
        NULL     };

    for (int i = 0; terminals[i] != NULL; i++) {
        // GLib helper: Returns full path if found, NULL if not
        char *path = g_find_program_in_path(terminals[i]);
        
        if (path != NULL) {
            g_free(path);  
	    return terminals[i];
        }
    }
 //fallback
    return "xterm"; 
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
	
			 scanInit();

 //learn: this creates a virtical box with 0 spacing in between items 
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
g_signal_connect(results_list, "row-activated", G_CALLBACK(on_row_activated), NULL);

g_signal_connect(input, "activate", G_CALLBACK(on_entry_activate), NULL);
g_signal_connect(input, "changed", G_CALLBACK(search), NULL);
	search(input, NULL);

GtkCssProvider *provider = gtk_css_provider_new();
GError *error = NULL;
    gtk_css_provider_load_from_path(provider,CSS_INSTALL_PATH, &error);
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


