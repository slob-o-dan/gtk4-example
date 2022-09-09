#include <gtk/gtk.h>

static GtkWidget *input_entry = NULL;
static GtkWidget *output_text_view = NULL;

/**
 * Creates a GtkBox for inputs and its contents, which are an input label and
 * the input_entry GtkEntry.
 *
 * It expects that input_entry is NULL.
 *
 * @return (transfer none) The input box on success, NULL on failure.
 */
static GtkWidget *create_input_box() {
  g_return_val_if_fail(input_entry == NULL, NULL);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *label = gtk_label_new("HOW MANY CHARACTERS?");
  input_entry = gtk_entry_new();
  gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(input_entry)), "5",
                            -1);
  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), input_entry);

  return box;
}

/**
 * Creates a GTKBox for outputs and its contents, which are an output label and
 * the output_text_entry GtkTextEntry.
 *
 * It expects that output_text_entry is NULL.
 *
 * @return (transfer none) The output box on success, NULL on failure.
 */
static GtkWidget *create_output_box() {
  g_return_val_if_fail(output_text_view == NULL, NULL);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *label = gtk_label_new("YOUR RANDOM PASSWORD");
  output_text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(output_text_view), FALSE);

  /* The call to set_size_request below is a rather bad way of displaying more
   * than one row of the GtkTextView. It seems that a width of 0 is ignored.
   * TODO: find out how to do this properly. */
  gtk_widget_set_size_request(output_text_view, 0, 100);

  gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), output_text_view);

  return box;
}

static inline gchar random_password_character() {
  static gchar valid_password_character[] = {'a', 'b', 'c', 'd', 'e',
                                             '*', '!', '-', '1', '9'};
  return valid_password_character[g_random_int_range(
      0, G_N_ELEMENTS(valid_password_character))];
}

/**
 * "Generates" a "random" password of desired length.
 *
 * If the length is greater than some desirable value, an empty string is
 * returned.
 *
 * @param len The desired password length.
 * @return (transfer full) The generated password, to be freed with g_free().
 */
static gchar *generate_password(gsize len) {
  const gsize valid_len = 100;
  if (len > valid_len) {
    g_warning("%s: won't generate passwords greater than %lu (got %lu)",
              __PRETTY_FUNCTION__, valid_len, len);
    return g_strdup("");
  }
  GString *password = g_string_new("");
  for (gsize i = 0; i < len; ++i) {
    g_string_append_c(password, random_password_character());
  }
  return g_string_free(password, FALSE);
}

/**
 * Handles the "clicked" signal emitted by the button responsible for password
 * generation.
 *
 * @param widget The button that emitted the clicked signal.
 * @param user_data Additional data supplied by the caller.
 */
static void on_generate_button_clicked(GtkWidget *widget, gpointer user_data) {
  g_return_if_fail(input_entry != NULL);
  g_return_if_fail(output_text_view != NULL);

  gsize num_chars = g_ascii_strtoll(
      gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(input_entry))),
      NULL, 10);
  gchar *password = generate_password(num_chars);

  GtkTextBuffer *buffer =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text_view));
  gtk_text_buffer_set_text(buffer, password, -1);
  g_free(password);
}

/**
 * Handles the "clicked" signal emitted by the copy button.
 *
 * @param widget The button that emitted the clicked signal.
 * @param user_data Additional data supplied by the caller.
 */
static void on_copy_button_clicked(GtkWidget *widget, gpointer user_data) {
  /*
   * TODO: copy the contents of output_text_view's buffer to the (system)
   * clipboard: https://blog.gtk.org/2020/01/29/data-transfer-in-gtk4/
   */
}

/**
 * Creates a GTKBox for the Generate and Copy buttons.
 *
 * @return (transfer none) The buttons box.
 */
static GtkWidget *create_buttons_box() {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
  GtkWidget *generate_button = gtk_button_new_with_label("GENERATE");
  g_signal_connect(generate_button, "clicked",
                   G_CALLBACK(on_generate_button_clicked), NULL);
  GtkWidget *copy_button = gtk_button_new_with_label("COPY");
  g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked),
                   NULL);
  gtk_box_append(GTK_BOX(box), generate_button);
  gtk_box_append(GTK_BOX(box), copy_button);

  return box;
}

/**
 * Handles the "activate" signal emitted by the GApplication object.
 *
 * Creates the widgets, lays them out, connects the signals, and shows the
 * window.
 *
 * @param app The GApplication that emitted the "activate" signal.
 * @param user_data Additional data supplied by the caller.
 */
static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *input_box = create_input_box();
  GtkWidget *output_box = create_output_box();
  GtkWidget *buttons_box = create_buttons_box();
  gtk_box_append(GTK_BOX(root_box), input_box);
  gtk_box_append(GTK_BOX(root_box), output_box);
  gtk_box_append(GTK_BOX(root_box), buttons_box);

  GtkWidget *window = gtk_application_window_new(app);

  gtk_window_set_child(GTK_WINDOW(window), root_box);
  gtk_window_set_title(GTK_WINDOW(window), "GTK4 example");
  const gint window_width = 480;
  const gint window_height = 360;
  gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
  gtk_widget_show(window);
}

int main(int argc, char *argv[]) {
  gint status;
  GtkApplication *app =
      gtk_application_new("org.example.gtk4", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
