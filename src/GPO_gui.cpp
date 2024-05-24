//imgui
#include "GpO.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void setupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void renderImGui(int *scene_flag, int *model_flag, int *render_texture, int *color_levels, float *toon_border,
                vec3 *model_color, float *b_lightness, float *y_lightness, float *alpha, float *beta) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // First window in top right corner (only first use, can be moved)
    int width_window = 255;
    int height_window = 175;
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + (1200 - width_window), main_viewport->WorkPos.y)); //ImGuiCond_FirstUseEver for only first use
    ImGui::SetNextWindowSize(ImVec2(width_window, height_window));
    
    // First window: shader and model selector
    ImGui::Begin("Scenes", NULL, ImGuiWindowFlags_NoMove || ImGuiWindowFlags_NoResize);
    ImGui::SeparatorText("Shaders");
    ImGui::RadioButton("Pixel1", scene_flag, PIXEL1); ImGui::SameLine();
    ImGui::RadioButton("Pixel2", scene_flag, PIXEL2);
    if(*model_flag != SPIDER){ //TODO
        ImGui::RadioButton("Toon", scene_flag, TOON); ImGui::SameLine();
        ImGui::RadioButton("Phong", scene_flag, PHONG); ImGui::SameLine();
        ImGui::RadioButton("Blinn-Phong", scene_flag, BLINN);
        ImGui::RadioButton("Gooch", scene_flag, GOOCH);
    }
    
    ImGui::SeparatorText("Models");
    ImGui::RadioButton("Spiderman", model_flag, SPIDER); ImGui::SameLine();
    ImGui::RadioButton("Fountain-Ball", model_flag, BALL);
    ImGui::RadioButton("Helmet", model_flag, HELMET); ImGui::SameLine();
    ImGui::RadioButton("Cat", model_flag, CAT);
    ImGui::End();

    // Second window pos: top left corner
    width_window = 300;
    height_window = 350;
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(300, 350));

    // Second window: Shader and texture options
    ImGui::Begin("Scene options", NULL, ImGuiWindowFlags_NoMove && ImGuiWindowFlags_NoResize);
    if(*scene_flag != PIXEL1 && *scene_flag != PIXEL2){ // TODO PIXEL
        if(*render_texture == 0){ // color shouldn't be defined with active texture
            float col[3] = {model_color->x,model_color->y,model_color->z};
            ImGui::ColorPicker3("Model color", col, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
            ImGui::ColorEdit3("Model color", col);
            model_color->x = col[0];
            model_color->y = col[1];
            model_color->z = col[2];
        }
        if(*model_flag != BALL){
            ImGui::Checkbox("Render textures", (bool *)render_texture); // Fountain ball doesn't have texture,
        } else {
            ImGui::Dummy(ImVec2(0.0f, 20.0f)); // vertical spacing
        }
    }

    ImGui::SeparatorText("Shader params");
    if(*scene_flag == TOON){
        ImGui::SliderFloat("Border", toon_border, 0.0f, 0.5f, "ratio = %.05f");
        ImGui::SliderInt("Color levels", color_levels, 3, 7);
    }
    if(*scene_flag == GOOCH){
        ImGui::SliderFloat("B_lightness", b_lightness, 0.0f, 1.f, "ratio = %.05f");
        ImGui::SliderFloat("Y_lightness", y_lightness, 0.0f, 1.f, "ratio = %.05f");
        ImGui::SliderFloat("Alpha", alpha, 0.0f, 1.f, "ratio = %.05f");
        ImGui::SliderFloat("Beta", beta, 0.0f, 1.f, "ratio = %.05f");
    }
    ImGui::End();
        
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void terminateImGui(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}