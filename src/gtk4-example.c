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
  /* A GtkEntry has an internal GtkTextBuffer that manages its text. In order
   * to set a default value in input_entry, we have to manipulate its underlying
   * buffer.
   */
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
  /* A silly place-holder. */
  static gchar valid_password_characters[] = {'a', 'b', 'c', 'd', 'e',
                                              '*', '!', '-', '1', '9'};
  return valid_password_characters[g_random_int_range(
      0, G_N_ELEMENTS(valid_password_characters))];
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
  static const gsize valid_len = 100;
  if (len > valid_len) {
    g_warning("%s: won't generate passwords greater than %lu (got %lu)",
              __PRETTY_FUNCTION__, valid_len, len);
    /* Return an empty string on the heap to be freed by the caller. */
    return g_strdup("");
  }
  /* Use GLib's GString for easy appending. Appending character by character is
   * not very nice, but it gets the job done for now (and it's just slightly
   * less messy to code than g_malloc()-ing a gchar* block of size len).
   */
  GString *password = g_string_new("");
  for (gsize i = 0; i < len; ++i) {
    g_string_append_c(password, random_password_character());
  }
  /* g_string_free() will free the GString object and return the pointer to its
   * internal buffer. Since we also want to return a pointer to this buffer, we
   * pass FALSE to g_string_free() in order to retain it. It's up to the caller
   * of this function to free it. */
  return g_string_free(password, FALSE);
}

/**
 * Handles the "clicked" signal emitted by the button responsible for password
 * generation.
 *
 * @param button The button that emitted the clicked signal.
 * @param user_data Additional data supplied by the caller.
 */
static void on_generate_button_clicked(GtkButton *button, gpointer user_data) {
  g_return_if_fail(input_entry != NULL);
  g_return_if_fail(output_text_view != NULL);

  /* A GtkEntry has an internal GtkTextBuffer that manages its text. In order
   * to get the text currently in input_entry, we have to query its underlying
   * text buffer.
   *
   * g_ascii_strtoll() will return 0 on error (see:
   * https://docs.gtk.org/glib/func.ascii_strtoll.html), which means that no
   * password will be generated if the text from the input field cannot be
   * converted to an integer. This is fine for us.
   */
  gsize num_chars = g_ascii_strtoll(
      gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(input_entry))),
      NULL, 10);
  gchar *password = generate_password(num_chars);
  /* Similar to GtkEntry, a GtkTextView also has an internal GtkTextBuffer that
   * manages its text. In order to set the generated password in the
   * output_text_view, we have to manipulate the underlying text buffer.
   */
  GtkTextBuffer *buffer =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text_view));
  gtk_text_buffer_set_text(buffer, password, -1);
  g_free(password);
}

/**
 * Handles the "clicked" signal emitted by the copy button.
 *
 * @param button The button that emitted the clicked signal.
 * @param user_data Additional data supplied by the caller.
 */
static void on_copy_button_clicked(GtkButton *button, gpointer user_data) {
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
  /* A GtkButton will emit a "clicked" signal when clicked; for a full list of
   * signals that GtkButtons emit, see:
   * https://docs.gtk.org/gtk4/class.Button.html#signals
   *
   * Our handler, on_generate_button_clicked(), should look like this:
   * https://docs.gtk.org/gtk4/signal.Button.clicked.html
   */
  g_signal_connect(generate_button, "clicked",
                   G_CALLBACK(on_generate_button_clicked), NULL);
  GtkWidget *copy_button = gtk_button_new_with_label("COPY");
  /* Connect the "clicked" signal emitted by the copy_button to its handler. */
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
 * @param app The GtkApplication that emitted the "activate" signal.
 * @param user_data Additional data supplied by the caller.
 */
static void activate(GtkApplication *app, gpointer user_data) {
  /* Documentation for functions that create new GtkWidget objects, such as
   * gtk_box_new(), state that the returned data is "owned by the called
   * function". This means that we shouldn't explicitly clean these objects up.
   */
  GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  /* I delegated the creation of input, output, and buttons boxes that are
   * appended to the root_box to separate functions (just a personal
   * preference). They return non-owning pointers to the created GtkBoxes that
   * are then appended.
   */
  GtkWidget *input_box = create_input_box();
  GtkWidget *output_box = create_output_box();
  GtkWidget *buttons_box = create_buttons_box();

  /* Note that root_box, like all widgets above, is of type GtkWidget*, the base
   * class for all Gtk widgets. gtk_box_append() takes a GtkBox* as its first
   * argument, so it's a good idea to downcast root_box to a GtkBox*. GObject
   * types usually have a number of useful macros, including those for casting.
   */
  gtk_box_append(GTK_BOX(root_box), input_box);
  gtk_box_append(GTK_BOX(root_box), output_box);
  gtk_box_append(GTK_BOX(root_box), buttons_box);

  GtkWidget *window = gtk_application_window_new(app);

  /* Similar to root_box, window is downcast to its instance type. */
  gtk_window_set_child(GTK_WINDOW(window), root_box);
  gtk_window_set_title(GTK_WINDOW(window), "GTK4 example");
  static const gint window_width = 480;
  static const gint window_height = 360;
  gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
  gtk_widget_show(window);
}

int main(int argc, char *argv[]) {
  gint status;
  GtkApplication *app =
      gtk_application_new("org.example.gtk4", G_APPLICATION_FLAGS_NONE);
  /* The GtkApplication object inherits the "activate" signal from GApplication:
   * https://docs.gtk.org/gtk4/class.Application.html#signals
   *
   * When the app emits "activate", the callback below is invoked. The signature
   * of the handler is dictated by the signal. In this case, our activate()
   * handler function should look like this:
   * https://docs.gtk.org/gio/signal.Application.activate.html
   *
   * Since g_signal_connect() expects a pointer to a GCallback, we cast our
   * handler to it via the G_CALLBACK macro.
   *
   * We can pass any other data to the callback using the last arguments to
   * g_signal_connect() (we don't need to in this case, so we just pass NULL).
   */
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  /* Run the application (and the main loop). Note that our app, a
   * GtkApplication, is upcasted to its parent, GApplication.
   */
  status = g_application_run(G_APPLICATION(app), argc, argv);

  /* Main loop done, clean up. g_object_unref() decreases the reference count of
   * a GObject instance, which is cleaned up when that count reaches 0. Unless a
   * particular GObject type defines an alternative free function (like GString
   * above), we can always use g_object_unref() to clean GObjects up.
   */
  g_object_unref(app);

  return status;
}
