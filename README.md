# gtk4-example

This is a tiny GTK example that shows a window with two text areas and two buttons.

## Useful documentation

Going through some code examples
in ["Getting Started" from GTK 4.0 documentation](https://docs.gtk.org/gtk4/getting_started.html)
is a valuable starting point for getting familiar with GTK.

For further references, see:

* [GTK 4.0 documentation](https://docs.gtk.org/gtk4/)
* [GLib 2.0 documentation](https://docs.gtk.org/glib/)
* [GDK 4.0 Clipboard documentation](https://docs.gtk.org/gdk4/class.Clipboard.html)

## Building

### Dependencies

* CMake 3.0+ (with pkg-config support)
* [GLib 2.0](https://wiki.gnome.org/Projects/GLib)
* [GTK4](https://www.gtk.org/)

### Compiling all targets

From the root directory of the project, run:

```
mkdir build
cd build
cmake ..
cmake --build .
```

### Running the `gtk4-example` target

The target is found in the `src/` subdirectory of the `build` directory. From the root directory of the project (and
assuming that the build directory is called `build` and resides there), run:

```
cd build
./src/gtk4-example
```
