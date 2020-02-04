#include "cg.h"
#include "camera.h"
#include "vbocube.h"
#include "vboplane.h"

void startup();
void render();
void shutdown();

struct scene {
	int width = 800, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	// 모델링 변수
	VBOCube *cube;
	VBOPlane *plane;

	// 뷰잉 변수
	mat4 model, view, proj;
	GLint m_loc, v_loc, proj_loc, color_loc;

	float theta;
};

scene sc;


// 맨 처음 한번 실행
void startup()
{
	//------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	sc.rendering_program = compile_shaders("vview.glsl", "fview.glsl");
	glUseProgram(sc.rendering_program);
	
	glGenVertexArrays(1, &sc.vertex_array_object);
	glBindVertexArray(sc.vertex_array_object);

	sc.cube = new VBOCube();
	sc.plane = new VBOPlane(5.0f, 10.0f, 10, 10);
	
	sc.view = LookAt(vec3(0.0f, 1.0f, 5.0f), 
		vec3(0.0f, 0.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f));

	float cameraZ = sc.height * 0.5 / tan(radians(0.5*50.0));
//	sc.proj = Perspective(80.0f, (float)sc.width/(float)sc.height, (float)0.001*cameraZ, (float)10.0*cameraZ);
	sc.proj = Perspective(50.0f, 1.33f, 0.5f, 100.0f);
//	sc.proj= Ortho(-5.0, 5.0,-2.0, 8.0, 0.3, 100.0);

	
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0))); 
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));	

	sc.theta = 0.0;
}

//  계속해서 실행됨
void idle()
{
	//sc.theta += 0.01;
	
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glUseProgram(sc.rendering_program);

	float x, z;
	x = 5.0 * sin(sc.theta);
	z = 5.0 * cos(sc.theta);
	sc.view = LookAt(vec3(0.0f, 1.0f, 5.0f-sc.theta), 
		vec3(0.0f, 0.0f, -sc.theta), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));

	// floor
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniform4f(sc.color_loc, 0.3, 0.3, 0.3, 1.0);
	sc.plane->render();
     
	// Cube
	mat4 model_base;
	model_base = translate(mat4(1.0), vec3(0.0, 0.5, 0.0));
	model_base = scale(model_base, vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniform4f(sc.color_loc, 0.3, 0.2, 0.9, 1.0);
	sc.cube->render();
	
	glutSwapBuffers();
}

void shutdown()
{
	glDeleteVertexArrays(1, &sc.vertex_array_object);
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); 
	// MAC
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);

	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("3D Viewing");
	glewInit();
	version();

	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
