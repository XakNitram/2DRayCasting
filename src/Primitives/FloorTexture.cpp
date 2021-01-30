#include "pch.hpp"
#include "FloorTexture.hpp"


void FloorTexture::render(uint32_t width, uint32_t height) {
    lwvl::Framebuffer frame;

    texture.bind();
    texture.construct(width, height, nullptr);
    texture.filter(lwvl::Filter::Linear);

    frame.bind();
    frame.attach(lwvl::Attachment::Color, texture);

    Quad floor(-1.0f, -1.0f, 2.0f, 2.0f);
    lwvl::ShaderProgram texturePipeline;
    lwvl::VertexShader fv("#version 330 core\nlayout(location=0) in vec4 position;\nvoid main() { gl_Position = position; }");
    lwvl::FragmentShader ff(readFile("Shaders/default.frag"));
//        lwvl::FragmentShader ff(readFile("Shaders/mazing.frag"));
    texturePipeline.link(fv, ff);
    texturePipeline.bind();

    // Floor color:
    texturePipeline.uniform("u_Color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
//        texturePipeline.uniform("u_Color").set3f(0.61569f, 0.63529f, 0.67059f);  // Silver

    // resolution for mazing.frag
//        texturePipeline.uniform("u_Resolution").set2f(static_cast<float>(width), static_cast<float>(height));

    GLsizei prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    const GLsizei prevWidth = prevViewport[2];
    const GLsizei prevHeight = prevViewport[3];

    // Translate the viewport and draw the floor to the texture.
    glViewport(0, 0, width + 1, height + 1);
    floor.draw();

    // Undo the viewport translation.
    glViewport(0, 0, prevWidth, prevHeight);
}

uint32_t FloorTexture::slot() { return texture.slot(); }
void FloorTexture::slot(uint32_t value) { texture.slot(value); }

void FloorTexture::bind() {
    texture.bind();
}
