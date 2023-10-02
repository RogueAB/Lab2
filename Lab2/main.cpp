
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

const char* vsFilename = "C:\\Users\\Rane\\Documents\\College\\Computer Graphics\\Code\\Lab 2\\Lab2\\Lab2\\shader.vs";
const char* fsFilename = "C:\\Users\\Rane\\Documents\\College\\Computer Graphics\\Code\\Lab 2\\Lab2\\Lab2\\shader.fs";
const char* fs2Filename = "C:\\Users\\Rane\\Documents\\College\\Computer Graphics\\Code\\Lab 2\\Lab2\\Lab2\\shader2.fs";
const char* yellowFragShader = "C:\\Users\\Rane\\Documents\\College\\Computer Graphics\\Code\\Lab 2\\Lab2\\Lab2\\yellowFragShader.fs";

std::string readFile(const char* filename) {
	std::ifstream stream(filename);

	std::string line;
	std::stringstream s;
	while (getline(stream, line)) {
		s << line << '\n';
	}

	return s.str();
}


#pragma region ShaderClass
class Shader
{
public:
	GLuint shaderProgramID;
	Shader() {
		//Start the process of setting up our shaders by creating a program ID
		//Note: we will link all the shaders together into this ID
		shaderProgramID = glCreateProgram();

		#pragma region ErrorCheck 
		if (shaderProgramID == 0) {
			fprintf(stderr, "Error creating shader program\n");
			exit(1);
		}
		#pragma endregion

	};

	void linkAndValidateProgram() {
		GLint Success = 0;
		GLchar ErrorLog[1024] = { 0 };

		glLinkProgram(shaderProgramID);

		#pragma region ErrorCheck
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
		if (Success == 0) {
			glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
			exit(1);
		}
#pragma endregion

		// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
		glValidateProgram(shaderProgramID);

		#pragma region ErrorCheck
				// check for program related errors using glGetProgramiv
				glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
				if (!Success) {
					glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
					fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
					exit(1);
				}
		#pragma endregion 

	}

	void use() {
		glUseProgram(shaderProgramID);
	}

	void AddShader(const char* filename, GLenum ShaderType) {

		// create a shader object
		GLuint ShaderObj = glCreateShader(ShaderType);
		string file = readFile(filename);
		const char* filep = file.c_str();

		#pragma region ErrorCheck
		if (ShaderObj == 0) {
			fprintf(stderr, "Error creating shader type %d\n", ShaderType);
			exit(0);
		}
		#pragma endregion

		// Bind the source code to the shader, this happens before compilation
		glShaderSource(ShaderObj, 1, (const GLchar**)&filep, NULL);
		// compile the shader and check for errors
		glCompileShader(ShaderObj);

		#pragma region ErrorCheck
		GLint success;
		// check for shader related errors using glGetShaderiv
		glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar InfoLog[1024];
			glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
			fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
			exit(1);
		}
		#pragma endregion


		// Attach the compiled shader object to the program object
		glAttachShader(shaderProgramID, ShaderObj);
	}

private:

};
#pragma endregion

#pragma region VertexBufferObject
class VertexBufferObject
{
public:
	GLuint VBO;
	GLuint numVertices;

	VertexBufferObject(GLuint numVertices, glm::vec3 positions[], glm::vec4 colors[]) {
		this->numVertices = numVertices;
		generateObjectBuffer(positions, colors);
	}

	void generateObjectBuffer(glm::vec3 positions[], glm::vec4 colors[]) {
		// Genderate 1 generic buffer object, called VBO
		// Need to GENERALISE****************
		int sizeOfVertices = sizeof(positions[0]) * numVertices;
		int sizeOfColors = sizeof(colors[0]) * numVertices;

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
		glBufferData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfColors, NULL, GL_STATIC_DRAW);
		// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfVertices, positions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices, sizeOfColors, colors);

		// Define attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(numVertices * 3 * sizeof(GLfloat)));


	}



private:

};
#pragma endregion

#pragma region IndexBufferObject
class IndexBufferObject
{
public:
	GLuint IBO;
	int numElements;

	IndexBufferObject(int numElements, unsigned int indices[]) {
		this->numElements = numElements;

		glGenBuffers(1, &IBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numElements, indices, GL_STATIC_DRAW);

	}

private:

};
#pragma endregion

int numVertices;
GLuint u_ColorID;
void init() {
	glm::vec3 positions[6] = { 
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.25f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),

		glm::vec3(0.25f, 0.0f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(0.0f, -0.5f, 0.0f),
	};

	glm::vec4 colors[6] = {
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),

		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	};

	unsigned int indices[9] = {
		0, 1, 5,
		1, 2, 3,
		3, 4, 5
	};

	// Yellow shader initialization
	Shader yellowShader = Shader();

	yellowShader.AddShader(vsFilename, GL_VERTEX_SHADER);
	yellowShader.AddShader(yellowFragShader, GL_FRAGMENT_SHADER);
	yellowShader.linkAndValidateProgram();

	yellowShader.use();


	// vertex buffer initialization
	VertexBufferObject object1 = VertexBufferObject(6, positions, colors);
	numVertices = object1.numVertices;

	// index buffer initialization
	IndexBufferObject indexBuffer = IndexBufferObject(9, indices);

	u_ColorID = glGetUniformLocation(yellowShader.shaderProgramID, "u_color");





}



float rChannel = 1.0f, gChannel = 0.0f, bChannel = 0.0f, aChannel = 1.0f;

void display() {

	glClear(GL_COLOR_BUFFER_BIT);
	glUniform4f(u_ColorID, rChannel,gChannel, bChannel, aChannel);

	if (rChannel > 1.0f) {
		rChannel = 1.0f;
	}

	if (rChannel < 0.0f) {
		rChannel = 0.0f;
	}

	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, NULL);
	glutPostRedisplay();
	glutSwapBuffers();
}


void colorIntensityChange(unsigned char key, int x, int y) {
	if (key == 'w') {
		rChannel += 0.05;
	} else if (key == 's') {
		rChannel -= 0.05;
	}
}

void changeYellow(int button, int state, int x, int y) {
	if (state == GLUT_LEFT_BUTTON) {
		gChannel = 1.0f;
	}
	
}

int main(int argc, char** argv) {
	#pragma region window
	// Create Window using GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(800, 600);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	#pragma endregion

	// Add key handler 
	glutKeyboardFunc(colorIntensityChange);
	glutMouseFunc(changeYellow);

	glewInit();

	// initialise everything
	init();

	// run
	glutMainLoop();

	return 0;
}