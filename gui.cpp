#include <math.h>
#include <stdio.h>

#include "gui.h"
#include "u_misc.h"

namespace gui {

void queue::addScissor(int x, int y, int w, int h) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandScissor;
    cmd.flags = x < 0 ? 0 : 1;
    cmd.color = 0;
    cmd.asScissor.x = x;
    cmd.asScissor.y = y;
    cmd.asScissor.w = w;
    cmd.asScissor.h = h;
}

void queue::addRectangle(float x, float y, float w, float h, uint32_t color) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandRectangle;
    cmd.flags = 0;
    cmd.color = color;
    cmd.asRectangle.x = int(x * 8.0f);
    cmd.asRectangle.y = int(y * 8.0f);
    cmd.asRectangle.w = int(w * 8.0f);
    cmd.asRectangle.h = int(h * 8.0f);
    cmd.asRectangle.r = 0;
}

void queue::addLine(float x0, float y0, float x1, float y1, float r, uint32_t color) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandLine;
    cmd.flags = 0;
    cmd.color = color;
    cmd.asLine.x[0] = int(x0 * 8.0f);
    cmd.asLine.y[0] = int(y0 * 8.0f);
    cmd.asLine.x[1] = int(x1 * 8.0f);
    cmd.asLine.y[1] = int(y1 * 8.0f);
    cmd.asLine.r = int(r * 8.0f);
}

void queue::addRectangle(float x, float y, float w, float h, float r, uint32_t color) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandRectangle;
    cmd.flags = 0;
    cmd.color = color;
    cmd.asRectangle.x = int(x * 8.0f);
    cmd.asRectangle.y = int(y * 8.0f);
    cmd.asRectangle.w = int(w * 8.0f);
    cmd.asRectangle.h = int(h * 8.0f);
    cmd.asRectangle.r = int(r * 8.0f);
}

void queue::addTriangle(int x, int y, int w, int h, int flags, uint32_t color) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandTriangle;
    cmd.flags = flags;
    cmd.color = color;
    cmd.asTriangle.x = x * 8;
    cmd.asTriangle.y = y * 8;
    cmd.asTriangle.w = w * 8;
    cmd.asTriangle.h = h * 8;
}

void queue::addText(int x, int y, int align, const u::string &contents, uint32_t color) {
    if (m_size >= kCommandQueueSize)
        return;
    auto &cmd = m_data[m_size++];
    cmd.type = kCommandText;
    cmd.flags = 0;
    cmd.color = color;
    cmd.asText.x = x;
    cmd.asText.y = y;
    cmd.asText.align = align;
    cmd.asText.contents = contents;
}

// A reference to something in the gui
typedef size_t ref;

// A widget is just a bounding box with no height.
struct widget {
    static const size_t kInitialIndentation = 100;
    widget();
    void reset();

    int x;
    int y;
    int w;
    ref id;
};

inline widget::widget() {
    reset();
    w = kInitialIndentation;
}

inline void widget::reset() {
    x = 0;
    y = 0;
    w = 0;
    id = 1; // The root reference is 0
}

// Scrollable area
struct scroll {
    scroll();

    int top;
    int bottom;
    int right;
    int areaTop;
    int *value;
    int focusTop;
    int focusBottom;
    ref id;
    bool inside;
};

inline scroll::scroll()
    : top(0)
    , bottom(0)
    , right(0)
    , areaTop(0)
    , value(nullptr)
    , focusTop(0)
    , focusBottom(0)
    , id(0)
    , inside(false)
{
}

struct state {
    state();

    bool anyActive();
    bool isActive(ref thing);
    bool isHot(ref thing);

    bool inRectangle(int x, int y, int w, int h, bool checkScroll = true);

    // To get the commands
    const queue &commands() const;

    // To update input state
    void update(int mx, int my, int mb, int scroll);

    void clearInput(); // Clear any input state
    void clearActive(); // Clear any active state (reference to active thing)

    // Set active/hot things
    void setActive(ref thing);
    void setHot(ref thing);

    // Implements the logic for button hover/click
    bool buttonLogic(ref thing, bool over);

    ref m_active; // Reference to active thing
    ref m_hot; // Reference to hot thing
    ref m_nextHot; // Reference to hotToBe
    ref m_area; // Reference to root area

    int m_mouse[3]; // X, Y, Scroll
    int m_drag[2]; // X, Y
    float m_dragOrigin;

    bool m_insideCurrentScroll; // Inside a scrollable area
    bool m_isHot; // Something is hot
    bool m_isActive; // Something is active
    bool m_wentActive; // Something just became active

    bool m_left; // left mouse button
    bool m_leftPressed; // left pressed
    bool m_leftReleased; // left released

    widget m_widget;
    queue m_queue;
    scroll m_scroll;
};

inline state::state()
    : m_active(0)
    , m_hot(0)
    , m_nextHot(0)
    , m_area(0)
    , m_dragOrigin(0.0f)
    , m_insideCurrentScroll(false)
    , m_isHot(false)
    , m_isActive(false)
    , m_wentActive(false)
    , m_left(false)
    , m_leftPressed(false)
    , m_leftReleased(false)
{
    m_mouse[0] = -1;
    m_mouse[1] = -1;
    m_mouse[2] = 0; // Scroll
    m_drag[0] = 0;
    m_drag[1] = 0;
}

inline bool state::anyActive() {
    return m_active != 0;
}

inline bool state::isActive(ref thing) {
    return m_active == thing;
}

inline bool state::isHot(ref thing) {
    return m_hot == thing;
}

inline bool state::inRectangle(int x, int y, int w, int h, bool checkScroll) {
    return (!checkScroll || m_insideCurrentScroll)
        && m_mouse[0] >= x && m_mouse[0] <= x + w
        && m_mouse[1] >= y && m_mouse[1] <= y + h;
}

inline void state::clearInput() {
    m_leftPressed = false;
    m_leftReleased = false;
    m_mouse[2] = 0; // Scroll
}

inline void state::clearActive() {
    m_active = 0;
    clearInput();
}

inline void state::setActive(ref thing) {
    m_active = thing;
    m_wentActive = true;
}

inline void state::setHot(ref thing) {
    m_nextHot = thing;
}

inline bool state::buttonLogic(ref thing, bool over) {
    bool result = false;
    // Nothing is active
    if (!anyActive()) {
        if (over) // Hovering it, make hot
            setHot(thing);
        if (isHot(thing) && m_leftPressed) // Hot and pressed, make active
            setActive(thing);
    }
    // Button is active, react on left released
    if (isActive(thing)) {
        m_isActive = true;
        if (over) // Hovering it, make hot
            setHot(thing);
        if (m_leftReleased) { // It's been clicked
            if (isHot(thing))
                result = true;
            clearActive();
        }
    }
    if (isHot(thing))
        m_isHot = true;
    return result;
}

inline void state::update(int mx, int my, int mb, int scroll) {
    // Update the input state
    bool left = mb & kMouseButtonLeft;
    m_mouse[0] = mx;
    m_mouse[1] = my;
    m_mouse[2] = scroll;
    m_leftPressed = !m_left && left;
    m_leftReleased = m_left && !left;
    m_left = left;
}

////////////////////////////////////////////////////////////////////////////////
/// The Singleton
static state S; // [S]tate

#define Q (S.m_queue)  // [Q]ueue
#define W (S.m_widget) // [W]idget
#define A (S.m_area)   // [A]rea
#define B (S.m_scroll) // Scroll [B]ar

// Constants
static constexpr int kButtonHeight = 20;
static constexpr int kSliderHeight = 20;
static constexpr int kSliderMarkerWidth = 10;
static constexpr int kCheckBoxSize = 8;
static constexpr int kDefaultSpacing = 4;
static constexpr int kTextHeight = 8;
static constexpr int kScrollAreaPadding = 6;
static constexpr int kIndentationSize = 16;
static constexpr int kAreaHeader = 28;

bool areaBegin(const u::string &contents, int x, int y, int w, int h, int &value) {
    A++;
    W.id = 0;
    B.id = (A << 16) | W.id;

    W.x = x + kScrollAreaPadding;
    W.y = y+h-kAreaHeader + value;
    W.w = w - kScrollAreaPadding*4;

    B.top = y-kAreaHeader+h;
    B.bottom = y+kScrollAreaPadding;
    B.right = x+w-kScrollAreaPadding*3;
    B.value = &value;
    B.areaTop = W.y;
    B.focusTop = y-kAreaHeader;
    B.focusBottom = y-kAreaHeader+h;
    B.inside = S.inRectangle(x, y, w, h, false);

    S.m_insideCurrentScroll = B.inside;

    Q.addRectangle(x, y, w, h, 6.0f, RGBA(0, 0, 0, 192));
    Q.addText(x+kAreaHeader/2, y+h-kAreaHeader/2-kTextHeight/2, kAlignLeft,
        contents, RGBA(255, 255, 255, 128));
    Q.addScissor(x+kScrollAreaPadding, y+kScrollAreaPadding, w-kScrollAreaPadding*4,
        h-kAreaHeader-kScrollAreaPadding);

    return B.inside;
}

void areaFinish(int inc) {
    Q.addScissor(-1, -1, -1, -1);

    const int x = B.right + kScrollAreaPadding/2;
    const int y = B.bottom;
    const int w = kScrollAreaPadding*2;
    const int h = B.top - B.bottom;

    const int stop = B.areaTop;
    const int sbot = W.y;
    const int sh = stop - sbot;

    float barHeight = float(h)/float(sh);

    if (barHeight < 1.0f) {
        float barY = float(y - sbot) / float(sh);
        if (barY < 0.0f) barY = 0.0f;
        if (barY > 1.0f) barY = 1.0f;

        const auto id = B.id;
        const int hx = x;
        const int hy = y + int(barY * h);
        const int hw = w;
        const int hh = int(barHeight * h);
        const int range = h - (hh - 1);

        S.buttonLogic(id, S.inRectangle(hx, hy, hw, hh));
        if (S.isActive(id)) {
            float u = float(hy - y) / float(range);
            if (S.m_wentActive) {
                S.m_drag[1] = S.m_mouse[1];
                S.m_dragOrigin = u;
            }
            if (S.m_drag[1] != S.m_mouse[1]) {
                u = S.m_dragOrigin + (S.m_mouse[1] - S.m_drag[1]) / float(range);
                if (u < 0.0f) u = 0.0f;
                if (u > 1.0f) u = 1.0f;
                *B.value = int((1 - u) * (sh - h));
            }
        }

        // Background
        Q.addRectangle(x, y, w, h, float(w)/2-1, RGBA(0, 0, 0, 196));

        // Bar
        if (S.isActive(id)) {
            Q.addRectangle(hx, hy, hw, hh, float(w)/2-1, RGBA(255, 196, 0, 196));
        } else {
            Q.addRectangle(hx, hy, hw, hh, float(w)/2-1,
                S.isHot(id) ? RGBA(255, 196, 0, 96) : RGBA(255, 255, 255, 64));
        }

        // Scrolling
        if (B.inside) {
            *B.value += inc*S.m_mouse[2];
            if (*B.value < 0) *B.value = 0;
            if (*B.value > (sh - h)) *B.value = sh - h;
        }
    }
    S.m_insideCurrentScroll = false;
}

bool button(const u::string &contents, bool enabled) {
    const auto id = (A << 16) | ++W.id;

    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;
    const int h = kButtonHeight;

    W.y -= kButtonHeight + kDefaultSpacing;

    const bool over = enabled && S.inRectangle(x, y, w, h);
    const bool result = S.buttonLogic(id, over);

    Q.addRectangle(x, y, w, h, float(kButtonHeight)/2-1,
        RGBA(128, 128, 128, S.isActive(id) ? 196 : 96));
    if (enabled) {
        Q.addText(x+kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, S.isHot(id) ? RGBA(255, 196, 0, 255) : RGBA(255, 255, 255, 200));
    } else {
        Q.addText(x+kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(128, 128, 128, 200));
    }

    return result;
}

bool item(const u::string &contents, bool enabled) {
    const auto id = (A << 16) | ++W.id;

    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;
    const int h = kButtonHeight;

    W.y -= kButtonHeight + kDefaultSpacing;

    const bool over = enabled && S.inRectangle(x, y, w, h);
    const bool result = S.buttonLogic(id, over);

    if (S.isHot(id))
        Q.addRectangle(x, y, w, h, 2.0f, RGBA(255,196,0,S.isActive(id)?196:96));
    if (enabled) {
        Q.addText(x+kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(255,255,255,200));
    } else {
        Q.addText(x+kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(128,128,128,200));
    }

    return result;
}

bool check(const u::string &contents, bool checked, bool enabled) {
    const auto id = (A << 16) | ++W.id;

    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;
    const int h = kButtonHeight;

    W.y -= kButtonHeight + kDefaultSpacing;

    const bool over = enabled && S.inRectangle(x, y, w, h);
    const bool result = S.buttonLogic(id, over);

    const int cx = x+kButtonHeight/2-kCheckBoxSize/2;
    const int cy = y+kButtonHeight/2-kCheckBoxSize/2;

    Q.addRectangle(float(cx)-3, float(cy)-3, float(kCheckBoxSize)+6, float(kCheckBoxSize)+6, 4.0f,
        RGBA(128, 128, 128, S.isActive(id) ? 196 : 96));
    if (checked) {
        if (enabled) {
            Q.addRectangle(cx, cy, kCheckBoxSize, kCheckBoxSize, float(kCheckBoxSize)/2-1,
                RGBA(255, 255, 255, S.isActive(id) ? 255 : 200));
        } else {
            Q.addRectangle(cx, cy, kCheckBoxSize, kCheckBoxSize, float(kCheckBoxSize)/2-1,
                RGBA(128, 128, 128, 200));
        }
    }
    if (enabled) {
        Q.addText(x+kButtonHeight, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, S.isHot(id) ? RGBA(255, 196, 0, 255) : RGBA(255, 255, 255, 200));
    } else {
        Q.addText(x+kButtonHeight, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(128, 128, 128, 200));
    }

    return result;
}

bool collapse(const u::string &contents, const char *subtext, bool checked, bool enabled) {
    const auto id = (A << 16) | ++W.id;

    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;
    const int h = kButtonHeight;

    W.y -= kButtonHeight;

    const int cx = x+kButtonHeight/2-kCheckBoxSize/2;
    const int cy = y+kButtonHeight/2-kCheckBoxSize/2;

    const bool over = enabled && S.inRectangle(x, y, w, h);
    const bool result = S.buttonLogic(id, over);

    Q.addTriangle(cx, cy, kCheckBoxSize, kCheckBoxSize, checked ? 2 : 1,
        RGBA(255, 255, 255, S.isActive(id) ? 255 : 200));

    if (enabled) {
        Q.addText(x+kButtonHeight, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, S.isHot(id) ? RGBA(255, 196, 0, 255) : RGBA(255, 255, 255, 200));
    } else {
        Q.addText(x+kButtonHeight, y+kButtonHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(128, 128, 128, 200));
    }

    if (subtext && *subtext) {
        Q.addText(x+w-kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignRight,
            subtext, RGBA(255, 255, 255, 128));
    }

    return result;
}

void label(const u::string &contents) {
    const int x = W.x;
    const int y = W.y - kButtonHeight;

    W.y -= kButtonHeight;

    Q.addText(x, y+kButtonHeight/2-kTextHeight/2, kAlignLeft, contents,
        RGBA(255, 255, 255, 255));
}

void value(const u::string &contents) {
    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;

    W.y -= kButtonHeight;

    Q.addText(x+w-kButtonHeight/2, y+kButtonHeight/2-kTextHeight/2, kAlignRight,
        contents, RGBA(255, 255, 255, 200));
}

bool slider(const u::string &contents, float &value, float min, float max, float inc, bool enabled) {
    const auto id = (A << 16) | ++W.id;

    const int x = W.x;
    const int y = W.y - kButtonHeight;
    const int w = W.w;
    const int h = kSliderHeight;

    W.y -= kSliderHeight + kDefaultSpacing;

    Q.addRectangle(x, y, w, h, 4.0f, RGBA(0, 0, 0, 128));

    const int range = w - kSliderMarkerWidth;
    float u = (value - min) / (max - min);
    if (u < 0.0f) u = 0.0f;
    if (u > 1.0f) u = 1.0f;

    int m = int(u * range);

    const bool over = enabled && S.inRectangle(x+m, y, kSliderMarkerWidth, kSliderHeight);
    const bool result = S.buttonLogic(id, over);
    bool changed = false;

    if (S.isActive(id)) {
        if (S.m_wentActive) {
            S.m_drag[0] = S.m_mouse[0];
            S.m_dragOrigin = u;
        }
        if (S.m_drag[0] != S.m_mouse[0]) { // Mouse and drag don't share same coordinate on the X axis
            u = S.m_dragOrigin + float(S.m_mouse[0] - S.m_drag[0]) / float(range);
            if (u < 0.0f) u = 0.0f;
            if (u > 1.0f) u = 1.0f;
            value = min + u * (max - min);
            value = floorf(value / inc + 0.5f) * inc; // Snap to increments
            m = int(u * range);
            changed = true;
        }
    }

    if (S.isActive(id)) {
        Q.addRectangle(float(x+m), y, kSliderMarkerWidth, kSliderHeight, 4.0f,
            RGBA(255, 255, 255, 255));
    } else {
        Q.addRectangle(float(x+m), y, kSliderMarkerWidth, kSliderHeight, 4.0f,
            S.isHot(id) ? RGBA(255, 196, 0, 128) : RGBA(255, 255, 255, 64));
    }

    const int digits = int(ceilf(log10f(inc)));
    u::string msg = u::format("%.*f", digits >= 0 ? 0 : -digits, value);

    if (enabled) {
        Q.addText(x+kSliderHeight/2, y+kSliderHeight/2-kTextHeight/2, kAlignLeft,
            contents, S.isHot(id) ? RGBA(255, 196, 0, 255) : RGBA(255, 255, 255, 200));
        Q.addText(x+w-kSliderHeight/2, y+kSliderHeight/2-kTextHeight/2, kAlignRight,
            msg, S.isHot(id) ? RGBA(255, 196, 0, 255) : RGBA(255, 255, 255, 200));
    } else {
        Q.addText(x+kSliderHeight/2, y+kSliderHeight/2-kTextHeight/2, kAlignLeft,
            contents, RGBA(128, 128, 128, 200));
        Q.addText(x+w-kSliderHeight/2, y+kSliderHeight/2-kTextHeight/2, kAlignRight,
            msg, RGBA(128, 128, 128, 200));
    }

    return result || changed;
}

void indent() {
    W.x += kIndentationSize;
    W.w -= kIndentationSize;
}

void dedent() {
    W.x -= kIndentationSize;
    W.w += kIndentationSize;
}

void separator() {
    W.y -= kDefaultSpacing*3;
}

void heading() {
    const int x = W.x;
    const int y = W.y - kDefaultSpacing*2;
    const int w = W.w;
    const int h = 1;

    W.y -= kDefaultSpacing*4;

    Q.addRectangle(x, y, w, h, RGBA(255, 255, 255, 32));
}

/// Primitive drawing (not part of the managed menu), useful for doing HUD
void drawLine(float x0, float y0, float x1, float y1, float r, uint32_t color) {
    Q.addLine(x0, y0, x1, y1, r, color);
}

void drawRectangle(float x, float y, float w, float h, uint32_t color) {
    Q.addRectangle(x, y, w, h, color);
}

void drawRectangle(float x, float y, float w, float h, float r, uint32_t color) {
    Q.addRectangle(x, y, w, h, r, color);
}

void drawText(int x, int y, int align, const u::string &contents, uint32_t color) {
    Q.addText(x, y, align, contents, color);
}

const queue &commands() {
    return Q;
}

void begin(int mx, int my, int mb, int scroll) {
    S.update(mx, my, mb, scroll);

    // This hot becomes the nextHot
    S.m_hot = S.m_nextHot;
    S.m_nextHot = 0;

    // Nothing went active, is active or hot
    S.m_wentActive = false;
    S.m_isActive = false;
    S.m_isHot = false;

    S.m_widget.reset();
    S.m_queue.reset();

    S.m_area = 1;
}

void finish() {
    S.clearInput();
}

}