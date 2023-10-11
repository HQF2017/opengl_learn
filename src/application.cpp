#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class ShaderConfStr {
public:
    ShaderConfStr(std::string& shaderName, std::string& confStr)
    {
        _shaderName = shaderName;
        _confStr = confStr;
    }

    const void printInfo() const
    {
        std::cout << _shaderName << " shader: \n" << _confStr << std::endl;
    }
    std::string _shaderName;
    std::string _confStr;
};

static std::vector<ShaderConfStr> ParseShaderConf(const std::string& confPath) {
    std::ifstream stream(confPath);
    std::string line;
    std::string shaderName;
    std::string confStr;
    std::vector<ShaderConfStr> shaderConfs;
    static const std::string begineStr = "#Shader";
    while (getline(stream, line))
    {
        size_t pos;
        pos = line.find(begineStr);
        if (pos != std::string::npos)
        {
            if (!shaderName.empty())
            {
                shaderConfs.push_back(ShaderConfStr(shaderName, confStr));
            }
            confStr = "";
            shaderName = line.substr(pos + begineStr.length() + 1, line.length() - 1);
        }
        else
        {
            confStr += line + "\n";
        }
    }
    if (!shaderName.empty())
    {
        shaderConfs.push_back(ShaderConfStr(shaderName, confStr));
    }
    return shaderConfs;
}

static unsigned int CompileShader(const std::string& source, unsigned int type) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int res;
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "compile shader " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "failed" <<std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glew init error" << std::endl;
    }
    std::cout << GLEW_VERSION << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    unsigned int buffer;
    float points[6] = {
        -1.0f, -1.9f,
        2.0f, 1.9f,
        1.0f, 3.0f,
    };
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    const std::string confPath = "res/shader.conf";
    std::vector<ShaderConfStr> shaderConfStrs = ParseShaderConf(confPath);
    std::string vertexShader;
    std::string fragmentShader;
    for (const auto shaderConfStr : shaderConfStrs)
    {
        shaderConfStr.printInfo();
        if (shaderConfStr._shaderName == "vertex")
        {
            vertexShader = shaderConfStr._confStr;
        }
        else if (shaderConfStr._shaderName == "fragment")
        {
            fragmentShader = shaderConfStr._confStr;
        }
    }

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}