#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    char debug_mode;
    char display_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;

} state;

static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

// Function prototypes
void toggle_debug_mode(state *s);
void set_file_name(state *s);
void set_unit_size(state *s);
void quit(state *s);
void load_into_memory(state *s);
void save_into_file(state *s);
void memory_display(state* s);
void memory_modify(state *s);
void toggle_display_mode(state *s);


typedef struct {
    char *name;
    void (*fun)(state*);
} menu_item;

void toggle_display_mode(state *s) {
    if (s->display_mode) {
        printf("Display flag now off, decimal representation\n");
        s->display_mode = 0; 
    } else {
        printf("Display flag now on, hexadecimal representation\n");
        s->display_mode = 1; 
    }
}

void memory_display(state* s) {
    printf("Enter address and length:\n");
    unsigned int addr, length;
    char input[256];
    int unit_size = s->unit_size;
    fgets(input, sizeof(input), stdin);

    if (sscanf(input, "%x %d", &addr, &length) != 2) {
        printf("Error: Invalid input format.\n");
        return;
    }
    
    printf((s->display_mode == 0) ? "Decimal\n=======\n" : "Hexadecimal\n======\n");
    char* format = (s->display_mode == 0) ? dec_formats[unit_size - 1] : hex_formats[unit_size - 1];
    
    unsigned char* buf1 = (unsigned char*)s->mem_buf;
    unsigned short* buf2 = (unsigned short*)s->mem_buf;
    unsigned int* buf4 = (unsigned int*)s->mem_buf;

    for (unsigned int i = addr; i < addr + length; i++) {
        switch (unit_size) {
            case 1:
                printf(format, buf1[i]);
                break;
            case 2:
                printf(format, buf2[i]);
                break;
            case 4:
                printf(format, buf4[i]);
                break;
        }
    }
}

void save_into_file(state *s) {
    printf("Please enter <source-address> <target-location> <length>\n");
    unsigned int source_address, target_location;
    int length;
    char input[256];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%x %x %d", &source_address, &target_location, &length) != 3) {
        printf("Error: Invalid input format.\n");
        return;
    }

    // Check if file_name is empty
    if (strlen(s->file_name) == 0) {
        printf("Error: File name is not set.\n");
        return;
    }

    // Open file for writing, without truncating
    FILE *file = fopen(s->file_name, "r+b");
    if (file == NULL) {
        printf("Error: Failed to open file.\n");
        return;
    }
    
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    if (target_location >= file_size) {
        printf("Error: Target location exceeds the size of the file.\n");
        fclose(file);
        return;
    }

    fseek(file,0,SEEK_SET);
    // Seek to the specified target location in the file
    if (fseek(file, target_location, SEEK_SET) != 0) {
        printf("Error: Failed to seek to the specified location in the file.\n");
        fclose(file);
        return;
    }
    size_t bytes_written = 0;
    if (source_address){// Write data from memory to the file
        bytes_written = fwrite(&(source_address), s->unit_size, length, file);
    }
    else {
        bytes_written = fwrite(s->mem_buf, s->unit_size, length, file);
    }
    if (bytes_written != length) {
        printf("Error: Failed to write data to file.\n");
        fclose(file);
        return;
    }

    // Close the file
    fclose(file);

    printf("Successfully wrote %zu units into file.\n", bytes_written);
}
void memory_modify(state *s) {
    printf( "Please enter <location> <val> in hexadecimal\n");
    int location,val;
    char input[256];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%x %x", &location, &val) != 2) {
        printf("Error: Invalid input format.\n");
        return;
    }
    if(s->debug_mode == 1)
    {
        printf("Location: %x\nVal: %x\n", location, val);
    }
    memcpy(&s->mem_buf[location], &val, s->unit_size);
}


void load_into_memory(state *s) {
    // Check if file_name is empty
    if (strlen(s->file_name) == 0) {
        printf("Error: File name is not set.\n");
        return;
    }

    // Open file for reading
    FILE *file = fopen(s->file_name, "rb");
    if (file == NULL) {
        printf("Error: Failed to open file for reading.\n");
        return;
    }

    // Prompt the user for location and length
    printf("Please enter location in hexadecimal and length in decimal: ");
    char input[256];
    fgets(input, sizeof(input), stdin);

    // Parse input
    unsigned int location;
    int length;
    if (sscanf(input, "%x %d", &location, &length) != 2) {
        printf("Error: Invalid input format.\n");
        fclose(file);
        return;
    }

    // Print debug info if debug mode is on
    if (s->debug_mode) {
        fprintf(stderr, "file name: %s \n", s->file_name);
        fprintf(stderr, " Location: 0x%x, Length: %d\n", location, length);
    }

    // Seek to the specified location in the file
    if (fseek(file, location, SEEK_SET) != 0) {
        printf("Error: Failed to seek to the specified location in the file.\n");
        fclose(file);
        return;
    }

    // Read data into memory buffer
    size_t bytes_read = fread(s->mem_buf, s->unit_size, length, file);
    if (bytes_read != length) {
        printf("Error: Failed to read data from file.\n");
        fclose(file);
        return;
    }

    // Close the file
    fclose(file);

    // Update memory count
    s->mem_count = bytes_read;

    printf("Loaded %zu units into memory\n", bytes_read);
}


void toggle_debug_mode(state *s) {
    s->debug_mode = !s->debug_mode;
    if (s->debug_mode)
        fprintf(stderr, "Debug flag now on\n");
    else
        fprintf(stderr, "Debug flag now off\n");
}

void set_file_name(state *s) {
    printf("Enter file name: ");
    char inBuf[BUFSIZ];
    fgets(inBuf,sizeof(inBuf),stdin);
    sscanf(inBuf,"%s",s->file_name);
    if (s->debug_mode)
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
}

void set_unit_size(state *s) {
    int size;
    printf("Enter unit size: ");
    char inBuf[BUFSIZ];
    fgets(inBuf,sizeof(inBuf),stdin);
    sscanf(inBuf,"%d", &size);
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode)
            fprintf(stderr, "Debug: set size to %d\n", size);
    } else {
        printf("Invalid unit size. Please enter 1, 2, or 4.\n");
    }
}

void quit(state *s) {
    if (s->debug_mode)
        fprintf(stderr, "Quitting\n");
    exit(0);
}

void menu_loop() {
    state* program_state = (state*)malloc(sizeof(state));
    char input[10];
    // Define menu
    menu_item menu[] = {
        {"Toggle Debug Mode", toggle_debug_mode},
        {"Set File Name", set_file_name},
        {"Set Unit Size", set_unit_size},
        {"Load into memory", load_into_memory},
        {"Toggle Display Mode" , toggle_display_mode },
        {"Memory Display", memory_display},
        {"Save into file", save_into_file},
        {"Memory modify", memory_modify},
        {"Quit", quit},
        {NULL, NULL} // Terminating item
    };
    int bound = sizeof(menu) / (sizeof(menu[0]))-1;
    int opChosen;
    while (1) {

        // Print menu
        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d) %s\n", i, menu[i].name);
        }

       if (fgets(input, sizeof(input), stdin) == NULL) {
            // Exit the loop on EOF
            printf("Exiting- Null in fgets\n");
            exit(1);
        }
        sscanf(input,"%d",&opChosen);
        // Validate choice
        if (opChosen < 0 || opChosen >= bound) {
            printf("Invalid choice. Please choose a number from the menu.\n");
            continue;
        }

        // Execute chosen function
        menu[opChosen].fun(program_state);
    }
    free(program_state);
}

int main(int argc, char **argv)
{
    menu_loop();
    return 0;
}