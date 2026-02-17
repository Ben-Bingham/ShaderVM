#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>

#include <imgui.h>

#include <utility/OpenGl/Shader.h>
#include <utility/OpenGl/VertexAttributeObject.h>
#include <utility/OpenGl/Buffer.h>
#include <utility/OpenGl/RenderTarget.h>

#include <utility/Camera.h>
#include <utility/TimeScope.h>
#include <utility/Transform.h>
#include <utility/Geometry.h>

#include "MoveCamera.h"
#include "Boilerplate.h"
#include <utility/OpenGl/SSBO.h>

using namespace RenderingUtilities;

std::vector<unsigned int> instructions;

unsigned int Build3R(int opcode, int r1, int r2, int r3) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);
    r2 = glm::clamp(r2, 0, 15);
    r3 = glm::clamp(r3, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);
    inst |= ((unsigned int)r2 << 20);
    inst |= ((unsigned int)r3 << 16);

    return inst;
}

unsigned int Build2R(int opcode, int r1, int r2) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);
    r2 = glm::clamp(r2, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);
    inst |= ((unsigned int)r2 << 20);

    return inst;
}

unsigned int BuildRS(int opcode, int r1, int constant) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);

    unsigned int adjustedConstant = (unsigned int)constant << 8;
    adjustedConstant = adjustedConstant >> 8;

    inst |= adjustedConstant;

    return inst;
}

int main() {
    GLFWwindow* window = InitGraphics();

    glm::ivec2 defaultFramebufferSize{ 1600, 900 };
    glm::ivec2 lastFrameViewportSize{ defaultFramebufferSize };

    RenderTarget rendererTarget{ defaultFramebufferSize };

    Shader assemblyParser{
        "assets\\shaders\\assemblyParser.vert",
        "assets\\shaders\\assemblyParser.frag"
    };

    Camera camera{ };

    VertexAttributeObject vao{ };

    Shape square = GetSquare();

    VertexBufferObject vbo{ square.vertices };

    ElementBufferObject ebo{ square.indices };

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    // Registers:
    const unsigned int reg_z = 0;
    const unsigned int reg_s0 = 1;
    const unsigned int reg_s1 = 2;
    const unsigned int reg_s2 = 3;
    const unsigned int reg_s3 = 4;
    const unsigned int reg_s4 = 5;
    const unsigned int reg_s5 = 6;
    const unsigned int reg_v0 = 7;
    const unsigned int reg_v1 = 8;
    const unsigned int reg_v2 = 9;
    const unsigned int reg_v3 = 10;
    const unsigned int reg_v4 = 11;
    const unsigned int reg_pc = 12;
    const unsigned int reg_t = 13;
    const unsigned int reg_s = 14;
    const unsigned int reg_c = 15;

    // Op codes:
    const unsigned int inst_add = 0;
    const unsigned int inst_multiply = 1;
    const unsigned int inst_negate = 2;
    const unsigned int inst_reciprocal = 3;
    const unsigned int inst_move = 4;
    const unsigned int inst_getComponent = 5;
    const unsigned int inst_setComponent = 6;
    const unsigned int inst_load = 7;
    const unsigned int inst_readMemory = 8;
    const unsigned int inst_writeMemory = 9;

    instructions.resize(11);
    instructions[0] = Build3R(inst_getComponent, reg_pc, reg_s0, reg_z);  // getComponent(pc, s0)    # s0 = pc.x
    instructions[1] = BuildRS(inst_load, reg_s5, 1);			          // load(s5, 1)             # s5 = 1
    instructions[2] = Build3R(inst_getComponent, reg_pc, reg_s1, reg_s5); // getComponent(pc, s1)    # s1 = pc.y

    instructions[3] = Build3R(inst_getComponent, reg_s, reg_s3, reg_z);   // getComponent(s, s3, z)  # s3 = s.x
    instructions[4] = Build2R(inst_reciprocal, reg_s3, reg_s2);           // reciprocal(s3, s2)      # s2 = 1 / s3
    instructions[5] = Build3R(inst_getComponent, reg_s, reg_s4, reg_s5);  // getComponent(s, s4, s5) # s4 = s.y
    instructions[6] = Build2R(inst_reciprocal, reg_s4, reg_s3);           // reciprocal(s4, s3)      # s3 = 1 / s4

    instructions[7] = Build3R(inst_multiply, reg_s0, reg_s2, reg_s0);     // multiply(s0, s2, s0)    # s0 = s0 * s2
    instructions[8] = Build3R(inst_multiply, reg_s1, reg_s3, reg_s1);     // multiply(s1, s3, s1)    # s1 = s1 * s3

    instructions[9] = Build3R(inst_setComponent, reg_c, reg_s0, reg_z);   // setComponent(c, s0, z)  # c.x = s0
    instructions[10] = Build3R(inst_setComponent, reg_c, reg_s1, reg_s5); // setComponent(c, s1, s5) # c.y = s1

    SSBO<unsigned int> instructionsSSBO{ 0, instructions };

    Transform transform{ };
    transform.position = glm::vec3{ 0.0f, 0.0f, 5.0f };

    std::chrono::duration<double> frameTime{ };
    std::chrono::duration<double> renderTime{ };

    bool mouseOverViewPort{ false };
    glm::ivec2 viewportOffset{ 0, 0 };

    while (!glfwWindowShouldClose(window)) {
        TimeScope frameTimeScope{ &frameTime };

        glfwPollEvents();

        glm::ivec2 mousePositionWRTViewport{ mousePosition.x - viewportOffset.x, lastFrameViewportSize.y - (viewportOffset.y - mousePosition.y) };

        MoveCamera(camera, window, static_cast<float>(frameTime.count()), mousePositionWRTViewport, lastFrameViewportSize, mouseOverViewPort);

        {
            TimeScope renderingTimeScope{ &renderTime };

            rendererTarget.Bind();

            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            assemblyParser.Bind();
            assemblyParser.SetVec3("screenSize", glm::vec3{ (float)rendererTarget.GetSize().x, (float)rendererTarget.GetSize().y, 0.0f});
            assemblyParser.SetInt("instructionCount", instructions.size());

            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)rendererTarget.GetSize().x / (float)rendererTarget.GetSize().y, camera.nearPlane, camera.farPlane);
            transform.CalculateMatrix();
            glm::mat4 mvp = projection * camera.View() * transform.matrix;

            assemblyParser.SetMat4("mvp", mvp);

            vao.Bind();
            glDrawElements(GL_TRIANGLES, square.Size(), GL_UNSIGNED_INT, nullptr);

            rendererTarget.Unbind();
        }

        ImGuiStartFrame();

        //ImGui::ShowDemoWindow();
        //ImGui::ShowMetricsWindow();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Keep track of this so that we can make these changes after the imgui frame is finished
        size_t changedPointLightIndex{ 0 };

        glm::ivec2 newViewportSize{ };

        { ImGui::Begin("Viewport");
            // Needs to be the first call after "Begin"
            newViewportSize = glm::ivec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

            // Display the frame with the last frames viewport size (The same size it was rendered with)
            ImGui::Image((ImTextureID)rendererTarget.GetTexture().Get(), ImVec2{ (float)lastFrameViewportSize.x, (float)lastFrameViewportSize.y }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

            mouseOverViewPort = ImGui::IsItemHovered();

            viewportOffset = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y };

        } ImGui::End(); // Viewport

        ImGuiEndFrame();

        // After ImGui has rendered its frame, we resize the framebuffer if needed for next frame
        if (newViewportSize != lastFrameViewportSize) {
            rendererTarget.Resize(newViewportSize);
        }

        lastFrameViewportSize = newViewportSize;

        glfwSwapBuffers(window);
    }

    DestroyGraphics();
}
