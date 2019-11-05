/*

CG Homework1 - Phong Shading, Gouraud Shading, Flat Shading ...

Objective - learning GLSL, glm model datatype(for .obj file) 

Overview:

	1. Render the model with texture using VBO and/or VAO

	2. Implement Phong shading, Gouraud Shading and Flat Shading

Good luck!

!!!IMPORTANT!!! 

1. Make sure to change the window name to your student ID!
2. Make sure to remove glmDraw() at line 207.
   It's meant for a quick preview of the ball object and shouldn't exist in your final code.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "glm/glm.h"
//my include
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
//using namespace std;


extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

//my function
void shaderReader(std::string path);

namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *main_tex_dir = "../Resources/WoodFine.ppm";
	
	Vertex *vertices;

	GLfloat light_rad = 0.05; //radius of the light bulb
	float eyet = 0.0; //theta in degree
	float eyep = 90.0; //phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;

	int mousex = 0;
	int mousey = 0;

	//my var
	std::string shaderSource;
	const GLchar* vss;  //vertexShaderSource
	const GLchar* fss;  //fragmentShaderSource
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint shaderProgram;
	int c=0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you

GLMmodel *model; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat viewPos[];
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };


GLuint vaoHandle;
GLuint vbo_ids;

char* vertex_shader_resource = "...void main(){...}"; // shader source code
//GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);    // GL_FRAGMENT_SHADER
//GLuint program_id = glCreateProgram();
void shaderReader(std::string path)
{
	std::ifstream file;
	file = std::ifstream(path);
	std::string lineBuffer;
	std::string buffer;
	if (!file.good())
	{
		std::cout << path << " dose not exists." << std::endl;
		return;
	}
	else
	{
		std::cout << path << " is OK." << std::endl;
	}
	while (std::getline(file, buffer))
	{
		lineBuffer += buffer + "\n";
	}
	shaderSource = lineBuffer;
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW1_0856641");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}


void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);

	model = glmReadOBJ(obj_file_dir);

	mainTextureID = loadTexture(main_tex_dir, 1024, 1024);
	
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	

	//glShaderSource(vert_id, 1, &vertex_shader_resource, NULL);
	//glCompileShader(vert_id);
	//glAttachShader(program_id, vert_id);
	/* you can attach another shader (fragment shader) */
	//glLinkProgram(program_id);
	//glDetachShader(program_id, vert_id);
	/* detach another shader (fragment shader)*/

								  //頂點著色
	shaderReader("../GLSL_example/Shaders/vertShader.txt");
	vss = shaderSource.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vss, NULL);
	glCompileShader(vertexShader);
	//測試有沒有成功
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//片段著色     
	shaderReader("../GLSL_example/Shaders/fragShader.txt");
	fss = shaderSource.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fss, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Shader Program
	



	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//測試有沒有成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}
	glUseProgram(shaderProgram);

	//deleteShader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	

//-----------------------------------VAO VBO----------------------------
	unsigned size = model->numtriangles * 3 * sizeof(Vertex); //大小
	vertices = (Vertex*)malloc(size);
	for (int a = 0; a < model->numtriangles; a++) {
		//拿到每個頂點
		int vIndex[3] = { model->triangles[a].vindices[0], model->triangles[a].vindices[1] ,model->triangles[a].vindices[2] };
		int tIndex[3] = { model->triangles[a].tindices[0], model->triangles[a].tindices[1] ,model->triangles[a].tindices[2] };
		int nIndex[3] = { model->triangles[a].nindices[0], model->triangles[a].nindices[1], model->triangles[a].nindices[2] };
		//點的八個值
		for (int j = 0; j < 3; j++) {
			vertices[a * 3 + j].position[0] = model->vertices[vIndex[j] * 3];
			vertices[a * 3 + j].position[1] = model->vertices[vIndex[j] * 3 + 1];
			vertices[a * 3 + j].position[2] = model->vertices[vIndex[j] * 3 + 2];

			vertices[a * 3 + j].texcoord[0] = model->vertices[tIndex[j] * 3];
			vertices[a * 3 + j].texcoord[1] = model->vertices[tIndex[j] * 3 + 1];

			vertices[a * 3 + j].normal[0] = model->vertices[nIndex[j] * 3];
			vertices[a * 3 + j].normal[1] = model->vertices[nIndex[j] * 3 + 1];
			vertices[a * 3 + j].normal[2] = model->vertices[nIndex[j] * 3 + 2];
		}
	}

	glGenVertexArrays(1, &vaoHandle);                 //創建並綁定VAO
	glBindVertexArray(vaoHandle);
	glGenBuffers(1, &vbo_ids);                          //創建並綁定VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids);

	glBufferData(GL_ARRAY_BUFFER,                     //分配空間傳送數據
		size,
		vertices,
		GL_STATIC_DRAW);       //GL_STREAM_DRAW, GL_DYNAMIC_DRAW      
	
	//指定GPU解析數據的方式
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	
	glVertexAttribPointer(0, 3,                     // # of components ex: (x,y,z) 
		GL_FLOAT,              // type of components 
		GL_FALSE,              // normalized
		sizeof(Vertex),                     // stride 
		(void*)0 // offset 
	);
	
	glVertexAttribPointer(1, 3,                      
		GL_FLOAT,               
		GL_FALSE,              
		sizeof(Vertex),                     	
		(void*)(offsetof(Vertex, position)) 
	);
	glVertexAttribPointer(2, 2,                     
		GL_FLOAT,              
		GL_FALSE,              
		sizeof(Vertex),                     
		(void*)(offsetof(Vertex, normal)) 
	);
//--------------------


	

	// you may need to do something here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)

	// APIs for creating shaders and creating shader programs have been done by TAs
	// following is an example for creating a shader program using given vertex shader and fragment shader
	/*
		GLuint vert = createShader("Shaders/bump.vert", "vertex");
		GLuint frag = createShader("Shaders/bump.frag", "fragment");
		GLuint program = createProgram(vert, frag);
	*/
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);


	//you may need to do something here(declare some local variables you need and maybe load Model matrix here...)

	//please try not to modify the following block of code(you can but you are not supposed to)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		eyex, 
		eyey, 
		eyez,
		eyex+cos(eyet*M_PI/180)*cos(eyep*M_PI / 180), 
		eyey+sin(eyet*M_PI / 180), 
		eyez-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	//拿view
	GLfloat view[16];
	glGetFloatv(GL_PROJECTION_MATRIX, view);
	GLint vLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, view);
	glUseProgram(NULL);


	draw_light_bulb();
	glPushMatrix();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
	// please try not to modify the previous block of code

	// you may need to do something here(pass uniform variable(s) to shader and render the model)
	//glmDraw(model, GLM_TEXTURE); // please delete this line in your final code! It's just a preview of rendered object
    
	glUseProgram(shaderProgram);
	
	GLint loc = glGetUniformLocation(shaderProgram, "MyTexture_1");
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(loc, 0);
	



	GLfloat projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	GLfloat modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	GLfloat viewPos[] = { eyex, eyey, eyez };


	GLint modelLoc = glGetUniformLocation(shaderProgram, "modelview");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "viewPos");
	
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelview);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
	glUniform3fv(lightPosLoc, 1, light_pos);
	glUniform3fv(viewLoc, 1, viewPos);
	


	glBindVertexArray(vaoHandle);

	/* draw objects with the vao */
	glDrawArrays(GL_TRIANGLES, 0, model->numtriangles * 3);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);


	
	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement mode toggle(switch mode between phongShading/gouraudShading/flatPhongShading) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	// ESC
		break;
	}
	case 'b': // toggle mode
	{
		c++;
		c = c % 3;
		if (c == 0) {
			shaderReader("../GLSL_example/Shaders/vertShader.txt");
		}
		else if (c == 1) {
			shaderReader("../GLSL_example/Shaders/vertShader1.txt");
		}
		else {
			shaderReader("../GLSL_example/Shaders/vertShader2.txt");
		}
		vss = shaderSource.c_str();
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vss, NULL);
		glCompileShader(vertexShader);
		//測試有沒有成功
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//片段著色 
		if (c == 0) {
			shaderReader("../GLSL_example/Shaders/fragShader.txt");
		}
		else if (c == 1) {
			shaderReader("../GLSL_example/Shaders/fragShader1.txt");
		}
		else {
			shaderReader("../GLSL_example/Shaders/fragShader2.txt");
		}
		fss = shaderSource.c_str();
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fss, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//Shader Program




		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		//測試有沒有成功
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		}
		glUseProgram(shaderProgram);

		//deleteShader
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);



		//-----------------------------------VAO VBO----------------------------
		unsigned size = model->numtriangles * 3 * sizeof(Vertex); //大小
		vertices = (Vertex*)malloc(size);
		for (int a = 0; a < model->numtriangles; a++) {
			//拿到每個頂點
			int vIndex[3] = { model->triangles[a].vindices[0], model->triangles[a].vindices[1] ,model->triangles[a].vindices[2] };
			int tIndex[3] = { model->triangles[a].tindices[0], model->triangles[a].tindices[1] ,model->triangles[a].tindices[2] };
			int nIndex[3] = { model->triangles[a].nindices[0], model->triangles[a].nindices[1], model->triangles[a].nindices[2] };
			//點的八個值
			for (int j = 0; j < 3; j++) {
				vertices[a * 3 + j].position[0] = model->vertices[vIndex[j] * 3];
				vertices[a * 3 + j].position[1] = model->vertices[vIndex[j] * 3 + 1];
				vertices[a * 3 + j].position[2] = model->vertices[vIndex[j] * 3 + 2];

				vertices[a * 3 + j].texcoord[0] = model->vertices[tIndex[j] * 3];
				vertices[a * 3 + j].texcoord[1] = model->vertices[tIndex[j] * 3 + 1];

				vertices[a * 3 + j].normal[0] = model->vertices[nIndex[j] * 3];
				vertices[a * 3 + j].normal[1] = model->vertices[nIndex[j] * 3 + 1];
				vertices[a * 3 + j].normal[2] = model->vertices[nIndex[j] * 3 + 2];
			}
		}

		glGenVertexArrays(1, &vaoHandle);                 //創建並綁定VAO
		glBindVertexArray(vaoHandle);
		glGenBuffers(1, &vbo_ids);                          //創建並綁定VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo_ids);

		glBufferData(GL_ARRAY_BUFFER,                     //分配空間傳送數據
			size,
			vertices,
			GL_STATIC_DRAW);       //GL_STREAM_DRAW, GL_DYNAMIC_DRAW      

		//指定GPU解析數據的方式
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);


		glVertexAttribPointer(0, 3,                     // # of components ex: (x,y,z) 
			GL_FLOAT,              // type of components 
			GL_FALSE,              // normalized
			sizeof(Vertex),                     // stride 
			(void*)0 // offset 
		);

		glVertexAttribPointer(1, 3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)(offsetof(Vertex, position))
		);
		glVertexAttribPointer(2, 2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)(offsetof(Vertex, normal))
		);
		// you may need to do somting here
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
