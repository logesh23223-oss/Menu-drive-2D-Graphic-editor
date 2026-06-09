#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cross-platform clear screen
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
    fflush(stdout);
#endif
}

// Robust integer reader to avoid scanf problems
int read_int(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[128];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        // Remove trailing newline character
        buffer[strcspn(buffer, "\n")] = 0;

        // Parse integer
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf("Error: Invalid input. Please enter a valid integer.\n");
            continue;
        }
        if (parsed < min_val || parsed > max_val) {
            printf("Error: Value out of range (%d to %d). Please try again.\n", min_val, max_val);
            continue;
        }
        val = (int)parsed;
        break;
    }
    return val;
}

// Display current shape settings in detail
void list_shapes_summary(void) {
    int active_found = 0;
    printf("\n==================== Active Shapes ====================\n");
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].active) {
            active_found = 1;
            printf("  [ID: %02d] ", shapes[i].id);
            switch (shapes[i].type) {
                case SHAPE_LINE:
                    printf("Line: Point1(%d, %d) -> Point2(%d, %d)\n",
                           shapes[i].params.line.x1, shapes[i].params.line.y1,
                           shapes[i].params.line.x2, shapes[i].params.line.y2);
                    break;
                case SHAPE_RECTANGLE:
                    printf("Rectangle: Top-Left(%d, %d), Width: %d, Height: %d\n",
                           shapes[i].params.rect.x, shapes[i].params.rect.y,
                           shapes[i].params.rect.width, shapes[i].params.rect.height);
                    break;
                case SHAPE_TRIANGLE:
                    printf("Triangle: Vertices A(%d, %d), B(%d, %d), C(%d, %d)\n",
                           shapes[i].params.tri.x1, shapes[i].params.tri.y1,
                           shapes[i].params.tri.x2, shapes[i].params.tri.y2,
                           shapes[i].params.tri.x3, shapes[i].params.tri.y3);
                    break;
                case SHAPE_CIRCLE:
                    printf("Circle: Center(%d, %d), Radius: %d\n",
                           shapes[i].params.circle.cx, shapes[i].params.circle.cy,
                           shapes[i].params.circle.r);
                    break;
            }
        }
    }
    if (!active_found) {
        printf("  (No shapes are currently drawn on the canvas)\n");
    }
    printf("=======================================================\n");
}

int main(void) {
    clear_canvas();
    char message[256] = "Welcome to the 2D Graphic Editor! Select an option below.";

    while (1) {
        clear_screen();
        printf("--- 2D GRAPHIC EDITOR IN C ---\n");
        printf("Canvas Dimension: %d x %d | Current Shapes: %d/%d\n\n", WIDTH, HEIGHT, shape_count, MAX_SHAPES);
        
        render_canvas();
        display_canvas();
        list_shapes_summary();
        
        if (strlen(message) > 0) {
            printf("\nStatus: %s\n", message);
            strcpy(message, ""); // Clear message after showing
        }

        printf("\nMenu Actions:\n");
        printf("1. Add a Shape\n");
        printf("2. Delete a Shape\n");
        printf("3. Modify a Shape\n");
        printf("4. Clear Canvas (Delete All)\n");
        printf("5. Exit Program\n");

        int choice = read_int("Select action (1-5): ", 1, 5);

        if (choice == 5) {
            printf("\nExiting editor. Goodbye!\n");
            break;
        }

        switch (choice) {
            case 1: { // Add Shape
                printf("\nSelect Shape Type:\n");
                printf("1. Line\n");
                printf("2. Rectangle\n");
                printf("3. Triangle\n");
                printf("4. Circle\n");
                int type_choice = read_int("Select type (1-4): ", 1, 4);

                if (type_choice == 1) {
                    int x1 = read_int("Enter x1 (0-79): ", 0, WIDTH - 1);
                    int y1 = read_int("Enter y1 (0-23): ", 0, HEIGHT - 1);
                    int x2 = read_int("Enter x2 (0-79): ", 0, WIDTH - 1);
                    int y2 = read_int("Enter y2 (0-23): ", 0, HEIGHT - 1);
                    int id = add_line(x1, y1, x2, y2);
                    if (id != -1) {
                        sprintf(message, "Successfully added Line with ID %d.", id);
                    } else {
                        sprintf(message, "Error: Canvas reached max shape limit.");
                    }
                } 
                else if (type_choice == 2) {
                    int x = read_int("Enter top-left x (0-79): ", 0, WIDTH - 1);
                    int y = read_int("Enter top-left y (0-23): ", 0, HEIGHT - 1);
                    int w = read_int("Enter width (1-80): ", 1, WIDTH);
                    int h = read_int("Enter height (1-24): ", 1, HEIGHT);
                    int id = add_rectangle(x, y, w, h);
                    if (id != -1) {
                        sprintf(message, "Successfully added Rectangle with ID %d.", id);
                    } else {
                        sprintf(message, "Error: Canvas reached max shape limit.");
                    }
                } 
                else if (type_choice == 3) {
                    int x1 = read_int("Enter x1 (0-79): ", 0, WIDTH - 1);
                    int y1 = read_int("Enter y1 (0-23): ", 0, HEIGHT - 1);
                    int x2 = read_int("Enter x2 (0-79): ", 0, WIDTH - 1);
                    int y2 = read_int("Enter y2 (0-23): ", 0, HEIGHT - 1);
                    int x3 = read_int("Enter x3 (0-79): ", 0, WIDTH - 1);
                    int y3 = read_int("Enter y3 (0-23): ", 0, HEIGHT - 1);
                    int id = add_triangle(x1, y1, x2, y2, x3, y3);
                    if (id != -1) {
                        sprintf(message, "Successfully added Triangle with ID %d.", id);
                    } else {
                        sprintf(message, "Error: Canvas reached max shape limit.");
                    }
                } 
                else if (type_choice == 4) {
                    int cx = read_int("Enter center x (0-79): ", 0, WIDTH - 1);
                    int cy = read_int("Enter center y (0-23): ", 0, HEIGHT - 1);
                    int r = read_int("Enter radius (0-80): ", 0, 80);
                    int id = add_circle(cx, cy, r);
                    if (id != -1) {
                        sprintf(message, "Successfully added Circle with ID %d.", id);
                    } else {
                        sprintf(message, "Error: Canvas reached max shape limit.");
                    }
                }
                break;
            }

            case 2: { // Delete Shape
                // Check if any shape is active
                int has_shapes = 0;
                for (int i = 0; i < shape_count; i++) {
                    if (shapes[i].active) has_shapes = 1;
                }
                if (!has_shapes) {
                    sprintf(message, "Error: No shapes to delete.");
                    break;
                }

                int target_id = read_int("Enter Shape ID to delete: ", 1, next_id - 1);
                if (delete_shape(target_id)) {
                    sprintf(message, "Successfully deleted Shape ID %d.", target_id);
                } else {
                    sprintf(message, "Error: Shape ID %d not found or already deleted.", target_id);
                }
                break;
            }

            case 3: { // Modify Shape
                // Check if any shape is active
                int has_shapes = 0;
                for (int i = 0; i < shape_count; i++) {
                    if (shapes[i].active) has_shapes = 1;
                }
                if (!has_shapes) {
                    sprintf(message, "Error: No shapes to modify.");
                    break;
                }

                int target_id = read_int("Enter Shape ID to modify: ", 1, next_id - 1);
                Shape *s = find_shape(target_id);
                if (s == NULL) {
                    sprintf(message, "Error: Shape ID %d not found.", target_id);
                    break;
                }

                printf("\nModifying Shape ID %d (Type: ", target_id);
                switch (s->type) {
                    case SHAPE_LINE:
                        printf("Line)\n");
                        int lx1 = read_int("Enter new x1 (0-79): ", 0, WIDTH - 1);
                        int ly1 = read_int("Enter new y1 (0-23): ", 0, HEIGHT - 1);
                        int lx2 = read_int("Enter new x2 (0-79): ", 0, WIDTH - 1);
                        int ly2 = read_int("Enter new y2 (0-23): ", 0, HEIGHT - 1);
                        modify_line(target_id, lx1, ly1, lx2, ly2);
                        sprintf(message, "Successfully modified Line ID %d.", target_id);
                        break;
                    case SHAPE_RECTANGLE:
                        printf("Rectangle)\n");
                        int rx = read_int("Enter new top-left x (0-79): ", 0, WIDTH - 1);
                        int ry = read_int("Enter new top-left y (0-23): ", 0, HEIGHT - 1);
                        int rw = read_int("Enter new width (1-80): ", 1, WIDTH);
                        int rh = read_int("Enter new height (1-24): ", 1, HEIGHT);
                        modify_rectangle(target_id, rx, ry, rw, rh);
                        sprintf(message, "Successfully modified Rectangle ID %d.", target_id);
                        break;
                    case SHAPE_TRIANGLE:
                        printf("Triangle)\n");
                        int tx1 = read_int("Enter new x1 (0-79): ", 0, WIDTH - 1);
                        int ty1 = read_int("Enter new y1 (0-23): ", 0, HEIGHT - 1);
                        int tx2 = read_int("Enter new x2 (0-79): ", 0, WIDTH - 1);
                        int ty2 = read_int("Enter new y2 (0-23): ", 0, HEIGHT - 1);
                        int tx3 = read_int("Enter new x3 (0-79): ", 0, WIDTH - 1);
                        int ty3 = read_int("Enter new y3 (0-23): ", 0, HEIGHT - 1);
                        modify_triangle(target_id, tx1, ty1, tx2, ty2, tx3, ty3);
                        sprintf(message, "Successfully modified Triangle ID %d.", target_id);
                        break;
                    case SHAPE_CIRCLE:
                        printf("Circle)\n");
                        int ccx = read_int("Enter new center x (0-79): ", 0, WIDTH - 1);
                        int ccy = read_int("Enter new center y (0-23): ", 0, HEIGHT - 1);
                        int cr = read_int("Enter new radius (0-80): ", 0, 80);
                        modify_circle(target_id, ccx, ccy, cr);
                        sprintf(message, "Successfully modified Circle ID %d.", target_id);
                        break;
                }
                break;
            }

            case 4: { // Clear Canvas
                int confirmed = read_int("Are you sure you want to clear all shapes? (1 for Yes, 0 for No): ", 0, 1);
                if (confirmed == 1) {
                    for (int i = 0; i < shape_count; i++) {
                        shapes[i].active = 0;
                    }
                    sprintf(message, "Successfully cleared all shapes.");
                } else {
                    sprintf(message, "Clear action canceled.");
                }
                break;
            }
        }
    }

    return 0;
}
