#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifndef SIZE
#define SIZE 16
#endif

#define DIFF (1.0f / ((float)SIZE / 2.0f))

#if SIZE == 8
unsigned char stored_data[SIZE];
#elif SIZE == 16
unsigned short stored_data[SIZE];
#elif SIZE == 32
unsigned int stored_data[SIZE];
#elif SIZE == 64
unsigned long long stored_data[SIZE];
#endif

const float pixel_state[2] = { 0.1f, 0.9f };

const char *vs = 
"#version 450 core\n"
"layout(location = 0) in vec4 iPos;\n"
"layout(location = 1) in vec4 iCol;\n"
"out vec4 vCol;\n"
"void main() {\n"
"   vCol = iCol;\n"
"   gl_Position = iPos;\n"
"}\n";

const char *fs = 
"#version 450 core\n"
"in vec4 vCol;\n"
"out vec4 oCol;\n"
"void main() {\n"
"oCol = vCol;\n"
"}\n";

float vertices[6 * 2 * (SIZE * SIZE)];

float colors[6 * 4 * (SIZE * SIZE)];

unsigned int LoadShader(const char* src, int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    return shader;
}

int Clampi(int value, int min, int max) {
    if(value <= min) {
        return min;
    }
    else if(value >= max) {
        return max;
    }
    else {
        return value;
    }
}

int main() {
    for(int y = 0; y < SIZE; y++) {
        for(int x = 0; x < SIZE; x++) {
            vertices[((y * SIZE) + x) * 12 + 0]  = -1.0f  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 1]  = 1.0f   - (DIFF * y);
            vertices[((y * SIZE) + x) * 12 + 2]  = -(1.0f - DIFF)  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 3]  = 1.0f   - (DIFF * y);
            vertices[((y * SIZE) + x) * 12 + 4]  = -1.0f  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 5]  = (1.0f - DIFF)  - (DIFF * y);

            vertices[((y * SIZE) + x) * 12 + 6]  = -(1.0f - DIFF)  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 7]  = 1.0f   - (DIFF * y);
            vertices[((y * SIZE) + x) * 12 + 8]  = -1.0f  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 9]  = (1.0f - DIFF)   - (DIFF * y);
            vertices[((y * SIZE) + x) * 12 + 10] = -(1.0f - DIFF)  + (DIFF * x);
            vertices[((y * SIZE) + x) * 12 + 11] = (1.0f - DIFF)   - (DIFF * y);
        }
    }

    for(int i = 0; i < SIZE * SIZE * 6; i++) {
        colors[i * 4 + 0] = 0.9f;
        colors[i * 4 + 1] = 0.9f;
        colors[i * 4 + 2] = 0.9f;
        colors[i * 4 + 3] = 1.0f;
    }

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "PixelMaker", NULL, NULL);

    if(!window) {
        perror("[ERROR]; No window!\n");

        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        perror("[ERORR]: No OpenGL!\n");

        exit(-2);
    }

    unsigned int pr, vao, vert, col;

    pr = glCreateProgram();
    glAttachShader(pr, LoadShader(vs, GL_VERTEX_SHADER));
    glAttachShader(pr, LoadShader(fs, GL_FRAGMENT_SHADER));
    glLinkProgram(pr);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vert);
    glBindBuffer(GL_ARRAY_BUFFER, vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &col);
    glBindBuffer(GL_ARRAY_BUFFER, col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    double mouse_x, mouse_y;
    int current_id = 0, last_id = 0;
    int last_mouse_state = 0;

    while(!glfwWindowShouldClose(window)) {
        glClear(0x4000 | 0x100);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        mouse_x /= 800.0;
        mouse_x *= (double)SIZE;
        mouse_y /= 800.0;
        mouse_y *= (double)SIZE;

        int clamped = Clampi(((int)mouse_y * SIZE) + (int)mouse_x, 0, (SIZE * SIZE) - 1);

        last_id = current_id;
        current_id = clamped;

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            last_mouse_state = 0;
        }

        if(last_id != current_id) {
            last_mouse_state = 0;
        }

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && last_mouse_state == 0) {
            last_mouse_state = 1;
            float change_state_to = pixel_state[1];

            if(colors[current_id * 24 + 0] == pixel_state[1]) {
                change_state_to = pixel_state[0];
            }

            stored_data[current_id / SIZE] ^= 1 << current_id % SIZE;

            for(int i = 0; i < 24; i++) {
                colors[current_id * 24 + 0] = change_state_to;
                colors[current_id * 24 + 1] = change_state_to;
                colors[current_id * 24 + 2] = change_state_to;
                colors[current_id * 24 + 3] = 1.0f;

                colors[current_id * 24 + 4] = change_state_to;
                colors[current_id * 24 + 5] = change_state_to;
                colors[current_id * 24 + 6] = change_state_to;
                colors[current_id * 24 + 7] = 1.0f;

                colors[current_id * 24 + 8] = change_state_to;
                colors[current_id * 24 + 9] = change_state_to;
                colors[current_id * 24 + 10] = change_state_to;
                colors[current_id * 24 + 11] = 1.0f;

                colors[current_id * 24 + 12] = change_state_to;
                colors[current_id * 24 + 13] = change_state_to;
                colors[current_id * 24 + 14] = change_state_to;
                colors[current_id * 24 + 15] = 1.0f;

                colors[current_id * 24 + 16] = change_state_to;
                colors[current_id * 24 + 17] = change_state_to;
                colors[current_id * 24 + 18] = change_state_to;
                colors[current_id * 24 + 19] = 1.0f;

                colors[current_id * 24 + 20] = change_state_to;
                colors[current_id * 24 + 21] = change_state_to;
                colors[current_id * 24 + 22] = change_state_to;
                colors[current_id * 24 + 23] = 1.0f;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            printf("\t\t\tSTART\n");

            for(int i = 0; i < SIZE; i++) {
                #if SIZE == 8
                printf("0x%02x, ", stored_data[i]);
                #elif SIZE == 16
                printf("0x%04x, ", stored_data[i]);
                #elif SIZE == 32
                printf("0x%08x, ", stored_data[i]);
                #elif SIZE == 64
                printf("0x%llx, ", stored_data[i]);
                #endif
            }

            printf("\n\t\t\tEND\n\n");
        }

        glUseProgram(pr);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, col);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / 3);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwPollEvents();

        glfwSwapBuffers(window);

        glfwSwapInterval(1);
    }

    glfwTerminate();

    return 0;
}