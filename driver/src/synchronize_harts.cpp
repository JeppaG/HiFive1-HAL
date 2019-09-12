/*
 * _synchronize_harts() is called by crt0.S to cause harts > 0 to wait for
 * hart 0 to finish copying the datat section, zeroing the BSS, and running
 * the libc contstructors. In HiFive1 nothing needs to be done.
 */
extern "C" void _synchronize_harts();

void _synchronize_harts() {
}
