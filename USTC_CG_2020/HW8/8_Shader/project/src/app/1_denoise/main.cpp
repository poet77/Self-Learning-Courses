#include <UGL/UGL>
#include <UGM/UGM>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "../../tool/Camera.h"
#include "../../tool/SimpleLoader.h"

#include <ANN/ANN.h>					
#include <ANN/ANNx.h>					
#include <ANN/ANNperf.h>	
#include <iostream>

#pragma comment(lib, "../../../../lib/ANN.lib")

using namespace Ubpa;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
gl::Texture2D loadTexture(char const* path);
gl::Texture2D genDisplacementmap(const SimpleLoader::OGLResources* resources);

// settings
unsigned int scr_width = 800;
unsigned int scr_height = 600;
float displacement_bias = 0.f;
float displacement_scale = 1.f;
float displacement_lambda = 0.2f;
bool have_denoise = false;

// camera
Camera camera(pointf3(0.0f, 0.0f, 3.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "HW8 - denoise", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    gl::Enable(gl::Capability::DepthTest);

    // build and compile our shader zprogram
    // ------------------------------------
    gl::Shader vs(gl::ShaderType::VertexShader, "../data/shaders/p3t2n3_denoise.vert");
    gl::Shader fs(gl::ShaderType::FragmentShader, "../data/shaders/light.frag");
    gl::Program program(&vs, &fs);
    rgbf ambient{ 0.2f,0.2f,0.2f };
    program.SetTex("albedo_texture", 0);
    program.SetTex("displacementmap", 1);
    program.SetVecf3("point_light_pos", { 0,5,0 });
    program.SetVecf3("point_light_radiance", { 100,100,100 });
    program.SetVecf3("ambient_irradiance", ambient);
    program.SetFloat("roughness", 0.5f );
    program.SetFloat("metalness", 0.f);

    // load model
    // ------------------------------------------------------------------
    auto spot = SimpleLoader::LoadObj("../data/models/spot_triangulated_good.obj", true);
    // world space positions of our cubes
    pointf3 instancePositions[] = {
        pointf3(0.0f,  0.0f,  0.0f),
        pointf3(2.0f,  5.0f, -15.0f),
        pointf3(-1.5f, -2.2f, -2.5f),
        pointf3(-3.8f, -2.0f, -12.3f),
        pointf3(2.4f, -0.4f, -3.5f),
        pointf3(-1.7f,  3.0f, -7.5f),
        pointf3(1.3f, -2.0f, -2.5f),
        pointf3(1.5f,  2.0f, -2.5f),
        pointf3(1.5f,  0.2f, -1.5f),
        pointf3(-1.3f,  1.0f, -1.5f)
    };

    // load and create a texture 
    // -------------------------
    gl::Texture2D spot_albedo = loadTexture("../data/textures/spot_albedo.png");

    gl::Texture2D displacementmap = genDisplacementmap(spot);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        gl::ClearColor({ ambient, 1.0f });
        gl::Clear(gl::BufferSelectBit::ColorBufferBit | gl::BufferSelectBit::DepthBufferBit); // also clear the depth buffer now!

        program.SetVecf3("camera_pos", camera.Position);

        // bind textures on corresponding texture units
        program.Active(0, &spot_albedo);
        program.Active(1, &displacementmap);

        // pass projection matrix to shader (note that in this case it could change every frame)
        transformf projection = transformf::perspective(to_radian(camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.f);
        program.SetMatf4("projection", projection);

        // camera/view transformation
        program.SetMatf4("view", camera.GetViewMatrix());
        program.SetFloat("displacement_bias", displacement_bias);
        program.SetFloat("displacement_scale", displacement_scale);
        program.SetFloat("displacement_lambda", displacement_lambda);
        program.SetBool("have_denoise", have_denoise);

        // render spots
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            float angle = 20.0f * i + 10.f * (float)glfwGetTime();
            transformf model(instancePositions[i], quatf{ vecf3(1.0f, 0.3f, 0.5f), to_radian(angle) });
            program.SetMatf4("model", model);
            spot->va->Draw(&program);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    delete spot;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::Movement::DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        have_denoise = !have_denoise;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    gl::Viewport({ 0, 0 }, width, height);
    scr_width = width;
    scr_height = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

gl::Texture2D loadTexture(char const* path)
{
    gl::Texture2D tex;
    tex.SetWrapFilter(gl::WrapMode::Repeat, gl::WrapMode::Repeat, gl::MinFilter::Linear, gl::MagFilter::Linear);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    gl::PixelDataFormat c2f[4] = {
        gl::PixelDataFormat::Red,
        gl::PixelDataFormat::Rg,
        gl::PixelDataFormat::Rgb,
        gl::PixelDataFormat::Rgba
    };
    gl::PixelDataInternalFormat c2if[4] = {
        gl::PixelDataInternalFormat::Red,
        gl::PixelDataInternalFormat::Rg,
        gl::PixelDataInternalFormat::Rgb,
        gl::PixelDataInternalFormat::Rgba
    };
    if (data)
    {
        tex.SetImage(0, c2if[nrChannels - 1], width, height, c2f[nrChannels - 1], gl::PixelDataType::UnsignedByte, data);
        tex.GenerateMipmap();
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return tex;
}


void ChangeGlobalVariable(float* displacementData)
{
    // Set bias and scale to let [bias, bias + scale]->[0, 1]
    float min = displacementData[0], max = displacementData[0];
    for (int i = 0; i < 1024 * 1024; i++)
    {
        min = displacementData[i] < min ? displacementData[i] : min;
        max = displacementData[i] > max ? displacementData[i] : max;
    }
    displacement_bias = min;
    displacement_scale = max - min;
    displacement_lambda = 0.3f;

    for (int i = 0; i < 1024 * 1024; i++)
    {
        displacementData[i] = (displacementData[i] - displacement_bias) / displacement_scale;
    }
    //std::cout << min << " " << max << std::endl;
}

void FillDisplacementMap(float* displacementData, const SimpleLoader::OGLResources* resources)
{
    bool* isDrawed = new bool[1024 * 1024];
    std::vector<vecf3> sum(resources->positions.size(), vecf3(0.f));    // Sum of the positions of the adjoint points
    std::vector<size_t> num(resources->positions.size(), 0);            // Numbers of the adjoint points

    // visit every triangles
    for (size_t i = 0; i < resources->indices.size(); i += 3)
    {
        size_t idx0 = resources->indices[i];
        size_t idx1 = resources->indices[i + 1];
        size_t idx2 = resources->indices[i + 2];
        const auto& p0 = resources->positions[idx0].cast_to<vecf3>();
        const auto& p1 = resources->positions[idx1].cast_to<vecf3>();
        const auto& p2 = resources->positions[idx2].cast_to<vecf3>();
        num[idx0] += 2;         num[idx1] += 2;         num[idx2] += 2;
        sum[idx0] += p1 + p2;   sum[idx1] += p2 + p0;   sum[idx2] += p0 + p1;
    }

    // Consider the same vertices on the boundary
    std::vector<bool> isChecked(resources->positions.size(), false);

    // Init
    for (int i = 0; i < 1024 * 1024; i++)
    {
        displacementData[i] = 0;
        isDrawed[i] = false;
    }

    // Set the displacement data for the vertices
    for (size_t i = 0; i < resources->positions.size(); i++)
    {
        if (isChecked[i]) continue;
        auto a = resources->positions[i];
        auto a_tex = resources->texcoords[i];
        int x = (int)std::round(std::clamp(a_tex[0], 0.f, 1.f) * 1024 - 0.5f);
        int y = (int)std::round(std::clamp(a_tex[1], 0.f, 1.f) * 1024 - 0.5f);
        auto& val = displacementData[y * 1024 + x];
        std::vector<size_t> indice;     // Index of the vertices which have the same position with a

        for (size_t j = i + 1; j < resources->positions.size(); j++)
        {
            auto b = resources->positions[j];
            if (!isChecked[j] && (a - b).norm() < 1e-5)
            {
                isChecked[j] = true;
                indice.push_back(j);
                num[i] += num[j];
                sum[i] += sum[j];
            }
        }
        // <delta_i, n_i>(note that every edge has been calculated twice in sum[i] and num[i])
        val = (resources->positions[i].cast_to<vecf3>() - sum[i] / (float)num[i]).dot(resources->normals[i].cast_to<vecf3>());
        isDrawed[y * 1024 + x] = true;

        // Copy the value to the vertices with same positions
        for (size_t k = 0; k < indice.size(); k++)
        {
            auto b_tex = resources->texcoords[indice[k]];
            int bx = (int)std::round(std::clamp(b_tex[0], 0.f, 1.f) * 1024 - 0.5f);
            int by = (int)std::round(std::clamp(b_tex[1], 0.f, 1.f) * 1024 - 0.5f);
            displacementData[by * 1024 + bx] = val;
            isDrawed[by * 1024 + bx] = true;
        }
    }

    // Fill the displacement map using ANN
    int POINT_NUM = 1024 * 1024;
    int ARR_POINT_NUM = 0;
    constexpr int DIM = 2;
    constexpr int K = 1;	//Find K points to decide the value of the point

    ANNpointArray ptsArr = annAllocPts(POINT_NUM, DIM);
    for (int x = 0; x < 1024; x++)
        for (int y = 0; y < 1024; y++)
            if (isDrawed[y * 1024 + x])
            {
                ptsArr[ARR_POINT_NUM][0] = x;
                ptsArr[ARR_POINT_NUM][1] = y;
                ARR_POINT_NUM++;
            }

    ANNbd_tree tree(ptsArr, ARR_POINT_NUM, DIM);
    ANNpoint queryPt = annAllocPt(2);

    for (int x = 0; x < 1024; x++)
    {
        for (int y = 0; y < 1024; y++)
        {
            if (isDrawed[y * 1024 + x]) continue;
            ANNidx idxArr[K];
            ANNdist distArr[K];
            queryPt[0] = x;
            queryPt[1] = y;
            tree.annkSearch(queryPt, K, idxArr, distArr);

            // Decide the value with the average of K points
            float weight_sum = 0;
            for (int k = 0; k < K; k++)
            {
                //if (abs(ptsArr[idxArr[k]][0] - x) < 2 && abs(ptsArr[idxArr[k]][1] - y) < 2)
                {
                    int m = (int)ptsArr[idxArr[k]][1];
                    int n = (int)ptsArr[idxArr[k]][0];
                    float weight = 1.f / ((m - y) * (m - y) + (n - x) * (n - x));
                    displacementData[y * 1024 + x] += weight * displacementData[m * 1024 + n];
                    weight_sum += weight;
                }
            }
            displacementData[y * 1024 + x] /= weight_sum < 1e-6 ? 1 : weight_sum;
        }
    }

    annDeallocPts(ptsArr);
    annDeallocPt(queryPt);
    delete[] isDrawed;
}

gl::Texture2D genDisplacementmap(const SimpleLoader::OGLResources* resources) {
    //const float* displacementData = new float[1024 * 1024];
    float* displacementData = new float[1024 * 1024];
    // TODO: HW8 - 1_denoise | genDisplacementmap
    // 1. set displacementData with resources's positions, indices, normals, ...
    // 2. change global variable: displacement_bias, displacement_scale, displacement_lambda

    // ...
    FillDisplacementMap(displacementData, resources);
    ChangeGlobalVariable(displacementData);

    gl::Texture2D displacementmap;
    displacementmap.SetImage(0, gl::PixelDataInternalFormat::Red, 1024, 1024, gl::PixelDataFormat::Red, gl::PixelDataType::Float, displacementData);
    displacementmap.SetWrapFilter(gl::WrapMode::Repeat, gl::WrapMode::Repeat,
        gl::MinFilter::Linear, gl::MagFilter::Linear);
    stbi_uc* stbi_data = new stbi_uc[1024 * 1024];
    for (size_t i = 0; i < 1024 * 1024; i++)
        stbi_data[i] = static_cast<stbi_uc>(std::clamp(displacementData[i] * 255.f, 0.f, 255.f));
    stbi_write_png("../data/1_denoise_displacement_map.png", 1024, 1024, 1, stbi_data, 1024);
    delete[] stbi_data;
    delete[] displacementData;
    return displacementmap;
}









