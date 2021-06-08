#include "pch.hpp"
#include "FloorTexture.hpp"


void FloorTexture::render(uint32_t width, uint32_t height) {
    lwvl::Framebuffer frame;

    bind();
    construct(
        width, height, nullptr,
        lwvl::ChannelLayout::RGBA16F,
        lwvl::ChannelOrder::RGBA,
        lwvl::ByteFormat::HalfFloat
    );
    filter(lwvl::Filter::Linear);

    frame.bind();
    frame.attach(lwvl::Attachment::Color, *this);

    Quad floor(-1.0f, -1.0f, 2.0f, 2.0f);
    lwvl::ShaderProgram textureControl;
    lwvl::VertexShader fv(
        "#version 330 core\nlayout(location=0) in vec4 position;\nvoid main() { gl_Position = position; }"
    );
    lwvl::FragmentShader ff(lwvl::FragmentShader::readFile("Data/Shaders/default.frag"));
    //    lwvl::FragmentShader ff(readFile("Shaders/mazing.frag"));
    textureControl.link(fv, ff);
    textureControl.bind();

    // Floor color:
    textureControl.uniform("u_Color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
    //textureControl.uniform("u_Color").set3f(0.61569f, 0.63529f, 0.67059f);  // Silver

    // resolution for mazing.frag
    //    textureControl.uniform("u_Resolution").set2f(static_cast<float>(width), static_cast<float>(height));

    // Viewport is not bound to the framebuffer so we have to restore the previous viewport.
    GLsizei prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    const auto[prevX, prevY, prevWidth, prevHeight] = prevViewport;

    // Translate the viewport and draw the floor to the texture.
    glViewport(0, 0, GLsizei(width + 1), GLsizei(height + 1));
    lwvl::clear();
    floor.draw();

    // Undo the viewport translation.
    glViewport(prevX, prevY, prevWidth, prevHeight);
    lwvl::Framebuffer::clear();
    lwvl::ShaderProgram::clear();
}
