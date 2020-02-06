	#include "cg.h"
	#include "camera.h"
	#include "vboplane.h"
	#include "vboteapot.h"
	#include "bmpreader.h"
	#include "vbomesh.h"
	#include "vbomeshadj.h"
	#include "vbocube.h"
	#include "vbosphere.h"
	#include <algorithm>
	#include <vector>

	void startup();
	void render();
	void shutdown();
	extern void version();
	extern GLuint compile_shaders(const char*  vsource, const char*  fsource);
	float tmp = 0;

	struct scene {
		const int WIDTH = 1200; const int HEIGHT = 900;
	
		GLuint rendering_program;
		VBOPlane *plane;
		VBOTeapot *teapot;
		VBOMesh *mesh; 
		VBOMesh *mesh2;
		VBOMesh *mesh3;
		VBOMesh *mesh4;
		VBOCube *cube;
		VBOCube *cube2;
		VBOMesh *floor;
		VBOSphere *sphere;
		mat4 model;
		vector <mat4> mvs;
		mat4 view;
		mat4 proj;
	
		float angle, bounce, drop, time, r, t;

		GLint v_loc, m_loc, proj_loc;
		GLuint tex_object[3];
	};
	scene sc;


	void generate_texture(GLubyte * data, int width, int height)
	{
		int x, y;
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				GLubyte c = (((x & 0x8) == 0) ^ ((y & 0x8) == 0))*255.0;
				data[(y * width + x) * 4 + 0] = c;
				data[(y * width + x) * 4 + 1] = c;
				data[(y * width + x) * 4 + 2] = c;
				data[(y * width + x) * 4 + 3] = 1.0f;
			}
		}
	}

	void startup()
	{
		/******* OpenGL Initialization */
		glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
		glEnable(GL_SMOOTH);
		/**** Shaders as variables */
		sc.rendering_program = compile_shaders("tex_lighting_v.glsl", "tex_lighting_f.glsl");
		glUseProgram(sc.rendering_program);

		// modeling
		sc.plane = new VBOPlane(80.0f, 80.0f, 0, 0);
	
		sc.mesh = new VBOMesh("monu10.obj",true);
		sc.mesh2 = new VBOMesh("monu9.obj", true);
		sc.floor = new VBOMesh("monu8.obj", true);
		sc.mesh3 = new VBOMesh("chr_sword.obj", true);
		sc.mesh4 = new VBOMesh("10602_Rubber_Duck_v1_L3.obj", true);
		sc.cube = new VBOCube();
		sc.sphere = new VBOSphere();
		sc.drop = 2700.0f;
		sc.cube2 = new VBOCube();
		sc.r = 0.0f;
		sc.r = 0.0f;
		// viewing
		sc.angle = 0.957283f;
		sc.model = mat4(1.0);
		sc.view = LookAt(vec3(60.0f, 50.0f, 60.0f), vec3(0.0f, 12.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		sc.proj = Perspective(70.0f, (float)sc.WIDTH / sc.HEIGHT, 0.3f, 100.0f);

		sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
		sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
		sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
		glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
		glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

		// Lighting
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.8, 0.8, 0.8);

		// Load texture file
		glActiveTexture(GL_TEXTURE0);
		sc.tex_object[0] = BMPReader::loadTex("monu10.bmp");
		glActiveTexture(GL_TEXTURE1);
		sc.tex_object[1] = BMPReader::loadTex("monu9.bmp");
		glActiveTexture(GL_TEXTURE2);
		sc.tex_object[2] = BMPReader::loadTex("monu8.bmp");
		glActiveTexture(GL_TEXTURE3);
		sc.tex_object[3] = BMPReader::loadTex("chr_sword.bmp");
		glActiveTexture(GL_TEXTURE4);
		sc.tex_object[4] = BMPReader::loadTex("10602_Rubber_Duck_v1_diffuse.bmp");
	
	}

	void idle() {
		
		if(sc.r < 70.0f) {
			sc.r = sc.r + 0.5f;
		}

		if (sc.t < 70.0f) {
			sc.t = sc.t + 0.2f;
		}

		sc.angle += 0.01f;
		if (sc.angle > 2.0*3.141592) sc.angle -= 2.0*3.141592;
		glutPostRedisplay();

		sc.bounce += 0.1f;
		if (sc.time < 6.3) {
			sc.drop = sc.drop - (5.8*sc.time*sc.time) / 2;
			tmp = sc.drop;
		}
		else {
			sc.drop = tmp + 300 * sin(-(sc.time - 6.3));
		}

		sc.time += 0.1f;
	}

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.3f, 0.6f, 0.6f, 1.0f);
		glUseProgram(sc.rendering_program);

		// Lighting
		glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"),
			1, value_ptr(sc.view*vec4(70.0f*cos(sc.angle*5), 100.0, 70.0f*sin(sc.angle*5), 1.0)));

		sc.view = LookAt(vec3((55.0f+sc.r)*cos(sc.angle)- 7.9f, 7.0f + sc.t + abs(12.0f*sin(sc.angle * 5)), (55.0f+sc.r)*sin(sc.angle) - 7.9f), vec3(0.0f, 12.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));

		//sc.view = LookAt(vec3(60.0f, 50.0f, 60.0f), vec3(0.0f, 12.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
		/********Object  */
	
		// Teapot Material & texture
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.7f, 0.7f, 0.7f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 0);

		// Teapot Matrix
		sc.model = mat4(1.0);
		sc.model = scale(sc.model, vec3(5.5f,5.5f,5.5f));
		sc.model = translate(sc.model, vec3(0.0f, 6.0f, -7.0f));
		// rotate(mat4(1.0f), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.mesh->render();

		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.7f, 0.7f, 0.7f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 1);

		// Teapot Matrix
		sc.model = mat4(1.0);
		sc.model = scale(sc.model, vec3(5.5f, 5.5f, 5.5f));
		sc.model = translate(sc.model, vec3(-7.5f, 4.5f, 0.0f));
		// rotate(mat4(1.0f), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.mesh2->render();

	

		for (int j = 0; j < 80; j++) {
			for (int i = 0; i < 80; i++) {
				float color_row = i;
				float color_col = j;

				int x = i - 40, y = j - 40;
				float d = sqrt(x * x + y * y);
				float rtime = sc.time / 1.5 + 5.3;
				glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0, 0.7, 0.2);
				glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.2f, 0.2f, 0.2f);
				glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.9f, 0.9f, 0.9f);
				glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 0.9f);

				// Models

				mat4 CUBE;
				CUBE = mat4(1.0);

				CUBE = translate(mat4(1.0), vec3(color_row / 2 - 20, 1.3, color_col / 2 - 20));

				if (sc.time >= 5.514602 + (d*d / 20)) {
					CUBE = translate(CUBE, vec3(0, (-7.5f * sin(sc.time - 5.514602 - d)) / (d), 0));
				}
				glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(CUBE));
				//glUniform4f(sc.color_loc, 0.2, 0.7, 0.7, 1.0);


				sc.cube->render();
			}
		}
		mat4 shpere;
		shpere = mat4(1.0);

		shpere = scale(shpere, vec3(0.01, 0.01, 0.01));
		shpere = translate(shpere, vec3(0, sc.drop, 0));
		//m3 = rotate(m3, (float)radians(-90.0), vec3(0.0, 0.0, 1.0));

		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(shpere));
		//glUniform4f(sc.color_loc, 0.2, 0.7, 0.7, 1.0);


		sc.sphere->render();


		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.0f, 0.78f, 0.1f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 1.f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.f, 0.f, 0.f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 2);

		// Teapot Matrix
		sc.model = mat4(1.0);
		sc.model = scale(sc.model, vec3(10000.5f, 0.5f, 10000.5f));
		//sc.model = rotate(sc.model, float(-180.0f), vec3(0.0f, 0.0f, 1.0f));
		sc.model = translate(sc.model, vec3(0.0f, -2.0f, 0.0f));
		// rotate(mat4(1.0f), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.cube2->render();
	
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 3);

		// Teapot Matrix
		sc.model = mat4(1.0);
	
		sc.model = translate(sc.model, vec3(55.0f*cos(sc.angle) - 7.9f, 7.0f + abs(12.0f*sin(sc.angle * 5)), 55.0f*sin(sc.angle) - 7.9f));
		sc.mvs.push_back(sc.model);
		sc.model = scale(sc.model, vec3(7.5f, 7.5f, 7.5f));
		sc.model = rotate(sc.model, sc.angle, vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
		sc.mesh3->render();
		sc.model = sc.mvs.back();


		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 4);

		// Teapot Matrix
		mat4 child = mat4(1.0f);
		//child = translate(sc.model, vec3(0.0f,5.0f,0.0f));
		child = translate(sc.model, vec3(5.0f*cos(sc.angle*5.0f), 7.0f, 5.0f*sin(sc.angle*5.0f)));
		child = scale(child, vec3(0.7f, 0.7f, 0.7f));
		child = rotate(child, float(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(child));
	
		sc.mesh4->render();


		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.7f, 0.7f, 0.7f);
		glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
		glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 4);

		// Teapot Matrix
		mat4 child2 = mat4(1.0f);
		//child = translate(sc.model, vec3(0.0f,5.0f,0.0f));
		child2 = translate(sc.model, vec3(5.0f*cos(sc.angle*5.0f + radians(180.0f)), 7.0f, 5.0f*sin(sc.angle*5.0+radians(180.0f))));
		child2 = scale(child2, vec3(0.7f, 0.7f, 0.7f));
		child2 = rotate(child2, float(-90.0f), vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(child2));

		sc.mesh4->render();
		
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
		glutInitWindowSize(sc.WIDTH, sc.HEIGHT);
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
