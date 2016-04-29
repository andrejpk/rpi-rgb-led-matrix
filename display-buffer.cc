 

#include "led-matrix.h"
#include "graphics.h"
#include "transformer.h"
#include "ak-transformer.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace rgb_matrix;

static int usage(const char *progname) {
  fprintf(stderr, "usage: %s [options]\n", progname);
  fprintf(stderr, "Reads binary image data from stdin and displays it.\n"
          "Image format is a straight bitmap, 4 bytes per pixel (RGBA). Alpha is ignored.\n");
  fprintf(stderr, "Options:\n"
          "\t-r <rows>     : Display rows. 16 for 16x32, 32 for 32x32. "
          "Default: 32\n"
          "\t-P <parallel> : For Plus-models or RPi2: parallel chains. 1..3. "
          "Default: 1\n"
          "\t-c <chained>  : Daisy-chained boards. Default: 1.\n"
          "\t-L            : Large display (alternate layout)\n"
          "\t-b <brightness>: Sets brightness percent. Default: 100.\n"
          "\t-d            : Run with debug settings (framerate pixel, diag messages)");
  return 1;
}

int main(int argc, char *argv[]) {
  Color color(255, 255, 0);
  int rows = 32;
  int chain = 1;
  int parallel = 1;
  int brightness = 100;
  bool large_display = false;
  bool debug = false;

  int opt;
  while ((opt = getopt(argc, argv, "r:P:Lc:b:d")) != -1) {
    switch (opt) {
    case 'r': rows = atoi(optarg); break;
    case 'P': parallel = atoi(optarg); break;
    case 'c': chain = atoi(optarg); break;
    case 'b': brightness = atoi(optarg); break;
    case 'L':
      // The 'large' display assumes a chain of four displays with 32x32
      chain = 4;
      rows = 16;
      large_display = true;
      break;  
    case 'd': debug = true;
    default:
      return usage(argv[0]);
    }
  }

  if (rows != 16 && rows != 32) {
    fprintf(stderr, "Rows can either be 16 or 32\n");
    return 1;
  }

  if (chain < 1) {
    fprintf(stderr, "Chain outside usable range\n");
    return 1;
  }
  if (chain > 8) {
    fprintf(stderr, "That is a long chain. Expect some flicker.\n");
  }
  if (parallel < 1 || parallel > 3) {
    fprintf(stderr, "Parallel outside usable range.\n");
    return 1;
  }
  if (brightness < 1 || brightness > 100) {
    fprintf(stderr, "Brightness is outside usable range.\n");
    return 1;
  }

  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;

  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  RGBMatrix *canvas = new RGBMatrix(&io, rows, chain, parallel);
  canvas->SetBrightness(brightness);
  
  LinkedTransformer *transformer = new LinkedTransformer();
  canvas->SetTransformer(transformer);

  if (large_display) {
    // Mapping the coordinates of a 32x128 display mapped to a square of 64x64
    transformer->AddTransformer(new Rect64x32Transformer());
  }

  bool all_extreme_colors = brightness == 100;
  all_extreme_colors &= color.r == 0 || color.r == 255;
  all_extreme_colors &= color.g == 0 || color.g == 255;
  all_extreme_colors &= color.b == 0 || color.b == 255;
  if (all_extreme_colors)
    canvas->SetPWMBits(1);

  size_t buffer_rows = canvas->height();
  size_t buffer_cols = canvas->width();
  int bytesPerPixel = 4;  // rgb @ 8 bytes each
  int displayBufferSize = buffer_rows * buffer_cols * bytesPerPixel; 
  char *displayBuffer = (char *)malloc(displayBufferSize); 
  if (isatty(STDIN_FILENO)) {
    // Only give a message if we are interactive. If connected via pipe, be quiet
    printf("Buffer ready, %d x %d (%d bytes)\n", buffer_cols, buffer_rows, displayBufferSize );
  }
  
  char* thisPixel;
  int frameCounter = 0;
  while (true) {
    fread(displayBuffer, 1, displayBufferSize, stdin);
    thisPixel = displayBuffer; 
    // canvas->Clear();
    for (size_t y = 0; y < buffer_rows; ++y) {
      for (size_t x = 0; x < buffer_cols; ++x) {  
        if (debug && x == 0 && y == 0) // frame diag pixel
        {  
          canvas
            ->SetPixel(x, y, (frameCounter % 2) * 0xff, 0xff, 0xff);
        } 
        else
        {      
          canvas
            ->SetPixel(x, y,
                        thisPixel[0],
                        thisPixel[1],
                        thisPixel[2]);
        }
        thisPixel += bytesPerPixel;
      }
    }
    if (debug) printf("frame done");
    frameCounter++;
    // usleep(100 * 1000);
  }
  
  free(displayBuffer);

  // Finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
