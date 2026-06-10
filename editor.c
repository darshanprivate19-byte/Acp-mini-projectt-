#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#define WIDTH 60
#define HEIGHT 20
#define MAX_SHAPES 100

// Data structures for geometry
typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    LINE = 1,
    RECTANGLE,
    CIRCLE,
    TRIANGLE
} ShapeType;

typedef struct {
    Point start;
    Point end;
} LineData;

typedef struct {
    Point topLeft;
    int width;
    int height;
} RectData;

typedef struct {
    Point center;
    int radius;
} CircleData;

typedef struct {
    Point p1;
    Point p2;
    Point p3;
} TriangleData;

typedef union {
    LineData line;
    RectData rect;
    CircleData circle;
    TriangleData triangle;
} ShapeData;

typedef struct {
    int id;
    ShapeType type;
    ShapeData data;
    bool active;
} Shape;

// Global State
Shape shapes[MAX_SHAPES];
int next_shape_id = 1;
char canvas[HEIGHT][WIDTH];

// Initialize canvas with '_'
void init_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Safely draw a pixel onto the canvas
void draw_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Helper for drawing 8 symmetric points of a circle
static void plot_circle_points(int xc, int yc, int x, int y) {
    draw_pixel(xc + x, yc + y);
    draw_pixel(xc - x, yc + y);
    draw_pixel(xc + x, yc - y);
    draw_pixel(xc - x, yc - y);
    draw_pixel(xc + y, yc + x);
    draw_pixel(xc - y, yc + x);
    draw_pixel(xc + y, yc - x);
    draw_pixel(xc - y, yc - x);
}

// Midpoint Circle Algorithm
void draw_circle(int xc, int yc, int r) {
    if (r <= 0) {
        draw_pixel(xc, yc);
        return;
    }
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    plot_circle_points(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(xc, yc, x, y);
    }
}

// Wireframe Rectangle Drawing
void draw_rectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    // Top and bottom horizontal edges
    for (int i = 0; i < w; i++) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + h - 1);
    }
    // Left and right vertical edges
    for (int i = 0; i < h; i++) {
        draw_pixel(x, y + i);
        draw_pixel(x + w - 1, y + i);
    }
}

// Triangle Drawing (connecting 3 vertices with lines)
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Redraw canvas with all active shapes
void render_canvas() {
    init_canvas();
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) continue;
        
        switch (shapes[i].type) {
            case LINE:
                draw_line(shapes[i].data.line.start.x, shapes[i].data.line.start.y,
                          shapes[i].data.line.end.x, shapes[i].data.line.end.y);
                break;
            case RECTANGLE:
                draw_rectangle(shapes[i].data.rect.topLeft.x, shapes[i].data.rect.topLeft.y,
                               shapes[i].data.rect.width, shapes[i].data.rect.height);
                break;
            case CIRCLE:
                draw_circle(shapes[i].data.circle.center.x, shapes[i].data.circle.center.y,
                            shapes[i].data.circle.radius);
                break;
            case TRIANGLE:
                draw_triangle(shapes[i].data.triangle.p1.x, shapes[i].data.triangle.p1.y,
                              shapes[i].data.triangle.p2.x, shapes[i].data.triangle.p2.y,
                              shapes[i].data.triangle.p3.x, shapes[i].data.triangle.p3.y);
                break;
        }
    }
}

// Display the 2D grid canvas to the screen
void display_canvas() {
    // Print column header (tens digit)
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n   ");
    // Print column header (ones digit)
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
    
    // Top border
    printf("  +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    
    // Grid rows with line numbers
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d|", y);
        for (int x = 0; x < WIDTH; x++) {
            char c = canvas[y][x];
            if (c == '*') {
                printf("\033[1;33m*\033[0m"); // Bold Yellow
            } else {
                printf("\033[90m_\033[0m");    // Dark Gray
            }
        }
        printf("|\n");
    }
    
    // Bottom border
    printf("  +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
}

// Print information of a single shape
void print_shape_info(int idx) {
    if (idx < 0 || idx >= MAX_SHAPES || !shapes[idx].active) return;
    printf("[%d] ", shapes[idx].id);
    switch (shapes[idx].type) {
        case LINE:
            printf("Line from (%d,%d) to (%d,%d)", 
                   shapes[idx].data.line.start.x, shapes[idx].data.line.start.y,
                   shapes[idx].data.line.end.x, shapes[idx].data.line.end.y);
            break;
        case RECTANGLE:
            printf("Rectangle at (%d,%d), width %d, height %d", 
                   shapes[idx].data.rect.topLeft.x, shapes[idx].data.rect.topLeft.y,
                   shapes[idx].data.rect.width, shapes[idx].data.rect.height);
            break;
        case CIRCLE:
            printf("Circle at (%d,%d), radius %d", 
                   shapes[idx].data.circle.center.x, shapes[idx].data.circle.center.y,
                   shapes[idx].data.circle.radius);
            break;
        case TRIANGLE:
            printf("Triangle with vertices (%d,%d), (%d,%d), (%d,%d)", 
                   shapes[idx].data.triangle.p1.x, shapes[idx].data.triangle.p1.y,
                   shapes[idx].data.triangle.p2.x, shapes[idx].data.triangle.p2.y,
                   shapes[idx].data.triangle.p3.x, shapes[idx].data.triangle.p3.y);
            break;
    }
}

// Count active shapes
int get_active_shapes_count() {
    int count = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) count++;
    }
    return count;
}

// List all active shapes
void list_shapes() {
    int count = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            printf("  ");
            print_shape_info(i);
            printf("\n");
            count++;
        }
    }
    if (count == 0) {
        printf("  No shapes on canvas.\n");
    }
}

// Find local array index of a shape by its ID
int find_shape_index_by_id(int id) {
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active && shapes[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Robust user input reader for integers
int read_int(const char* prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\033[1;31mError reading input. Please try again.\033[0m\n");
            continue;
        }
        // strip newline
        buffer[strcspn(buffer, "\r\n")] = 0;
        char* endptr;
        val = (int)strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf("\033[1;31mInvalid input. Please enter an integer.\033[0m\n");
            continue;
        }
        if (val < min_val || val > max_val) {
            printf("\033[1;31mValue out of range (%d to %d). Please try again.\033[0m\n", min_val, max_val);
            continue;
        }
        return val;
    }
}

// Clear console screen using ANSI codes
void clear_screen() {
    printf("\033[2J\033[H");
}

// Press Enter to continue helper
void press_enter_to_continue() {
    printf("\nPress Enter to continue...");
    char buffer[100];
    fgets(buffer, sizeof(buffer), stdin);
}

// Add shape menu logic
void add_shape_menu() {
    clear_screen();
    printf("\033[1;36m=== ADD NEW SHAPE ===\033[0m\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Back to Main Menu\n");
    int choice = read_int("Select shape type: ", 1, 5);
    
    if (choice == 5) return;
    
    // Find free slot
    int idx = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        printf("\033[1;31mError: Maximum shape limit (%d) reached. Delete some shapes first.\033[0m\n", MAX_SHAPES);
        press_enter_to_continue();
        return;
    }
    
    shapes[idx].id = next_shape_id++;
    shapes[idx].active = true;
    
    if (choice == 1) {
        shapes[idx].type = LINE;
        printf("\nEnter start point coordinates:\n");
        shapes[idx].data.line.start.x = read_int("  Start X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.line.start.y = read_int("  Start Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter end point coordinates:\n");
        shapes[idx].data.line.end.x = read_int("  End X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.line.end.y = read_int("  End Y (0-19): ", 0, HEIGHT - 1);
    } else if (choice == 2) {
        shapes[idx].type = RECTANGLE;
        printf("\nEnter top-left corner coordinates:\n");
        shapes[idx].data.rect.topLeft.x = read_int("  Top-Left X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.rect.topLeft.y = read_int("  Top-Left Y (0-19): ", 0, HEIGHT - 1);
        
        int max_w = WIDTH - shapes[idx].data.rect.topLeft.x;
        int max_h = HEIGHT - shapes[idx].data.rect.topLeft.y;
        
        shapes[idx].data.rect.width = read_int("  Width (1-60): ", 1, max_w);
        shapes[idx].data.rect.height = read_int("  Height (1-20): ", 1, max_h);
    } else if (choice == 3) {
        shapes[idx].type = CIRCLE;
        printf("\nEnter circle center coordinates:\n");
        shapes[idx].data.circle.center.x = read_int("  Center X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.circle.center.y = read_int("  Center Y (0-19): ", 0, HEIGHT - 1);
        shapes[idx].data.circle.radius = read_int("  Radius (1-40): ", 1, 40);
    } else if (choice == 4) {
        shapes[idx].type = TRIANGLE;
        printf("\nEnter first vertex coordinates:\n");
        shapes[idx].data.triangle.p1.x = read_int("  P1 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p1.y = read_int("  P1 Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter second vertex coordinates:\n");
        shapes[idx].data.triangle.p2.x = read_int("  P2 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p2.y = read_int("  P2 Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter third vertex coordinates:\n");
        shapes[idx].data.triangle.p3.x = read_int("  P3 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p3.y = read_int("  P3 Y (0-19): ", 0, HEIGHT - 1);
    }
    
    printf("\n\033[1;32mShape added successfully!\033[0m\n");
    press_enter_to_continue();
}

// Delete shape menu logic
void delete_shape_menu() {
    clear_screen();
    printf("\033[1;36m=== DELETE SHAPE ===\033[0m\n");
    list_shapes();
    
    if (get_active_shapes_count() == 0) {
        press_enter_to_continue();
        return;
    }
    
    int id = read_int("\nEnter the ID of the shape to delete (0 to cancel): ", 0, next_shape_id - 1);
    if (id == 0) return;
    
    int idx = find_shape_index_by_id(id);
    if (idx == -1) {
        printf("\033[1;31mShape ID not found.\033[0m\n");
        press_enter_to_continue();
        return;
    }
    
    shapes[idx].active = false;
    printf("\n\033[1;32mShape [%d] deleted successfully!\033[0m\n", id);
    press_enter_to_continue();
}

// Modify shape menu logic
void modify_shape_menu() {
    clear_screen();
    printf("\033[1;36m=== MODIFY SHAPE ===\033[0m\n");
    list_shapes();
    
    if (get_active_shapes_count() == 0) {
        press_enter_to_continue();
        return;
    }
    
    int id = read_int("\nEnter the ID of the shape to modify (0 to cancel): ", 0, next_shape_id - 1);
    if (id == 0) return;
    
    int idx = find_shape_index_by_id(id);
    if (idx == -1) {
        printf("\033[1;31mShape ID not found.\033[0m\n");
        press_enter_to_continue();
        return;
    }
    
    printf("\nCurrently modifying: ");
    print_shape_info(idx);
    printf("\n\n");
    
    if (shapes[idx].type == LINE) {
        printf("Enter new start point coordinates:\n");
        shapes[idx].data.line.start.x = read_int("  Start X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.line.start.y = read_int("  Start Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter new end point coordinates:\n");
        shapes[idx].data.line.end.x = read_int("  End X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.line.end.y = read_int("  End Y (0-19): ", 0, HEIGHT - 1);
    } else if (shapes[idx].type == RECTANGLE) {
        printf("Enter new top-left corner coordinates:\n");
        shapes[idx].data.rect.topLeft.x = read_int("  Top-Left X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.rect.topLeft.y = read_int("  Top-Left Y (0-19): ", 0, HEIGHT - 1);
        
        int max_w = WIDTH - shapes[idx].data.rect.topLeft.x;
        int max_h = HEIGHT - shapes[idx].data.rect.topLeft.y;
        
        shapes[idx].data.rect.width = read_int("  Width (1-60): ", 1, max_w);
        shapes[idx].data.rect.height = read_int("  Height (1-20): ", 1, max_h);
    } else if (shapes[idx].type == CIRCLE) {
        printf("Enter new circle center coordinates:\n");
        shapes[idx].data.circle.center.x = read_int("  Center X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.circle.center.y = read_int("  Center Y (0-19): ", 0, HEIGHT - 1);
        shapes[idx].data.circle.radius = read_int("  Radius (1-40): ", 1, 40);
    } else if (shapes[idx].type == TRIANGLE) {
        printf("Enter new first vertex coordinates:\n");
        shapes[idx].data.triangle.p1.x = read_int("  P1 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p1.y = read_int("  P1 Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter new second vertex coordinates:\n");
        shapes[idx].data.triangle.p2.x = read_int("  P2 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p2.y = read_int("  P2 Y (0-19): ", 0, HEIGHT - 1);
        printf("\nEnter new third vertex coordinates:\n");
        shapes[idx].data.triangle.p3.x = read_int("  P3 X (0-59): ", 0, WIDTH - 1);
        shapes[idx].data.triangle.p3.y = read_int("  P3 Y (0-19): ", 0, HEIGHT - 1);
    }
    
    printf("\n\033[1;32mShape [%d] modified successfully!\033[0m\n", id);
    press_enter_to_continue();
}

// Clear all shapes menu logic
void clear_shapes_menu() {
    clear_screen();
    printf("\033[1;36m=== CLEAR ALL SHAPES ===\033[0m\n");
    printf("Are you sure you want to delete all shapes? (1 = Yes, 2 = No): ");
    int confirm = read_int("", 1, 2);
    if (confirm == 1) {
        for (int i = 0; i < MAX_SHAPES; i++) {
            shapes[i].active = false;
        }
        next_shape_id = 1;
        printf("\n\033[1;32mAll shapes cleared!\033[0m\n");
    } else {
        printf("\nOperation cancelled.\n");
    }
    press_enter_to_continue();
}

int main() {
    #ifdef _WIN32
    // Enable ANSI escape sequences on Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    #endif

    // Initialize shapes array
    for (int i = 0; i < MAX_SHAPES; i++) {
        shapes[i].active = false;
    }

    while (1) {
        render_canvas();
        clear_screen();
        
        printf("\033[1;36m==============================================================\n");
        printf("                 2D GRAPHICS EDITOR IN C\n");
        printf("==============================================================\033[0m\n\n");
        
        display_canvas();
        
        printf("\n\033[1;32mActive Shapes:\033[0m\n");
        list_shapes();
        printf("\n\033[1;36mMenu Options:\033[0m\n");
        printf("  1. Add Shape\n");
        printf("  2. Delete Shape\n");
        printf("  3. Modify Shape\n");
        printf("  4. Clear Canvas\n");
        printf("  5. Exit\n");
        
        int choice = read_int("\nSelect an option (1-5): ", 1, 5);
        
        if (choice == 1) {
            add_shape_menu();
        } else if (choice == 2) {
            delete_shape_menu();
        } else if (choice == 3) {
            modify_shape_menu();
        } else if (choice == 4) {
            clear_shapes_menu();
        } else if (choice == 5) {
            clear_screen();
            printf("Exiting 2D Graphics Editor. Goodbye!\n");
            break;
        }
    }
    return 0;
}
