#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#include <cmath>

#include "glew/glew.h"

#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr
#include "glm/gtx/rotate_vector.hpp""

#include "Sources/Player.h"
#include "Sources/Pipe.h"
#include "Sources/Bird.h"
#include "Sources/Bomb.h"
#include "Sources/PointLight.h"
#include "Sources/ConstantForce.h"
#include "Sources/LeapFrogSolver.h"
#include "Sources/GroundForce.h"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;    

// Shader utils
int check_compile_error(GLuint shader, const char ** sourceBuffer);
int check_link_error(GLuint program);
GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize);
GLuint compile_shader_from_file(GLenum shaderType, const char * fileName);

// OpenGL utils
bool checkError(const char* title);

struct Camera
{
    float radius;
    float theta;
    float phi;
    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;
};
void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);

struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;
void init_gui_states(GUIStates & guiStates);

int main( int argc, char **argv )
{
    int width = 1024, height= 768;
    float widthf = (float) width, heightf = (float) height;
    float t = 0;
	float dt = 0;
    float fps = 0.f;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    int const DPI = 1;
# endif

    // Open a window and create its OpenGL context
    GLFWwindow * window = glfwCreateWindow(width/DPI, height/DPI, "Flappy Runner", 0, 0);
    if( ! window )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }
    glfwMakeContextCurrent(window);

    // Init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
          exit( EXIT_FAILURE );
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len))
    {
        fprintf(stderr, "Could not init GUI renderer.\n");
        exit(EXIT_FAILURE);
    }

    // Init viewer structures
    Camera camera;
    camera_defaults(camera);
    GUIStates guiStates;
    init_gui_states(guiStates);

    // Load images and upload textures
    GLuint textures[9];
    glGenTextures(9, textures);
    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    unsigned char * spec = stbi_load("textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);

	unsigned char * pipeTexture = stbi_load("textures/pipe_ambient.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pipeTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Pipe Ambient %dx%d:%d\n", x, y, comp);

	unsigned char * pipeSpecTexture = stbi_load("textures/pipe_spec.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pipeSpecTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Pipe Spec %dx%d:%d\n", x, y, comp);
	
	unsigned char * bombTexture = stbi_load("textures/bomb.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bombTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Bomb %dx%d:%d\n", x, y, comp);

	unsigned char * birdTexture = stbi_load("textures/bird.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, birdTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Bird %dx%d:%d\n", x, y, comp);

	unsigned char * birdSpecTexture = stbi_load("textures/bird_spec.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, birdSpecTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Bird %dx%d:%d\n", x, y, comp);

	unsigned char * playerTexture = stbi_load("textures/player.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, playerTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Player %dx%d:%d\n", x, y, comp);

	unsigned char * playerSpecTexture = stbi_load("textures/player_spec.png", &x, &y, &comp, 4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, playerSpecTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	fprintf(stderr, "Player %dx%d:%d\n", x, y, comp);

	checkError("Texture Initialization");

    // Try to load and compile shaders
    GLuint vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, "src\\Shaders\\flappyrun.vert");
    GLuint geomShaderId = compile_shader_from_file(GL_GEOMETRY_SHADER, "src\\Shaders\\flappyrun.geom");
    GLuint fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, "src\\Shaders\\flappyrun.frag");
    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, vertShaderId);
    glAttachShader(programObject, geomShaderId);
    glAttachShader(programObject, fragShaderId);
    glLinkProgram(programObject);
    if (check_link_error(programObject) < 0)
        exit(1);

	// Try to load and compile shaders
	GLuint shadowVertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, "src\\Shaders\\shadow.vert");
	GLuint shadowFragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, "src\\Shaders\\shadow.frag");
	GLuint shadowProgramObject = glCreateProgram();
	glAttachShader(shadowProgramObject, shadowVertShaderId);
	glAttachShader(shadowProgramObject, shadowFragShaderId);
	glLinkProgram(shadowProgramObject);

	if (check_link_error(shadowProgramObject) < 0)
		exit(1);

    // Upload uniforms
    GLuint mvpLocation = glGetUniformLocation(programObject, "MVP");
    GLuint mvLocation = glGetUniformLocation(programObject, "MV");
    GLuint timeLocation = glGetUniformLocation(programObject, "Time");
    GLuint diffuseLocation = glGetUniformLocation(programObject, "Diffuse");
    GLuint specLocation = glGetUniformLocation(programObject, "Specular");
    GLuint lightLocation = glGetUniformLocation(programObject, "Light");
	GLuint TransLocation = glGetUniformLocation(programObject, "TranslationPlayer");
    GLuint specularPowerLocation = glGetUniformLocation(programObject, "SpecularPower");
	GLuint directionalLightDirectionLocation = glGetUniformLocation(programObject, "DirectionalLightDirection");
	GLuint directionalLightColorLocation = glGetUniformLocation(programObject, "DirectionalLightColor");
	GLuint directionalLightIntensityLocation = glGetUniformLocation(programObject, "DirectionalLightIntensity");
	GLuint pointLightPositionsLocation = glGetUniformLocation(programObject, "PointLightPositions");
	GLuint pointLightColorLocation = glGetUniformLocation(programObject, "PointLightColor");
	GLuint pointLightIntensityLocation = glGetUniformLocation(programObject, "PointLightIntensity");
	GLuint spotLightDirectionLocation = glGetUniformLocation(programObject, "SpotLightDirection");
	GLuint spotLightPositionLocation = glGetUniformLocation(programObject, "SpotLightPosition");
	GLuint spotLightAngleLocation = glGetUniformLocation(programObject, "SpotLightAngle");
	GLuint spotLightPenumbraAngleLocation = glGetUniformLocation(programObject, "SpotLightPenumbraAngle");
	GLuint spotLightColorLocation = glGetUniformLocation(programObject, "SpotLightColor");
	GLuint spotLightIntensityLocation = glGetUniformLocation(programObject, "SpotLightIntensity");
	GLuint rotationLocation = glGetUniformLocation(programObject, "Rotation");
    glProgramUniform1i(programObject, diffuseLocation, 0);
    glProgramUniform1i(programObject, specLocation, 1);
	glProgramUniform3fv(programObject, directionalLightColorLocation, 1, glm::value_ptr(glm::vec3(1.0,1.0, 1.0)));
	glProgramUniform1f(programObject, directionalLightIntensityLocation, 0.1);
	glProgramUniform3fv(programObject, pointLightColorLocation, 1, glm::value_ptr(glm::vec3(1.0, 0.8, 0.5)));
	glProgramUniform1f(programObject, pointLightIntensityLocation, 5.0);
	glProgramUniform3fv(programObject, spotLightColorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(programObject, spotLightIntensityLocation, 10.0);
	GLuint shadowLocation = glGetUniformLocation(programObject, "Shadow");
	GLuint WtLSLocation = glGetUniformLocation(programObject, "WorldToLightScreen");
	glProgramUniform1i(programObject, shadowLocation, 2);

	GLuint shadowMVPLocation = glGetUniformLocation(shadowProgramObject, "MVP");
	GLuint shadowMVLocation = glGetUniformLocation(shadowProgramObject, "MV");

    if (!checkError("Uniforms"))
        exit(1);

    // Load geometry
    int cube_triangleCount = 12;
    int cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	int plane_triangleCount = 2;
    int plane_triangleList[] = {
			0, 1, 2, 2, 1, 3,
	};
    float plane_uvs[] = {
			0.f, 0.f, 0.f, 50.f, 50.f, 0.f, 50.f, 50.f,
	};
    float plane_vertices[] = {
			-10.0, -1.0, 20.0, 10.0, -1.0, 20.0, -10.0, -1.0, -80.0, 10.0, -1.0, -80.0
	};
    float plane_normals[] = {
			0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
	};

    // Vertex Array Object
    GLuint vao[6];
    glGenVertexArrays(6, vao);

    // Vertex Buffer Objects
    GLuint vbo[24];
    glGenBuffers(24, vbo);

    // Cube
    glBindVertexArray(vao[0]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);

    // Plane
    glBindVertexArray(vao[1]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float widthCorridor = 10.f;

	Player player(widthCorridor);

	// Player
	glBindVertexArray(vao[2]);
	// Bind indices and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[16]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, player.GetTriangleList().size() * sizeof(unsigned int), &player.GetTriangleList()[0], GL_STATIC_DRAW);
	// Bind vertices and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[17]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, player.GetVertices().size() * sizeof(glm::vec3), &player.GetVertices()[0], GL_STATIC_DRAW);
	// Bind normals and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[18]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, player.GetNormals().size() * sizeof(glm::vec3), &player.GetNormals()[0], GL_STATIC_DRAW);
	// Bind uv coords and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[19]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 2, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, player.GetUVs().size() * sizeof(glm::vec3), &player.GetUVs()[0], GL_STATIC_DRAW);

	// Unbind everything. Potentially illegal on some implementations
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<Pipe*> pipes;
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -40.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -40.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -80.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -80.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -120.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -160.f), 20.f));
	pipes.push_back(new Pipe(glm::vec3(rand() % 11 - 5.f, 0.f, -160.f), 20.f));

	// Pipe
	glBindVertexArray(vao[3]);
	// Bind indices and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pipes[0]->GetTriangleList().size() * sizeof(unsigned int), &pipes[0]->GetTriangleList()[0], GL_STATIC_DRAW);
	// Bind vertices and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, pipes[0]->GetVertices().size() * sizeof(glm::vec3), &pipes[0]->GetVertices()[0], GL_STATIC_DRAW);
	// Bind normals and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, pipes[0]->GetNormals().size() * sizeof(glm::vec3), &pipes[0]->GetNormals()[0], GL_STATIC_DRAW);
	// Bind uv coords and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 2, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, pipes[0]->GetUVs().size() * sizeof(glm::vec3), &pipes[0]->GetUVs()[0], GL_STATIC_DRAW);

	// Unbind everything. Potentially illegal on some implementations
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<Bird*> birds;
	birds.push_back(new Bird(&player, -3.f));
	birds.push_back(new Bird(&player, 2.5f));
	birds.push_back(new Bird(&player, 0.f));

	// Bird
	glBindVertexArray(vao[4]);
	// Bind indices and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[20]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, birds[0]->GetTriangleList().size() * sizeof(unsigned int), &birds[0]->GetTriangleList()[0], GL_STATIC_DRAW);
	// Bind vertices and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[21]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, birds[0]->GetVertices().size() * sizeof(glm::vec3), &birds[0]->GetVertices()[0], GL_STATIC_DRAW);
	// Bind normals and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[22]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, birds[0]->GetNormals().size() * sizeof(glm::vec3), &birds[0]->GetNormals()[0], GL_STATIC_DRAW);
	// Bind uv coords and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[23]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 2, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, birds[0]->GetUVs().size() * sizeof(glm::vec3), &birds[0]->GetUVs()[0], GL_STATIC_DRAW);

	// Unbind everything. Potentially illegal on some implementations
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Bomb bomb(glm::vec3(rand() % 11 - 5.f, 0.f, -80.f), 20.f);
	// Bomb
	glBindVertexArray(vao[5]);
	// Bind indices and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[12]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bomb.GetTriangleList().size() * sizeof(unsigned int), &bomb.GetTriangleList()[0], GL_STATIC_DRAW);
	// Bind vertices and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, bomb.GetVertices().size() * sizeof(glm::vec3), &bomb.GetVertices()[0], GL_STATIC_DRAW);
	// Bind normals and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, bomb.GetNormals().size() * sizeof(glm::vec3), &bomb.GetNormals()[0], GL_STATIC_DRAW);
	// Bind uv coords and upload data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)* 2, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, bomb.GetUVs().size() * sizeof(glm::vec3), &bomb.GetUVs()[0], GL_STATIC_DRAW);

	// Unbind everything. Potentially illegal on some implementationsc
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<PointLight*> pointLights;
	pointLights.push_back(new PointLight(glm::vec3(4.5f, 5.0f, -20.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-4.5f, 5.0f, -20.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(5.0f, 5.0f, -40.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-5.0f, 5.0f, -40.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(5.0f, 5.0f, -60.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-5.0f, 5.0f, -60.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(5.0f, 5.0f, -80.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-5.0f, 5.0f, -80.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(5.0f, 5.0f, -100.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-5.0f, 5.0f, -100.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(5.0f, 5.0f, -120.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));
	pointLights.push_back(new PointLight(glm::vec3(-5.0f, 5.0f, -120.0f), glm::vec3(1.0, 0.5, 0.5), 1.0));

	float pointLigthsPositionInWorld[36];

	float angle = 3.14f / 2;
	glm::mat4 rotation = glm::mat4(glm::vec4(cos(angle), sin(angle), 0, 0), glm::vec4(-sin(angle), cos(angle), 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1));
	
	std::vector<MovableObject*> objects;
	objects.push_back(&bomb);

	for (Bird *bird : birds)
	{
		objects.push_back(bird);
	}

	for (Pipe *pipe : pipes)
	{
		objects.push_back(pipe);
	}

	for (PointLight *pL : pointLights)
	{
		objects.push_back(pL);
	}

    // Viewport 
    glViewport( 0, 0, width, height  );

	int numberPipesBeforeSpeedUp = 6;
	int numberPipesBeforeBomb = 6;
	float pipescrossed = 0;
	float combo = 0;

	ConstantForce gravity(glm::vec3(0.f, -10.f, 0.f));
	ConstantForce JumpForce(glm::vec3(0.f, 400.f, 0.f));
	ConstantForce FlapForce(glm::vec3(0.f, 200.f, 0.f));
	ConstantForce SpeedUpForce(glm::vec3(0.f, 0.f, 1.f));
	ConstantForce SpeedDownForce(glm::vec3(0.f, 0.f, -150.f));
	ConstantForce MovementBird(glm::vec3(0.f, 0.f, -1.01f));
	ConstantForce PipeHitForce(glm::vec3(0.f, 0.f, 200.f));
	ConstantForce BombHitForce(glm::vec3(0.f, 0.f, 600.f));

	LeapfrogSolver leapfrog;

	GroundForce groundForce(1.f, leapfrog, -1.f, WallType::Ground, glm::vec3(0.f, 1.f, 0.f), false);
	GroundForce backWallForce(1.f, leapfrog, 10.f, WallType::WallZ, glm::vec3(0.f, 0.f, -1.f), true);
	GroundForce frontWallForce(1.f, leapfrog, 0.f, WallType::WallZ, glm::vec3(0.f, 0.f, 1.f), false);
	GroundForce leftWallForce(1.f, leapfrog, -widthCorridor/2, WallType::WallX, glm::vec3(1.f, 0.f, 0.f), false);
	GroundForce rightWallForce(1.f, leapfrog, widthCorridor/2, WallType::WallX, glm::vec3(-1.f, 0.f, 0.f), true);

	float speed = 0.f;
	float LastTimePassed = 0.f;

	// Create shadow FBO
	GLuint shadowFbo;
	glGenFramebuffers(1, &shadowFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
	// Create shadow textures
	//const int LIGHT_COUNT = 13;
	const int LIGHT_SHADOW_RES = 1024;
	GLuint shadowTexture;
	glGenTextures(1, &shadowTexture);
	//for (int i = 0; i < LIGHT_COUNT; ++i)
	//{
		glBindTexture(GL_TEXTURE_2D, shadowTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,LIGHT_SHADOW_RES, LIGHT_SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//}
	// Create a render buffer since we don't need to read shadow color
	// in a texture
	GLuint shadowRenderBuffer;
	glGenRenderbuffers(1, &shadowRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, LIGHT_SHADOW_RES, LIGHT_SHADOW_RES);
	// Attach the first texture to the depth attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
	// Attach the renderbuffer
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building shadow framebuffer\n");
		exit(EXIT_FAILURE);
	}
	// Fall back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    do
    {
		t = glfwGetTime();
		
        // Mouse states
        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements
        /*int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            double x; double y;
            glfwGetCursorPos(window, &x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS)
        {
            double mousex; double mousey;
            glfwGetCursorPos(window, &mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera_pan(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }*/

		if (player.IsDead() == false)
		{
			// Left
			int leftPressed = glfwGetKey(window, GLFW_KEY_LEFT);
			if (leftPressed == GLFW_PRESS)
			{
				player.MoveLeft();
			}

			// Right
			int rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT);
			if (rightPressed == GLFW_PRESS)
			{
				player.MoveRight();
			}

			// Space
			int spacePressed = glfwGetKey(window, GLFW_KEY_SPACE);
			if (spacePressed == GLFW_PRESS)
			{
				player.Jump(t);
			}

			// Enter
			int enterPressed = glfwGetKey(window, GLFW_KEY_ENTER);
			if (enterPressed == GLFW_PRESS)
			{
				player.DropBomb();
			}
		}		

        // Default states
        glEnable(GL_DEPTH_TEST);

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get camera matrices
        glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 100.f); 
        glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
        glm::mat4 objectToWorld;
        glm::mat4 mv = worldToView * objectToWorld;
        glm::mat4 mvp = projection * mv;
        glm::vec4 light = worldToView * glm::vec4(10.0, 10.0, 0.0, 1.0);
		glm::mat4 inverseProjection = glm::inverse(projection);

		glm::vec3 ld(0.0, -5.0, -5.0);
		glm::vec3 spotPosition(0.f, 3.f, 8.f);
		float angle = 60.f;
		float penumbraAngle = 65.f;

		glm::mat4 shadowProjection = glm::perspective(glm::radians(penumbraAngle*2.f), 1.f, 1.f, 100.f);
		glm::mat4 worldToLight = glm::lookAt(spotPosition, spotPosition + ld, glm::vec3(0.f, 0.f, -1.f));
		glm::mat4 objectToLight = worldToLight * objectToWorld;
		glm::mat4 objectToLightScreen = shadowProjection * objectToLight;
		glm::mat4 worldToLightScreen = shadowProjection * worldToLight * glm::inverse(worldToView);

		glProgramUniform1f(programObject, spotLightAngleLocation, angle);
		glProgramUniform1f(programObject, spotLightPenumbraAngleLocation, penumbraAngle);

		glUseProgram(shadowProgramObject);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
		// Clear only the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		// Update scene uniforms
		glProgramUniformMatrix4fv(shadowProgramObject, shadowMVPLocation, 1, 0, glm::value_ptr(objectToLightScreen));
		glProgramUniformMatrix4fv(shadowProgramObject, shadowMVLocation, 1, 0, glm::value_ptr(objectToLight));


		// Fallback to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Select textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glActiveTexture(GL_TEXTURE3);

        // Select shader
       glUseProgram(programObject);

        // Upload uniforms
        glProgramUniformMatrix4fv(programObject, mvpLocation, 1, 0, glm::value_ptr(mvp));
        glProgramUniformMatrix4fv(programObject, mvLocation, 1, 0, glm::value_ptr(mv));
        glProgramUniform3fv(programObject, lightLocation, 1, glm::value_ptr(glm::vec3(light) / light.w));
        glProgramUniform1f(programObject, specularPowerLocation, 30.f);
		glProgramUniformMatrix4fv(programObject, WtLSLocation, 1, 0, glm::value_ptr(worldToLightScreen));
		glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(
			cos(0), sin(0), 0, 0,
			sin(0), cos(0), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			)));


        // Render vaos
		glBindVertexArray(vao[2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[7]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[8]);
		glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(player.GetPosition()));	
        glDrawElementsInstanced(GL_TRIANGLES, player.GetTriangleCount() * 3, GL_UNSIGNED_INT, (void*)0, 1);

		glBindVertexArray(vao[3]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		for (Pipe * pipe : pipes)
		{
			if (pipe != NULL && pipe->hasHit() == false)
			{
				glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(pipe->GetPosition()));
				glDrawElementsInstanced(GL_TRIANGLES, pipe->GetTriangleCount() * 3, GL_UNSIGNED_INT, (void*)0, 1);
			}
		}

		glBindVertexArray(vao[4]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[5]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[6]);
		for (Bird * bird : birds)
		{
			if (bird != NULL)
			{
				glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(bird->GetPosition()));
				glDrawElementsInstanced(GL_TRIANGLES, bird->GetTriangleCount() * 3, GL_UNSIGNED_INT, (void*)0, 1);
			}
		}

		glBindVertexArray(vao[5]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		if (bomb.IsActive())
		{
			glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(glm::vec3(bomb.GetPosition().x, bomb.GetPosition().y, bomb.GetPosition().z)));
			glDrawElementsInstanced(GL_TRIANGLES, bomb.GetTriangleCount() * 3, GL_UNSIGNED_INT, (void*)0, 1);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(glm::vec3(0.f)));
        //glDrawElements(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(vao[1]);
		glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

		glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(glm::vec3(0.0f, 10.0, 0.0)));
		glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

		glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, glm::value_ptr(rotation));

		glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(glm::vec3(0.0f, 8.0, 0.0)));
		glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
		glProgramUniform3fv(programObject, TransLocation, 1, glm::value_ptr(glm::vec3(0.0f,-5.0, 0.0)));
		glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glActiveTexture(GL_TEXTURE3);

		glBindTexture(GL_TEXTURE_2D, shadowTexture);
		glProgramUniform3fv(programObject, spotLightPositionLocation, 1, glm::value_ptr(glm::vec3(worldToView * glm::vec4(spotPosition, 1.0))));
		glProgramUniform3fv(programObject, spotLightDirectionLocation, 1, glm::value_ptr(glm::vec3(worldToView * glm::vec4(ld, 0.0))));

		glProgramUniform3fv(programObject, directionalLightDirectionLocation, 1, glm::value_ptr(glm::vec3(worldToView * glm::vec4(0.0, -5.0, -5.0, 0.0))));

		int i = 0;
		for (PointLight * pL : pointLights)
		{
			glm::vec3 position = glm::vec3(worldToView * pL->GetVec4Position());
			pointLigthsPositionInWorld[i] = position.x;
			pointLigthsPositionInWorld[i + 1] = position.y;
			pointLigthsPositionInWorld[i + 2] = position.z;

			i += 3;
		}
		glUseProgram(programObject);
		glProgramUniform3fv(programObject, pointLightPositionsLocation, 12, pointLigthsPositionInWorld);
		

		

#if 1
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        unsigned char mbut = 0;
        int mscroll = 0;
        double mousex; double mousey;
        glfwGetCursorPos(window, &mousex, &mousey);
        mousex*=DPI;
        mousey*=DPI;
        mousey = height - mousey;

        if( leftButton == GLFW_PRESS )
            mbut |= IMGUI_MBUT_LEFT;

        imguiBeginFrame(mousex, mousey, mbut, mscroll);
        int logScroll = 0;
        char lineBuffer[512];
        imguiBeginScrollArea("Flappy Runner", width - 210, height - 310, 200, 300, &logScroll);
        sprintf(lineBuffer, "FPS %f", fps);
        imguiLabel(lineBuffer);
		
		imguiSlider("Pipes Crossed", &pipescrossed, 0, 100, 1);
		imguiSlider("Combo", &combo, 0, 100, 1);

		float hasbomb = player.HasBomb();
		imguiSlider("Bomb", &hasbomb, 0, 1, 1);

        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height);

        glDisable(GL_BLEND);
#endif

		//Movement Pipes
		for (Pipe * pipe : pipes)
		{
			if (pipe != NULL && player.IsDead() == false)
			{

				if (pipe->HasPassedPlayer(&player))
				{
					++pipescrossed;
					++combo;

					if ((int)combo % numberPipesBeforeBomb == 0 && bomb.IsPicked() == false && bomb.IsActive() == false)
					{
						bomb.SetPosition(glm::vec3(rand() % 11 - 5.f, 0.f, -80.f));
						bomb.SetActive(true);
					}
				}

				if (pipe->isOutOfMap())
				{
					pipe->SetPosition(glm::vec3(rand() % 11 - 5.f, 0.f, -80.f));

					if (pipe == pipes[0])
					{
						speed = 1.f / (glfwGetTime() - LastTimePassed);
						LastTimePassed = glfwGetTime();
					}					
				}

				//If pipe didn't hit anything yet, we check the collision
				if (pipe->hasHit() == false)
				{
					if (pipe->CheckHitObject(&player))
					{
						SpeedDownForce.apply(objects);
						combo = 0;
					}

					for (Bird * bird : birds)
					{
						if (bird != NULL)
						{
							if (pipe->CheckHitObject(bird))
							{
								PipeHitForce.apply(bird);
							}
						}
					}
				}
			}
		}	

		//Movement Birds
		for (Bird * bird : birds)
		{
			if (bird != NULL && player.IsDead() == false)
			{
				bird->Move(birds, dt);
				backWallForce.apply(bird);
				frontWallForce.apply(bird);
				leftWallForce.apply(bird);
				rightWallForce.apply(bird);
				MovementBird.apply(bird);
				gravity.apply(bird);

				if (bird->HasToFlap(t))
				{
					FlapForce.apply(bird);
				}
				if (bird->CheckHitObject(&player))
				{
					player.KillPlayer();
				}
				else if (bomb.IsReadyToExplode() && bomb.CheckHitObject(bird))
				{
					BombHitForce.apply(bird);
					bomb.ExplodeBird(bird);
				}
			}
		}

		for (PointLight* pL : pointLights)
		{
			if (pL->isOutOfMap())
			{
				float x = pL->GetPosition().x;
				pL->SetPosition(glm::vec3(x, 5.f, -120.f));
			}

		}
		
		//Movement Bomb
		if (bomb.IsActive() && player.IsDead() == false)
		{
			if (bomb.CheckHitObject(&player))
			{
				player.PickBomb(&bomb);
			}
			else if (bomb.isOutOfMap())
			{
				bomb.SetActive(false);
				bomb.SetReadyToExplode(false);
			}
		}		


		//Force
		if (player.IsDead() == false)
		{
			// Objects
			
			groundForce.apply(objects);
			SpeedUpForce.apply(objects);

			leapfrog.solve(objects, dt);

			// Player
			gravity.apply(&player);
			groundForce.apply(&player);

			if (player.IsJumping())
			{
				JumpForce.apply(&player);
				player.SetJumping(false);
			}

			leapfrog.solve(&player, dt);
		}
		
        // Check for errors
        checkError("End loop");

        glfwSwapBuffers(window);
        glfwPollEvents();

		float newTime = glfwGetTime();		
		fps = 1.f / (newTime - t);
		dt = newTime - t;
		groundForce.setDt(dt);
		backWallForce.setDt(dt);
		frontWallForce.setDt(dt);
		rightWallForce.setDt(dt);
		leftWallForce.setDt(dt);
    } // Check if the ESC key was pressed
    while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

	for (Pipe * pipe : pipes)
	{
		if (pipe != NULL)
			delete(pipe);
	}

	for (Bird * bird : birds)
	{
		if (bird != NULL)
			delete(bird);
	}

	for (PointLight * pL : pointLights)
	{
		if (pL != NULL)
			delete(pL);
	}

    exit( EXIT_SUCCESS );
}

// No windows implementation of strsep
char * strsep_custom(char **stringp, const char *delim)
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s; ; ) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    return 0;
}

int check_compile_error(GLuint shader, const char ** sourceBuffer)
{
    // Get error log size and print it eventually
    int logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        char *token, *string;
        string = strdup(sourceBuffer[0]);
        int lc = 0;
        while ((token = strsep_custom(&string, "\n")) != NULL) {
           printf("%3d : %s\n", lc, token);
           ++lc;
        }
        fprintf(stderr, "Compile : %s", log);
        delete[] log;
    }
    // If an error happend quit
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        return -1;     
    return 0;
}

int check_link_error(GLuint program)
{
    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        fprintf(stderr, "Link : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);        
    if (status == GL_FALSE)
        return -1;
    return 0;
}

GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize)
{
    GLuint shaderObject = glCreateShader(shaderType);
    const char * sc[1] = { sourceBuffer };
    glShaderSource(shaderObject, 
                   1, 
                   sc,
                   NULL);
    glCompileShader(shaderObject);
    check_compile_error(shaderObject, sc);
    return shaderObject;
}

GLuint compile_shader_from_file(GLenum shaderType, const char * path)
{
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
        return 0;
    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    GLuint shaderObject = compile_shader(shaderType, buffer, fileSize );
    delete[] buffer;
    return shaderObject;
}


bool checkError(const char* title)
{
    int error;
    if((error = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch(error)
        {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        default:
            errorString = "UNKNOWN";
            break;
        }
        fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    }
    return error == GL_NO_ERROR;
}

void camera_compute(Camera & c)
{
    c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;   
    c.eye.y = cos(c.phi) * c.radius + c.o.y ;
    c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;   
    c.up = glm::vec3(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
}

void camera_defaults(Camera & c)
{
	c.phi = 3.14 / 2.5f;
	c.theta = 3.14 / 2.f;
	c.radius = 10.f;
    camera_compute(c);
}

/*void camera_zoom(Camera & c, float factor)
{
    c.radius += factor * c.radius ;
    if (c.radius < 0.1)
    {
        c.radius = 10.f;
        c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
    }
    camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
    c.theta += 1.f * theta;
    c.phi   -= 1.f * phi;
    if (c.phi >= (2 * M_PI) - 0.1 )
        c.phi = 0.00001;
    else if (c.phi <= 0 )
        c.phi = 2 * M_PI - 0.1;
    camera_compute(c);
}

void camera_pan(Camera & c, float x, float y)
{
    glm::vec3 up(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(c.o - c.eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    c.up = glm::normalize(glm::cross(side, fwd));
    c.o[0] += up[0] * y * c.radius * 2;
    c.o[1] += up[1] * y * c.radius * 2;
    c.o[2] += up[2] * y * c.radius * 2;
    c.o[0] -= side[0] * x * c.radius * 2;
    c.o[1] -= side[1] * x * c.radius * 2;
    c.o[2] -= side[2] * x * c.radius * 2;       
    camera_compute(c);
}*/

void init_gui_states(GUIStates & guiStates)
{
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}
