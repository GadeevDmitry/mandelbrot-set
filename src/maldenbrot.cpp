#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define LOG_NVERIFY

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include <SFML/Graphics.hpp>

#include "maldenbrot.h"

//--------------------------------------------------------------------------------------------------------------------------------
// static global
//--------------------------------------------------------------------------------------------------------------------------------

static sf::Font FPS_FONT;
static sf::Text FPS_TEXT;

static void maldenbort_fps_init();
static void maldenbrot_fps_upd (sf::RenderWindow *const wnd);

//--------------------------------------------------------------------------------------------------------------------------------
// DSL
//--------------------------------------------------------------------------------------------------------------------------------

#define $color  paint->pixels_color
#define $width  paint->pixels_width
#define $height paint->pixels_height

#define $scale paint->scale
#define $x_min paint->x_min
#define $y_max paint->y_max

//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_ctor(maldenbrot *const paint, const size_t pixels_width,
                                              const size_t pixels_height, const double scale,
                                                                          const double x_min,
                                                                          const double y_max)
{
    log_verify(paint != nullptr, false);

    $color  = (unsigned *) log_calloc(pixels_width * pixels_height, sizeof(unsigned));
    $width  = pixels_width;
    $height = pixels_height;

    $scale = scale;
    $x_min = x_min;
    $y_max = y_max;

    maldenbort_fps_init();

    return true;
}

static void maldenbort_fps_init()
{
    sf::Color fps_color = sf::Color::White;

    FPS_FONT.loadFromFile("../data/font.ttf");

    FPS_TEXT.setFont    (FPS_FONT);
    FPS_TEXT.setPosition(0, 0);
    FPS_TEXT.setColor   (fps_color);
}

void maldenbrot_dtor(maldenbrot *const paint)
{
    if (paint != nullptr) log_free($color);
}

//--------------------------------------------------------------------------------------------------------------------------------

void maldenbrot_scale_more (maldenbrot *const paint) { log_verify(paint != nullptr, ;); $scale *= 1.2; }
void maldenbrot_scale_less (maldenbrot *const paint) { log_verify(paint != nullptr, ;); $scale /= 1.2; }

void maldenbrot_shift_up   (maldenbrot *const paint) { log_verify(paint != nullptr, ;); $y_max += $scale * (double) $height * 0.2; }
void maldenbrot_shift_down (maldenbrot *const paint) { log_verify(paint != nullptr, ;); $y_max -= $scale * (double) $height * 0.2; }
void maldenbrot_shift_left (maldenbrot *const paint) { log_verify(paint != nullptr, ;); $x_min -= $scale * (double) $width  * 0.2; }
void maldenbrot_shift_right(maldenbrot *const paint) { log_verify(paint != nullptr, ;); $x_min += $scale * (double) $width  * 0.2; }

//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    sf::Color main_col = sf::Color::Red;

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_x = 0; pixels_x < $width ; ++pixels_x) {
    for (size_t pixels_y = 0; pixels_y < $height; ++pixels_y)
        {
            double prot_x = cur_x, img_x = 0,
                   prot_y = cur_y, img_y = 0;

            unsigned char opacity = 255;
            do
            {
                img_x = (prot_x * prot_x) - (prot_y * prot_y) + cur_x;
                img_y = 2 * prot_x * prot_y                   + cur_y;

                if ((img_x * img_x) + (img_y * img_y) > 100) break;

                prot_x = img_x;
                prot_y = img_y;

                opacity--;
            }
            while (opacity != 0);
            $color[pixels_y * $width + pixels_x] = main_col.toInteger() ^ opacity;

            cur_y -= $scale;
        }
        cur_x += $scale;
        cur_y  = $y_max;
    }

    return true;
}

bool maldenbrot_draw(maldenbrot *const paint, sf::RenderWindow *const wnd)
{
    log_verify(paint != nullptr, false);
    log_verify(wnd   != nullptr, false);

    sf::Image   img; img.create((unsigned) $width, (unsigned) $height, (sf::Uint8 *) $color);
    sf::Texture tex; tex.loadFromImage(img);
    sf::Sprite  spr(tex);

    (*wnd).clear();
    (*wnd).draw(spr);

    maldenbrot_fps_upd(wnd);

    (*wnd).display();

    return true;
}

static void maldenbrot_fps_upd(sf::RenderWindow *const wnd)
{
    log_verify(wnd != nullptr, ;);

    static bool     is_time_run     = false;
    static clock_t  last_frame_time =     0;

    if (!is_time_run) { is_time_run = true; last_frame_time = clock(); return; }

    clock_t cur_frame_time = clock();

    int     fps_val = (int) (CLOCKS_PER_SEC / (cur_frame_time - last_frame_time));
    last_frame_time = clock();

    char    fps_str[100] = "";
    sprintf(fps_str, "FPS: %d\n", fps_val);

    FPS_TEXT.setString(fps_str);
    (*wnd).draw(FPS_TEXT);
}