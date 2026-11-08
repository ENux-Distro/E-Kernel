#include <stdint.h> // Can work with freestanding

#define NULL 0

volatile unsigned short *vga = (volatile unsigned short *)0xB8000;

int cursor_x = 0;
int cursor_y = 0;

void printvga(const char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];

        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }

        if (c == '\b') {
            if (cursor_x > 0) {
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = 79;
            }

            vga[cursor_y * 80 + cursor_x] = (0x07 << 8) | ' ';
            continue;
        }

        int position = cursor_y * 80 + cursor_x;

        vga[position] = (0x07 << 8) | c;

        cursor_x++;

        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
        }
    }
}

void printvgaln(const char *message) {
    printvga(message);
    printvga("\n");
}

void clear() {
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = (0x07 << 8) | ' ';
    }

    cursor_x = 0;
    cursor_y = 0;
}

uint8_t inb(uint16_t port) {
    uint8_t value;

    asm volatile(
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

void outb(uint16_t port, uint8_t value) {
    asm volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

bool keyboardinit() {
    int timeout = 100000;

    while (inb(0x64) & 0x02) {
        if (--timeout <= 0)
            return false;
    }

    outb(0x60, 0xF4);

    timeout = 100000;

    while (timeout-- > 0) {
        if (inb(0x64) & 0x01) {
            int response = inb(0x60);
            return response == 0xFA;
        }
    }

    return false;
}

int getchar_scancode() {
    while (!(inb(0x64) & 0x01));

    return inb(0x60);
}

const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0,  'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,  '*',
    0,  ' '
};

const char scancode_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', 0,  'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0,  '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,  '*',
    0,  ' '
};

bool shift_pressed = false;

char getchar() {
    for (;;) {
        uint8_t scancode = getchar_scancode();

        if (scancode == 0xE0)
            continue;

        bool released = scancode & 0x80;
        uint8_t key = scancode & 0x7F;

        if (key == 0x2A || key == 0x36) {
            shift_pressed = !released;
            continue;
        }

        if (released)
            continue;

        char c = shift_pressed
            ? scancode_shift_map[key]
            : scancode_map[key];

        if (c != 0)
            return c;
    }
}

int read(char* buff, int count) {
    if (count <= 0)
        return 0;

    int length = 0;

    for (;;) {
        char c = getchar();

        if (c == '\n') {
            printvgaln("");
            break;
        }

        if (c == '\b') {
            if (length > 0) {
                length--;
                printvga("\b");
            }

            continue;
        }

        if (length < count - 1) {
            buff[length++] = c;

            char output[2] = {c, '\0'};
            printvga(output);
        }
    }

    buff[length] = '\0';

    return length;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

extern "C" void kernel_main() {

    clear();
    printvgaln("E-Kernel");
    printvgaln("It won't be professional like GNU\n - Linus Torvalds");

    printvga("Initializing keyboard ");
    if (keyboardinit()) {
        printvgaln("[ OK ]");
    } else {
        printvgaln("[ Failed ]");
    }

    char buff[1024];

    for (;;) {

        printvga("> ");
        int count = read(buff, sizeof(buff));
        if (count == NULL) continue;
        if (strcmp(buff, "whoami") == 0) {
            printvgaln("Someone special <3");
        } else if (strcmp(buff, "arch") == 0) {
            printvgaln("i386");
        } else if (strcmp(buff, "print") == 0) {
            char print_buff[1024];
            printvga("Enter the text you want to print> ");
            read(print_buff, sizeof(print_buff));
            printvgaln(print_buff);
        } else if (strcmp(buff, "clear") == 0) {
            clear();
        } else if (strcmp(buff, "exit") == 0) {
            printvgaln("Halting CPU");
            __asm(
                "cli\n"
                "hlt"
            );
        }
    }
}
