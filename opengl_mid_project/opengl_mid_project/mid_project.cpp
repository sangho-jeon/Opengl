#include "cg-2.h"
#include "time.h"
#include <vector>
void startup();
void render();
void shutdown();
float PI = 3.141592;
float No_theta = 9;
float No_phi = 9;
int tmp;
GLfloat rotate_angle = 0.001;
float thetaeye = +0.02f;
float rad = 0.25;
GLuint rendering_program;

GLuint vertex_array_object;
GLint mv_location;
vector<vec4> vertices[1];

struct scene {
	float theta = 0.0;

	vec3 distance;
	vec3 scale;
	mat4 model;

};
scene sc[8];


void make_shpere()
{
	sc[0].distance = vec3(0.0, 0.0, 0.0);//倔奔
	sc[1].distance = vec3(0.005, 0.2, 0.12);//传界
	sc[2].distance = vec3(-0.005, 0.2, 0.12);//传界
	sc[3].distance = vec3(-0.075, 0.08, 0.17);//传舅
	sc[4].distance = vec3(0.075, 0.08, 0.17);//传舅
	sc[5].distance = vec3(0.0, -0.02, 0.195);//内
	sc[6].distance = vec3(0.0, -0.15, 0.195);
	sc[7].distance = vec3(0.0, 0.0, 0.0);

	sc[0].scale = vec3(0.2, 0.3, 0.2);
	sc[1].scale = vec3(0.06, 0.03, 0.03);
	sc[2].scale = vec3(0.06, 0.03, 0.03);
	sc[3].scale = vec3(0.025, 0.025, 0.025);
	sc[4].scale = vec3(0.025, 0.025, 0.025);
	sc[5].scale = vec3(0.032, 0.05, 0.03);
	sc[6].scale = vec3(0.04, 0.025, 0.01);
	sc[7].scale = vec3(0.205, 0.305, 0.205);

	float theta, phi;
	float delta_theta, delta_phi;
	float start_theta, start_phi;

	start_theta = 0.0;
	delta_theta = 2 * PI / No_theta;

	start_phi = -PI / 2.0;
	delta_phi = PI / (No_phi - 1);

	float no_theta = 3 + 6 * 3, no_phi = no_theta;
	start_theta = 0.0;
	delta_theta = 2 * PI / no_theta;
	start_phi = -PI / 2.0;
	delta_phi = PI / (no_phi - 1);
	for (int j = 0; j < no_phi; j++)
	{
		phi = start_phi + j * delta_phi;
		for (int i = 0; i < no_theta; i++)
		{
			theta = start_theta + i * delta_theta;

			vertices[0].emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi), 1.0);
			vertices[0].emplace_back(cos(phi) * cos(theta + delta_theta), cos(phi) * sin(theta + delta_theta), sin(phi), 1.0);
			vertices[0].emplace_back(cos(phi + delta_phi) * cos(theta + delta_theta), cos(phi + delta_phi) * sin(theta + delta_theta), sin(phi + delta_phi), 1.0);

			vertices[0].emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi), 1.0);
			vertices[0].emplace_back(cos(phi + delta_phi) * cos(theta + delta_theta), cos(phi + delta_phi) * sin(theta + delta_theta), sin(phi + delta_phi), 1.0);
			vertices[0].emplace_back(cos(phi + delta_phi) * cos(theta), cos(phi + delta_phi) * sin(theta), sin(phi + delta_phi), 1.0);
		}

	}


	GLfloat rotate_angle = 0.5;
	for (int i = 0; i < 1; i++) {

		sc[i].model = translate(mat4(1.0), sc[i].distance);
		sc[i].model = scale(sc[i].model, sc[i].scale);

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[i].model));
	}
	sc[1].model = rotate(mat4(1.0), -radians(25.0f), vec3(0.0, -1.0, 1.0));
	sc[2].model = rotate(mat4(1.0), radians(25.0f), vec3(0.0, -1.0, 1.0));
	for (int i = 1; i < 3; i++) {

		sc[i].model = translate(sc[i].model, sc[i].distance);
		sc[i].model = scale(sc[i].model, sc[i].scale);

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[i].model));
	}
	for (int i = 3; i < 5; i++) {

		sc[i].model = translate(mat4(1.0), sc[i].distance);
		sc[i].model = scale(sc[i].model, sc[i].scale);

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[i].model));
	}

	sc[5].model = rotate(mat4(1.0), radians(-10.0f), vec3(1.0, 0.0, 0.0));
	sc[5].model = translate(sc[5].model, sc[5].distance);
	sc[5].model = scale(sc[5].model, sc[5].scale);


	sc[6].model = rotate(mat4(1.0), radians(15.0f), vec3(1.0, 0.0, 0.0));
	sc[6].model = translate(sc[6].model, sc[6].distance);
	sc[6].model = scale(sc[6].model, sc[6].scale);


	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");

	glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[5].model));
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[6].model));
	tmp = vertices[0].size();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	for (int i = 40; i < 100; i++) {
		float hair = 360 / 100;
		if (i % 2 == 0) {
			vertices[0].emplace_back(1.05*cos(radians((i + 5)*hair)), 0.0, 1.05*sin(radians((i + 5)*hair)), 1.0);
		}
		else
			vertices[0].emplace_back(cos(radians((i + 5)*hair)), 0.45, sin(radians((i + 5)*hair)), 1.0);
	}
	sc[7].model = rotate(mat4(1.0), radians(-40.0f), vec3(0.0, 1.0, 0.0));
	sc[7].model = scale(sc[7].model, sc[7].scale);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(sc[7].model));

}


// 盖 贸澜 茄锅 角青
void startup()
{
	//------------------------------------------------------------------------
	rendering_program = compile_shaders("vproject1.glsl", "fproject1.glsl");

	glGenVertexArrays(1, &vertex_array_object);

	glBindVertexArray(vertex_array_object);

	GLuint buffer[2];

	make_shpere();

	glGenBuffers(2, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices[0].size() * sizeof(vec4), vertices[0].data(), GL_STATIC_DRAW);
	GLuint vPosition = glGetAttribLocation(rendering_program, "vPosition");
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vPosition);

	glUseProgram(rendering_program);



}

//  拌加秦辑 角青凳
void idle()
{

	thetaeye += 1.0f;
	float mouth = 0.0f;
	mouth += 10.0;
	for (int i = 0; i < 8; i++) {
		sc[i].theta += 0.001;

	}

	for (int j = 1; j < 3; j++) {
		sc[j].model = translate(sc[j].model, vec3(0.0, 0.002*sin(radians(thetaeye)), 0.0));
	}



	glutPostRedisplay();

}

void render()
{
	glUseProgram(rendering_program);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	const GLfloat color[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	glClearBufferfv(GL_COLOR, 0, color);

	for (int i = 0; i < 7; i++) {
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr((rotate(mat4(1.), sc[i].theta, vec3(0., 1., 0.)) * sc[i].model)));

		glUniform1i(glGetUniformLocation(rendering_program, "i"), i);
		glBufferData(GL_ARRAY_BUFFER, vertices[0].size() * sizeof(vec4), vertices[0].data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, tmp);
	}
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr((rotate(mat4(1.), sc[7].theta, vec3(0., 1., 0.)) * sc[7].model)));
	glUniform1i(glGetUniformLocation(rendering_program, "i"), 7);
	glBufferData(GL_ARRAY_BUFFER, vertices[0].size() * sizeof(vec4), vertices[0].data(), GL_STATIC_DRAW);
	glLineWidth(4.0);
	glDrawArrays(GL_LINE_STRIP, tmp + 1, vertices[0].size());

	glDisableClientState(GL_VERTEX_ARRAY);

	glutSwapBuffers();
}


void shutdown()
{
	glDeleteVertexArrays(1, &vertex_array_object);

	glDeleteProgram(rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutCreateWindow("2D Drawing");
	glewInit();
	version();

	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
