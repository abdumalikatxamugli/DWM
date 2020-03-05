#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
static void getCSS(){

	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;

	const gchar * css_style_file="style.css";
	GFile *css_fp=g_file_new_for_path(css_style_file);
	GError *error=0;


	provider=gtk_css_provider_new();
	display=gdk_display_get_default();
	screen=gdk_display_get_default_screen(display);

	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_css_provider_load_from_file(provider, css_fp, &error);

	g_object_unref(provider);	

}

GtkBuilder *builder;
GtkBuilder *regGlade;
GtkWidget *window;
GtkWidget *createProjectWindow;
GtkWidget *label1;
GtkButton *btn1;

void on_regbutton_clicked(GtkButton *b,GtkWidget *w)
{
	printf("%s\n","hello world" );
	char *ck= "clicked";
	gtk_label_set_text (GTK_LABEL(label1),  ck);
	gtk_widget_hide(w);
	gtk_widget_show_all(createProjectWindow);
}	
int main(int argc, char *argv[]){
	
	gtk_init(NULL,NULL);

	getCSS();
	
	builder=gtk_builder_new_from_file("templates/regOnly.glade");

	regGlade=gtk_builder_new_from_file("templates/register.glade");

	window=GTK_WIDGET(gtk_builder_get_object(builder,"regWindow"));	

	createProjectWindow=GTK_WIDGET(gtk_builder_get_object(regGlade,"createProject"));	

	label1=GTK_WIDGET(gtk_builder_get_object(builder,"label1"));

	gtk_builder_connect_signals(builder, NULL);

	btn1=GTK_BUTTON(gtk_builder_get_object(builder,"regbutton"));

	g_signal_connect(window, "delete_event", gtk_main_quit, NULL);

	// g_signal_connect(btn1, "clicked", on_regbutton_clicked, NULL);

	

	gtk_widget_show_all(window);
	
	gtk_main();

	return 0;
}

