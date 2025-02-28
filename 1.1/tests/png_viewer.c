#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <png.h>



// Structure to represent an RGB color
typedef struct {
    uint8_t r, g, b;
} Color;

// Compare function for qsort & bsearch
int compare_colors(const void *a, const void *b) {
    const Color *colorA = (const Color *)a;
    const Color *colorB = (const Color *)b;
    if (colorA->r != colorB->r) return colorA->r - colorB->r;
    if (colorA->g != colorB->g) return colorA->g - colorB->g;
    return colorA->b - colorB->b;
}

// Function to check if color exists and insert it if not
int insert_color(Color **colors, int *size, int *capacity, Color newColor) {
    // Use bsearch to check if color already exists
    if (bsearch(&newColor, *colors, *size, sizeof(Color), compare_colors)) {
        return 0; // Already exists
    }

    // Resize if needed
    if (*size >= *capacity) {
        *capacity *= 2;
        *colors = realloc(*colors, (*capacity) * sizeof(Color));
        if (!*colors) {
            fprintf(stderr, "Error: Memory allocation failed!\n");
            exit(1);
        }
    }

    // Insert new color & sort
    (*colors)[(*size)++] = newColor;
    qsort(*colors, *size, sizeof(Color), compare_colors);

    return 1;
}

// Function to load PNG file using libpng and convert it to an SDL2 texture
SDL_Texture* load_png_as_texture(const char *filename, SDL_Renderer *renderer) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    // Read the PNG signature (first 8 bytes)
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        printf("Error: File %s is not a PNG.\n", filename);
        fclose(fp);
        return NULL;
    }

    // Initialize libpng structures
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        return NULL;
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return NULL;
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // Allocate memory for the image data
    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);
    fclose(fp);

    // Convert to SDL2 surface
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    for (int y = 0; y < height; y++) {
        memcpy((Uint8 *)surface->pixels + y * surface->pitch, row_pointers[y], png_get_rowbytes(png, info));
        free(row_pointers[y]);
    }
    free(row_pointers);

    png_destroy_read_struct(&png, &info, NULL);

    // Convert SDL_Surface to SDL_Texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

    if (!texture) {
        printf("Error: Could not create texture from PNG file %s\n", filename);
        return NULL;
    }



    // Open the file in write mode. If it doesn't exist, it will be created.
    FILE *file_one = fopen("tree.txt", "w");
    if (file_one == NULL) {
        printf("Error: Unable to open file_one for writing.\n");
        return;
    }
    

    // Pointer to the pixels of the surface
    uint32_t *pixels = (uint32_t *)surface->pixels;


    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32); // Ensure correct format

    // Dynamic array for unique colors
    int capacity = 1000;
    int size = 0;
    Color *unique_colors = malloc(capacity * sizeof(Color));
    int bytes = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t pixel = pixels[y * (surface->pitch / 4) + x]; // Ensure correct indexing

            uint8_t r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a); // Get correct color channels

            // Write to file, ignoring black pixels
            if (r != 0 || g != 0 || b != 0) {
                // if(bytes < 2500){
                    fprintf(file_one, "{%d, %d, %u, %u, %u},", x, y, r, g, b);
                // }
                // if(bytes > 2500 && bytes < 5000){
                //      fprintf(file_two, "{%d, %d, %u, %u, %u},", x, y, r, g, b);                   
                // }
                // if(bytes > 5000 && bytes < 7500){
                //      fprintf(file_three, "{%d, %d, %u, %u, %u},", x, y, r, g, b);                   
                // }
                // if(bytes > 7500 && bytes < 10000){
                //      fprintf(file_four, "{%d, %d, %u, %u, %u},", x, y, r, g, b);                   
                // }
                bytes+=1;
            }
            Color newColor = {r, g, b};
            insert_color(&unique_colors, &size, &capacity, newColor);
        }
    }

    // Close the file after writing
    fclose(file_one);
    printf("Pixel data written to image.txt\n");
    printf("Total Unique Colors: %d\n", size);



    return texture;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image.png>\n", argv[0]);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("PNG Viewer",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Error: Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error: Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *image_texture = load_png_as_texture(argv[1], renderer);
    if (!image_texture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(image_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


// gcc -o png_viewer png_viewer.c -lSDL2 -lpng -lm
// gcc -o png_viewer png_viewer.c -lSDL2 -lSDL2_image -lpng