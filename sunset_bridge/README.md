# Sunset Bridge Animation

An intricate, procedural 2D graphics animation built with C++ and OpenGL/GLUT. 

This project renders a dynamic scene featuring a bustling bridge over water, complete with vehicles, trains, boats, clouds, celestial bodies (sun/moon), and varying weather/time-of-day conditions.

## 🗂️ Architecture: Which Part is Responsible for What?

The structure of the project organizes responsibilities seamlessly through individual functions explicitly named for their roles:

### 1. The Rendering Engine & Core Primitives
*(Bottom-Level)*
These functions serve as the foundational building blocks of the app:
- **`drawRect`, `drawCircle`:** Draws standardized fundamental shapes using immediate mode OpenGL polygons and triangle fans.
- **`drawGradientRect`, `drawGradientCircle`:** Draws shapes with smooth color transitions, creating gradients. 

### 2. Time & Weather Management System
*(Logic-Level)*
- **`getTimePreset()`:** Acts as a data store specifying RGB color values for distinct times of day (e.g., dawn, morning, noon, afternoon, sunset, dusk, night).
- **`rebuildPalette()`:** The central orchestrator that manages the shift between times of the day, dynamically adjusting standard colors towards grayscale (`mixTowardGray()`) according to active weather conditions like rain or snow. It regulates the `ambientScene` and `g_nightBridgeLight` intensities.

### 3. Scene Elements & Environment
*(Mid-Level Renderers)*
Specific functions to draw the static nature elements framing the scene.
- **`drawSkyBands`, `drawStarsField`, `drawCelestialReflection`:** Procedural generation of the sky gradients, starry background layers, and the glinting reflection on the water depending on the `sun/moon` angles.
- **`drawMountains()`, `drawWater()`:** Draws background silhouettes and water volumes.
- **`drawSun()`, `drawSunGlow()`, `drawMoonDisk()`:** Handles rendering the sky's light sources.

### 4. Dynamic Weather Rendering
*(Particle-like Systems)*
- **`drawRainLayer()`, `drawSnowLayer()`:** Simulating rain and snow. It leverages alpha-blended lines and points.
- **`drawLightningFlash()`, `drawLightningBoltShape()`:** Used for creating sudden flashes tied to the environment's `timeVal` logic.

### 5. Transport & Man-Made Structures
*(High-Level Subjects)*
These functions rely on combining the lower-level primitives:
- **`drawCar()`, `drawBus()`, `drawTrainEngine()`, `drawTrainCarriage()`, `drawTrain()`:** Complete assembly functions for rendering multiple types of vehicles. Uses offset positions tied to variables like `car1Pos` ensuring the continuous motion logic.
- **`drawBridgeTower()`, `drawBridge()`:** Draws the complex bridge superstructure combining steel beams, structural logic, and warning/aviation lights.
- **`drawSailboat()`, `drawFishingBoat()`, `drawBoatWake()`:** Handles floating vessels moving in the water, generating wake trails.
- **`drawBridgeNightLighting()`, `drawBridgeLightReflectionsOnWater()`:** Additive lighting mapped to the bridge segments active only during dusk/night.

---

## ⚙️ Algorithms Used

The animation does not rely on intense computer science data structures but utilizes **Computer Graphics/Procedural Generation Algorithms**:

### 1. Procedural Parametric Animation (Trigonometric Algorithms)
Most movement is dictated by a continually updated parameter: `timeVal`. 
- **Boats:** Use sine waves (`sin(timeVal)`) mapped to their Y-position simulating bobbing in the water.
- **Birds & Lightning:** Random or pseudo-random coordinate generation within screen boundaries constrained by trigonometric bounds. 

### 2. Linear & Radial Color Interpolation (Gradients)
- **Linear Gradients (`drawGradientRect`):** Slices a rectangle into multiple horizontal bands (e.g., 56 bands) and uses linear interpolation to transition the RGB values from `top color` to `bottom color`. 
- **Radial Gradients (`drawGradientCircle`):** Utilizes `GL_TRIANGLE_FAN` passing varying alphas per vertex. The center vertex begins with full opacity and outer border vertices use 0% opacity, which OpenGL smoothly interpolates to create a glowing orb/light beam effect used by car headlights and the sun.

### 3. Alpha Compositing (Blending)
The program relies heavily on the `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` and `glBlendFunc(GL_SRC_ALPHA, GL_ONE)` additive blending algorithm.
- Used in water surfaces, clouds, fogs, headlight beams, and reflections. The blending math ensures that light sources (like a car's headlamp) can "add" effectively to the pixel framebuffer giving off authentic illumination logic in the dark.

### 4. Simple Geometric Transformation
- **`rotatePoint` Algorithm:** Calculates the 2D rotation of a vertex `(px, py)` around an origin point `(cx, cy)` by an arbitrary angle using standard trigonometric matrices: 
   `x' = x*cos(θ) - y*sin(θ)`
   `y' = x*sin(θ) + y*cos(θ)`
This allows the engine to accurately angle geometries without relying entirely on the native `glRotatef`, offering precise algorithmic control over vertex definitions.

### 5. Color Mixing / Desaturation 
- **`mixTowardGray()`:** Uses linear blending based on a weight factor `a` to pull standard vivid colors closer to a defined gray value (`r=0.52, g=0.52, b=0.56`). This algorithmic desaturation effortlessly alters an entire colorful scene to look washed out during rain storms.

### 6. Primitive Drawing Algorithms (Circles & Lines)
No low-level pixel-plotting algorithms (like Bresenham's, DDA, or the Midpoint Circle Algorithm) were manually implemented. The codebase delegates these mathematically to OpenGL:
- **Circles (Parametric/Trigonometric Method):** Rendered using trigonometric equations. `drawCircle` loops 64 times mathematically calculating vertices points via `x = cx + r*cos(a)` and `y = cy + r*sin(a)`. These coordinates are stitched into a continuous surface using `GL_TRIANGLE_FAN`.
- **Lines:** Handled directly using OpenGL hardware-acceleration via `glBegin(GL_LINES)`. The program provides the hardware coordinates (`x1, y1` to `x2, y2`) and lets the underlying graphics driver handle the specific rasterization.

---

## 📖 In-Depth Code Walkthrough (With Code Implementation)

The `main.cpp` file is structured sequentially, defining low-level logic first, advancing to complex drawings, and finally housing the core C++ OpenGL loop. Here is a thorough breakdown of how the entire code functions:

### 1. Initialization and Input Handling
The application entry point is standard for GLUT applications:
- **`main()` and `init()`:** Sets up a double-buffered display (`GLUT_DOUBLE`) with alpha channels and configures an orthographic 2D projection (`gluOrtho2D(-1.0, 1.0, -1.0, 1.0)`).
```cpp
void init() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    rebuildPalette();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Maps window edges from -1 to 1 
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha Blending setup
}
```
- **`keyboard()` and `specialKey()`:** Listens to user keystrokes to mutate global state variables. Numbers `1-7` change `g_timeIndex`, letters like `8-0`, `Y`, `U`, `O` alter the `g_weatherIndex`.

### 2. The Universal Logic Loop (`update`)
Operating at 60Hz via `glutTimerFunc(16, ...)`, the `update()` function acts as the "game loop" directing the entire simulation:
- **Time Evolution:** Advances `timeVal`. This variable exclusively drives all procedural animations (waves, swaying sails, etc.).
```cpp
void update(int /*value*/) {
    if (!g_paused) {
        timeVal += 0.015f; // Driving force of parametric animations
    }

    // Vehicle Movement logic with looping bounds
    if (!g_paused) {
        car1Pos += 0.003f;
        if (car1Pos > 1.3f) car1Pos = -1.3f; // Loops car back to start
    }
    
    glutPostRedisplay(); // Request next frame render
    glutTimerFunc(16, update, 0); // ~60 FPS Loop
}
```

### 3. Procedural Lighting Engine (`rebuildPalette`)
Before rendering a frame, the program executes `rebuildPalette()`. It reads `g_weatherIndex` (wind, rain, snow) to compute a grayscale mixing factor. `mixTowardGray()` algorithmically washes out the environment colors to emulate overcast/stormy weather. 
```cpp
static void mixTowardGray(Color& c, float a) {
    if (a <= 0.0f) return;
    // Mathematically linearly blends a vibrant color towards a dull gray
    c.r = c.r * (1.0f - a) + 0.52f * a;
    c.g = c.g * (1.0f - a) + 0.52f * a;
    c.b = c.b * (1.0f - a) + 0.56f * a;
}
```

### 4. Code Implementation of Procedural Elements
The shapes are often mathematically constructed rather than hardcoded geometry. For example, rendering the waves (`drawWater()`):
```cpp
// Generating chopping waves in the water
for (float wx = -1.0f; wx <= 1.0f; wx += 0.02f) {
    // Calculates the Y offset using Sin functions combined with timeVal, wind, and storm data
    float waveY = wy + sinf(timeVal * (1.2f * chop) + wx * (8.0f + windStrength * 3.0f + stormAgit * 1.2f) + wy * 3.0f) * baseAmp;
    glVertex2f(wx, waveY);
}
```

Rotating the sailboats based on wind (`drawSailboat()`):
```cpp
// 2D Rotation algorithm applying Trigonometry (Sine/Cosine matrix)
void rotatePoint(float &px, float &py, float cx, float cy, float angleRad) {
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);
    float dx = px - cx;
    float dy = py - cy;
    // Re-assigns the X and Y coordinates according to the rotation matrix
    px = cx + dx * cosA - dy * sinA;
    py = cy + dx * sinA + dy * cosA;
}
```

### 5. The Render Pipeline (`display`)
The heart of the graphical output. It clears the buffer (`glClear`) and draws entities strictly from **back to front** (Painters Algorithm):
```cpp
void display() {
    rebuildPalette(); // Apply lighting logic first
    glClear(GL_COLOR_BUFFER_BIT); // Wipe screen
    
    // Draw Background to Foreground layers
    drawSkyBands(); 
    drawStarsField();
    drawSun();
    drawMountains();
    drawWater();
    
    // Draw Midground and active subjects
    drawBridge();
    drawSailboat(boat1, -0.38f, 1.0f);
    
    // Draw active foreground overlays on top!
    drawRainLayer();
    drawLightningFlash();
    
    glutSwapBuffers(); // Push the completed frame to the screen
}
```
