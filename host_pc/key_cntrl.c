#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/uinput.h>

void press_up(int delay);
void press_down(int delay);
void press_right(int delay);
void press_left(int delay);

#define DELAY 100000

void emit_event(int fd, int type, int code, int value) {
  struct input_event ie;
  ie.type = type;
  ie.code = code;
  ie.value = value;
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;
  write(fd, &ie, sizeof(ie));
}

int fd;

int setup_key_emulation() {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fd < 0) {
    perror("Error opening /dev/uinput");
    return 1;
  }

  // Enable key events and directional keys
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_EVBIT, EV_SYN);
  ioctl(fd, UI_SET_KEYBIT, KEY_UP);
  ioctl(fd, UI_SET_KEYBIT, KEY_DOWN);
  ioctl(fd, UI_SET_KEYBIT, KEY_LEFT);
  ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT);

  // Configure the virtual device
  struct uinput_setup usetup;
  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1234;  // Arbitrary vendor ID
  usetup.id.product = 0x5678; // Arbitrary product ID
  strcpy(usetup.name, "Virtual Directional Keyboard");

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);

  // Simulate directional key presses
  /* sleep(1); */
  // Clean up

  /*
  ioctl(fd, UI_DEV_DESTROY);
  close(fd);
   */
  return 0;
}

void press_left(int delay) {
  // Press and release LEFT arrow key
  emit_event(fd, EV_KEY, KEY_LEFT, 1); // Press LEFT
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
  usleep(delay);
  emit_event(fd, EV_KEY, KEY_LEFT, 0); // Release LEFT
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

void press_right(int delay) {
  // Press and release RIGHT arrow key
  emit_event(fd, EV_KEY, KEY_RIGHT, 1); // Press RIGHT
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
  usleep(delay);
  emit_event(fd, EV_KEY, KEY_RIGHT, 0); // Release RIGHT
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

void press_up(int delay) {
  // Press and release UP arrow key
  emit_event(fd, EV_KEY, KEY_UP, 1); // Press UP
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
  usleep(delay);                    // 100ms delay
  emit_event(fd, EV_KEY, KEY_UP, 0); // Release UP
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

void press_down(int delay) {
  // Press and release DOWN arrow key
  emit_event(fd, EV_KEY, KEY_DOWN, 1); // Press DOWN
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
  usleep(delay);                      // 100ms delay
  emit_event(fd, EV_KEY, KEY_DOWN, 0); // Release DOWN
  emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

void sample_emu() {

  press_up(DELAY);
  sleep(5);
  press_up(DELAY);
  sleep(5);
  press_left(DELAY);
  sleep(5);
  press_right(DELAY);
  sleep(5);
}
