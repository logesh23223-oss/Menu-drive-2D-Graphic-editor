#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define global variables
char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;

// Clear canvas with underscores
void clear_canvas(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Plot a point if inside canvas bounds
void plot_point(int x, int y) {
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
        plot_point(x1, y1);
        if (x1 == x2 && y1 == y2) {
            break;
        }
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

// Rectangle Drawing Algorithm (outline)
void draw_rectangle(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    // Draw horizontal lines (top and bottom)
    for (int i = 0; i < width; i++) {
        plot_point(x + i, y);
        plot_point(x + i, y + height - 1);
    }
    // Draw vertical lines (left and right)
    for (int i = 0; i < height; i++) {
        plot_point(x, y + i);
        plot_point(x + width - 1, y + i);
    }
}

// Triangle Drawing Algorithm (three line segments)
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Midpoint Circle Algorithm (Bresenham's Circle)
void draw_circle(int cx, int cy, int r) {
    if (r < 0) {
        return;
    }
    if (r == 0) {
        plot_point(cx, cy);
        return;
    }
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        // Draw 8 symmetric octants
        plot_point(cx + x, cy + y);
        plot_point(cx - x, cy + y);
        plot_point(cx + x, cy - y);
        plot_point(cx - x, cy - y);
        plot_point(cx + y, cy + x);
        plot_point(cx - y, cy + x);
        plot_point(cx + y, cy - x);
        plot_point(cx - y, cy - x);

        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Re-render the canvas by drawing all active shapes
void render_canvas(void) {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].active) {
            switch (shapes[i].type) {
                case SHAPE_LINE:
                    draw_line(shapes[i].params.line.x1, shapes[i].params.line.y1,
                              shapes[i].params.line.x2, shapes[i].params.line.y2);
                    break;
                case SHAPE_RECTANGLE:
                    draw_rectangle(shapes[i].params.rect.x, shapes[i].params.rect.y,
                                   shapes[i].params.rect.width, shapes[i].params.rect.height);
                    break;
                case SHAPE_TRIANGLE:
                    draw_triangle(shapes[i].params.tri.x1, shapes[i].params.tri.y1,
                                  shapes[i].params.tri.x2, shapes[i].params.tri.y2,
                                  shapes[i].params.tri.x3, shapes[i].params.tri.y3);
                    break;
                case SHAPE_CIRCLE:
                    draw_circle(shapes[i].params.circle.cx, shapes[i].params.circle.cy,
                                shapes[i].params.circle.r);
                    break;
            }
        }
    }
}

// Display canvas with premium column labels, ticks, and row borders
void display_canvas(void) {
    // Print column markers
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 5 == 0) {
            printf("%-5d", x);
            x += 4;
        }
    }
    printf("\n");

    // Print column ticks
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 5 == 0) {
            printf("|");
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print top border
    printf("  +");
    for (int x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n");

    // Print canvas rows with numbers on left
    for (int y = 0; y < HEIGHT; y++) {
        printf("%02d|", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Print bottom border
    printf("  +");
    for (int x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n");
}

// Shape list operations: Add Line
int add_line(int x1, int y1, int x2, int y2) {
    if (shape_count >= MAX_SHAPES) {
        return -1;
    }
    Shape s;
    s.id = next_id++;
    s.type = SHAPE_LINE;
    s.params.line.x1 = x1;
    s.params.line.y1 = y1;
    s.params.line.x2 = x2;
    s.params.line.y2 = y2;
    s.active = 1;
    shapes[shape_count++] = s;
    return s.id;
}

// Shape list operations: Add Rectangle
int add_rectangle(int x, int y, int width, int height) {
    if (shape_count >= MAX_SHAPES) {
        return -1;
    }
    Shape s;
    s.id = next_id++;
    s.type = SHAPE_RECTANGLE;
    s.params.rect.x = x;
    s.params.rect.y = y;
    s.params.rect.width = width;
    s.params.rect.height = height;
    s.active = 1;
    shapes[shape_count++] = s;
    return s.id;
}

// Shape list operations: Add Triangle
int add_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    if (shape_count >= MAX_SHAPES) {
        return -1;
    }
    Shape s;
    s.id = next_id++;
    s.type = SHAPE_TRIANGLE;
    s.params.tri.x1 = x1;
    s.params.tri.y1 = y1;
    s.params.tri.x2 = x2;
    s.params.tri.y2 = y2;
    s.params.tri.x3 = x3;
    s.params.tri.y3 = y3;
    s.active = 1;
    shapes[shape_count++] = s;
    return s.id;
}

// Shape list operations: Add Circle
int add_circle(int cx, int cy, int r) {
    if (shape_count >= MAX_SHAPES) {
        return -1;
    }
    Shape s;
    s.id = next_id++;
    s.type = SHAPE_CIRCLE;
    s.params.circle.cx = cx;
    s.params.circle.cy = cy;
    s.params.circle.r = r;
    s.active = 1;
    shapes[shape_count++] = s;
    return s.id;
}

// Shape lookup
Shape* find_shape(int id) {
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id && shapes[i].active) {
            return &shapes[i];
        }
    }
    return NULL;
}

// Shape list operations: Delete
int delete_shape(int id) {
    Shape* s = find_shape(id);
    if (s != NULL) {
        s->active = 0;
        return 1;
    }
    return 0;
}

// Shape list operations: Modify Line
int modify_line(int id, int x1, int y1, int x2, int y2) {
    Shape* s = find_shape(id);
    if (s != NULL && s->type == SHAPE_LINE) {
        s->params.line.x1 = x1;
        s->params.line.y1 = y1;
        s->params.line.x2 = x2;
        s->params.line.y2 = y2;
        return 1;
    }
    return 0;
}

// Shape list operations: Modify Rectangle
int modify_rectangle(int id, int x, int y, int width, int height) {
    Shape* s = find_shape(id);
    if (s != NULL && s->type == SHAPE_RECTANGLE) {
        s->params.rect.x = x;
        s->params.rect.y = y;
        s->params.rect.width = width;
        s->params.rect.height = height;
        return 1;
    }
    return 0;
}

// Shape list operations: Modify Triangle
int modify_triangle(int id, int x1, int y1, int x2, int y2, int x3, int y3) {
    Shape* s = find_shape(id);
    if (s != NULL && s->type == SHAPE_TRIANGLE) {
        s->params.tri.x1 = x1;
        s->params.tri.y1 = y1;
        s->params.tri.x2 = x2;
        s->params.tri.y2 = y2;
        s->params.tri.x3 = x3;
        s->params.tri.y3 = y3;
        return 1;
    }
    return 0;
}

// Shape list operations: Modify Circle
int modify_circle(int id, int cx, int cy, int r) {
    Shape* s = find_shape(id);
    if (s != NULL && s->type == SHAPE_CIRCLE) {
        s->params.circle.cx = cx;
        s->params.circle.cy = cy;
        s->params.circle.r = r;
        return 1;
    }
    return 0;
}
