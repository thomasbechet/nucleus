#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

int
main (void)
{
    nu_vec2_t v = { .x = 0, .y = 1 };
    NU_ASSERT(v.x == v.data[0]);

    nu_surface_create(NU_ALLOC);

    if (!glfwInit())
    {
        return -1;
    }
    GLFWwindow *window = glfwCreateWindow(800, 600, "nucleus", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
