/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include <math.h>
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_canvas_rect.h"
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_list.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)
#define CURRENT_VIEW_NAME "heartrate_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void heartrate_design(gui_view_t *view);
static void clear_heartrate_cache(gui_view_t *view);
extern void time_update_cb(void *p);
extern void resting_heartrate_design(gui_obj_t *obj);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_img_t *img_heart = NULL;
extern uint8_t activeIndex;
static char time_text_content[10];
static char time_status_content[20];
extern gui_text_t *time_text;
static gui_win_t *win_heart = NULL;
gui_win_t *win_heart_menu = NULL;
static uint8_t *img_dot_data = NULL;
static gui_img_t *img_dot;

// wave background animation variables
static uint8_t *wave_bg_data = NULL;
static gui_img_t *wave_bg_img = NULL;
static float wave_animation_time = 0.0f;
static bool wave_animation_running = false;
// add reflection effect variables
static float g_reflectionPos = 0.0f;
static uint32_t g_lastUpdateTime = 0;
static bool g_heartMeasuring = false;
static gui_img_t *img_heart_reflection = NULL;  // reflection image
static uint8_t *heart_reflection_data = NULL;

static gui_text_t *text_current = NULL;
static gui_text_t *text_heart_rate = NULL;
static gui_text_t *text_status = NULL;

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = heartrate_design,
    .on_switch_out = clear_heartrate_cache,
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void clear_heartrate_cache(gui_view_t *view)
{
    (void)view;
    wave_animation_running = false;
    wave_animation_time = 0.0f;
    g_heartMeasuring = false;
    g_reflectionPos = 0.0f;
    g_lastUpdateTime = 0;

    if (wave_bg_data)
    {
        gui_lower_free(wave_bg_data);
        wave_bg_data = NULL;
    }
    if (img_dot_data)
    {
        gui_lower_free(img_dot_data);
        img_dot_data = NULL;
    }
    if (heart_reflection_data)
    {
        gui_lower_free(heart_reflection_data);
        heart_reflection_data = NULL;
    }

    if (img_heart && img_heart->base.timer)
    {
        gui_obj_stop_timer(GUI_BASE(img_heart));
    }
    activeIndex = 0;
}

// heart shape drawing function
static void drawHeartPath(NVGcontext *vg, float centerX, float centerY, float size)
{
    float x = centerX;
    float y = centerY;
    float w = size;
    float h = size * 1.3f;

    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y - h * 0.35f);

    //left side curve
    nvgBezierTo(vg,
                x - w * 0.5f, y - h * 0.6f,   // left upper control point
                x - w * 0.9f, y + h * 0.1f,   // left middle control point
                x, y + h * 0.5f);             // bottom point

    // right side curve
    nvgBezierTo(vg,
                x + w * 0.9f, y + h * 0.1f,   // right middle control point
                x + w * 0.5f, y - h * 0.6f,   // right upper control point
                x, y - h * 0.35f);            // top point

    nvgClosePath(vg);
    return;
}


// heart rate wave background drawing function
static void drawHeartRateWaveBackground(NVGcontext *vg)
{
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;

    // clear background
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, width, height);
    nvgFillColor(vg, nvgRGB(5, 5, 15));
    nvgFill(vg);

    float centerX = width / 2;
    float centerY = 180;

    // heart wave layers
    for (int i = 0; i < 4; i++)
    {
        float progress = fmod(wave_animation_time + i * 0.2f, 1.0f);
        float scale = 1.0f + progress * 3.0f;
        float opacity = 1.0f - progress; // opacity from 1.0 to 0.0
        float heartSize = 100.0f * scale;

        // color transition from red to light red
        int r = 255;
        int g = (int)(30 + progress * 50);
        int b = (int)(30 + progress * 50);

        // drawHeartPath(vg, centerX, centerY, heartSize);

        // heat shape fill with radial gradient
        NVGpaint fillPaint = nvgRadialGradient(vg, centerX, centerY,
                                               heartSize * 0.2f, heartSize * 0.7f,
                                               nvgRGBA(r, g, b, (int)(opacity * 80)),
                                               nvgRGBA(r, g, b, 0));
        nvgFillPaint(vg, fillPaint);
        nvgFill(vg);

        // heart shape stroke
        drawHeartPath(vg, centerX, centerY, heartSize);
        nvgStrokeColor(vg, nvgRGBA(r, g, b, (int)(opacity * 80)));
        nvgStrokeWidth(vg, 10.0f + progress * 1.0f);
        nvgStroke(vg);

        // heart glow effect
        if (progress > 0.6f)
        {
            drawHeartPath(vg, centerX, centerY, heartSize + 5.0f);
            nvgStrokeColor(vg, nvgRGBA(255, 150, 150, (int)((1.0f - progress) * 100)));
            nvgStrokeWidth(vg, 4.0f);
            nvgStroke(vg);
        }
    }
    return;
}

// wave animation timer callback
static void wave_animation_timer_cb(void *obj)
{
    GUI_UNUSED(obj);
    if (!wave_animation_running) { return; }

    wave_animation_time += 0.02f; // animation speed

    // messurement state affects animation speed
    if (g_heartMeasuring)
    {
        wave_animation_time += 0.01f;
    }

    if (wave_animation_time > 1.0f)
    {
        wave_animation_time = 0.0f;
    }

    // render to image buffer
    if (wave_bg_data)
    {
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                          drawHeartRateWaveBackground, wave_bg_data);

        if (wave_bg_img)
        {
            gui_img_refresh_size(wave_bg_img);
        }
    }

    return;
}
typedef struct NVGpoint
{
    float x, y;
    float dx, dy;
    float len;
    float dmx, dmy;
    unsigned char flags;
} NVGpoint;
static NVGpoint calculateHeartPoint(float t, float centerX, float centerY, float width,
                                    float height)
{
    float x = centerX;
    float y = centerY;
    float w = width;
    float h = height;

    NVGpoint point = {0};

    if (t < 0.5f)
    {
        // left path (0.0 - 0.5)
        float localT = t * 2.0f;
        float u = 1.0f - localT;
        float tt = localT * localT;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * localT;

        point.x = uuu * x +
                  3 * uu * localT * (x - w * 0.5f) +
                  3 * u * tt * (x - w * 0.9f) +
                  ttt * x;

        point.y = uuu * (y - h * 0.35f) +
                  3 * uu * localT * (y - h * 0.6f) +
                  3 * u * tt * (y + h * 0.1f) +
                  ttt * (y + h * 0.5f);
    }
    else
    {
        // right path (0.5 - 1.0)
        float localT = (t - 0.5f) * 2.0f;
        float u = 1.0f - localT;
        float tt = localT * localT;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * localT;

        point.x = uuu * x +
                  3 * uu * localT * (x + w * 0.9f) +
                  3 * u * tt * (x + w * 0.5f) +
                  ttt * x;

        point.y = uuu * (y + h * 0.5f) +
                  3 * uu * localT * (y + h * 0.1f) +
                  3 * u * tt * (y - h * 0.6f) +
                  ttt * (y - h * 0.35f);
    }

    return point;
}

// heart reflection drawing function
static void drawHeartReflection(NVGcontext *vg, float centerX, float centerY, float width,
                                float height)
{
    float x = centerX;
    float y = centerY;
    float w = width;
    float h = height;

    // update reflection position:time-based
    uint32_t currentTime = gui_ms_get();
    uint32_t elapsed = currentTime - g_lastUpdateTime;

    if (elapsed > 16)
    {
        g_reflectionPos += 0.03f;
        if (g_reflectionPos > 1.0f)
        {
            g_reflectionPos = 0.0f;
        }
        g_lastUpdateTime = currentTime;
    }

    float t = g_reflectionPos;
    float reflectionSize = w * 0.2f;

    NVGpoint reflectionPoint = calculateHeartPoint(t, x, y, w, h);

    nvgBeginPath(vg);
    NVGpaint reflectionPaint = nvgRadialGradient(vg,
                                                 reflectionPoint.x, reflectionPoint.y,
                                                 0, reflectionSize * 0.8f,
                                                 nvgRGBA(255, 255, 255, 240),
                                                 nvgRGBA(255, 255, 255, 0));

    nvgCircle(vg, reflectionPoint.x, reflectionPoint.y, reflectionSize);
    nvgFillPaint(vg, reflectionPaint);
    nvgFill(vg);

    float secondT = t - 0.1f;
    if (secondT < 0.0f) { secondT += 1.0f; }

    NVGpoint secondPoint = calculateHeartPoint(secondT, x, y, w, h);

    nvgBeginPath(vg);
    NVGpaint secondPaint = nvgRadialGradient(vg,
                                             secondPoint.x, secondPoint.y,
                                             0, reflectionSize * 0.6f,
                                             nvgRGBA(255, 255, 255, 180),
                                             nvgRGBA(255, 255, 255, 0));

    nvgCircle(vg, secondPoint.x, secondPoint.y, reflectionSize * 0.7f);
    nvgFillPaint(vg, secondPaint);
    nvgFill(vg);
}

// reference: scatter plot drawing function
static void draw_heart_with_reflection_cb(NVGcontext *vg)
{
    float centerX = 60.0f;
    float centerY = 60.0f;
    float size = 100.0f;

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, 120, 120);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 0));
    nvgFill(vg);

    nvgBeginPath(vg);

    // heart shape path
    nvgMoveTo(vg, centerX, centerY - size * 1.3f * 0.35f);

    // left side curve
    nvgBezierTo(vg,
                centerX - size * 0.5f, centerY - size * 1.3f * 0.6f,
                centerX - size * 0.9f, centerY + size * 1.3f * 0.1f,
                centerX, centerY + size * 1.3f * 0.5f);

    // right side curve
    nvgBezierTo(vg,
                centerX + size * 0.9f, centerY + size * 1.3f * 0.1f,
                centerX + size * 0.5f, centerY - size * 1.3f * 0.6f,
                centerX, centerY - size * 1.3f * 0.35f);

    nvgClosePath(vg);

    // fill heart shape
    nvgFillColor(vg, nvgRGBA(255, 0, 100, 255));
    nvgFill(vg);

    nvgStrokeColor(vg, nvgRGBA(255, 80, 80, 200));
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);

    // render heart reflection
    nvgBeginPath(vg);
    drawHeartReflection(vg, centerX, centerY, size, size * 1.3f);
}

// create heart reflection image
static void create_heart_reflection_image(void)
{
    int heart_width = 200;
    int heart_height = 200;
    int pixel_bytes = 4;
    size_t buffer_size = heart_width * heart_height * pixel_bytes + sizeof(gui_rgb_data_head_t);

    heart_reflection_data = gui_lower_malloc(buffer_size);
    if (!heart_reflection_data) { return; }

    memset(heart_reflection_data, 0, buffer_size);

    // create heart reflection image
    img_heart_reflection = gui_img_create_from_mem(win_heart, "heart_reflection_img",
                                                   (void *)heart_reflection_data,
                                                   132, 136, 0, 0);
    gui_img_set_mode(img_heart_reflection, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_heart_reflection, true);
    img_heart_reflection->base.not_show = true;  // not show initially
}

//update heart reflection image content
static void update_heart_reflection_image(void)
{
    if (!heart_reflection_data) { return; }

    int heart_width = 200;
    int heart_height = 200;

    size_t image_data_size = heart_width * heart_height * 4;
    memset(heart_reflection_data + sizeof(gui_rgb_data_head_t), 0, image_data_size);

    // render heart with reflection effect
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, heart_width, heart_height,
                                      draw_heart_with_reflection_cb, heart_reflection_data);

    if (img_heart_reflection)
    {
        gui_img_refresh_size(img_heart_reflection);
    }
}
static void set_measuring_ui_visibility(bool isMeasuring)
{
    // mesasuring:show reflection heart, hide wave background and original heart
    // not measuring: hide reflection heart, show wave background and original heart

    if (wave_bg_img)
    {
        wave_bg_img->base.not_show = isMeasuring;
    }

    if (img_heart)
    {
        img_heart->base.not_show = isMeasuring;
    }

    if (img_heart_reflection)
    {
        img_heart_reflection->base.not_show = !isMeasuring;
    }

    // refresh images
    if (wave_bg_img) { gui_img_refresh_size(wave_bg_img); }
    if (img_heart) { gui_img_refresh_size(img_heart); }
    if (img_heart_reflection) { gui_img_refresh_size(img_heart_reflection); }
}
static void update_measurement_text(bool isMeasuring, int heartRate)
{
    (void)heartRate;
    if (isMeasuring)
    {
        //measuring texts
        if (text_current)
        {
            gui_text_set(text_current, "Measuring...", GUI_FONT_SRC_TTF,
                         gui_rgb(255, 200, 100), strlen("Measuring..."), 40);
        }

        if (text_heart_rate)
        {
            gui_text_set(text_heart_rate, "--BPM", GUI_FONT_SRC_TTF,
                         gui_rgb(255, 255, 255), strlen("--BPM"), 60);
        }

        // if (text_bpm) {
        //     gui_text_set(text_bpm, "BPM", GUI_FONT_SRC_TTF,
        //                 gui_rgb(255, 200, 100), strlen("BPM"), 58);
        // }

        if (text_status)
        {
            gui_text_set(text_status, "Measuring heart rate...", GUI_FONT_SRC_TTF,
                         gui_rgb(200, 200, 100), strlen("Measuring heart rate..."), 36);
        }
    }
    else
    {
        // measured texts
        if (text_current)
        {
            gui_text_set(text_current, "Current", GUI_FONT_SRC_TTF,
                         gui_rgb(255, 255, 255), strlen("Current"), 40);
        }

        // if (text_heart_rate) {
        //     char rate_text[32];
        //     sprintf(rate_text, "%d", heartRate);
        gui_text_set(text_heart_rate, "74BPM", GUI_FONT_SRC_TTF,
                     gui_rgb(255, 255, 255), strlen("74BPM"), 60);
        // }

        // if (text_bpm) {
        //     gui_text_set(text_bpm, "BPM", GUI_FONT_SRC_TTF,
        //                 COLOR_RED, strlen("BPM"), 58);
        // }

        if (text_status)
        {
            // char status_text[32];
            sprintf(time_status_content, "73BPM, just now");
            // gui_text_set(text_status, status_text, GUI_FONT_SRC_TTF,
            //             gui_rgb(150, 150, 150), strlen(status_text), 36);

            gui_text_set(text_status, (void *)time_status_content, GUI_FONT_SRC_TTF, gui_rgb(150, 150, 150),
                         strlen(time_status_content), 30);
        }
    }
}
void start_heart_rate_measurement(void)
{
    g_heartMeasuring = true;
    g_reflectionPos = 0.0f;
    g_lastUpdateTime = gui_ms_get();

    // update reflection image initially
    update_heart_reflection_image();

    // update texts to measuring state
    update_measurement_text(true, 0);  // 0 standy for measuring

    // change UI to measuring state
    set_measuring_ui_visibility(true);

    // gui_log("Heart rate measurement started with reflection effect\n");
}
void stop_heart_rate_measurement(void)
{
    g_heartMeasuring = false;

    // simulate measured heart rate
    int measured_heart_rate = 73 + (rand() % 10);  // 73-82 are random numbers

    // update texts to show measured result
    update_measurement_text(false, measured_heart_rate);

    // change UI back to normal state
    set_measuring_ui_visibility(false);

}
void heart_drawCircles_cb(NVGcontext *vg)
{
    float circleDiameter = 12.0f;
    float circleRadius = circleDiameter / 2.0f;
    float spacing = 4.0f;

    float startX = 0;
    float startY = 0;

    NVGcolor circleColor = nvgRGB(120, 80, 90);
    NVGcolor specialCircleColor = nvgRGB(255, 255, 255);

    for (int i = 0; i < 2; ++i)
    {
        nvgBeginPath(vg);
        float y = startY + i * (circleDiameter + spacing) + circleRadius;
        nvgCircle(vg, startX + circleRadius, y, circleRadius);

        if (i == activeIndex)
        {
            gui_log("i=%d, activeIndex=%d\n", i, activeIndex);
            nvgFillColor(vg, specialCircleColor);
        }
        else
        {
            nvgFillColor(vg, circleColor);
        }
        nvgFill(vg);
    }
    return;
}

static void hr_timer_cb(void *obj)
{
    GUI_UNUSED(obj);
    static uint8_t count = 0;
    static int simulated_heart_rate = 0;
    float scale_factor;

    count++;

    if (g_heartMeasuring)
    {
        scale_factor = 1.0f + 0.05f * sinf(count * (float)M_PI / 3.0f);

        // 100ms update reflection image
        if (count % 1 == 0)
        {
            update_heart_reflection_image();
        }

        if (count % 10 == 0)    // 1s update heart rate value
        {
            simulated_heart_rate = 70 + (rand() % 20);  // 70-89 are random numbers
            update_measurement_text(true, simulated_heart_rate);
        }

        // 5s stop measurement
        if (count % 50 == 0)
        {
            stop_heart_rate_measurement();
        }
    }
    else
    {
        scale_factor = 1.0f + 0.02f * sinf(count * (float)M_PI / 5.0f);

        // 15s start measurement again
        if (count % 150 == 0)
        {
            start_heart_rate_measurement();
        }
    }
    if (img_heart && !img_heart->base.not_show)
    {
        gui_img_scale(img_heart, scale_factor, scale_factor);
        gui_img_refresh_size(img_heart);
    }

    return;
}
static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    // note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;

    if (index < 2)
    {
        if (index == 0)
        {
            activeIndex = 0;
            win_heart->base.not_show = false;
            time_update_cb(NULL);
            //refresh dot img
            gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96,
                                              heart_drawCircles_cb, img_dot_data);
            gui_img_set_image_data(img_dot, img_dot_data);
            gui_img_refresh_size(img_dot);

        }
        if (index == 1)
        {
            win_heart->base.not_show = true;
            // gui_obj_stop_timer(GUI_BASE(img_heart));
            // gui_obj_stop_timer(GUI_BASE(wave_bg_img));
            activeIndex = 1;
            resting_heartrate_design((gui_obj_t *)obj);

        }
        // else if (index == 2)
        // {
        //     activeIndex = 2;
        // }
        // else if (index == 3)
        // {

        // }

    }
    return;

}
static void heartrate_design(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    g_lastUpdateTime = gui_ms_get();

    win_heart = gui_win_create(obj, "win_heart", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    win_heart_menu = gui_win_create(obj, "win_heart_menu", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // create wave background image
    int pixel_bytes = 4;
    size_t buffer_size = SCREEN_WIDTH * SCREEN_HEIGHT * pixel_bytes + sizeof(gui_rgb_data_head_t);

    if (wave_bg_data == NULL)
    {
        wave_bg_data = gui_lower_malloc(buffer_size);
    }
    memset(wave_bg_data, 0, buffer_size);

    // init wave background image data
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                      drawHeartRateWaveBackground, wave_bg_data);

    wave_bg_img = gui_img_create_from_mem(win_heart, "wave_bg", (void *)wave_bg_data,
                                          0, 0, 0, 0);
    gui_img_set_mode(wave_bg_img, IMG_SRC_OVER_MODE);
    gui_img_set_quality(wave_bg_img, true);

    // start wave animation
    wave_animation_running = true;
    gui_obj_create_timer(GUI_BASE(win_heart), 30, true, wave_animation_timer_cb); // 33fps
    gui_obj_start_timer(GUI_BASE(win_heart));


    buffer_size = 12 * 96 * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_dot_data == NULL)
    {
        img_dot_data = gui_lower_malloc(buffer_size);
    }
    memset(img_dot_data, 0, buffer_size);

    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96,
                                      heart_drawCircles_cb, img_dot_data);
    img_dot = gui_img_create_from_mem(win_heart_menu, "dot_img", (void *)img_dot_data,
                                      393, 82, 0, 0);
    gui_img_set_mode(img_dot, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_dot, true);

    {
        sprintf(time_text_content, "16:36");
        time_text = gui_text_create(win_heart_menu, "date_text", -30, 33, 0, 0);
        gui_text_set(time_text, (void *)time_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(time_text_content), 28);
        gui_text_type_set(time_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(time_text, RIGHT);
        gui_text_rendermode_set(time_text, 2);
    }
    {
        char *text = "Current";
        int font_size = 40;
        text_current = gui_text_create(win_heart, "txt_current", 31, 318, 0, font_size);
        gui_text_set(text_current, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
        gui_text_mode_set(text_current, LEFT);
        gui_text_type_set(text_current, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_current, 2);
    }
    {
        char *text = "73BPM";
        int font_size = 60;
        text_heart_rate = gui_text_create(win_heart, "txt_heart_rate", 31, 350, 0, 96);
        gui_text_set(text_heart_rate, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text),
                     font_size);
        gui_text_mode_set(text_heart_rate, LEFT);
        gui_text_type_set(text_heart_rate, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_heart_rate, 2);
    }
    {
        // char *text = "BPM";
        // int font_size = 58;
        // text_bpm = gui_text_create(win_heart, "txt_bpm", 125, 380, 0, 96);
        // gui_text_set(text_bpm, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        // gui_text_mode_set(text_bpm, LEFT);
        // gui_text_type_set(text_bpm, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        // gui_text_rendermode_set(text_bpm, 2);
    }
    {
        sprintf(time_status_content, "71BPM, 1m ago");
        int font_size = 36;
        text_status = gui_text_create(win_heart, "txt_status", 40, 430, 0, 96);
        gui_text_set(text_status, (void *)time_status_content, GUI_FONT_SRC_TTF, gui_rgb(150, 150, 150),
                     strlen(time_status_content), font_size);
        gui_text_mode_set(text_status, LEFT);
        gui_text_type_set(text_status, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_status, 2);
    }
    {
        img_heart = gui_img_create_from_mem(win_heart, "heart_img", HEART_BIN, 132, 136, 0, 0);
        gui_obj_create_timer(GUI_BASE(img_heart), 100, true, hr_timer_cb);
        gui_obj_start_timer(GUI_BASE(img_heart));

        //initialize heart measuring state
        create_heart_reflection_image();

        // initially not measuring
        set_measuring_ui_visibility(false);
    }
    int length = 502;
    uint8_t space = 0;
    gui_list_t *list = gui_list_create(view, "list", 0, 0, 0, 0, length, space, VERTICAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_FADE);
    gui_list_set_note_num(list, 2);
    gui_list_set_auto_align(list, true);
    gui_list_set_inertia(list, false);
    gui_list_set_out_scope(list, length / 2);
    time_update_cb(NULL);
}