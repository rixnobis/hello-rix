/*

MIT License

Copyright (c) 2026 Rix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "psyqo/application.hh"
#include "psyqo/font.hh"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/triangles.hh"
#include "psyqo/scene.hh"

namespace {

class HelloRix final : public psyqo::Application {
    void prepare() override;
    void createScene() override;

  public:
    psyqo::Font<> m_font;
};

class FoxScene final : public psyqo::Scene {
    void frame() override;
    uint32_t m_frame = 0;
};

HelloRix helloRix;
FoxScene foxScene;

constexpr psyqo::Color c_orange{{.r = 232, .g = 112, .b = 24}};
constexpr psyqo::Color c_darkOrange{{.r = 168, .g = 64, .b = 16}};
constexpr psyqo::Color c_cream{{.r = 248, .g = 236, .b = 216}};
constexpr psyqo::Color c_black{{.r = 16, .g = 16, .b = 24}};

// A fox head, front on, as flat triangles in a 0..96 box. Drawn in order, so
// later ones sit on top of earlier ones.
struct Tri {
    int8_t ax, ay, bx, by, cx, cy;
    psyqo::Color color;
};

constexpr Tri c_fox[] = {
    // Ears, outer then inner.
    {8, 0, 0, 40, 36, 28, c_orange},
    {88, 0, 96, 40, 60, 28, c_orange},
    {10, 10, 6, 34, 26, 26, c_black},
    {86, 10, 90, 34, 70, 26, c_black},
    // Head: a wide top band and a point down to the muzzle.
    {0, 32, 96, 32, 48, 88, c_orange},
    {0, 32, 48, 22, 96, 32, c_orange},
    // Cheeks.
    {0, 32, 16, 60, 40, 58, c_cream},
    {96, 32, 80, 60, 56, 58, c_cream},
    // Muzzle.
    {36, 58, 60, 58, 48, 88, c_cream},
    // Shading down the bridge of the nose.
    {44, 34, 52, 34, 48, 62, c_darkOrange},
    // Eyes.
    {26, 40, 38, 40, 34, 47, c_black},
    {70, 40, 58, 40, 62, 47, c_black},
    // Nose.
    {42, 80, 54, 80, 48, 88, c_black},
};

}  // namespace

void HelloRix::prepare() {
    psyqo::GPU::Configuration config;
    config.set(psyqo::GPU::Resolution::W320)
        .set(psyqo::GPU::VideoMode::AUTO)
        .set(psyqo::GPU::ColorMode::C15BITS)
        .set(psyqo::GPU::Interlace::PROGRESSIVE);
    gpu().initialize(config);
}

void HelloRix::createScene() {
    m_font.uploadSystemFont(gpu());
    pushScene(&foxScene);
}

void FoxScene::frame() {
    auto& gpu = helloRix.gpu();
    gpu.clear({{.r = 24, .g = 28, .b = 36}});

    // A slow bob, two pixels each way.
    const int16_t bob = ((m_frame++ >> 4) & 3) == 0 ? 0 : (((m_frame >> 4) & 3) == 2 ? 2 : 1);
    const int16_t ox = 112, oy = int16_t(96 + bob);
    for (const auto& t : c_fox) {
        psyqo::Prim::Triangle tri(t.color);
        tri.setPointA({{.x = int16_t(ox + t.ax), .y = int16_t(oy + t.ay)}})
            .setPointB({{.x = int16_t(ox + t.bx), .y = int16_t(oy + t.by)}})
            .setPointC({{.x = int16_t(ox + t.cx), .y = int16_t(oy + t.cy)}});
        gpu.sendPrimitive(tri);
    }

    helloRix.m_font.print(gpu, "Hello world, I'm Rix.", {{.x = 76, .y = 40}}, c_cream);
}

int main() { return helloRix.run(); }
