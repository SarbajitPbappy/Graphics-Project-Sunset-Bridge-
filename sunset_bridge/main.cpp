#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#elif defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
/*#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#elif defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GL/glut.h>
#endif */
// Animation variables
float boat1 = -1.0f;
float boat2 = -0.3f;
float timeVal = 0.0f;

// Cloud positions
float cloudPos1 = -1.5f;
float cloudPos2 = 0.0f;
float cloudPos3 = 1.2f;

// Vehicle positions
float car1Pos = -1.5f;
float car2Pos = 0.3f;
float car3Pos = -0.8f;
float bus1Pos = 1.5f;
float bus2Pos = 0.0f;

// Train position
float trainPos = 2.0f;

// Celestial bodies (filled from time-of-day preset)
float sunX = 0.65f;
float sunY = 0.55f;
float sunRadius = 0.12f;
float sunStrength = 1.0f;
float moonX = -0.62f;
float moonY = 0.68f;
float moonRadius = 0.065f;
float moonStrength = 0.0f;
float starVisibility = 0.0f;
float ambientScene = 1.0f;

// Dynamic lighting (clouds modulate on top of ambientScene)
float brightness = 1.0f;
float targetBrightness = 1.0f;

struct Color {
    float r, g, b;
    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {}
    Color operator*(float f) const { return Color(r * f, g * f, b * f); }
};

struct TimePresetData {
    Color skyTop, skyMid, skyBottom, skyBand;
    Color waterTop, waterBottom;
    Color mountainFar, mountainNear;
    Color sunCore, sunEdge;
    Color cloud, cloudShadow;
    Color bridgeSteel, bridgeDark, bridgeRoad, bridgeRail;
    float sunx, suny, sunr;
    float sunStr;
    float moonx, moony, moonr;
    float moonStr;
    float stars;
    float ambient;
};

namespace Palette {
    Color skyTop, skyMid, skyBottom, skyBand;
    Color waterTop, waterBottom;
    Color cloud, cloudShadow;
    Color mountainFar, mountainNear;
    Color sunCore, sunEdge;
    Color bridgeSteel, bridgeDark, bridgeRoad, bridgeRail;
}

int g_timeIndex = 4;
int g_weatherIndex = 0;
float windStrength = 0.15f;
float rainIntensity = 0.0f;
float snowIntensity = 0.0f;
float lightningFlash = 0.0f;
float g_thunderBoost = 0.0f;
float g_boltTipX = 0.0f;
float g_boltPhase = 0.0f;
float g_boltSeed = 0.0f;
float g_nightBridgeLight = 0.0f;
bool g_paused = false;
bool g_showHelp = true;

static void mixTowardGray(Color& c, float a) {
    if (a <= 0.0f) return;
    c.r = c.r * (1.0f - a) + 0.52f * a;
    c.g = c.g * (1.0f - a) + 0.52f * a;
    c.b = c.b * (1.0f - a) + 0.56f * a;
}

void getTimePreset(int idx, TimePresetData& t) {
    switch (idx) {
        case 0: // Dawn
            t.skyTop = Color(0.16f, 0.14f, 0.36f);
            t.skyMid = Color(0.55f, 0.38f, 0.58f);
            t.skyBottom = Color(0.92f, 0.52f, 0.42f);
            t.skyBand = Color(0.98f, 0.72f, 0.48f);
            t.waterTop = Color(0.16f, 0.28f, 0.46f);
            t.waterBottom = Color(0.04f, 0.08f, 0.22f);
            t.mountainFar = Color(0.36f, 0.26f, 0.4f);
            t.mountainNear = Color(0.26f, 0.19f, 0.3f);
            t.sunCore = Color(1.0f, 0.9f, 0.72f);
            t.sunEdge = Color(1.0f, 0.48f, 0.2f);
            t.cloud = Color(0.96f, 0.88f, 0.86f);
            t.cloudShadow = Color(0.62f, 0.52f, 0.58f);
            t.bridgeSteel = Color(0.48f, 0.5f, 0.54f);
            t.bridgeDark = Color(0.26f, 0.28f, 0.31f);
            t.bridgeRoad = Color(0.2f, 0.2f, 0.22f);
            t.bridgeRail = Color(0.36f, 0.33f, 0.29f);
            t.sunx = -0.78f; t.suny = 0.34f; t.sunr = 0.105f;
            t.sunStr = 0.9f;
            t.moonx = 0.7f; t.moony = 0.52f; t.moonr = 0.035f; t.moonStr = 0.12f;
            t.stars = 0.1f;
            t.ambient = 0.78f;
            break;
        case 1: // Morning
            t.skyTop = Color(0.22f, 0.42f, 0.78f);
            t.skyMid = Color(0.48f, 0.68f, 0.95f);
            t.skyBottom = Color(0.72f, 0.82f, 0.95f);
            t.skyBand = Color(0.85f, 0.88f, 0.92f);
            t.waterTop = Color(0.14f, 0.32f, 0.52f);
            t.waterBottom = Color(0.05f, 0.12f, 0.28f);
            t.mountainFar = Color(0.34f, 0.32f, 0.38f);
            t.mountainNear = Color(0.26f, 0.24f, 0.3f);
            t.sunCore = Color(1.0f, 0.98f, 0.88f);
            t.sunEdge = Color(1.0f, 0.75f, 0.35f);
            t.cloud = Color(0.98f, 0.98f, 1.0f);
            t.cloudShadow = Color(0.72f, 0.74f, 0.8f);
            t.bridgeSteel = Color(0.46f, 0.49f, 0.53f);
            t.bridgeDark = Color(0.25f, 0.27f, 0.3f);
            t.bridgeRoad = Color(0.19f, 0.19f, 0.21f);
            t.bridgeRail = Color(0.35f, 0.32f, 0.28f);
            t.sunx = -0.55f; t.suny = 0.62f; t.sunr = 0.1f;
            t.sunStr = 1.0f;
            t.moonx = 0.75f; t.moony = 0.45f; t.moonr = 0.03f; t.moonStr = 0.0f;
            t.stars = 0.0f;
            t.ambient = 0.95f;
            break;
        case 2: // Noon
            t.skyTop = Color(0.12f, 0.35f, 0.82f);
            t.skyMid = Color(0.35f, 0.58f, 0.95f);
            t.skyBottom = Color(0.55f, 0.72f, 0.98f);
            t.skyBand = Color(0.7f, 0.8f, 0.95f);
            t.waterTop = Color(0.1f, 0.28f, 0.48f);
            t.waterBottom = Color(0.03f, 0.1f, 0.24f);
            t.mountainFar = Color(0.32f, 0.34f, 0.36f);
            t.mountainNear = Color(0.24f, 0.26f, 0.28f);
            t.sunCore = Color(1.0f, 1.0f, 0.95f);
            t.sunEdge = Color(1.0f, 0.88f, 0.45f);
            t.cloud = Color(1.0f, 1.0f, 1.0f);
            t.cloudShadow = Color(0.75f, 0.78f, 0.85f);
            t.bridgeSteel = Color(0.5f, 0.52f, 0.55f);
            t.bridgeDark = Color(0.28f, 0.29f, 0.32f);
            t.bridgeRoad = Color(0.17f, 0.17f, 0.19f);
            t.bridgeRail = Color(0.36f, 0.34f, 0.3f);
            t.sunx = 0.05f; t.suny = 0.86f; t.sunr = 0.115f;
            t.sunStr = 1.0f;
            t.moonx = -0.8f; t.moony = 0.4f; t.moonr = 0.03f; t.moonStr = 0.0f;
            t.stars = 0.0f;
            t.ambient = 1.05f;
            break;
        case 3: // Afternoon
            t.skyTop = Color(0.2f, 0.28f, 0.55f);
            t.skyMid = Color(0.55f, 0.55f, 0.85f);
            t.skyBottom = Color(0.88f, 0.72f, 0.55f);
            t.skyBand = Color(0.95f, 0.78f, 0.5f);
            t.waterTop = Color(0.14f, 0.26f, 0.44f);
            t.waterBottom = Color(0.04f, 0.09f, 0.22f);
            t.mountainFar = Color(0.34f, 0.28f, 0.38f);
            t.mountainNear = Color(0.25f, 0.2f, 0.3f);
            t.sunCore = Color(1.0f, 0.95f, 0.78f);
            t.sunEdge = Color(1.0f, 0.55f, 0.18f);
            t.cloud = Color(0.98f, 0.94f, 0.9f);
            t.cloudShadow = Color(0.68f, 0.62f, 0.65f);
            t.bridgeSteel = Color(0.45f, 0.48f, 0.52f);
            t.bridgeDark = Color(0.25f, 0.27f, 0.3f);
            t.bridgeRoad = Color(0.18f, 0.18f, 0.2f);
            t.bridgeRail = Color(0.35f, 0.32f, 0.28f);
            t.sunx = 0.72f; t.suny = 0.48f; t.sunr = 0.11f;
            t.sunStr = 0.98f;
            t.moonx = -0.65f; t.moony = 0.55f; t.moonr = 0.035f; t.moonStr = 0.0f;
            t.stars = 0.0f;
            t.ambient = 0.92f;
            break;
        case 4: // Sunset (original mood)
            t.skyTop = Color(0.15f, 0.05f, 0.25f);
            t.skyMid = Color(0.85f, 0.35f, 0.2f);
            t.skyBottom = Color(1.0f, 0.7f, 0.3f);
            t.skyBand = Color(1.0f, 0.55f, 0.28f);
            t.waterTop = Color(0.15f, 0.25f, 0.45f);
            t.waterBottom = Color(0.05f, 0.1f, 0.25f);
            t.mountainFar = Color(0.35f, 0.25f, 0.4f);
            t.mountainNear = Color(0.25f, 0.18f, 0.3f);
            t.sunCore = Color(1.0f, 1.0f, 0.9f);
            t.sunEdge = Color(1.0f, 0.6f, 0.0f);
            t.cloud = Color(1.0f, 0.95f, 0.9f);
            t.cloudShadow = Color(0.7f, 0.6f, 0.65f);
            t.bridgeSteel = Color(0.45f, 0.48f, 0.52f);
            t.bridgeDark = Color(0.25f, 0.27f, 0.3f);
            t.bridgeRoad = Color(0.18f, 0.18f, 0.2f);
            t.bridgeRail = Color(0.35f, 0.32f, 0.28f);
            t.sunx = 0.65f; t.suny = 0.55f; t.sunr = 0.12f;
            t.sunStr = 1.0f;
            t.moonx = -0.55f; t.moony = 0.62f; t.moonr = 0.04f; t.moonStr = 0.05f;
            t.stars = 0.08f;
            t.ambient = 1.0f;
            break;
        case 5: // Dusk
            t.skyTop = Color(0.08f, 0.06f, 0.22f);
            t.skyMid = Color(0.35f, 0.22f, 0.42f);
            t.skyBottom = Color(0.65f, 0.35f, 0.35f);
            t.skyBand = Color(0.45f, 0.28f, 0.38f);
            t.waterTop = Color(0.1f, 0.16f, 0.32f);
            t.waterBottom = Color(0.02f, 0.05f, 0.14f);
            t.mountainFar = Color(0.22f, 0.16f, 0.28f);
            t.mountainNear = Color(0.16f, 0.12f, 0.22f);
            t.sunCore = Color(0.95f, 0.45f, 0.25f);
            t.sunEdge = Color(0.55f, 0.2f, 0.15f);
            t.cloud = Color(0.55f, 0.48f, 0.55f);
            t.cloudShadow = Color(0.35f, 0.3f, 0.38f);
            t.bridgeSteel = Color(0.38f, 0.4f, 0.44f);
            t.bridgeDark = Color(0.22f, 0.23f, 0.26f);
            t.bridgeRoad = Color(0.16f, 0.16f, 0.18f);
            t.bridgeRail = Color(0.3f, 0.28f, 0.25f);
            t.sunx = 0.92f; t.suny = 0.22f; t.sunr = 0.09f;
            t.sunStr = 0.45f;
            t.moonx = -0.58f; t.moony = 0.68f; t.moonr = 0.06f; t.moonStr = 0.55f;
            t.stars = 0.45f;
            t.ambient = 0.55f;
            break;
        case 6: // Night
            t.skyTop = Color(0.02f, 0.03f, 0.1f);
            t.skyMid = Color(0.05f, 0.08f, 0.18f);
            t.skyBottom = Color(0.08f, 0.12f, 0.22f);
            t.skyBand = Color(0.1f, 0.14f, 0.26f);
            t.waterTop = Color(0.04f, 0.08f, 0.18f);
            t.waterBottom = Color(0.01f, 0.03f, 0.08f);
            t.mountainFar = Color(0.12f, 0.1f, 0.16f);
            t.mountainNear = Color(0.08f, 0.07f, 0.12f);
            t.sunCore = Color(0.3f, 0.25f, 0.2f);
            t.sunEdge = Color(0.15f, 0.1f, 0.08f);
            t.cloud = Color(0.22f, 0.22f, 0.28f);
            t.cloudShadow = Color(0.12f, 0.12f, 0.16f);
            t.bridgeSteel = Color(0.32f, 0.34f, 0.38f);
            t.bridgeDark = Color(0.18f, 0.19f, 0.22f);
            t.bridgeRoad = Color(0.14f, 0.14f, 0.16f);
            t.bridgeRail = Color(0.26f, 0.24f, 0.22f);
            t.sunx = 1.15f; t.suny = -0.2f; t.sunr = 0.06f;
            t.sunStr = 0.0f;
            t.moonx = -0.58f; t.moony = 0.72f; t.moonr = 0.07f; t.moonStr = 1.0f;
            t.stars = 1.0f;
            t.ambient = 0.46f;
            break;
        default:
            getTimePreset(4, t);
            break;
    }
}

void rebuildPalette() {
    TimePresetData T;
    getTimePreset(g_timeIndex, T);
    Palette::skyTop = T.skyTop;
    Palette::skyMid = T.skyMid;
    Palette::skyBottom = T.skyBottom;
    Palette::skyBand = T.skyBand;
    Palette::waterTop = T.waterTop;
    Palette::waterBottom = T.waterBottom;
    Palette::mountainFar = T.mountainFar;
    Palette::mountainNear = T.mountainNear;
    Palette::sunCore = T.sunCore;
    Palette::sunEdge = T.sunEdge;
    Palette::cloud = T.cloud;
    Palette::cloudShadow = T.cloudShadow;
    Palette::bridgeSteel = T.bridgeSteel;
    Palette::bridgeDark = T.bridgeDark;
    Palette::bridgeRoad = T.bridgeRoad;
    Palette::bridgeRail = T.bridgeRail;
    sunX = T.sunx;
    sunY = T.suny;
    sunRadius = T.sunr;
    sunStrength = T.sunStr;
    moonX = T.moonx;
    moonY = T.moony;
    moonRadius = T.moonr;
    moonStrength = T.moonStr;
    starVisibility = T.stars;
    ambientScene = T.ambient;

    float gmix = 0.0f;
    windStrength = 0.12f;
    rainIntensity = 0.0f;
    snowIntensity = 0.0f;
    switch (g_weatherIndex) {
        case 1:
            windStrength = 0.65f;
            gmix = 0.06f;
            break;
        case 2:
            windStrength = 0.42f;
            rainIntensity = 0.75f;
            gmix = 0.24f;
            sunStrength *= 0.65f;
            break;
        case 3:
            windStrength = 0.3f;
            snowIntensity = 0.92f;
            gmix = 0.2f;
            sunStrength *= 0.7f;
            break;
        case 4:
            windStrength = 0.18f;
            gmix = 0.12f;
            sunStrength *= 0.5f;
            moonStrength *= 0.65f;
            break;
        case 5:
            windStrength = 0.88f;
            rainIntensity = 1.0f;
            gmix = 0.3f;
            sunStrength *= 0.32f;
            moonStrength *= 0.38f;
            break;
        default:
            break;
    }
    mixTowardGray(Palette::skyTop, gmix);
    mixTowardGray(Palette::skyMid, gmix);
    mixTowardGray(Palette::skyBottom, gmix);
    mixTowardGray(Palette::skyBand, gmix);
    mixTowardGray(Palette::cloud, gmix * 0.85f);
    mixTowardGray(Palette::cloudShadow, gmix);
    mixTowardGray(Palette::mountainFar, gmix * 0.9f);
    mixTowardGray(Palette::mountainNear, gmix * 0.85f);
    mixTowardGray(Palette::waterTop, gmix * 0.7f);
    mixTowardGray(Palette::waterBottom, gmix * 0.65f);

    if (g_weatherIndex == 5) {
        float pulse = 0.5f + 0.5f * sinf(timeVal * 0.5f);
        rainIntensity *= (0.74f + 0.34f * pulse);
    }

    g_nightBridgeLight = fmaxf(0.0f, 1.0f - sunStrength * 2.3f) * fminf(1.0f, (1.0f - ambientScene) * 1.85f + starVisibility * 0.22f);
    if (g_timeIndex == 6) {
        g_nightBridgeLight = fmaxf(g_nightBridgeLight, 0.72f);
    } else if (g_timeIndex == 5) {
        g_nightBridgeLight = fmaxf(g_nightBridgeLight * 0.55f, 0.28f);
    }
}

// Utility functions
float getCloudSunOverlap(float cloudX, float cloudY, float cloudWidth) {
    float dx = fabs(cloudX - sunX);
    float dy = fabs(cloudY - sunY);
    float distance = sqrt(dx * dx + dy * dy);
    float overlapThreshold = sunRadius + cloudWidth * 0.4f;
    if (distance < overlapThreshold) {
        return 1.0f - (distance / overlapThreshold);
    }
    return 0.0f;
}

void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b, float a = 1.0f) {
    glColor4f(r * brightness, g * brightness, b * brightness, a);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1); glVertex2f(x2, y1);
    glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

void drawGradientRect(float x1, float y1, float x2, float y2, Color top, Color bottom) {
    const int bands = 56;
    float bmul = brightness;
    for (int i = 0; i < bands; i++) {
        float f0 = i / (float)bands;
        float f1 = (i + 1) / (float)bands;
        float yLo = y1 + f0 * (y2 - y1);
        float yHi = y1 + f1 * (y2 - y1);
        float r0 = bottom.r + (top.r - bottom.r) * f0;
        float g0 = bottom.g + (top.g - bottom.g) * f0;
        float b0 = bottom.b + (top.b - bottom.b) * f0;
        float r1 = bottom.r + (top.r - bottom.r) * f1;
        float g1 = bottom.g + (top.g - bottom.g) * f1;
        float b1 = bottom.b + (top.b - bottom.b) * f1;
        glBegin(GL_QUADS);
        glColor3f(r0 * bmul, g0 * bmul, b0 * bmul);
        glVertex2f(x1, yLo);
        glVertex2f(x2, yLo);
        glColor3f(r1 * bmul, g1 * bmul, b1 * bmul);
        glVertex2f(x2, yHi);
        glVertex2f(x1, yHi);
        glEnd();
    }
}

void drawCircle(float cx, float cy, float radius, float r, float g, float b, float alpha = 1.0f) {
    glColor4f(r * brightness, g * brightness, b * brightness, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 64; i++) {
        float angle = i * 6.28318f / 64.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}

void drawGradientCircle(float cx, float cy, float radius, Color inner, Color outer, float alpha = 1.0f) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(inner.r, inner.g, inner.b, alpha);
    glVertex2f(cx, cy);
    glColor4f(outer.r, outer.g, outer.b, 0.0f);
    for (int i = 0; i <= 64; i++) {
        float angle = i * 6.28318f / 64.0f;
        glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
    }
    glEnd();
}

void rotatePoint(float &px, float &py, float cx, float cy, float angleRad) {
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    float dx = px - cx;
    float dy = py - cy;
    px = cx + dx * cosA - dy * sinA;
    py = cy + dx * sinA + dy * cosA;
}

// ==================== VEHICLE FUNCTIONS ====================

void drawCar(float x, float y, float scale, Color bodyColor, bool facingRight = true) {
    float dir = facingRight ? 1.0f : -1.0f;

    // Car shadow
    drawRect(x - 0.04f * scale, y - 0.012f * scale,
             x + 0.04f * scale, y - 0.008f * scale, 0.1f, 0.1f, 0.1f, 0.3f);

    // Car body (main)
    glColor4f(bodyColor.r * brightness, bodyColor.g * brightness, bodyColor.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.035f * scale, y);
    glVertex2f(x + 0.035f * scale, y);
    glVertex2f(x + 0.038f * scale, y + 0.012f * scale);
    glVertex2f(x + 0.03f * scale, y + 0.015f * scale);
    glVertex2f(x - 0.03f * scale, y + 0.015f * scale);
    glVertex2f(x - 0.038f * scale, y + 0.012f * scale);
    glEnd();

    // Car roof/cabin
    glColor4f(bodyColor.r * 0.85f * brightness, bodyColor.g * 0.85f * brightness,
              bodyColor.b * 0.85f * brightness, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.02f * scale, y + 0.015f * scale);
    glVertex2f(x + 0.018f * scale, y + 0.015f * scale);
    glVertex2f(x + 0.012f * scale, y + 0.028f * scale);
    glVertex2f(x - 0.014f * scale, y + 0.028f * scale);
    glEnd();

    // Windows
    glColor4f(0.6f * brightness, 0.8f * brightness, 0.95f * brightness, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(x + 0.005f * scale * dir, y + 0.016f * scale);
    glVertex2f(x + 0.016f * scale * dir, y + 0.016f * scale);
    glVertex2f(x + 0.011f * scale * dir, y + 0.026f * scale);
    glVertex2f(x + 0.006f * scale * dir, y + 0.026f * scale);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x - 0.018f * scale * dir, y + 0.016f * scale);
    glVertex2f(x - 0.003f * scale * dir, y + 0.016f * scale);
    glVertex2f(x - 0.004f * scale * dir, y + 0.026f * scale);
    glVertex2f(x - 0.013f * scale * dir, y + 0.026f * scale);
    glEnd();

    // Wheels
    drawCircle(x - 0.022f * scale, y, 0.008f * scale, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 0.022f * scale, y, 0.008f * scale, 0.15f, 0.15f, 0.15f);
    drawCircle(x - 0.022f * scale, y, 0.004f * scale, 0.6f, 0.6f, 0.65f);
    drawCircle(x + 0.022f * scale, y, 0.004f * scale, 0.6f, 0.6f, 0.65f);

    // Headlights (+ soft beams at night)
    float nBeam = fmaxf(0.0f, g_nightBridgeLight - 0.12f);
    if (facingRight) {
        drawCircle(x + 0.036f * scale, y + 0.008f * scale, 0.003f * scale, 1.0f, 1.0f, 0.8f);
        drawCircle(x - 0.036f * scale, y + 0.008f * scale, 0.003f * scale, 0.9f, 0.1f, 0.1f);
        if (nBeam > 0.02f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            float rr = (0.018f + nBeam * 0.07f) * scale;
            drawCircle(x + 0.036f * scale, y + 0.008f * scale, rr, 1.0f, 0.95f, 0.7f, 0.22f * nBeam);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    } else {
        drawCircle(x - 0.036f * scale, y + 0.008f * scale, 0.003f * scale, 1.0f, 1.0f, 0.8f);
        drawCircle(x + 0.036f * scale, y + 0.008f * scale, 0.003f * scale, 0.9f, 0.1f, 0.1f);
        if (nBeam > 0.02f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            float rr = (0.018f + nBeam * 0.07f) * scale;
            drawCircle(x - 0.036f * scale, y + 0.008f * scale, rr, 1.0f, 0.95f, 0.7f, 0.22f * nBeam);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void drawBus(float x, float y, float scale, Color bodyColor, bool facingRight = true) {
    float dir = facingRight ? 1.0f : -1.0f;

    // Bus shadow
    drawRect(x - 0.08f * scale, y - 0.012f * scale,
             x + 0.08f * scale, y - 0.006f * scale, 0.1f, 0.1f, 0.1f, 0.3f);

    // Bus body
    glColor4f(bodyColor.r * brightness, bodyColor.g * brightness, bodyColor.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.075f * scale, y);
    glVertex2f(x + 0.075f * scale, y);
    glVertex2f(x + 0.078f * scale, y + 0.035f * scale);
    glVertex2f(x - 0.078f * scale, y + 0.035f * scale);
    glEnd();

    // Bus roof
    glColor4f(bodyColor.r * 0.7f * brightness, bodyColor.g * 0.7f * brightness,
              bodyColor.b * 0.7f * brightness, 1.0f);
    drawRect(x - 0.076f * scale, y + 0.035f * scale,
             x + 0.076f * scale, y + 0.042f * scale,
             bodyColor.r * 0.7f, bodyColor.g * 0.7f, bodyColor.b * 0.7f);

    // Windows strip
    glColor4f(0.55f * brightness, 0.75f * brightness, 0.9f * brightness, 0.9f);
    drawRect(x - 0.07f * scale, y + 0.015f * scale,
             x + 0.06f * scale, y + 0.032f * scale,
             0.55f, 0.75f, 0.9f, 0.9f);

    // Window dividers
    glColor4f(bodyColor.r * 0.8f * brightness, bodyColor.g * 0.8f * brightness,
              bodyColor.b * 0.8f * brightness, 1.0f);
    for (float wx = -0.055f; wx < 0.06f; wx += 0.025f) {
        drawRect(x + wx * scale, y + 0.015f * scale,
                 x + (wx + 0.004f) * scale, y + 0.032f * scale,
                 bodyColor.r * 0.8f, bodyColor.g * 0.8f, bodyColor.b * 0.8f);
    }

    // Front windshield
    glColor4f(0.5f * brightness, 0.7f * brightness, 0.85f * brightness, 0.9f);
    float frontX = facingRight ? 0.062f : -0.062f;
    glBegin(GL_QUADS);
    glVertex2f(x + frontX * scale, y + 0.01f * scale);
    glVertex2f(x + (frontX + 0.014f * dir) * scale, y + 0.01f * scale);
    glVertex2f(x + (frontX + 0.012f * dir) * scale, y + 0.032f * scale);
    glVertex2f(x + frontX * scale, y + 0.032f * scale);
    glEnd();

    // Wheels
    drawCircle(x - 0.05f * scale, y, 0.012f * scale, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 0.05f * scale, y, 0.012f * scale, 0.12f, 0.12f, 0.12f);
    drawCircle(x - 0.05f * scale, y, 0.006f * scale, 0.5f, 0.5f, 0.55f);
    drawCircle(x + 0.05f * scale, y, 0.006f * scale, 0.5f, 0.5f, 0.55f);

    // Lights
    float headX = facingRight ? 0.076f : -0.076f;
    drawCircle(x + headX * scale, y + 0.025f * scale, 0.004f * scale, 1.0f, 1.0f, 0.8f);
    drawCircle(x + headX * scale, y + 0.01f * scale, 0.004f * scale, 1.0f, 1.0f, 0.8f);
    float nBeam = fmaxf(0.0f, g_nightBridgeLight - 0.12f);
    if (nBeam > 0.02f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float rr = (0.02f + nBeam * 0.08f) * scale;
        drawCircle(x + headX * scale, y + 0.018f * scale, rr, 1.0f, 0.94f, 0.65f, 0.2f * nBeam);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    float tailX = facingRight ? -0.076f : 0.076f;
    drawCircle(x + tailX * scale, y + 0.025f * scale, 0.004f * scale, 0.9f, 0.15f, 0.1f);
    drawCircle(x + tailX * scale, y + 0.01f * scale, 0.004f * scale, 0.9f, 0.15f, 0.1f);
}

void drawTrainEngine(float x, float y, float scale) {
    // Engine shadow
    drawRect(x - 0.1f * scale, y - 0.01f * scale,
             x + 0.1f * scale, y - 0.005f * scale, 0.05f, 0.05f, 0.05f, 0.4f);

    Color engineColor(0.15f, 0.35f, 0.55f);

    // Main engine body
    glColor4f(engineColor.r * brightness, engineColor.g * brightness, engineColor.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.09f * scale, y);
    glVertex2f(x + 0.08f * scale, y);
    glVertex2f(x + 0.1f * scale, y + 0.015f * scale);
    glVertex2f(x + 0.1f * scale, y + 0.04f * scale);
    glVertex2f(x - 0.09f * scale, y + 0.04f * scale);
    glEnd();

    // Cab
    glColor4f(engineColor.r * 0.8f * brightness, engineColor.g * 0.8f * brightness,
              engineColor.b * 0.8f * brightness, 1.0f);
    drawRect(x - 0.09f * scale, y + 0.04f * scale,
             x - 0.03f * scale, y + 0.065f * scale,
             engineColor.r * 0.8f, engineColor.g * 0.8f, engineColor.b * 0.8f);

    // Cab roof
    glColor4f(0.2f * brightness, 0.2f * brightness, 0.22f * brightness, 1.0f);
    drawRect(x - 0.095f * scale, y + 0.065f * scale,
             x - 0.025f * scale, y + 0.072f * scale, 0.2f, 0.2f, 0.22f);

    // Cab windows
    glColor4f(0.5f * brightness, 0.7f * brightness, 0.85f * brightness, 0.9f);
    drawRect(x - 0.085f * scale, y + 0.045f * scale,
             x - 0.055f * scale, y + 0.06f * scale, 0.5f, 0.7f, 0.85f, 0.9f);
    drawRect(x - 0.05f * scale, y + 0.045f * scale,
             x - 0.035f * scale, y + 0.06f * scale, 0.5f, 0.7f, 0.85f, 0.9f);

    // Engine vents
    glColor4f(0.1f * brightness, 0.1f * brightness, 0.12f * brightness, 1.0f);
    for (float vx = -0.02f; vx < 0.07f; vx += 0.015f) {
        drawRect(x + vx * scale, y + 0.025f * scale,
                 x + (vx + 0.008f) * scale, y + 0.038f * scale, 0.1f, 0.1f, 0.12f);
    }

    // Yellow stripe
    glColor4f(0.95f * brightness, 0.8f * brightness, 0.1f * brightness, 1.0f);
    drawRect(x - 0.09f * scale, y + 0.018f * scale,
             x + 0.1f * scale, y + 0.023f * scale, 0.95f, 0.8f, 0.1f);

    // Headlight
    drawCircle(x + 0.095f * scale, y + 0.03f * scale, 0.008f * scale, 1.0f, 1.0f, 0.7f);
    drawCircle(x + 0.095f * scale, y + 0.03f * scale, 0.015f * scale, 1.0f, 1.0f, 0.5f, 0.3f);

    // Wheels
    for (float wx = -0.07f; wx <= 0.07f; wx += 0.035f) {
        drawCircle(x + wx * scale, y, 0.012f * scale, 0.1f, 0.1f, 0.1f);
        drawCircle(x + wx * scale, y, 0.007f * scale, 0.4f, 0.4f, 0.42f);
    }
}

void drawTrainCarriage(float x, float y, float scale, Color carriageColor, bool isPassenger = true) {
    // Shadow
    drawRect(x - 0.09f * scale, y - 0.01f * scale,
             x + 0.09f * scale, y - 0.005f * scale, 0.05f, 0.05f, 0.05f, 0.4f);

    // Main carriage body
    glColor4f(carriageColor.r * brightness, carriageColor.g * brightness,
              carriageColor.b * brightness, 1.0f);
    drawRect(x - 0.085f * scale, y,
             x + 0.085f * scale, y + 0.045f * scale,
             carriageColor.r, carriageColor.g, carriageColor.b);

    // Roof
    glColor4f(carriageColor.r * 0.7f * brightness, carriageColor.g * 0.7f * brightness,
              carriageColor.b * 0.7f * brightness, 1.0f);
    drawRect(x - 0.088f * scale, y + 0.045f * scale,
             x + 0.088f * scale, y + 0.055f * scale,
             carriageColor.r * 0.7f, carriageColor.g * 0.7f, carriageColor.b * 0.7f);

    if (isPassenger) {
        // Windows
        glColor4f(0.5f * brightness, 0.7f * brightness, 0.85f * brightness, 0.9f);
        for (float wx = -0.075f; wx < 0.08f; wx += 0.03f) {
            drawRect(x + wx * scale, y + 0.015f * scale,
                     x + (wx + 0.022f) * scale, y + 0.038f * scale, 0.5f, 0.7f, 0.85f, 0.9f);
        }
    }

    // Stripe
    glColor4f(0.95f * brightness, 0.8f * brightness, 0.1f * brightness, 1.0f);
    drawRect(x - 0.085f * scale, y + 0.008f * scale,
             x + 0.085f * scale, y + 0.012f * scale, 0.95f, 0.8f, 0.1f);

    // Wheels
    for (float wx = -0.06f; wx <= 0.06f; wx += 0.04f) {
        drawCircle(x + wx * scale, y, 0.01f * scale, 0.1f, 0.1f, 0.1f);
        drawCircle(x + wx * scale, y, 0.006f * scale, 0.4f, 0.4f, 0.42f);
    }

    // Connectors
    glColor4f(0.25f * brightness, 0.25f * brightness, 0.27f * brightness, 1.0f);
    drawRect(x - 0.095f * scale, y + 0.015f * scale,
             x - 0.085f * scale, y + 0.025f * scale, 0.25f, 0.25f, 0.27f);
    drawRect(x + 0.085f * scale, y + 0.015f * scale,
             x + 0.095f * scale, y + 0.025f * scale, 0.25f, 0.25f, 0.27f);
}

void drawTrain(float x, float y, float scale) {
    drawTrainEngine(x, y, scale);

    Color carriageColors[] = {
        Color(0.2f, 0.4f, 0.6f),
        Color(0.2f, 0.4f, 0.6f),
        Color(0.6f, 0.25f, 0.2f),
        Color(0.2f, 0.4f, 0.6f),
    };

    for (int i = 0; i < 4; i++) {
        float carriageX = x - 0.2f * scale - i * 0.19f * scale;
        drawTrainCarriage(carriageX, y, scale, carriageColors[i], true);
    }
}

// ==================== BRIDGE FUNCTIONS ====================

void drawBridgeTower(float x, float baseY, float topY, float width) {
    // Main tower structure
    glColor4f(Palette::bridgeSteel.r * brightness, Palette::bridgeSteel.g * brightness,
              Palette::bridgeSteel.b * brightness, 1.0f);

    // Tower legs
    float legOffset = 0.015f;
    glBegin(GL_QUADS);
    glVertex2f(x - width * 0.5f - legOffset, baseY);
    glVertex2f(x - width * 0.3f, baseY);
    glVertex2f(x - width * 0.35f, topY);
    glVertex2f(x - width * 0.5f, topY);
    glVertex2f(x + width * 0.3f, baseY);
    glVertex2f(x + width * 0.5f + legOffset, baseY);
    glVertex2f(x + width * 0.5f, topY);
    glVertex2f(x + width * 0.35f, topY);
    glEnd();

    // Cross braces
    glColor4f(Palette::bridgeDark.r * brightness, Palette::bridgeDark.g * brightness,
              Palette::bridgeDark.b * brightness, 1.0f);
    glLineWidth(2.0f);

    float braceSpacing = (topY - baseY) / 4.0f;
    for (int i = 1; i < 4; i++) {
        float braceY = baseY + i * braceSpacing;
        float leftX = x - width * 0.5f + (i * 0.02f);
        float rightX = x + width * 0.5f - (i * 0.02f);

        glBegin(GL_LINES);
        glVertex2f(leftX + 0.01f, braceY);
        glVertex2f(rightX - 0.01f, braceY);
        glEnd();

        if (i < 3) {
            glBegin(GL_LINES);
            glVertex2f(leftX + 0.01f, braceY);
            glVertex2f(rightX - 0.01f, braceY + braceSpacing);
            glVertex2f(rightX - 0.01f, braceY);
            glVertex2f(leftX + 0.01f, braceY + braceSpacing);
            glEnd();
        }
    }

    // Tower top
    glColor4f(Palette::bridgeSteel.r * 0.9f * brightness, Palette::bridgeSteel.g * 0.9f * brightness,
              Palette::bridgeSteel.b * 0.9f * brightness, 1.0f);
    drawRect(x - width * 0.55f, topY, x + width * 0.55f, topY + 0.025f,
             Palette::bridgeSteel.r * 0.9f, Palette::bridgeSteel.g * 0.9f, Palette::bridgeSteel.b * 0.9f);

    // Warning / aviation lights (stronger at night)
    float lightBlink = (sin(timeVal * 3) > 0) ? 1.0f : 0.5f;
    float nb = 0.4f + 0.6f * g_nightBridgeLight;
    drawCircle(x, topY + 0.03f, 0.008f, 1.0f * lightBlink * nb, 0.22f * lightBlink * nb, 0.12f * lightBlink * nb);
    if (g_nightBridgeLight > 0.12f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.12f, 0.28f * g_nightBridgeLight * lightBlink * brightness);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, topY + 0.03f);
        glColor4f(1.0f, 0.5f, 0.2f, 0.0f);
        for (int i = 0; i <= 24; i++) {
            float ang = i * 6.28318f / 24.0f;
            glVertex2f(x + 0.045f * cosf(ang), topY + 0.03f + 0.045f * sinf(ang));
        }
        glEnd();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void drawBridge() {
    // Bridge dimensions - TRAIN BELOW, ROAD ABOVE
    float trainY = 0.0f;          // Train level (lower deck)
    float roadY = 0.1f;           // Road level (upper deck)
    float bridgeBottom = -0.65f;  // Water level supports

    // Main support pillars in water
    float pillarPositions[] = {-0.7f, 0.0f, 0.7f};

    for (float px : pillarPositions) {
        // Pillar base
        glColor4f(0.35f * brightness, 0.33f * brightness, 0.3f * brightness, 1.0f);
        glBegin(GL_POLYGON);
        glVertex2f(px - 0.06f, bridgeBottom);
        glVertex2f(px + 0.06f, bridgeBottom);
        glVertex2f(px + 0.05f, trainY - 0.02f);
        glVertex2f(px - 0.05f, trainY - 0.02f);
        glEnd();

        // Pillar top detail
        drawRect(px - 0.055f, trainY - 0.025f, px + 0.055f, trainY - 0.015f, 0.4f, 0.38f, 0.35f);
    }

    // === LOWER DECK (RAILWAY) ===

    // Railway support beams
    glColor4f(Palette::bridgeDark.r * brightness, Palette::bridgeDark.g * brightness,
              Palette::bridgeDark.b * brightness, 1.0f);
    drawRect(-1.0f, trainY - 0.02f, 1.0f, trainY,
             Palette::bridgeDark.r, Palette::bridgeDark.g, Palette::bridgeDark.b);

    // Railway deck
    glColor4f(Palette::bridgeRail.r * brightness, Palette::bridgeRail.g * brightness,
              Palette::bridgeRail.b * brightness, 1.0f);
    drawRect(-1.0f, trainY, 1.0f, trainY + 0.015f,
             Palette::bridgeRail.r, Palette::bridgeRail.g, Palette::bridgeRail.b);

    // Railway ballast
    glColor4f(0.4f * brightness, 0.38f * brightness, 0.35f * brightness, 1.0f);
    drawRect(-1.0f, trainY + 0.015f, 1.0f, trainY + 0.025f, 0.4f, 0.38f, 0.35f);

    // Rails
    glColor4f(0.5f * brightness, 0.48f * brightness, 0.45f * brightness, 1.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, trainY + 0.027f);
    glVertex2f(1.0f, trainY + 0.027f);
    glVertex2f(-1.0f, trainY + 0.022f);
    glVertex2f(1.0f, trainY + 0.022f);
    glEnd();

    // Rail ties
    glColor4f(0.3f * brightness, 0.25f * brightness, 0.2f * brightness, 1.0f);
    for (float tx = -0.98f; tx < 1.0f; tx += 0.04f) {
        drawRect(tx, trainY + 0.018f, tx + 0.025f, trainY + 0.03f, 0.3f, 0.25f, 0.2f);
    }

    // Railway safety barriers
    glColor4f(0.55f * brightness, 0.55f * brightness, 0.58f * brightness, 1.0f);
    drawRect(-1.0f, trainY + 0.03f, 1.0f, trainY + 0.035f, 0.55f, 0.55f, 0.58f);

    // === VERTICAL SUPPORTS BETWEEN DECKS ===

    glColor4f(Palette::bridgeSteel.r * brightness, Palette::bridgeSteel.g * brightness,
              Palette::bridgeSteel.b * brightness, 1.0f);

    for (float sx = -0.9f; sx <= 0.9f; sx += 0.12f) {
        drawRect(sx - 0.006f, trainY + 0.035f, sx + 0.006f, roadY - 0.005f,
                 Palette::bridgeSteel.r, Palette::bridgeSteel.g, Palette::bridgeSteel.b);
    }

    // Diagonal braces between decks
    glLineWidth(1.5f);
    glColor4f(Palette::bridgeDark.r * brightness, Palette::bridgeDark.g * brightness,
              Palette::bridgeDark.b * brightness, 1.0f);
    for (float sx = -0.85f; sx < 0.85f; sx += 0.12f) {
        glBegin(GL_LINES);
        glVertex2f(sx, trainY + 0.04f);
        glVertex2f(sx + 0.12f, roadY - 0.01f);
        glVertex2f(sx + 0.12f, trainY + 0.04f);
        glVertex2f(sx, roadY - 0.01f);
        glEnd();
    }

    // === UPPER DECK (ROAD) ===

    // Road support beams
    glColor4f(Palette::bridgeDark.r * brightness, Palette::bridgeDark.g * brightness,
              Palette::bridgeDark.b * brightness, 1.0f);
    drawRect(-1.0f, roadY - 0.015f, 1.0f, roadY,
             Palette::bridgeDark.r, Palette::bridgeDark.g, Palette::bridgeDark.b);

    // Road surface
    drawRect(-1.0f, roadY, 1.0f, roadY + 0.05f,
             Palette::bridgeRoad.r, Palette::bridgeRoad.g, Palette::bridgeRoad.b);

    // Road lane markings - center dashed yellow
    glColor4f(0.95f * brightness, 0.9f * brightness, 0.2f * brightness, 1.0f);
    for (float lx = -0.95f; lx < 1.0f; lx += 0.1f) {
        drawRect(lx, roadY + 0.024f, lx + 0.06f, roadY + 0.026f, 0.95f, 0.9f, 0.2f);
    }

    // Edge lines - solid white
    glColor4f(0.9f * brightness, 0.9f * brightness, 0.9f * brightness, 1.0f);
    drawRect(-1.0f, roadY + 0.003f, 1.0f, roadY + 0.005f, 0.9f, 0.9f, 0.9f);
    drawRect(-1.0f, roadY + 0.045f, 1.0f, roadY + 0.047f, 0.9f, 0.9f, 0.9f);

    // Road barriers/railings
    glColor4f(0.6f * brightness, 0.6f * brightness, 0.62f * brightness, 1.0f);
    drawRect(-1.0f, roadY + 0.048f, 1.0f, roadY + 0.058f, 0.6f, 0.6f, 0.62f);

    // Railing posts
    for (float rx = -0.95f; rx <= 0.95f; rx += 0.1f) {
        drawRect(rx - 0.004f, roadY + 0.048f, rx + 0.004f, roadY + 0.07f, 0.5f, 0.5f, 0.52f);
    }

    // Top railing
    drawRect(-1.0f, roadY + 0.068f, 1.0f, roadY + 0.072f, 0.55f, 0.55f, 0.58f);

    // Bridge towers
    drawBridgeTower(-0.7f, bridgeBottom, roadY + 0.18f, 0.12f);
    drawBridgeTower(0.7f, bridgeBottom, roadY + 0.18f, 0.12f);

    // Suspension cables
    glColor4f(0.2f * brightness, 0.2f * brightness, 0.22f * brightness, 1.0f);
    glLineWidth(3.5f);

    glBegin(GL_LINE_STRIP);
    for (float cx = -1.0f; cx <= 1.0f; cx += 0.02f) {
        float cableY;
        if (cx < -0.7f) {
            cableY = roadY + 0.18f - (cx + 1.0f) * 0.35f;
        } else if (cx > 0.7f) {
            cableY = roadY + 0.18f - (1.0f - cx) * 0.35f;
        } else {
            cableY = roadY + 0.08f + 0.12f * cosh(cx * 1.0f) - 0.12f;
        }
        glVertex2f(cx, cableY);
    }
    glEnd();

    // Vertical suspender cables
    glLineWidth(1.2f);
    for (float vx = -0.65f; vx <= 0.65f; vx += 0.08f) {
        float cableTopY = roadY + 0.08f + 0.12f * cosh(vx * 1.0f) - 0.12f;
        glBegin(GL_LINES);
        glVertex2f(vx, cableTopY);
        glVertex2f(vx, roadY + 0.072f);
        glEnd();
    }

    // Overhead electric lines for train
    glColor4f(0.15f * brightness, 0.15f * brightness, 0.17f * brightness, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, trainY + 0.085f);
    glVertex2f(1.0f, trainY + 0.085f);
    glEnd();

    // Catenary supports
    for (float cs = -0.8f; cs <= 0.8f; cs += 0.4f) {
        glBegin(GL_LINES);
        glVertex2f(cs, trainY + 0.035f);
        glVertex2f(cs, trainY + 0.085f);
        glEnd();
    }
}

void drawBridgeNightLighting(float roadY, float trainY) {
    if (g_nightBridgeLight < 0.022f) return;
    float L = g_nightBridgeLight * brightness;
    if (g_timeIndex == 6) {
        L *= 1.42f;
    }
    glEnable(GL_BLEND);

    static const float pillarPos[] = {-0.7f, 0.0f, 0.7f};
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (float px : pillarPos) {
        drawCircle(px, trainY - 0.015f, 0.09f, 1.0f, 0.58f, 0.12f, 0.38f * L);
        drawCircle(px, trainY - 0.015f, 0.05f, 1.0f, 0.75f, 0.25f, 0.55f * L);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (float lx = -0.92f; lx < 1.0f; lx += 0.13f) {
        glColor4f(0.32f * brightness, 0.32f * brightness, 0.36f * brightness, L);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(lx, roadY + 0.048f);
        glVertex2f(lx, roadY + 0.09f);
        glEnd();
        glLineWidth(1.0f);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (float lx = -0.92f; lx < 1.0f; lx += 0.13f) {
        drawCircle(lx, roadY + 0.094f, 0.011f, 1.0f, 0.95f, 0.65f, 0.75f * L);
        drawCircle(lx, roadY + 0.094f, 0.024f, 1.0f, 0.85f, 0.55f, 0.22f * L);
        glBegin(GL_QUADS);
        glColor4f(1.0f * L, 0.88f * L, 0.42f * L, 0.28f * L);
        glVertex2f(lx - 0.055f, roadY + 0.048f);
        glVertex2f(lx + 0.055f, roadY + 0.048f);
        glColor4f(0.75f * L, 0.62f * L, 0.28f * L, 0.0f);
        glVertex2f(lx + 0.09f, roadY + 0.008f);
        glVertex2f(lx - 0.09f, roadY + 0.008f);
        glEnd();
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.55f * L, 0.7f * L, 1.0f * L, 0.12f * L);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, trainY + 0.085f);
    glVertex2f(1.0f, trainY + 0.085f);
    glEnd();
    glLineWidth(1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void drawBridgeLightReflectionsOnWater() {
    if (g_nightBridgeLight < 0.06f) return;
    float L = g_nightBridgeLight * brightness;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (float lx = -0.92f; lx < 1.0f; lx += 0.13f) {
        for (int i = 0; i < 7; i++) {
            float yy = -0.22f - i * 0.016f - fmod(lx * 1.7f + timeVal * 0.4f, 0.02f);
            float ww = 0.018f + i * 0.008f;
            float wv = sinf(timeVal * 1.4f + lx * 6.0f + i) * 0.015f;
            float a = (0.16f - i * 0.02f) * L;
            if (a > 0.0f) {
                glColor4f(1.0f, 0.82f, 0.38f, a);
                glBegin(GL_QUADS);
                glVertex2f(lx - ww + wv, yy);
                glVertex2f(lx + ww + wv, yy);
                glVertex2f(lx + ww * 0.75f - wv, yy - 0.014f);
                glVertex2f(lx - ww * 0.75f - wv, yy - 0.014f);
                glEnd();
            }
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void drawBuoyLights() {
    if (g_nightBridgeLight < 0.12f) return;
    float L = g_nightBridgeLight * brightness;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    drawCircle(-0.52f, -0.44f, 0.016f, 1.0f, 0.25f, 0.15f, 0.55f * L);
    drawCircle(-0.52f, -0.44f, 0.028f, 1.0f, 0.35f, 0.2f, 0.15f * L);
    drawCircle(0.48f, -0.5f, 0.016f, 0.15f, 0.95f, 0.35f, 0.5f * L);
    drawCircle(0.48f, -0.5f, 0.028f, 0.25f, 0.85f, 0.3f, 0.14f * L);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ==================== BOAT FUNCTIONS (FIXED - SMOOTH MOTION) ====================

void drawBoatWake(float x, float y, float scale, float /*speed*/) {
    glEnable(GL_BLEND);
    for (int i = 0; i < 15; i++) {
        float dist = i * 0.025f * scale;
        float spread = i * 0.012f * scale;
        float alpha = (0.4f - i * 0.025f) * brightness;
        if (alpha > 0) {
            float waveOffset = sin(timeVal * 2 + i * 0.5f) * 0.003f;
            glColor4f(0.7f, 0.85f, 0.95f, alpha * 0.6f);
            glBegin(GL_LINES);
            glVertex2f(x - 0.1f * scale - dist, y + spread + waveOffset);
            glVertex2f(x - 0.1f * scale - dist - 0.02f, y + spread * 1.1f + waveOffset);
            glVertex2f(x - 0.1f * scale - dist, y - spread + waveOffset);
            glVertex2f(x - 0.1f * scale - dist - 0.02f, y - spread * 1.1f + waveOffset);
            glEnd();
        }
    }
    for (int i = 0; i < 8; i++) {
        float bx = x - 0.1f * scale - i * 0.015f + sin(timeVal * 1.5f + i) * 0.005f;
        float by = y + sin(timeVal * 2.5f + i * 1.5f) * 0.008f;
        float bsize = (0.008f - i * 0.0008f) * scale;
        float balpha = (0.5f - i * 0.05f) * brightness;
        if (balpha > 0 && bsize > 0) {
            drawCircle(bx, by, bsize, 0.85f, 0.92f, 0.98f, balpha);
        }
    }
}

void drawBoatReflection(float x, float y, float scale, Color hullColor) {
    float reflectY = y - 0.06f * scale;
    for (int i = 0; i < 8; i++) {
        float ry = reflectY - i * 0.012f * scale;
        float wave = sin(timeVal * 1.5f + i * 0.8f + x * 5) * 0.01f;
        float alpha = (0.35f - i * 0.04f) * brightness;
        float width = (0.08f - i * 0.008f) * scale;
        if (alpha > 0) {
            glColor4f(hullColor.r * 0.5f, hullColor.g * 0.5f, hullColor.b * 0.5f, alpha);
            glBegin(GL_QUADS);
            glVertex2f(x - width + wave, ry);
            glVertex2f(x + width + wave, ry);
            glVertex2f(x + width * 0.9f - wave, ry - 0.01f * scale);
            glVertex2f(x - width * 0.9f - wave, ry - 0.01f * scale);
            glEnd();
        }
    }
}

void drawSailboat(float x, float y, float scale) {
    // SMOOTH motion — amplified by wind
    float rockAngle = sinf(timeVal * 0.8f + x * 0.3f) * (2.5f + windStrength * 5.5f);
    float bob = sinf(timeVal * 0.6f + x * 0.2f) * (0.008f + windStrength * 0.012f) * scale;
    y += bob;
    float angleRad = rockAngle * 3.14159f / 180.0f;

    Color hullMain(0.45f, 0.25f, 0.12f);
    Color hullBottom(0.25f, 0.12f, 0.06f);
    Color deckColor(0.75f, 0.6f, 0.4f);
    Color sailMain(0.98f, 0.95f, 0.88f);
    Color mastColor(0.5f, 0.35f, 0.2f);
    Color flagColor(0.9f, 0.15f, 0.1f);

    drawBoatWake(x, y, scale, 1.0f);
    drawBoatReflection(x, y, scale, hullMain);

    float hullCenterX = x;
    float hullCenterY = y - 0.01f * scale;

    // Hull bottom
    glColor4f(hullBottom.r * brightness, hullBottom.g * brightness, hullBottom.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float hx = x + (-0.1f + t * 0.22f) * scale;
        float hy = y - 0.025f * scale - sin(t * 3.14159f) * 0.035f * scale;
        rotatePoint(hx, hy, hullCenterX, hullCenterY, angleRad);
        glVertex2f(hx, hy);
    }
    glEnd();

    // Main hull
    glColor4f(hullMain.r * brightness, hullMain.g * brightness, hullMain.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    float points[6][2] = {
        {x - 0.1f * scale, y + 0.015f * scale},
        {x + 0.12f * scale, y + 0.02f * scale},
        {x + 0.13f * scale, y - 0.01f * scale},
        {x + 0.1f * scale, y - 0.04f * scale},
        {x - 0.08f * scale, y - 0.04f * scale},
        {x - 0.11f * scale, y - 0.01f * scale}
    };
    for (int i = 0; i < 6; i++) {
        rotatePoint(points[i][0], points[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(points[i][0], points[i][1]);
    }
    glEnd();

    // Hull stripe
    glColor4f(0.6f * brightness, 0.4f * brightness, 0.2f * brightness, 1.0f);
    glLineWidth(2.0f);
    float stripe1x = x - 0.09f * scale, stripe1y = y;
    float stripe2x = x + 0.11f * scale, stripe2y = y + 0.005f * scale;
    rotatePoint(stripe1x, stripe1y, hullCenterX, hullCenterY, angleRad);
    rotatePoint(stripe2x, stripe2y, hullCenterX, hullCenterY, angleRad);
    glBegin(GL_LINES);
    glVertex2f(stripe1x, stripe1y);
    glVertex2f(stripe2x, stripe2y);
    glEnd();

    // Deck
    glColor4f(deckColor.r * brightness, deckColor.g * brightness, deckColor.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    float deckPoints[4][2] = {
        {x - 0.08f * scale, y + 0.015f * scale},
        {x + 0.1f * scale, y + 0.02f * scale},
        {x + 0.08f * scale, y + 0.025f * scale},
        {x - 0.06f * scale, y + 0.022f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(deckPoints[i][0], deckPoints[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(deckPoints[i][0], deckPoints[i][1]);
    }
    glEnd();

    // Cabin
    glColor4f(0.55f * brightness, 0.35f * brightness, 0.18f * brightness, 1.0f);
    glBegin(GL_QUADS);
    float cabinPoints[4][2] = {
        {x - 0.035f * scale, y + 0.022f * scale},
        {x + 0.02f * scale, y + 0.022f * scale},
        {x + 0.015f * scale, y + 0.05f * scale},
        {x - 0.03f * scale, y + 0.05f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(cabinPoints[i][0], cabinPoints[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(cabinPoints[i][0], cabinPoints[i][1]);
    }
    glEnd();

    // Cabin window
    glColor4f(0.5f * brightness, 0.7f * brightness, 0.85f * brightness, 0.9f);
    float winx1 = x - 0.02f * scale, winy1 = y + 0.03f * scale;
    float winx2 = x + 0.005f * scale, winy2 = y + 0.045f * scale;
    rotatePoint(winx1, winy1, hullCenterX, hullCenterY, angleRad);
    rotatePoint(winx2, winy2, hullCenterX, hullCenterY, angleRad);
    glBegin(GL_QUADS);
    glVertex2f(winx1, winy1);
    glVertex2f(winx2, winy1);
    glVertex2f(winx2, winy2);
    glVertex2f(winx1, winy2);
    glEnd();

    // Mast
    float mastX = x + 0.04f * scale;
    float mastTop = y + 0.18f * scale;
    float mastBottomX = mastX, mastBottomY = y + 0.022f * scale;
    float mastTopX = mastX, mastTopY = mastTop;
    rotatePoint(mastBottomX, mastBottomY, hullCenterX, hullCenterY, angleRad);
    rotatePoint(mastTopX, mastTopY, hullCenterX, hullCenterY, angleRad);

    glColor4f(mastColor.r * brightness, mastColor.g * brightness, mastColor.b * brightness, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(mastBottomX, mastBottomY);
    glVertex2f(mastTopX, mastTopY);
    glEnd();

    // Boom
    float boomX1 = mastX, boomY1 = y + 0.08f * scale;
    float boomX2 = mastX - 0.07f * scale, boomY2 = y + 0.065f * scale;
    rotatePoint(boomX1, boomY1, hullCenterX, hullCenterY, angleRad);
    rotatePoint(boomX2, boomY2, hullCenterX, hullCenterY, angleRad);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(boomX1, boomY1);
    glVertex2f(boomX2, boomY2);
    glEnd();

    // Sail
    float sailWave = sin(timeVal * 1.5f) * 0.012f * scale;
    glColor4f(sailMain.r * brightness, sailMain.g * brightness, sailMain.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 12; i++) {
        float t = i / 12.0f;
        float sx = mastX + t * 0.08f * scale + sin(t * 3.14159f) * sailWave;
        float sy = mastTop - 0.005f * scale - t * (mastTop - y - 0.045f * scale);
        rotatePoint(sx, sy, hullCenterX, hullCenterY, angleRad);
        glVertex2f(sx, sy);
    }
    glEnd();

    // Sail detail lines
    glColor4f(0.85f * brightness, 0.82f * brightness, 0.78f * brightness, 0.6f);
    glLineWidth(1.0f);
    for (int i = 1; i < 4; i++) {
        float t = i / 4.0f;
        float lx1 = mastX, ly1 = mastTop - t * (mastTop - y - 0.06f * scale);
        float lx2 = mastX + t * 0.06f * scale, ly2 = y + 0.06f * scale;
        rotatePoint(lx1, ly1, hullCenterX, hullCenterY, angleRad);
        rotatePoint(lx2, ly2, hullCenterX, hullCenterY, angleRad);
        glBegin(GL_LINES);
        glVertex2f(lx1, ly1);
        glVertex2f(lx2, ly2);
        glEnd();
    }

    // Flag
    float flagWave = sin(timeVal * 4.0f) * 0.006f * scale;
    glColor4f(flagColor.r * brightness, flagColor.g * brightness, flagColor.b * brightness, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(mastTopX, mastTopY);
    glVertex2f(mastTopX, mastTopY - 0.025f * scale);
    glVertex2f(mastTopX + 0.035f * scale + flagWave, mastTopY - 0.012f * scale);
    glEnd();

    // Rigging
    glColor4f(0.3f * brightness, 0.25f * brightness, 0.2f * brightness, 0.7f);
    glLineWidth(1.0f);
    float bowX = x + 0.12f * scale, bowY = y + 0.02f * scale;
    float sternX = x - 0.09f * scale, sternY = y + 0.015f * scale;
    rotatePoint(bowX, bowY, hullCenterX, hullCenterY, angleRad);
    rotatePoint(sternX, sternY, hullCenterX, hullCenterY, angleRad);
    glBegin(GL_LINES);
    glVertex2f(mastTopX, mastTopY);
    glVertex2f(bowX, bowY);
    glVertex2f(mastTopX, mastTopY);
    glVertex2f(sternX, sternY);
    glEnd();
}

void drawFishingBoat(float x, float y, float scale) {
    float rockAngle = sinf(timeVal * 0.7f + x * 0.25f) * (2.0f + windStrength * 4.5f);
    float bob = sinf(timeVal * 0.5f + x * 0.15f) * (0.007f + windStrength * 0.01f) * scale;
    y += bob;
    float angleRad = rockAngle * 3.14159f / 180.0f;

    Color hullMain(0.2f, 0.35f, 0.55f);
    Color hullAccent(0.85f, 0.85f, 0.8f);
    Color hullBottom(0.12f, 0.2f, 0.35f);
    Color deckColor(0.65f, 0.55f, 0.4f);
    Color cabinColor(0.9f, 0.88f, 0.82f);
    Color roofColor(0.15f, 0.25f, 0.4f);

    float hullCenterX = x;
    float hullCenterY = y - 0.01f * scale;

    drawBoatWake(x, y, scale * 0.8f, 0.7f);
    drawBoatReflection(x, y, scale, hullMain);

    // Hull bottom
    glColor4f(hullBottom.r * brightness, hullBottom.g * brightness, hullBottom.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 16; i++) {
        float t = i / 16.0f;
        float hx = x + (-0.08f + t * 0.18f) * scale;
        float hy = y - 0.02f * scale - sin(t * 3.14159f) * 0.025f * scale;
        rotatePoint(hx, hy, hullCenterX, hullCenterY, angleRad);
        glVertex2f(hx, hy);
    }
    glEnd();

    // Main hull
    glColor4f(hullMain.r * brightness, hullMain.g * brightness, hullMain.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    float hullPoints[6][2] = {
        {x - 0.08f * scale, y + 0.025f * scale},
        {x + 0.1f * scale, y + 0.025f * scale},
        {x + 0.1f * scale, y - 0.005f * scale},
        {x + 0.08f * scale, y - 0.03f * scale},
        {x - 0.06f * scale, y - 0.03f * scale},
        {x - 0.085f * scale, y - 0.005f * scale}
    };
    for (int i = 0; i < 6; i++) {
        rotatePoint(hullPoints[i][0], hullPoints[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(hullPoints[i][0], hullPoints[i][1]);
    }
    glEnd();

    // White stripe
    glColor4f(hullAccent.r * brightness, hullAccent.g * brightness, hullAccent.b * brightness, 1.0f);
    glBegin(GL_QUADS);
    float stripePoints[4][2] = {
        {x - 0.075f * scale, y + 0.008f * scale},
        {x + 0.095f * scale, y + 0.008f * scale},
        {x + 0.093f * scale, y - 0.002f * scale},
        {x - 0.078f * scale, y - 0.002f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(stripePoints[i][0], stripePoints[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(stripePoints[i][0], stripePoints[i][1]);
    }
    glEnd();

    // Red stripe
    glColor4f(0.7f * brightness, 0.15f * brightness, 0.1f * brightness, 1.0f);
    glBegin(GL_QUADS);
    float redStripe[4][2] = {
        {x - 0.04f * scale, y + 0.003f * scale},
        {x + 0.04f * scale, y + 0.003f * scale},
        {x + 0.038f * scale, y - 0.007f * scale},
        {x - 0.038f * scale, y - 0.007f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(redStripe[i][0], redStripe[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(redStripe[i][0], redStripe[i][1]);
    }
    glEnd();

    // Deck
    glColor4f(deckColor.r * brightness, deckColor.g * brightness, deckColor.b * brightness, 1.0f);
    glBegin(GL_POLYGON);
    float deckPoints[4][2] = {
        {x - 0.07f * scale, y + 0.025f * scale},
        {x + 0.09f * scale, y + 0.025f * scale},
        {x + 0.085f * scale, y + 0.032f * scale},
        {x - 0.065f * scale, y + 0.032f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(deckPoints[i][0], deckPoints[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(deckPoints[i][0], deckPoints[i][1]);
    }
    glEnd();

    // Deck planks
    glColor4f(deckColor.r * 0.8f * brightness, deckColor.g * 0.8f * brightness,
              deckColor.b * 0.8f * brightness, 1.0f);
    glLineWidth(1.0f);
    for (float dx = -0.05f; dx < 0.08f; dx += 0.015f) {
        float px1 = x + dx * scale, py1 = y + 0.026f * scale;
        float px2 = x + dx * scale, py2 = y + 0.031f * scale;
        rotatePoint(px1, py1, hullCenterX, hullCenterY, angleRad);
        rotatePoint(px2, py2, hullCenterX, hullCenterY, angleRad);
        glBegin(GL_LINES);
        glVertex2f(px1, py1);
        glVertex2f(px2, py2);
        glEnd();
    }

    // Cabin
    float cabinX = x - 0.01f * scale;
    glColor4f(cabinColor.r * brightness, cabinColor.g * brightness, cabinColor.b * brightness, 1.0f);
    glBegin(GL_QUADS);
    float cabinBase[4][2] = {
        {cabinX - 0.04f * scale, y + 0.032f * scale},
        {cabinX + 0.04f * scale, y + 0.032f * scale},
        {cabinX + 0.035f * scale, y + 0.07f * scale},
        {cabinX - 0.035f * scale, y + 0.07f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(cabinBase[i][0], cabinBase[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(cabinBase[i][0], cabinBase[i][1]);
    }
    glEnd();

    // Cabin roof
    glColor4f(roofColor.r * brightness, roofColor.g * brightness, roofColor.b * brightness, 1.0f);
    glBegin(GL_QUADS);
    float cabinRoof[4][2] = {
        {cabinX - 0.042f * scale, y + 0.07f * scale},
        {cabinX + 0.042f * scale, y + 0.07f * scale},
        {cabinX + 0.038f * scale, y + 0.082f * scale},
        {cabinX - 0.038f * scale, y + 0.082f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(cabinRoof[i][0], cabinRoof[i][1], hullCenterX, hullCenterY, angleRad);
        glVertex2f(cabinRoof[i][0], cabinRoof[i][1]);
    }
    glEnd();

    // Cabin windows
    glColor4f(0.5f * brightness, 0.7f * brightness, 0.85f * brightness, 0.9f);
    float winPoints[4][2] = {
        {cabinX - 0.03f * scale, y + 0.042f * scale},
        {cabinX + 0.03f * scale, y + 0.042f * scale},
        {cabinX + 0.028f * scale, y + 0.062f * scale},
        {cabinX - 0.028f * scale, y + 0.062f * scale}
    };
    for (int i = 0; i < 4; i++) {
        rotatePoint(winPoints[i][0], winPoints[i][1], hullCenterX, hullCenterY, angleRad);
    }
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        glVertex2f(winPoints[i][0], winPoints[i][1]);
    }
    glEnd();

    // Window dividers
    glColor4f(0.3f * brightness, 0.3f * brightness, 0.35f * brightness, 1.0f);
    glLineWidth(1.5f);
    for (float wx = -0.015f; wx <= 0.015f; wx += 0.015f) {
        float px1 = cabinX + wx * scale, py1 = y + 0.042f * scale;
        float px2 = cabinX + wx * scale, py2 = y + 0.062f * scale;
        rotatePoint(px1, py1, hullCenterX, hullCenterY, angleRad);
        rotatePoint(px2, py2, hullCenterX, hullCenterY, angleRad);
        glBegin(GL_LINES);
        glVertex2f(px1, py1);
        glVertex2f(px2, py2);
        glEnd();
    }

    // Antenna
    float antX = cabinX, antY1 = y + 0.082f * scale;
    rotatePoint(antX, antY1, hullCenterX, hullCenterY, angleRad);
    float antX2 = cabinX, antY2r = y + 0.11f * scale;
    rotatePoint(antX2, antY2r, hullCenterX, hullCenterY, angleRad);
    glColor4f(0.4f * brightness, 0.4f * brightness, 0.45f * brightness, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(antX, antY1);
    glVertex2f(antX2, antY2r);
    glEnd();

    // Fishing poles
    Color poleColor(0.45f, 0.35f, 0.25f);
    glColor4f(poleColor.r * brightness, poleColor.g * brightness, poleColor.b * brightness, 1.0f);
    glLineWidth(1.5f);

    float poleWave = sin(timeVal * 1.2f) * 0.008f * scale;

    // Left pole
    float lp1x = x - 0.05f * scale, lp1y = y + 0.032f * scale;
    float lp2x = x - 0.08f * scale, lp2y = y + 0.09f * scale + poleWave;
    float lp3x = x - 0.1f * scale, lp3y = y + 0.11f * scale + poleWave * 1.5f;
    rotatePoint(lp1x, lp1y, hullCenterX, hullCenterY, angleRad);
    rotatePoint(lp2x, lp2y, hullCenterX, hullCenterY, angleRad);
    rotatePoint(lp3x, lp3y, hullCenterX, hullCenterY, angleRad);
    glBegin(GL_LINE_STRIP);
    glVertex2f(lp1x, lp1y);
    glVertex2f(lp2x, lp2y);
    glVertex2f(lp3x, lp3y);
    glEnd();

    // Right pole
    float rp1x = x + 0.05f * scale, rp1y = y + 0.032f * scale;
    float rp2x = x + 0.08f * scale, rp2y = y + 0.095f * scale - poleWave;
    float rp3x = x + 0.1f * scale, rp3y = y + 0.115f * scale - poleWave * 1.2f;
    rotatePoint(rp1x, rp1y, hullCenterX, hullCenterY, angleRad);
    rotatePoint(rp2x, rp2y, hullCenterX, hullCenterY, angleRad);
    rotatePoint(rp3x, rp3y, hullCenterX, hullCenterY, angleRad);
    glBegin(GL_LINE_STRIP);
    glVertex2f(rp1x, rp1y);
    glVertex2f(rp2x, rp2y);
    glVertex2f(rp3x, rp3y);
    glEnd();

    // Fishing lines
    glColor4f(0.4f * brightness, 0.4f * brightness, 0.4f * brightness, 0.5f);
    glLineWidth(1.0f);
    float lineWave = sin(timeVal * 1.8f) * 0.006f * scale;

    glBegin(GL_LINE_STRIP);
    glVertex2f(lp3x, lp3y);
    glVertex2f(lp3x - 0.02f * scale + lineWave, y + 0.02f * scale);
    glVertex2f(lp3x - 0.01f * scale, y - 0.04f * scale);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(rp3x, rp3y);
    glVertex2f(rp3x + 0.025f * scale - lineWave, y + 0.01f * scale);
    glVertex2f(rp3x + 0.015f * scale, y - 0.045f * scale);
    glEnd();

    // Life ring
    float ringX = x + 0.06f * scale, ringY = y + 0.04f * scale;
    rotatePoint(ringX, ringY, hullCenterX, hullCenterY, angleRad);
    glColor4f(0.95f * brightness, 0.5f * brightness, 0.15f * brightness, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = i * 6.28318f / 32.0f;
        glVertex2f(ringX + 0.01f * scale * cos(angle), ringY + 0.01f * scale * sin(angle));
    }
    glEnd();
}

static void drawString2D(float x, float y, const char* s) {
    glRasterPos2f(x, y);
    for (const char* p = s; *p; ++p) {
        if (*p == '\n') {
            y -= 0.048f;
            glRasterPos2f(x, y);
        } else {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
        }
    }
}

static void drawStarsField() {
    if (starVisibility < 0.02f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(2.2f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 140; i++) {
        float fx = fmod(i * 0.413579f + 0.17f, 1.75f) - 0.88f;
        float fy = 0.18f + fmod(i * 0.271839f + 0.31f, 0.78f);
        float tw = 0.35f + 0.65f * powf(sinf(timeVal * 2.2f + i * 0.7f) * 0.5f + 0.5f, 2.0f);
        float a = starVisibility * tw * (0.35f + 0.65f * fmod(i * 0.19f, 1.0f));
        glColor4f(0.92f, 0.94f, 1.0f, a);
        glVertex2f(fx, fy);
    }
    glEnd();
    glPointSize(1.0f);
}

static void drawMoonDisk() {
    if (moonStrength < 0.02f) return;
    float ms = moonStrength * brightness;
    drawGradientCircle(moonX, moonY, moonRadius * 1.55f,
                       Color(0.92f, 0.93f, 0.98f), Color(0.45f, 0.5f, 0.62f), 0.22f * ms);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.96f * brightness, 0.97f * brightness, 0.9f * brightness, ms);
    glVertex2f(moonX, moonY);
    glColor4f(0.78f * brightness, 0.8f * brightness, 0.72f * brightness, ms);
    for (int i = 0; i <= 48; i++) {
        float ang = i * 6.28318f / 48.0f;
        glVertex2f(moonX + moonRadius * cosf(ang), moonY + moonRadius * sinf(ang));
    }
    glEnd();
    glColor4f(0.55f * brightness, 0.56f * brightness, 0.52f * brightness, 0.35f * ms);
    drawCircle(moonX - moonRadius * 0.35f, moonY + moonRadius * 0.25f, moonRadius * 0.22f,
               0.55f, 0.56f, 0.52f, 0.4f * ms);
    drawCircle(moonX + moonRadius * 0.4f, moonY - moonRadius * 0.15f, moonRadius * 0.18f,
               0.55f, 0.56f, 0.52f, 0.3f * ms);
}

static void drawCelestialReflection() {
    float cx = (sunStrength > moonStrength) ? sunX : moonX;
    float warmR = (sunStrength > moonStrength) ? 1.0f : 0.75f;
    float warmG = (sunStrength > moonStrength) ? 0.78f : 0.8f;
    float warmB = (sunStrength > moonStrength) ? 0.28f : 0.95f;
    float vis = fmaxf(sunStrength, moonStrength * 0.85f);
    if (vis < 0.03f) return;
    for (int i = 0; i < 36; i++) {
        float y = -0.25f - i * 0.0195f;
        float w = 0.008f + i * 0.0035f;
        float wave = sinf(timeVal * (1.4f + rainIntensity * 0.8f) + i * 0.65f) * (0.022f + rainIntensity * 0.02f);
        float alpha = (0.42f - i * 0.0105f) * brightness * vis;
        if (alpha > 0.0f) {
            glColor4f(warmR, warmG, warmB, alpha);
            glBegin(GL_QUADS);
            glVertex2f(cx - w + wave, y);
            glVertex2f(cx + w + wave, y);
            glVertex2f(cx + w * 0.88f - wave * 0.5f, y - 0.022f);
            glVertex2f(cx - w * 0.88f - wave * 0.5f, y - 0.022f);
            glEnd();
        }
    }
}

static float rainHash(int i, float salt) {
    float v = sinf((float)i * 12.9898f + salt * 78.233f) * 43758.5453f;
    return v - floorf(v);
}

static void drawRainLayer() {
    if (rainIntensity < 0.04f) return;
    float windShift = windStrength * 0.16f;
    int drops = 520 + (int)(rainIntensity * 320) + (g_weatherIndex == 5 ? 260 : 0) + (int)(g_thunderBoost * 200);
    drops = (drops > 1100) ? 1100 : drops;
    float fall = 1.92f + rainIntensity * 1.5f + g_thunderBoost * 0.75f;
    float drift = 0.38f + windStrength * 0.62f;
    glLineWidth(g_weatherIndex == 5 ? 1.35f : 1.05f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    for (int i = 0; i < drops; i++) {
        float h1 = rainHash(i, 1.7f);
        float h2 = rainHash(i, 4.3f);
        float h3 = rainHash(i, 9.1f);
        float x = -1.12f + fmod(h1 * 2.28f + timeVal * drift + sinf(timeVal * 0.7f + h2 * 6.28f) * 0.04f, 2.32f);
        float y = 0.98f - fmod(h2 * 1.82f + timeVal * fall + h3 * 0.35f, 1.82f);
        float len = 0.036f + h3 * (0.038f + rainIntensity * 0.042f) + g_thunderBoost * 0.028f;
        float a = (0.1f + rainIntensity * (0.32f + h1 * 0.2f) + g_thunderBoost * 0.2f) * brightness;
        a *= 0.75f + 0.5f * h2;
        float wob = sinf(timeVal * (2.1f + h1 * 3.0f) + (float)i * 0.31f) * 0.004f * windStrength;
        glColor4f(0.72f + h1 * 0.08f, 0.78f + h2 * 0.06f, 0.95f, fminf(0.92f, a));
        glVertex2f(x + wob, y);
        glVertex2f(x - windShift + wob * 0.5f, y - len);
    }
    glEnd();
    glLineWidth(1.0f);
}

static void drawSnowLayer() {
    if (snowIntensity < 0.04f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < 260; i++) {
        float x = -1.05f + fmod(i * 0.319f + sinf(timeVal * 0.15f + i) * 0.08f + windStrength * 0.12f, 2.15f);
        float y = 0.92f - fmod(i * 0.091f + timeVal * (0.35f + windStrength * 0.25f), 1.85f);
        float s = (0.004f + fmod(i * 0.02f, 0.006f)) * (0.8f + 0.4f * sinf(timeVal * 3 + i));
        float a = (0.15f + snowIntensity * 0.45f) * brightness;
        glColor4f(0.95f, 0.96f, 1.0f, a);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int k = 0; k <= 6; k++) {
            float ang = k * 1.0472f;
            glVertex2f(x + s * cosf(ang), y + s * sinf(ang));
        }
        glEnd();
    }
}

static void drawLightningBoltShape() {
    if (lightningFlash < 0.1f) return;
    float fade = fminf(1.0f, lightningFlash * 1.15f);
    float x = g_boltTipX;
    float y = 0.92f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glLineWidth(3.0f);
    glColor4f(0.9f, 0.95f, 1.0f, fade * 0.9f);
    glBegin(GL_LINE_STRIP);
    for (int s = 0; s < 20; s++) {
        glVertex2f(x, y);
        float jig = sinf(g_boltPhase + s * 1.83f) * 0.055f + sinf(g_boltSeed + s * 2.1f) * 0.04f;
        x += jig;
        y -= 0.038f + (s % 4) * 0.006f;
        if (y < 0.05f) break;
    }
    glEnd();
    glLineWidth(1.5f);
    glColor4f(1.0f, 1.0f, 1.0f, fade * 0.35f);
    glBegin(GL_LINE_STRIP);
    x = g_boltTipX + 0.012f;
    y = 0.9f;
    for (int s = 0; s < 16; s++) {
        glVertex2f(x, y);
        x += sinf(g_boltSeed * 0.7f + s * 1.5f) * 0.04f;
        y -= 0.042f;
        if (y < 0.08f) break;
    }
    glEnd();
    glLineWidth(1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void drawLightningFlash() {
    if (lightningFlash < 0.02f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.95f, 0.97f, 1.0f, lightningFlash * 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
}

static void evalSkyBandColor(float y, Color& c) {
    if (y >= 0.42f) {
        float t = (y - 0.42f) / (1.0f - 0.42f);
        c.r = Palette::skyMid.r + (Palette::skyTop.r - Palette::skyMid.r) * t;
        c.g = Palette::skyMid.g + (Palette::skyTop.g - Palette::skyMid.g) * t;
        c.b = Palette::skyMid.b + (Palette::skyTop.b - Palette::skyMid.b) * t;
    } else if (y >= 0.02f) {
        float t = (y - 0.02f) / (0.42f - 0.02f);
        c.r = Palette::skyBottom.r + (Palette::skyMid.r - Palette::skyBottom.r) * t;
        c.g = Palette::skyBottom.g + (Palette::skyMid.g - Palette::skyBottom.g) * t;
        c.b = Palette::skyBottom.b + (Palette::skyMid.b - Palette::skyBottom.b) * t;
    } else {
        float t = (y + 0.2f) / (0.02f + 0.2f);
        c.r = Palette::skyBand.r + (Palette::skyBottom.r - Palette::skyBand.r) * t;
        c.g = Palette::skyBand.g + (Palette::skyBottom.g - Palette::skyBand.g) * t;
        c.b = Palette::skyBand.b + (Palette::skyBottom.b - Palette::skyBand.b) * t;
    }
}

static void drawSkyBands() {
    const int bands = 80;
    const float yMin = -0.2f;
    const float yMax = 1.0f;
    float bmul = brightness;
    for (int i = 0; i < bands; i++) {
        float f0 = i / (float)bands;
        float f1 = (i + 1) / (float)bands;
        float y0 = yMin + f0 * (yMax - yMin);
        float y1s = yMin + f1 * (yMax - yMin);
        Color c0, c1;
        evalSkyBandColor(y0, c0);
        evalSkyBandColor(y1s, c1);
        glBegin(GL_QUADS);
        glColor3f(c0.r * bmul, c0.g * bmul, c0.b * bmul);
        glVertex2f(-1.0f, y0);
        glVertex2f(1.0f, y0);
        glColor3f(c1.r * bmul, c1.g * bmul, c1.b * bmul);
        glVertex2f(1.0f, y1s);
        glVertex2f(-1.0f, y1s);
        glEnd();
    }
}

static void drawTrainWindowLights(float tx, float ty, float sc) {
    if (moonStrength < 0.35f && sunStrength > 0.25f) return;
    float glow = (0.4f + 0.6f * moonStrength) * (1.0f - sunStrength * 0.9f);
    glow *= 1.0f + g_nightBridgeLight * 0.45f;
    if (glow < 0.08f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int c = 0; c < 4; c++) {
        float cx = tx - 0.2f * sc - c * 0.19f * sc;
        for (float wx = -0.06f; wx < 0.07f; wx += 0.03f) {
            float px = cx + wx * sc;
            glColor4f(1.0f, 0.92f, 0.55f, 0.55f * glow * brightness);
            drawCircle(px, ty + 0.026f * sc, 0.008f * sc, 1.0f, 0.92f, 0.55f, 0.5f * glow);
        }
    }
}

static void drawVignette() {
    float v = 0.12f + (1.0f - ambientScene) * 0.17f;
    v = fminf(0.48f, v);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto band = [](float x1, float y1, float x2, float y2, float a) {
        glColor4f(0.02f, 0.02f, 0.08f, a);
        glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
        glEnd();
    };
    band(-1.0f, 0.62f, 1.0f, 1.0f, v * 0.95f);
    band(-1.0f, -1.0f, 1.0f, -0.48f, v * 0.8f);
    band(-1.0f, -1.0f, -0.62f, 1.0f, v * 0.7f);
    band(0.62f, -1.0f, 1.0f, 1.0f, v * 0.7f);
}

static void drawHelpOverlay() {
    if (!g_showHelp) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(-0.99f, 0.98f);
    glVertex2f(-0.32f, 0.98f);
    glVertex2f(-0.32f, -0.02f);
    glVertex2f(-0.99f, -0.02f);
    glEnd();
    glColor3f(0.95f, 0.95f, 0.92f);
    const char* lines[] = {
        "SUNSET BRIDGE — ATMOSPHERE",
        "Time 1-7: Dawn Morn Noon PM",
        "  Sunset Dusk Night",
        "[ ]  cycle time   , . weather",
        "Weather direct: 8Clr 9Wnd 0Rain",
        " Y snow  O thunderstorm",
        "H help  F1 help  P pause",
        "R reset scene  ESC quit",
    };
    float y = 0.9f;
    for (const char* ln : lines) {
        drawString2D(-0.97f, y, ln);
        y -= 0.05f;
    }
}

// ==================== SCENE ELEMENTS ====================

void drawCloud(float x, float y, float scale, bool isFront = true) {
    Color mainColor = isFront ? Palette::cloud : Palette::cloudShadow;
    float alpha = isFront ? 0.95f : 0.7f;
    if (isFront) {
        drawCircle(x - 0.06f * scale, y - 0.02f * scale, 0.055f * scale, 0.5f, 0.45f, 0.5f, 0.3f);
        drawCircle(x + 0.02f * scale, y - 0.02f * scale, 0.065f * scale, 0.5f, 0.45f, 0.5f, 0.3f);
    }
    drawCircle(x - 0.1f * scale, y, 0.05f * scale, mainColor.r, mainColor.g, mainColor.b, alpha);
    drawCircle(x - 0.04f * scale, y + 0.025f * scale, 0.06f * scale, mainColor.r, mainColor.g, mainColor.b, alpha);
    drawCircle(x + 0.04f * scale, y + 0.02f * scale, 0.07f * scale, mainColor.r, mainColor.g, mainColor.b, alpha);
    drawCircle(x + 0.1f * scale, y, 0.05f * scale, mainColor.r, mainColor.g, mainColor.b, alpha);
    drawCircle(x, y + 0.01f * scale, 0.065f * scale, mainColor.r, mainColor.g, mainColor.b, alpha);
}

void drawSunGlow() {
    if (sunStrength < 0.03f) return;
    for (int i = 5; i >= 0; i--) {
        float radius = sunRadius + i * 0.08f;
        float alpha = (0.15f - i * 0.02f) * brightness * sunStrength;
        drawGradientCircle(sunX, sunY, radius, Color(1.0f, 0.82f, 0.35f), Color(1.0f, 0.45f, 0.08f),
                         alpha * (0.85f + 0.15f * sunStrength));
    }
}

void drawSun() {
    if (sunStrength < 0.03f) return;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(Palette::sunCore.r * brightness * sunStrength, Palette::sunCore.g * brightness * sunStrength,
              Palette::sunCore.b * brightness * sunStrength, sunStrength);
    glVertex2f(sunX, sunY);
    glColor4f(Palette::sunEdge.r * brightness * sunStrength, Palette::sunEdge.g * brightness * sunStrength,
              Palette::sunEdge.b * brightness * sunStrength, sunStrength);
    for (int i = 0; i <= 64; i++) {
        float angle = i * 6.28318f / 64.0f;
        glVertex2f(sunX + sunRadius * cos(angle), sunY + sunRadius * sin(angle));
    }
    glEnd();
}

void drawMountains() {
    float haze = (1.0f - ambientScene) * 0.14f;
    glColor3f(Palette::mountainFar.r * brightness, Palette::mountainFar.g * brightness,
              Palette::mountainFar.b * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.2f, -0.2f); glVertex2f(-0.4f, 0.35f); glVertex2f(0.2f, -0.2f);
    glVertex2f(-0.1f, -0.2f); glVertex2f(0.5f, 0.28f); glVertex2f(1.0f, -0.2f);
    glVertex2f(0.6f, -0.2f); glVertex2f(1.1f, 0.22f); glVertex2f(1.4f, -0.2f);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(Palette::skyBand.r * brightness, Palette::skyBand.g * brightness, Palette::skyBand.b * brightness,
              0.18f * haze + 0.06f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.2f, -0.2f); glVertex2f(-0.4f, 0.35f); glVertex2f(0.2f, -0.2f);
    glVertex2f(-0.1f, -0.2f); glVertex2f(0.5f, 0.28f); glVertex2f(1.0f, -0.2f);
    glVertex2f(0.6f, -0.2f); glVertex2f(1.1f, 0.22f); glVertex2f(1.4f, -0.2f);
    glEnd();

    glColor3f(Palette::mountainNear.r * brightness, Palette::mountainNear.g * brightness,
              Palette::mountainNear.b * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.3f, -0.2f); glVertex2f(-0.7f, 0.15f); glVertex2f(-0.2f, -0.2f);
    glVertex2f(0.3f, -0.2f); glVertex2f(0.8f, 0.12f); glVertex2f(1.2f, -0.2f);
    glEnd();

    glColor4f(0.55f * brightness, 0.58f * brightness, 0.62f * brightness, 0.12f + haze * 0.35f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.95f, -0.2f);
    glVertex2f(-0.55f, 0.08f);
    glVertex2f(-0.35f, 0.02f);
    glVertex2f(-0.15f, 0.12f);
    glVertex2f(0.15f, -0.05f);
    glVertex2f(0.45f, 0.18f);
    glVertex2f(0.75f, -0.08f);
    glVertex2f(1.05f, 0.06f);
    glEnd();
    glLineWidth(1.0f);
}

void drawWater() {
    Color deep = Palette::waterBottom;
    if (rainIntensity > 0.1f) {
        deep = Color(deep.r * 0.92f, deep.g * 0.95f, deep.b * 1.02f);
    }
    drawGradientRect(-1.0f, -1.0f, 1.0f, -0.2f, Palette::waterTop, deep);
    drawCelestialReflection();
    float specX = (sunStrength > moonStrength) ? sunX : moonX;
    float specB = (sunStrength > moonStrength) ? 0.95f : 0.75f;
    float specStr = fmaxf(sunStrength, moonStrength * 0.9f) * brightness;
    if (specStr > 0.04f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int k = 0; k < 5; k++) {
            float yy = -0.21f - k * 0.014f;
            float ww = 0.04f + k * 0.018f;
            float glint = sinf(timeVal * 1.8f + k * 0.7f) * 0.012f;
            float a = (0.14f - k * 0.022f) * specStr;
            if (a > 0.0f) {
                glColor4f(1.0f, specB, 0.65f, a);
                glBegin(GL_QUADS);
                glVertex2f(specX - ww + glint, yy);
                glVertex2f(specX + ww + glint, yy);
                glVertex2f(specX + ww * 0.65f - glint, yy - 0.012f);
                glVertex2f(specX - ww * 0.65f - glint, yy - 0.012f);
                glEnd();
            }
        }
    }
    float stormAgit = 0.0f;
    if (g_weatherIndex == 5) {
        stormAgit = 2.1f + 0.55f * sinf(timeVal * 2.4f) + g_thunderBoost * 1.6f;
    }
    float chop = 1.0f + windStrength * 0.85f + rainIntensity * 0.75f + stormAgit;
    float baseAmp = 0.007f + windStrength * 0.006f + rainIntensity * 0.005f + stormAgit * 0.012f;
    for (float wy = -0.22f; wy > -0.95f; wy -= 0.08f) {
        float waveIntensity = 0.3f + ((-0.22f - wy) / 0.73f) * 0.4f;
        waveIntensity *= (1.0f - snowIntensity * 0.25f);
        glColor4f(0.3f * brightness, 0.52f * brightness, 0.72f * brightness, waveIntensity * 0.52f);
        glBegin(GL_LINE_STRIP);
        for (float wx = -1.0f; wx <= 1.0f; wx += 0.02f) {
            float waveY = wy + sinf(timeVal * (1.2f * chop) + wx * (8.0f + windStrength * 3.0f + stormAgit * 1.2f) + wy * 3.0f) * baseAmp;
            if (rainIntensity > 0.12f) {
                waveY += sinf(timeVal * (6.0f + stormAgit * 0.8f) + wx * (22.0f + stormAgit * 3.0f)) * (0.0025f * rainIntensity + stormAgit * 0.0012f);
            }
            if (stormAgit > 0.5f) {
                waveY += sinf(timeVal * 9.0f + wx * 31.0f) * 0.0018f * stormAgit;
            }
            glVertex2f(wx, waveY);
        }
        glEnd();
    }
    drawBridgeLightReflectionsOnWater();
    if (rainIntensity > 0.12f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.85f, 0.9f, 1.0f, 0.08f * rainIntensity * brightness);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.2f);
        glVertex2f(1.0f, -0.2f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glEnd();
    }
}

void drawBirds() {
    float birdX = sin(timeVal * 0.2f) * 0.25f;
    float flapPhase = timeVal * 6.0f;
    glColor3f(0.1f * brightness, 0.08f * brightness, 0.12f * brightness);
    glLineWidth(2.0f);

    struct Bird { float x, y, size, phaseOffset; };
    Bird birds[] = {
        {-0.25f, 0.52f, 1.0f, 0.0f},
        {0.05f, 0.58f, 0.8f, 1.5f},
        {-0.08f, 0.63f, 0.9f, 3.0f},
        {0.25f, 0.50f, 0.7f, 4.5f},
        {-0.35f, 0.45f, 0.6f, 2.0f}
    };

    float windDrift = windStrength * 0.12f;
    for (const auto& bird : birds) {
        float bx = bird.x + birdX * (0.5f + bird.size * 0.5f) + sinf(timeVal * 1.1f + bird.phaseOffset) * windDrift;
        float by = bird.y + sinf(timeVal * 0.8f + bird.phaseOffset) * (0.01f + windStrength * 0.018f);
        float flap = sin(flapPhase + bird.phaseOffset) * 0.025f * bird.size;
        float wingSpan = 0.03f * bird.size;
        glBegin(GL_LINE_STRIP);
        glVertex2f(bx - wingSpan, by + flap);
        glVertex2f(bx - wingSpan * 0.3f, by + flap * 0.5f);
        glVertex2f(bx, by);
        glVertex2f(bx + wingSpan * 0.3f, by + flap * 0.5f);
        glVertex2f(bx + wingSpan, by + flap);
        glEnd();
    }
}

void drawDarknessOverlay() {
    float cloudDim = (brightness < 0.95f) ? (1.0f - brightness) * 0.34f : 0.0f;
    float nightDim = (1.0f - ambientScene) * 0.24f;
    if (g_timeIndex == 6) {
        nightDim *= 0.72f;
    }
    float darkness = fminf(0.52f, cloudDim + nightDim);
    if (darkness < 0.02f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.02f, 0.02f, 0.09f, darkness);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void display() {
    rebuildPalette();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float clearR = Palette::skyBottom.r * 0.35f;
    float clearG = Palette::skyBottom.g * 0.35f;
    float clearB = Palette::skyBottom.b * 0.4f;
    glClearColor(clearR, clearG, clearB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawSkyBands();
    drawStarsField();
    drawMoonDisk();
    drawSunGlow();
    drawSun();
    drawCloud(cloudPos3 - 0.5f + sinf(timeVal * 0.4f) * windStrength * 0.02f, 0.45f, 0.6f, false);
    drawMountains();
    drawWater();
    drawBuoyLights();

    drawSailboat(boat1, -0.38f, 1.0f);
    drawFishingBoat(boat2, -0.55f, 0.9f);

    drawBridge();
    drawBridgeNightLighting(0.1f, 0.0f);

    float trainY = 0.0f;
    drawTrain(trainPos, trainY + 0.03f, 0.85f);
    drawTrainWindowLights(trainPos, trainY + 0.03f, 0.85f);

    float roadY = 0.1f;
    drawCar(car1Pos, roadY + 0.008f, 0.75f, Color(0.8f, 0.15f, 0.1f), true);
    drawCar(car2Pos, roadY + 0.008f, 0.7f, Color(0.2f, 0.4f, 0.7f), true);
    drawCar(car3Pos, roadY + 0.008f, 0.8f, Color(0.9f, 0.9f, 0.9f), true);
    drawBus(bus1Pos, roadY + 0.01f, 0.65f, Color(0.9f, 0.6f, 0.1f), false);
    drawBus(bus2Pos, roadY + 0.01f, 0.6f, Color(0.2f, 0.5f, 0.3f), false);

    drawCloud(cloudPos1 + sinf(timeVal * 0.55f) * windStrength * 0.03f, 0.68f, 1.1f);
    drawCloud(cloudPos2, 0.75f, 0.9f);
    drawCloud(cloudPos3 - windStrength * 0.04f, 0.62f, 1.0f);

    drawBirds();
    drawRainLayer();
    drawSnowLayer();
    drawDarknessOverlay();
    drawLightningBoltShape();
    drawLightningFlash();
    drawVignette();
    drawHelpOverlay();

    glDisable(GL_BLEND);
    glutSwapBuffers();
}

void update(int /*value*/) {
    if (!g_paused) {
        timeVal += 0.015f;
    }

    float wmul = 1.0f + windStrength * 2.8f + rainIntensity * 0.35f;
    if (!g_paused) {
        cloudPos1 += 0.0008f * wmul;
        cloudPos2 += 0.0005f * wmul;
        cloudPos3 += 0.001f * wmul;
    }
    if (cloudPos1 > 1.8f) cloudPos1 = -1.8f;
    if (cloudPos2 > 1.8f) cloudPos2 = -1.8f;
    if (cloudPos3 > 1.8f) cloudPos3 = -1.8f;

    rebuildPalette();
    float coverage = 0.0f;
    coverage = fmaxf(coverage, getCloudSunOverlap(cloudPos1 + sinf(timeVal * 0.55f) * windStrength * 0.03f, 0.68f, 0.25f));
    coverage = fmaxf(coverage, getCloudSunOverlap(cloudPos2, 0.75f, 0.2f));
    coverage = fmaxf(coverage, getCloudSunOverlap(cloudPos3 - windStrength * 0.04f, 0.62f, 0.22f));
    targetBrightness = ambientScene * (1.0f - coverage * 0.48f);
    brightness += (targetBrightness - brightness) * 0.055f;

    if (!g_paused) {
        boat1 += 0.0012f * (1.0f + windStrength * 0.35f);
        if (boat1 > 1.5f) boat1 = -1.6f;
        boat2 += 0.0008f * (1.0f + windStrength * 0.25f);
        if (boat2 > 1.5f) boat2 = -1.6f;

        car1Pos += 0.003f;
        if (car1Pos > 1.3f) car1Pos = -1.3f;
        car2Pos += 0.0025f;
        if (car2Pos > 1.3f) car2Pos = -1.3f;
        car3Pos += 0.0028f;
        if (car3Pos > 1.3f) car3Pos = -1.3f;

        bus1Pos -= 0.002f;
        if (bus1Pos < -1.3f) bus1Pos = 1.3f;
        bus2Pos -= 0.0015f;
        if (bus2Pos < -1.3f) bus2Pos = 1.3f;

        trainPos -= 0.0018f;
        if (trainPos < -2.5f) trainPos = 2.0f;
    }

    if (!g_paused) {
        g_thunderBoost *= 0.89f;
        if (g_thunderBoost < 0.02f) {
            g_thunderBoost = 0.0f;
        }
    }

    if (g_weatherIndex == 5 && !g_paused) {
        g_boltPhase += 0.08f;
        if ((rand() % 140) == 0) {
            lightningFlash = 1.0f;
            g_thunderBoost = 1.0f;
            g_boltTipX = -0.42f + (rand() % 160) * 0.0055f;
            g_boltSeed = (float)(rand() % 628) * 0.01f;
        }
    }
    lightningFlash *= 0.86f;
    if (lightningFlash < 0.015f) {
        lightningFlash = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void reshape(int w, int h) {
    if (h <= 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    rebuildPalette();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27:
            std::exit(0);
            break;
        case '1':
            g_timeIndex = 0;
            break;
        case '2':
            g_timeIndex = 1;
            break;
        case '3':
            g_timeIndex = 2;
            break;
        case '4':
            g_timeIndex = 3;
            break;
        case '5':
            g_timeIndex = 4;
            break;
        case '6':
            g_timeIndex = 5;
            break;
        case '7':
            g_timeIndex = 6;
            break;
        case '[':
            g_timeIndex = (g_timeIndex + 6) % 7;
            break;
        case ']':
            g_timeIndex = (g_timeIndex + 1) % 7;
            break;
        case ',':
            g_weatherIndex = (g_weatherIndex + 5) % 6;
            break;
        case '.':
            g_weatherIndex = (g_weatherIndex + 1) % 6;
            break;
        case '8':
            g_weatherIndex = 0;
            break;
        case '9':
            g_weatherIndex = 1;
            break;
        case '0':
            g_weatherIndex = 2;
            break;
        case 'y':
        case 'Y':
            g_weatherIndex = 3;
            break;
        case 'u':
        case 'U':
            g_weatherIndex = 4;
            break;
        case 'o':
        case 'O':
            g_weatherIndex = 5;
            break;
        case 'h':
        case 'H':
            g_showHelp = !g_showHelp;
            break;
        case 'p':
        case 'P':
            g_paused = !g_paused;
            break;
        case 'r':
        case 'R':
            boat1 = -1.0f;
            boat2 = -0.3f;
            car1Pos = -1.5f;
            car2Pos = 0.3f;
            car3Pos = -0.8f;
            bus1Pos = 1.5f;
            bus2Pos = 0.0f;
            trainPos = 2.0f;
            cloudPos1 = -1.5f;
            cloudPos2 = 0.0f;
            cloudPos3 = 1.2f;
            lightningFlash = 0.0f;
            g_thunderBoost = 0.0f;
            rebuildPalette();
            brightness = ambientScene;
            break;
        default:
            break;
    }
}

void specialKey(int key, int, int) {
    if (key == GLUT_KEY_F1) {
        g_showHelp = !g_showHelp;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow(
        "Sunset Bridge Showcase — 1-7 time | 8-0,Y,U,O weather | [ ] , . | H F1 P R");

    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}