#include "cg.h"
#include "camera.h"
#include "vbomesh.h"
#include "vbotrianglebar.h"
#include "vboplane.h"
#include "vbocube.h"
#include "vbosphere.h"
void startup();
void render();
void shutdown();
extern void version();
float tmp = 0;
struct scene {
	int width = 800, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	// �𵨸� ����
	//VBOMeshAdj *mesh;
	VBOMesh *mesh;
	VBOPlane *plane;
	VBOCube *cube;
	VBOSphere *shpere;
	// ���� ����
	mat4 model, view, proj;
	GLint m_loc, v_loc, proj_loc, color_loc;
	float angle,bounce,drop,time;
};
scene sc;

void startup()
{
	
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("perfrag.glsl", "perfragf.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	/*
	sc.teapot = new VBOTeapot(14, mat4(1.0f));
	*/
	//sc.mesh = new VBOMesh("Zuccarello.obj", true);
	sc.plane = new VBOPlane(50.0f, 50.0f, 1, 1);
	sc.shpere = new VBOSphere();
	sc.cube = new VBOCube();
	
	// viewing
	sc.angle = 0.957283f; //light angle
	sc.model = mat4(1.0); 
	sc.drop = 2700.0f;
	sc.view = LookAt(vec3(50.0f, 50.0f, 50.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	sc.time = 0.0f;
	float cameraZ = sc.height * 0.5 / tan(radians(0.5*50.0));
	sc.proj = Perspective(50.0f, (float)sc.width / (float)sc.height, (float)0.001*cameraZ, (float)10.0*cameraZ);	

		//ortho
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

	// Lighting
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.8, 0.8, 0.8);
}

void idle() {
	
	sc.angle += 0.01f;
	if (sc.angle > 2.0*3.141592) sc.angle -= 2.0*3.141592;
	glutPostRedisplay();
	sc.bounce += 0.1f;
	if (sc.time < 6.3) {
		sc.drop = sc.drop - (5.8*sc.time*sc.time) / 2;
		tmp = sc.drop;
	}
	else {
		sc.drop = tmp + 300*sin(-(sc.time - 6.3));
	}
	
	sc.time+=0.1f;
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glUseProgram(sc.rendering_program);

	// Lighting 
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"),
		1, value_ptr(sc.view*vec4(50.0f*cos(sc.angle), 100.0f, 50.0f*sin(sc.angle), 1.0f)));
	sc.view = LookAt(vec3(50.0f, 50.0f, 50.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//cube 
	for (int j = 0; j < 80; j++) {
		for (int i = 0; i < 80; i++) {
			float color_row = i ;
			float color_col = j ;
		
			int x = i - 40, y = j - 40;
			float d = sqrt(x * x + y * y);
			float rtime = sc.time/1.5 + 5.3;
			glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0,color_row/80,color_col/80);
			glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.2f,0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.9f, 0.9f, 0.9f);
			glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 0.9f);

			// Models

			mat4 CUBE ;
			CUBE = mat4(1.0);

			CUBE = translate(mat4(1.0), vec3(color_row/2-20, 1.3, color_col/2-20));

			if (sc.time >=5.514602+(d*d/20)) {
				CUBE = translate(CUBE, vec3(0, (- 3* sin(sc.time - 5.514602-d))/(d), 0));
			}
			glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(CUBE));
			//glUniform4f(sc.color_loc, 0.2, 0.7, 0.7, 1.0);


			sc.cube->render();
		}
	}
	// plane
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.3f, 0.3f, 0.6f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.3f, 0.3f, 0.3f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 0.3f);


	mat4 plane;
	plane = mat4(1.0);
	
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(plane));
	sc.plane->render();
	
	
	//shpere
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f,0.5f,0.5f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 0.4f);

	
	mat4 shpere;
	shpere = mat4(1.0);
	
	shpere = scale(shpere, vec3(0.01, 0.01, 0.01));
	shpere = translate(shpere, vec3(0, sc.drop, 0));
	//m3 = rotate(m3, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));
	
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(shpere));
	//glUniform4f(sc.color_loc, 0.2, 0.7, 0.7, 1.0);


	sc.shpere->render();

	
	
	// Mesh  


	



	glutSwapBuffers();
}

void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("Phong Shading");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
