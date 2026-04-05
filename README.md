# 🌉 Sunset Bridge Animation

An intricate, procedural 2D animated scene built entirely in **C++ with OpenGL/GLUT**. The scene renders a busy bridge over reflective water with moving vehicles, trains, sailboats, procedural weather (rain, snow, lightning), and a full day-night cycle — all with no external assets or textures.

---

## 🚀 How to Run

### ✅ macOS

**Requirements:** Xcode Command Line Tools (includes OpenGL and GLUT by default)

**Step 1 — Install Xcode CLT (if not already installed):**
```bash
xcode-select --install
```

**Step 2 — Compile:**
```bash
cd /path/to/sunset_bridge
g++ -std=c++17 main.cpp -o sunset_bridge \
    -framework OpenGL \
    -framework GLUT \
    -Wno-deprecated
```
> The `-Wno-deprecated` flag suppresses macOS deprecation warnings for GLUT. The code handles this with `#define GL_SILENCE_DEPRECATION 1` at the top.

**Step 3 — Run:**
```bash
./sunset_bridge
```

---

### ✅ Windows

**Requirements:** MinGW-w64 (GCC for Windows) + FreeGLUT

**Step 1 — Install MinGW-w64:**
- Download from: https://www.mingw-w64.org/downloads/
- Add `C:\mingw64\bin` to your Windows `PATH` environment variable.

**Step 2 — Download FreeGLUT:**
- Download pre-built binaries from: https://www.transmissionzero.co.uk/software/freeglut-devel/
- Extract and copy:
  - `freeglut.dll` → `C:\Windows\System32\`
  - `freeglut.h` → `C:\mingw64\include\GL\`
  - `libfreeglut.a` or `libfreeglut_static.a` → `C:\mingw64\lib\`

**Step 3 — Compile (in Command Prompt or PowerShell):**
```cmd
g++ -std=c++17 main.cpp -o sunset_bridge.exe ^
    -lopengl32 -lglu32 -lfreeglut ^
    -I"C:\path\to\freeglut\include" ^
    -L"C:\path\to\freeglut\lib"
```

**Step 4 — Run:**
```cmd
sunset_bridge.exe
```

---

### ✅ Linux (Ubuntu/Debian)

**Step 1 — Install dependencies:**
```bash
sudo apt-get install freeglut3-dev libglu1-mesa-dev
```

**Step 2 — Compile:**
```bash
g++ -std=c++17 main.cpp -o sunset_bridge -lGL -lGLU -lglut
```

**Step 3 — Run:**
```bash
./sunset_bridge
```

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `1` | Dawn |
| `2` | Morning |
| `3` | Noon |
| `4` | Afternoon |
| `5` | Sunset (default) |
| `6` | Dusk |
| `7` | Night |
| `[` / `]` | Cycle time of day |
| `,` / `.` | Cycle weather |
| `8` | Clear weather |
| `9` | Windy |
| `0` | Rain |
| `Y` | Snow |
| `O` | Thunderstorm |
| `P` | Pause / Resume |
| `R` | Reset scene |
| `H` / `F1` | Toggle help overlay |
| `ESC` | Quit |

---

## 🗂️ Architecture: Which Part is Responsible for What?

The codebase uses no classes — everything is organized through well-named standalone functions. Responsibilities are layered from low-level primitives up to high-level scene composition.

### Layer 1 — Core Primitive Renderers
These are the absolute foundational drawing utilities. Every complex object is assembled from these.

- **`drawRect(x1, y1, x2, y2, r, g, b, alpha)`**  
  Draws a solid-colored axis-aligned rectangle using `GL_QUADS`. Also multiplies the color by the global `brightness` variable so all geometry responds to cloud dimming and nighttime automatically.

- **`drawGradientRect(x1, y1, x2, y2, top, bottom)`**  
  Divides a rectangle into 56 horizontal slices and linearly interpolates color from `bottom` to `top`. Used for the sky, water, and other gradients.

- **`drawCircle(cx, cy, radius, r, g, b, alpha)`**  
  Draws a smooth disk using `GL_TRIANGLE_FAN` with 64 triangular slices computed via `sin/cos`. Used for wheels, sun, moon, lights.

- **`drawGradientCircle(cx, cy, radius, inner, outer, alpha)`**  
  Same as `drawCircle` but the center vertex has full opacity and the border has zero, creating a natural glow effect via OpenGL's per-vertex color interpolation.

- **`rotatePoint(px, py, cx, cy, angleRad)`**  
  Applies a 2D rotation matrix manually to a single point. This is used intensively for sailboats to rotate every single vertex of every polygon individually.

### Layer 2 — Time & Weather System
Controls the entire color palette of the scene based on time of day and weather.

- **`getTimePreset(idx, t)`**  
  A large `switch` statement that hardcodes the RGB color palette for 7 time periods: Dawn (0), Morning (1), Noon (2), Afternoon (3), Sunset (4), Dusk (5), Night (6). Each preset defines sky gradients, water colors, mountain shades, sun/moon positions, and ambient light levels.

- **`rebuildPalette()`**  
  The "brain" of the lighting system. Called once per frame. Reads `g_timeIndex` and `g_weatherIndex` and combines them:
  1. Loads the base palette from `getTimePreset()`
  2. Calculates a `gmix` factor (gray-mix) based on weather
  3. Calls `mixTowardGray()` on every palette color (sky, mountain, water, clouds)
  4. Computes `g_nightBridgeLight` — a float from 0.0 to 1.0 controlling how much the bridge's lamppost glows

- **`mixTowardGray(c, amount)`**  
  Blends a Color toward `RGB(0.52, 0.52, 0.56)` — a cool blue-gray — linearly. Used to desaturate the world under rain, fog, and overcast skies.

### Layer 3 — Environment Renderers
Draw the static background world elements. Each is self-contained.

- **`drawSkyBands()`** — 80 horizontal bands covering Y[-0.2, 1.0]. Each band samples `evalSkyBandColor()` which interpolates between `skyTop`, `skyMid`, `skyBottom`, and `skyBand` palette colors based on height.
- **`drawStarsField()`** — 140 pseudo-random points using deterministic hash (`i * 0.413579f`) for stable positions. Twinkle via `sin(timeVal * 2.2f + i * 0.7f)`.
- **`drawMoonDisk()`** — Draws the moon with a radial gradient core, plus 2 dark circles overlaid to fake craters.
- **`drawSun()` / `drawSunGlow()`** — The sun disk is a `GL_TRIANGLE_FAN`. The glow is 6 layered `drawGradientCircle` calls at increasing radii.
- **`drawMountains()`** — Draws `GL_TRIANGLES` for 3 far peaks and 2 near peaks using two separate palette shades to create a depth illusion. Adds a faint atmospheric haze overlay.
- **`drawWater()`** — The water is a `drawGradientRect` as a base, then `drawCelestialReflection()` adds the shimmering sun/moon trail. On top, a loop of sine-driven `GL_LINE_STRIP` waves layers at different Y depths for parallax.

### Layer 4 — Dynamic Weather Renderers
These are particle-like systems using pseudo-random hash functions.

- **`drawRainLayer()`** — Renders 520–1100 rain drops as `GL_LINES`. Each drop uses `rainHash(i, salt)` — a sin-based hash for stable but varied positions. X and Y positions are driven by `timeVal * fall` to animate falling. Wind shifts the X axis.
- **`drawSnowLayer()`** — 260 snow flakes rendered as tiny hexagons (`GL_TRIANGLE_FAN` with 6 vertices). Size and alpha pulsate with `sin(timeVal * 3 + i)`.
- **`drawLightningBoltShape()`** — Draws a jagged `GL_LINE_STRIP` from top to bottom, with each step jittered by `sin(g_boltPhase + s * 1.83f)` to simulate electricity.
- **`drawLightningFlash()`** — Overlays a full-screen semi-transparent white quad during a lightning event.

### Layer 5 — Transport & Structures
High-level composite objects assembled from Layer 1 primitives.

- **`drawCar()` / `drawBus()`** — Built from `GL_POLYGON` for body, `drawCircle` for wheels, `GL_QUADS` for windows. At night, headlights use additive blending (`GL_SRC_ALPHA, GL_ONE`) to create real glow halos.
- **`drawTrainEngine()` / `drawTrainCarriage()` / `drawTrain()`** — The train is an engine plus 4 carriages offset backwards. Carriages use alternating colors and share a yellow stripe.
- **`drawBridgeTower()`** — Two trapezoidal legs with cross-brace `GL_LINES` between them and a blinking aviation light at the top.
- **`drawBridge()`** — The most complex function. Order of operations:
  1. Water pillars (`GL_POLYGON`)
  2. Lower train deck with rails, ballast, ties
  3. Vertical support columns between decks
  4. X-shaped diagonal braces rendered as `GL_LINES`
  5. Upper road deck with lane markings and railings
  6. Two `drawBridgeTower()` calls
  7. Main suspension cable using `GL_LINE_STRIP` + `cosh()` for natural droop
  8. Vertical suspenders hanging from the cable to the road
  9. Overhead catenary wires for the train
- **`drawSailboat()` / `drawFishingBoat()`** — Most complex objects. Every vertex is first positioned relative to the hull center, then rotated by `angleRad` (derived from wind-driven `sin(timeVal * 0.8f)`). Sails use a sine wave deformation to billow in the wind.

---

## ⚙️ Algorithms Used

### 1. Procedural Parametric Animation
Everything moves through a single global clock: `timeVal`, which increments `+= 0.015f` every frame (~60 FPS). Objects compute their state from this value rather than storing intermediate positions.

```cpp
// Boat bobbing — Y oscillates ±(0.008 + wind) units
float bob = sinf(timeVal * 0.6f + x * 0.2f) * (0.008f + windStrength * 0.012f) * scale;

// Boat rocking — angle oscillates ±(2.5 + wind*5.5) degrees
float rockAngle = sinf(timeVal * 0.8f + x * 0.3f) * (2.5f + windStrength * 5.5f);
float angleRad = rockAngle * 3.14159f / 180.0f;
```
Every object gets a unique `phaseOffset` (like `x * 0.3f`) to prevent them all from moving in sync, making the scene feel naturally random.

### 2. 2D Rotation Matrix
The `rotatePoint` function implements a standard 2D rotation matrix from scratch. This is used instead of `glRotatef` because it rotates individual polygon vertices directly, allowing boats to tilt in place:
```cpp
void rotatePoint(float &px, float &py, float cx, float cy, float angleRad) {
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    float dx = px - cx;  // translate to origin
    float dy = py - cy;
    // Apply rotation matrix:
    // [x'] = [cos  -sin] [dx]
    // [y']   [sin   cos] [dy]
    px = cx + dx * cosA - dy * sinA;
    py = cy + dx * sinA + dy * cosA;
}
```

### 3. Parametric Circle Drawing
All circles are drawn by computing 64 vertices along the circumference using the unit circle formula. OpenGL's `GL_TRIANGLE_FAN` stitches them into a filled disk:
```cpp
void drawCircle(float cx, float cy, float radius, ...) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);  // center point
    for (int i = 0; i <= 64; i++) {
        float angle = i * 6.28318f / 64.0f;  // 2π divided into 64 steps
        glVertex2f(cx + radius * cos(angle),  // x = cx + r*cos(θ)
                   cy + radius * sin(angle)); // y = cy + r*sin(θ)
    }
    glEnd();
}
```

### 4. Linear Color Interpolation (Gradients)
The sky is divided into 80 bands. Each band's color is sampled from a 3-zone piecewise linear function:
```cpp
static void evalSkyBandColor(float y, Color& c) {
    if (y >= 0.42f) {
        float t = (y - 0.42f) / (1.0f - 0.42f);
        // Lerp between skyMid and skyTop
        c.r = Palette::skyMid.r + (Palette::skyTop.r - Palette::skyMid.r) * t;
        // ... same for g and b
    } else if (y >= 0.02f) {
        // Lerp between skyBottom and skyMid
    } else {
        // Lerp between skyBand (horizon glow) and skyBottom
    }
}
```

### 5. Alpha Compositing — Two Modes
Two different blend modes are used for different effects:

**Standard blending** — for transparency (glass, water, clouds):
```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// RESULT = src.rgb * src.alpha + dst.rgb * (1 - src.alpha)
```

**Additive blending** — for glowing lights, headlights, lightning:
```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE);
// RESULT = src.rgb * src.alpha + dst.rgb * 1.0
// This ADDS light values — the more objects overlap, the brighter it gets
```

### 6. Pseudo-Random Hash for Rain
Rather than calling `rand()` every frame (which would make drops jump), rain positions are computed via a deterministic sine hash. For any index `i` and salt value, it always returns the same float in [0, 1]:
```cpp
static float rainHash(int i, float salt) {
    float v = sinf((float)i * 12.9898f + salt * 78.233f) * 43758.5453f;
    return v - floorf(v); // returns fractional part only, always [0,1]
}
```
The drop's position is then: `baseX + timeVal * driftSpeed` modulo screen width — making it scroll smoothly and wrap around.

### 7. Suspension Cable with Hyperbolic Cosine
The bridge's main suspension cable is not a parabola — it's drawn using `cosh()`, the mathematically correct catenary curve for a suspended rope:
```cpp
glBegin(GL_LINE_STRIP);
for (float cx = -1.0f; cx <= 1.0f; cx += 0.02f) {
    float cableY;
    if (cx < -0.7f) {
        // Straight section from anchor to left tower
        cableY = roadY + 0.18f - (cx + 1.0f) * 0.35f;
    } else if (cx > 0.7f) {
        // Straight section from right tower to anchor
        cableY = roadY + 0.18f - (1.0f - cx) * 0.35f;
    } else {
        // Catenary curve between the two towers
        cableY = roadY + 0.08f + 0.12f * cosh(cx * 1.0f) - 0.12f;
    }
    glVertex2f(cx, cableY);
}
glEnd();
```

### 8. Color Desaturation (Gray Mixing)
When weather changes to rain/storm, every palette color is pushed toward a blue-gray value through linear blending: 
```cpp
static void mixTowardGray(Color& c, float a) {
    // When a=0: color stays original. When a=1: color becomes pure gray.
    c.r = c.r * (1.0f - a) + 0.52f * a;  // target gray red   = 0.52
    c.g = c.g * (1.0f - a) + 0.52f * a;  // target gray green = 0.52
    c.b = c.b * (1.0f - a) + 0.56f * a;  // target gray blue  = 0.56 (slightly blue)
}
```
The `gmix` strength varies by weather: `0.06` for windy, `0.24` for rain, `0.2` for snow, `0.3` for thunderstorm.

---

## 📖 In-Depth Code Walkthrough (With Code Snippets)

### 1. Program Startup — `main()` and `init()`
```cpp
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA); // Double-buffer + alpha channel
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Sunset Bridge ...");

    init();                          // Setup OpenGL state
    glutReshapeFunc(reshape);        // Called on resize
    glutDisplayFunc(display);        // Called to draw each frame
    glutKeyboardFunc(keyboard);      // Keyboard input
    glutTimerFunc(0, update, 0);     // Start first timer tick immediately

    glutMainLoop();                  // Hand control to GLUT
    return 0;
}
```
**Why double-buffer?** `GLUT_DOUBLE` means the scene is drawn to an offscreen buffer, then swapped to the screen at once via `glutSwapBuffers()`. This prevents visual tearing/flickering.

The `init()` function configures the coordinate system with `gluOrtho2D(-1.0, 1.0, -1.0, 1.0)`. This means the entire screen maps from **-1.0 to +1.0** on both axes. The origin (0,0) is the center, and screen edges are exactly ±1. All draw coordinates use this space.

### 2. The Animation Loop — `update()`
```cpp
void update(int /*value*/) {
    if (!g_paused) {
        timeVal += 0.015f;  // Advance global clock ~0.9 units/sec at 60fps
    }

    // Cloud movement — speed scales with wind
    float wmul = 1.0f + windStrength * 2.8f + rainIntensity * 0.35f;
    cloudPos1 += 0.0008f * wmul;
    if (cloudPos1 > 1.8f) cloudPos1 = -1.8f; // Wrap around

    // Cloud shadow — modulates scene brightness dynamically
    float coverage = getCloudSunOverlap(cloudPos1, 0.68f, 0.25f);
    targetBrightness = ambientScene * (1.0f - coverage * 0.48f);
    brightness += (targetBrightness - brightness) * 0.055f; // Smooth lerp

    // Lightning — random probability trigger per frame
    if (g_weatherIndex == 5 && (rand() % 140) == 0) {
        lightningFlash = 1.0f;
        g_boltTipX = -0.42f + (rand() % 160) * 0.0055f;
    }
    lightningFlash *= 0.86f; // Flash decay per frame

    glutTimerFunc(16, update, 0); // Schedule next tick in 16ms
}
```
**Why `brightness += (target - brightness) * 0.055f`?** This is an **exponential smoothing** (lerp-based approach). Instead of snapping to the target, the brightness chases the target at 5.5% per frame. This gives a smooth, natural dimming effect when clouds pass in front of the sun.

### 3. The Color System — `getTimePreset()` and `rebuildPalette()`
Each time preset is a full description of the world's color palette:
```cpp
case 4: // Sunset (default)
    t.skyTop    = Color(0.15f, 0.05f, 0.25f); // Dark purple-blue top
    t.skyMid    = Color(0.85f, 0.35f, 0.20f); // Orange-red middle
    t.skyBottom = Color(1.00f, 0.70f, 0.30f); // Warm yellow-orange at horizon
    t.skyBand   = Color(1.00f, 0.55f, 0.28f); // Intense orange band near horizon
    t.waterTop  = Color(0.15f, 0.25f, 0.45f); // Blue-tinted surface
    t.sunx = 0.65f; t.suny = 0.55f; t.sunr = 0.12f; // Sun position & size
    t.sunStr = 1.0f;    // Full sun intensity
    t.ambient = 1.0f;   // Full scene brightness
    break;

case 6: // Night
    t.skyTop    = Color(0.02f, 0.03f, 0.10f); // Near-black deep sky
    t.ambient = 0.46f;  // 54% darker scene
    t.moonStr = 1.0f;   // Full moon
    t.stars = 1.0f;     // Full star field
    break;
```
`rebuildPalette()` then applies weather desaturation on top of these values and computes `g_nightBridgeLight`:
```cpp
// Night bridge lights activate when sun is weak AND ambience is dark
g_nightBridgeLight = fmaxf(0.0f, 1.0f - sunStrength * 2.3f) 
                   * fminf(1.0f, (1.0f - ambientScene) * 1.85f + starVisibility * 0.22f);
```

### 4. Drawing the Bridge — `drawBridge()`
The bridge is the most complex single function. It has two decks and uses exact Y-level constants:
```cpp
float trainY = 0.0f;    // Train tracks sit at Y=0.0 in world space
float roadY  = 0.1f;    // Road surface sits at Y=0.1
float bridgeBottom = -0.65f; // Pillars go down to Y=-0.65 (deep in water)
```
The suspension cable uses `cosh()` between the tower positions at X=±0.7:
```cpp
// For the central span between towers: a catenary curve
cableY = roadY + 0.08f + 0.12f * cosh(cx * 1.0f) - 0.12f;
// cosh(0) = 1.0, so at center: y = roadY + 0.08 + 0.12 - 0.12 = roadY + 0.08 (cable sags)
// cosh(0.7) ≈ 1.255, so at towers: y ≈ roadY + 0.08 + 0.031 ≈ roadY + 0.111 (cable rises)
```

### 5. Boat Physics — `drawSailboat()`
Every boat vertex goes through three transformations:
1. Define position relative to a center `(hullCenterX, hullCenterY)`
2. Pass through `rotatePoint()` with the wind-driven `angleRad`
3. Submit to OpenGL via `glVertex2f()`

```cpp
// Step 1: Compute rock angle from wind and time
float rockAngle = sinf(timeVal * 0.8f + x * 0.3f) * (2.5f + windStrength * 5.5f);
float angleRad  = rockAngle * 3.14159f / 180.0f;

// Step 2: Define hull vertices
float points[6][2] = {
    {x - 0.1f * scale,  y + 0.015f * scale},  // left bow
    {x + 0.12f * scale, y + 0.020f * scale},  // right bow
    // ...
};

// Step 3: Rotate each vertex and submit
for (int i = 0; i < 6; i++) {
    rotatePoint(points[i][0], points[i][1], hullCenterX, hullCenterY, angleRad);
    glVertex2f(points[i][0], points[i][1]);
}
```
The mast, boom, sail, rigging, and even the flag are all individually rotated, so when the boat rocks, the entire boat pivots realistically.

### 6. Rain System — `drawRainLayer()`
```cpp
void drawRainLayer() {
    int drops = 520 + (int)(rainIntensity * 320); // More drops = heavier rain
    
    glBegin(GL_LINES);
    for (int i = 0; i < drops; i++) {
        float h1 = rainHash(i, 1.7f); // Stable pseudo-random [0,1] for this drop
        float h2 = rainHash(i, 4.3f); // Different salt = different pseudo-random
        
        // X position: base offset + time * drift (wind) → wraps around screen
        float x = -1.12f + fmod(h1 * 2.28f + timeVal * drift, 2.32f);
        
        // Y position: base offset + time * fall → wraps from top to bottom
        float y = 0.98f - fmod(h2 * 1.82f + timeVal * fall, 1.82f);
        
        // Each drop is 1 line: top to (x-windShift, y-length)
        glVertex2f(x, y);
        glVertex2f(x - windShift, y - len);
    }
    glEnd();
}
```
**Why hash instead of `rand()`?** Calling `rand()` every frame gives different values each frame, making rain drops appear to teleport randomly. The hash function maps `i` → a stable position, then `timeVal` translates that position downward over time — producing smooth, consistent falling motion for each individual raindrop identity `i`.

---

## 🔦 Global State Variables

| Variable | Type | Purpose |
|---|---|---|
| `timeVal` | `float` | Master animation clock |
| `g_timeIndex` | `int` | Current time preset (0-6) |
| `g_weatherIndex` | `int` | Current weather (0=clear, 5=storm) |
| `brightness` | `float` | Current scene brightness (smoothed) |
| `g_nightBridgeLight` | `float` | Bridge lamp intensity (0-1) |
| `sunStrength` | `float` | Sun contribution (0-1) |
| `moonStrength` | `float` | Moon contribution (0-1) |
| `windStrength` | `float` | Wind intensity (affects waves, boats) |
| `rainIntensity` | `float` | Rain density (0-1) |
| `lightningFlash` | `float` | Lightning opacity (decays per frame) |
| `car1Pos` / `boat1` | `float` | X-axis position of moving objects |
| `trainPos` | `float` | X position of train (moves left) |
| `g_paused` | `bool` | Freeze all animations |
