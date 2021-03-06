/**
 * A wonderful tin man!
 *
 * Tyler McCarthy - Models & Animation
 * Charles Fallert - Lighting & Colors
 * 
 * mccartty@mail.gvsu.edu
 * fallertc@mail.gvsu.edu
 */
#include <unistd.h>
#include <GL/glew.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include "Arm.h"
#include "SwingFrame.h"
#include "Cylinder.h"
#include "Wheel.h"
#include "Sphere.h"
using namespace std;
void displayCallback(GLFWwindow*);

/* define global variables here */
Arm* forearm;
Arm* upperarm;
Cylinder* spot;
Cylinder* torso;
Sphere sphere;
SwingFrame frame;
int cf_mode = 2;

int SPEED_MODE = 3;

glm::mat4 forearm_cf, r_forearm_cf, l_forearm_cf;    
glm::mat4 upperarm_cf, r_upperarm_cf, l_upperarm_cf; 
glm::mat4 frame_cf;
glm::mat4 camera_cf, light1_cf, light0_cf;
glm::mat4 *active;

const float INIT_SWING_ANGLE = 35.0f; /* degrees */
const float GRAVITY = 9.8;   /* m/sec^2 */
bool is_anim_running = false;

/* light source setting */
GLfloat light0_color[] = {1.0, 1.0, 1.0, 1.0};   /* color */
GLfloat light1_color[] = {0.5, 0.0, 0.5, 1.0};  /* color */
GLfloat black_color[] = {0.0, 0.0, 0.0, 1.0};   /* color */

void myModelInit ()
{
    torso = new Cylinder();
	torso -> build(4, 3.5, 10);
	
	sphere.build(15, 20);
    spot = new Cylinder();
    spot -> build(1 + tan(glm::radians(40.0f)), 1, 2);

    upperarm = new Arm;
    upperarm->build(false, 7);

    forearm = new Arm();
    forearm->build(true, 7);
    
	r_forearm_cf = glm::translate(glm::vec3{0.0f, 0.0f, -upperarm->length()});
	l_forearm_cf = glm::translate(glm::vec3{0.0f, 0.0f, -upperarm->length()});
	
	r_forearm_cf *= glm::rotate(glm::radians(30.0f), glm::vec3{0,1,0});
    l_forearm_cf *= glm::rotate(glm::radians(30.0f), glm::vec3{0,1,0});
    
	frame.build();
    frame_cf = glm::translate(glm::vec3{0, 0 , 25});
    active = &camera_cf;

	//streetlamp = new 

    light0_cf = glm::translate(glm::vec3{-25, 8, 40});

    light1_cf = glm::translate(glm::vec3{0, 0, 0});
   	light1_cf = light1_cf * glm::rotate (glm::radians(-180.0f), glm::vec3{1,0,0});
}

/*================================================================*
 * Idle Callback function. This is the main engine for simulation *
 *================================================================*/
void updateCoordFrames()
{
    static double last_timestamp = 0;
    static float swing_time = 0;
    static float r_swing_angle = 0;
	static float l_swing_angle = 0;
	static int deg = 0;
    float delta, current;


    current = glfwGetTime();

	//ANIMATE!
	if (is_anim_running) {
        delta = (current - last_timestamp);

        /* use the pendulum equation to calculate its angle */
        swing_time += delta * SPEED_MODE;
        float r_angle = INIT_SWING_ANGLE * cos(swing_time * sqrt(GRAVITY / upperarm->length()));
        float l_angle = INIT_SWING_ANGLE * -cos(swing_time * sqrt(GRAVITY / upperarm->length()));
		
		r_upperarm_cf *= glm::rotate(glm::radians(r_angle - r_swing_angle), glm::vec3{0.0f, 1.0f, 0.0f});
		l_upperarm_cf *= glm::rotate(glm::radians(l_angle - l_swing_angle), glm::vec3{0.0f, 1.0f, 0.0f});
		
        r_forearm_cf *= glm::rotate(glm::radians(r_angle - r_swing_angle), glm::vec3{0.0f, 1.0f, 0.0f});
        l_forearm_cf *= glm::rotate(glm::radians(l_angle - l_swing_angle), glm::vec3{0.0f, 1.0f, 0.0f});
		
		r_swing_angle = r_angle;
		l_swing_angle = l_angle;
	}
    last_timestamp = current;
}

/*--------------------------------*
 * GLUT display callback function *
 *--------------------------------*/
void displayCallback (GLFWwindow *win)
{
    /* clear the window */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadMatrixf(glm::value_ptr(camera_cf));

	glDisable(GL_LIGHTING);
    glBegin (GL_LINES);
	glLineWidth(50);
	glColor3f (1, 0, 0);
    glVertex3i (0, 0, 0);
    glVertex3i (5, 0, 0);
    glColor3f (0, 1, 0);
    glVertex3i (0, 0, 0);
    glVertex3i (0, 5, 0);
    glColor3f (0, 0, 1);
    glVertex3i (0, 0, 0);
    glVertex3i (0, 0, 5);
    glEnd();
	glEnable(GL_LIGHTING);

    /* Specify the reflectance property of the ground using glColor
       (instead of glMaterial....)
     */
    glEnable (GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub (29, 100, 56);

	//draw ground with subdivisions
    const int GROUND_SIZE = 100;
	for (int y_count = GROUND_SIZE; y_count > -GROUND_SIZE; y_count--){
			
		for (int x_count = GROUND_SIZE; x_count > -GROUND_SIZE; x_count--){
  	 		glBegin(GL_QUADS);
			glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
   			glVertex2i (x_count, y_count);
   	 		glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    		glVertex2i (x_count-1, y_count);
    		glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    		glVertex2i (x_count-1, y_count-1);
    		glNormal3f (0.0f, 0.0f, 1.0f); /* normal vector for the ground */
    		glVertex2i (x_count, y_count-1);
			glEnd();
		}
	}
    glDisable (GL_COLOR_MATERIAL);

    /* place the light source in the scene. */
    glLightfv (GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::column(light0_cf, 3)));

    /* the curly bracket pairs below are only for readability */
    glPushMatrix();
    {
        glMultMatrixf(glm::value_ptr(light0_cf));

        /* Render light-0 as an emmisive object */
        if (glIsEnabled(GL_LIGHT0))
            glMaterialfv(GL_FRONT, GL_EMISSION, light0_color);
        sphere.render();
        glMaterialfv(GL_FRONT, GL_EMISSION, black_color);
    }
    glPopMatrix();

	/* Begin body build */
	glPushMatrix();
	{
		glMultMatrixf(glm::value_ptr(frame_cf));
		glPushMatrix();
		{
			glTranslatef(0, 0, -9);
    		glLightfv (GL_LIGHT1, GL_POSITION, glm::value_ptr(glm::column(light1_cf, 3)));
    		glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, glm::value_ptr(glm::column(light1_cf, 2)));
		}
		glPopMatrix();

		frame.render();
		//drop in a head
		glPushMatrix();
		{
			glScalef(2, 2, 2.5);
			glTranslatef(0, 0, 0.5);
			sphere.render();
		}
		glPopMatrix();
		glTranslatef(0, 0, -4.0);
		glScalef(0.75, 1, 1);
		torso -> render();
		glTranslatef(0, 0, -6.0);
		glRotatef(180.0, 0, 1, 0);
		glScalef(1, 1, 0.33);
		//drop in a pelvis
		torso -> render();	
		
		float COPPER_AMBIENT[] = {0.247250, 0.224500, 0.064500, 1.000000};
    	float COPPER_DIFFUSE[] = {0.346150, 0.314300, 0.090300, 1.000000};
    	float COPPER_SPECULAR[] = {0.797357, 0.723991, 0.208006, 1.000000};

    	glMaterialfv(GL_FRONT, GL_AMBIENT, COPPER_AMBIENT);
    	glMaterialfv(GL_FRONT, GL_DIFFUSE, COPPER_DIFFUSE);
    	glMaterialfv(GL_FRONT, GL_SPECULAR, COPPER_SPECULAR);
    	glMaterialf(GL_FRONT, GL_SHININESS, 83.199997);
		//drop in a belt
		glPushMatrix();
		{
			glTranslatef(0, 0, -5);
			glScalef(5, 5, 5);
			sphere.render();
		}
		glPopMatrix();
		
	}
	glPopMatrix();

	//render LEVITATION MEGADRIVE
	glPushMatrix();
	{
		glMultMatrixf(glm::value_ptr(frame_cf));
		glTranslatef(0, 0, -11.5);
		glScalef(3.0, 3.0, 3.0);
		sphere.render();
	}
	glPopMatrix();

	/* render left arm */
	glPushMatrix();
    {
		glMultMatrixf(glm::value_ptr(frame_cf));
        glPushMatrix();
        {	
        	glTranslatef(0, -5, 0);
			
			//shoulder sphere
			glPushMatrix();{
				glScalef(1.5, 1.5, 1.5);
				sphere.render();
			}
			glPopMatrix();

			glMultMatrixf(glm::value_ptr(l_upperarm_cf));
            glTranslatef(0, 0, -4);
			//upper arm
            upperarm->render(false);
            //glPushMatrix();
            //{
                glTranslatef(0, 0, 4);
				glMultMatrixf(glm::value_ptr(l_forearm_cf));
				forearm->render(true);
				glPushMatrix();
				{
					glScalef(1.5, 1.5, 1.5);
					sphere.render();
				}
				glPopMatrix();
				glTranslatef(0, 0, -8);
				sphere.render();
            //}
            //glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();

	/* render right arm */
	glPushMatrix();
    {
		glMultMatrixf(glm::value_ptr(frame_cf));
        glPushMatrix();
        {	
        	glTranslatef(0, 5, 0);
			
			//shoulder sphere
			glPushMatrix();{
				glScalef(1.5, 1.5, 1.5);
				sphere.render();
			}
			glPopMatrix();
			
			glMultMatrixf(glm::value_ptr(r_upperarm_cf));
            glTranslatef(0, 0, -4);
			//upper arm
            upperarm->render(false);
            //glPushMatrix();
            //{
                glTranslatef(0, 0, 4);
				glMultMatrixf(glm::value_ptr(r_forearm_cf));
				forearm->render(true);
				glPushMatrix();
				{
					glScalef(1.5, 1.5, 1.5);
					sphere.render();
				}
				glPopMatrix();
				glTranslatef(0, 0, -8);
				sphere.render();
            //}
            //glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();

	/* to make smooth transition between frame */
    glfwSwapBuffers(win);
}

void keyCallback (GLFWwindow *win, int key, int scan_code, int action, int mods) {
    if (action == GLFW_RELEASE) return; /* ignore key release action */

    if (mods == GLFW_MOD_SHIFT) {
        switch (key) {
            case GLFW_KEY_UP: /* raise */
				camera_cf *= glm::translate(glm::vec3(0.0f, 0.0f, -1.0f));
                break;
            case GLFW_KEY_DOWN: /* tilt */
				camera_cf *= glm::translate(glm::vec3(0.0f, 0.0f, 1.0f));
                break;
            case GLFW_KEY_LEFT: /* pan left */
				camera_cf *= glm::translate(glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            case GLFW_KEY_RIGHT: /* pan right */
				camera_cf *= glm::translate(glm::vec3(0.0f, -1.0f, 0.0f));
                break;
            case GLFW_KEY_W:
                frame_cf *= glm::rotate(glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            case GLFW_KEY_S:
                frame_cf *= glm::rotate(glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            case GLFW_KEY_A:
                frame_cf *= glm::rotate(glm::radians(-3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                break;
            case GLFW_KEY_D:
                frame_cf *= glm::rotate(glm::radians(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                break;
            default:
                break;
        };
    } else {
        switch (key) {
			case GLFW_KEY_W:
				frame_cf *= glm::translate(glm::vec3(-SPEED_MODE/2, 0.0f, 0.0f));
				is_anim_running = true;
				break;
			case GLFW_KEY_S:
				frame_cf *= glm::translate(glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case GLFW_KEY_A:
				frame_cf *= glm::rotate(glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case GLFW_KEY_D:
				frame_cf *= glm::rotate(glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
            case GLFW_KEY_UP:
				camera_cf *= glm::translate(glm::vec3(1.0f, 0.0f, 0.0f));
                break;
            case GLFW_KEY_DOWN:
				camera_cf *= glm::translate(glm::vec3(-1.0f, 0.0f, 0.0f));
                break;
            case GLFW_KEY_LEFT:
                camera_cf *= glm::rotate(glm::radians(3.0f), glm::vec3{0.0f, 0.0f, 1.0f});
                break;
            case GLFW_KEY_RIGHT:
                camera_cf *= glm::rotate(glm::radians(-3.0f), glm::vec3{0.0f, 0.0f, 1.0f});
                break;
            case GLFW_KEY_T:
                if(SPEED_MODE == 3){
					SPEED_MODE = 8;
					break;
				}
				SPEED_MODE = 3;
				break;
            
			
			case GLFW_KEY_ESCAPE:
                exit(0);
          
		  	case GLFW_KEY_0:
                if (glIsEnabled(GL_LIGHT0))
                    glDisable(GL_LIGHT0);
                else
                    glEnable(GL_LIGHT0);
                break;
          
		  	case GLFW_KEY_1:
                if (glIsEnabled(GL_LIGHT1))
                    glDisable(GL_LIGHT1);
                else
                    glEnable(GL_LIGHT1);
                break;

            case GLFW_KEY_SPACE: /* pause the animation */
                is_anim_running ^= true;
                
				break;

          /* Transition between CF modes */
		   	
			case GLFW_KEY_TAB:
				switch (cf_mode) {
                	case 1:
						active = &camera_cf;
						cf_mode = 2;
						break;
					case 2:
						active = &frame_cf;
						cf_mode = 3;
						break;
					case 3:
						active = &light1_cf;
						cf_mode = 1;
						break;
				};
        }
    }
}

void myGLInit ()
{
    glClearColor (0.1, 0.1, 0.2, 1.0); /*background color*/

    /* fill front-facing polygon */
    glPolygonMode (GL_FRONT, GL_FILL);
    /* draw outline of back-facing polygon */
    glPolygonMode (GL_BACK, GL_LINE);

    /* Enable depth test for hidden surface removal */
    glEnable (GL_DEPTH_TEST);

    /* enable back-face culling */
    glEnable (GL_CULL_FACE);
    //glCullFace (GL_BACK);

    /* Enable shading */
    glEnable (GL_LIGHTING);
    glEnable (GL_NORMALIZE); /* Tell OpenGL to renormalize normal vector
                              after transformation */

    /* initialize two light sources */
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT0, GL_AMBIENT, light0_color);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light0_color);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light0_color);
    glEnable (GL_LIGHT1);
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_color);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_color);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_color);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 40);

    glEnableClientState(GL_VERTEX_ARRAY);
}

/*--------------------------------*
 * GLUT Reshape callback function *
 *--------------------------------*/
void reshapeCallback (GLFWwindow *win, int w, int h)
{
    glViewport (0, 0, w, h);

    /* switch to Projection matrix mode */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective (60, (float) w / (float) h, 5.0, 1000.0);

    /* switch back to Model View matrix mode */
    glMatrixMode (GL_MODELVIEW);
    camera_cf = glm::lookAt(glm::vec3(25,20,20), glm::vec3(0,0,10), glm::vec3(0,0,1));
}

int main (int argc, char **argv)
{

    glfwInit();
    GLFWwindow *win;
    win = glfwCreateWindow(100, 50, "Animation", NULL, NULL);

    glfwMakeContextCurrent(win);
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf (stderr, "GLEW init error %s\n",
                glewGetErrorString(err));
        exit (EXIT_FAILURE);
    }

    srand (time(0));
    
    myGLInit ();
    myModelInit ();
    
    /* setup display callback function */
    glfwSetFramebufferSizeCallback(win, reshapeCallback);
    glfwSetWindowRefreshCallback(win, displayCallback);
    glfwSetKeyCallback(win, keyCallback);
    glfwSetWindowSize(win, 800, 600);

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        updateCoordFrames();
        displayCallback(win);
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    
}
