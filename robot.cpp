/***********************************************************
             CSC418/2504, Fall 2009
  
                 robot.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;
float joint_rot = 0.0f;

//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////
//More Joint Parameteres

float joint_rightleg = 0.0f;
float joint_neck = 0.0f;
float joint_leftarm = 0.0f;
float joint_rightarm = 0.0f;
float joint_leftwrist = 0.0f;
float joint_rightwrist = 0.0f;

//parameters for robot's up down motion
const float MAX_UP = 40.0f;
const float MAX_DOWN = -40.0f;
float UpDown = 0.0f;

//parameters for arm lengthing motion
const float MAX_EXT = 1.25f;
const float MIN_EXT = 0.5f;
float arm_extension = 1.0f;


// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawCircle(float radius);

// Return the current system clock (in seconds)
double getTime();


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 600x400 window by default...\n");
        Win[0] = 600;
        Win[1] = 400;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the left leg joint
    GLUI_Spinner *joint_spinner
        = glui->add_spinner("Left Leg Joint", GLUI_SPINNER_FLOAT, &joint_rot);
    joint_spinner->set_speed(0.1);
    joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    ///////////////////////////////////////////////////////////
    // TODO: 
    //   Add controls for additional joints here
    ///////////////////////////////////////////////////////////
	//Creates spinner that controls rotation for right leg
	GLUI_Spinner *rightleg_spinner
		= glui->add_spinner("Right Leg Joint", GLUI_SPINNER_FLOAT, &joint_rightleg);
	rightleg_spinner->set_speed(0.1);
	rightleg_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	//Creates spinner that controls rotation for left arm
	GLUI_Spinner *leftarm_spinner
		= glui->add_spinner("Left Arm Joint", GLUI_SPINNER_FLOAT, &joint_leftarm);
	leftarm_spinner->set_speed(0.1);
	leftarm_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);


	//Creates spinner that controls rotation for right arm
	GLUI_Spinner *rightarm_spinner
		= glui->add_spinner("Right Arm Joint", GLUI_SPINNER_FLOAT, &joint_rightarm);
	rightarm_spinner->set_speed(0.1);
	rightarm_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	//Creates spinner that controls rotation for neck
	GLUI_Spinner *neck_spinner
		= glui->add_spinner("Neck Joint", GLUI_SPINNER_FLOAT, &joint_neck);
	neck_spinner->set_speed(0.1);
	neck_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	//Creates spinner that controls rotation for right wrist
	GLUI_Spinner *rightwrist_spinner
		= glui->add_spinner("Right Wrist Joint", GLUI_SPINNER_FLOAT, &joint_rightwrist);
	rightwrist_spinner->set_speed(0.1);
	rightwrist_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	//Creates spinner that controls rotation for left wrist
	GLUI_Spinner *leftwrist_spinner
		= glui->add_spinner("Left Wrist Joint", GLUI_SPINNER_FLOAT, &joint_leftwrist);
	leftwrist_spinner->set_speed(0.1);
	leftwrist_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);


	//Creates spinner that controls Up and Down motion of robot
	GLUI_Spinner *updown_spinner
		= glui->add_spinner("Vertical Position of Robot", GLUI_SPINNER_FLOAT, &UpDown);
	updown_spinner->set_speed(0.1);
	updown_spinner->set_float_limits(MAX_DOWN, MAX_UP, GLUI_LIMIT_CLAMP);


	//Creates spinner that controls extension of robot's arms
	GLUI_Spinner *ext_spinner
		= glui->add_spinner("Arm Extension Scale", GLUI_SPINNER_FLOAT, &arm_extension);
	ext_spinner->set_speed(0.1);
	ext_spinner->set_float_limits(MIN_EXT, MAX_EXT, GLUI_LIMIT_CLAMP);




    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
	const double scaling_speed = 0.5;
	const double translation_speed = 0.5;
    const double joint_rot_speed = 0.5;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
    joint_rot = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////
	/////CHANGED CERTAIN SINUSOIDS TO COS for cooler animations
	//Rotation angle for right leg
	double joint_rot_rightleg = (cos(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_rightleg = joint_rot_rightleg * JOINT_MIN + (1 - joint_rot_rightleg) * JOINT_MAX;

	//Rotation angle for neck
	double joint_rot_neck = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_neck = joint_rot_neck* JOINT_MIN + (1 - joint_rot_neck) * JOINT_MAX;

	//Rotation angle for left arm
	double joint_rot_leftarm = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_leftarm = joint_rot_leftarm * JOINT_MIN + (1 - joint_rot_leftarm) * JOINT_MAX;

	//Rotation angle for right arm
	double joint_rot_rightarm = (cos(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_rightarm = joint_rot_rightarm* JOINT_MIN + (1 - joint_rot_rightarm) * JOINT_MAX;

	//Rotation angle for left wrist
	double joint_rot_leftwrist = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_leftwrist = joint_rot_leftwrist* JOINT_MIN + (1 - joint_rot_leftwrist) * JOINT_MAX;

	//Rotation angle for right wrist
	double joint_rot_rightwrist = (cos(animation_frame*joint_rot_speed) + 1.0) / 2.0;
	joint_rightwrist = joint_rot_rightwrist* JOINT_MIN + (1 - joint_rot_rightwrist) * JOINT_MAX;
	
	//Translation factor up/down of robot
	double updown_trans = (sin(animation_frame*translation_speed) + 1.0) / 2.0;
	UpDown = updown_trans* MAX_DOWN + (1 - updown_trans) * MAX_UP;

	//Scaling of robot's hands
	double scale_hands = (sin(animation_frame*scaling_speed) + 1.0) / 2.0;
	arm_extension = scale_hands* MIN_EXT + (1 - scale_hands) * MAX_EXT;


    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}


// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    const float BODY_WIDTH = 60.0f;
    const float BODY_LENGTH = 80.0f;
    const float ARM_LENGTH = 80.0f;
    const float ARM_WIDTH = 20.0f;
	const float TORSO_LENGTH = ARM_LENGTH*1.4;

    // Push the current transformation matrix on the stack
    glPushMatrix();
        //All the object that are parents in kinematic trees are subject to an offset 'UpDown' for the Up and down motion of the robot
        // Draw the 'body' //////////////////////////////////////
        glPushMatrix();

			glTranslatef(0.0, UpDown, 0.0);
            // Scale square to size of body
            glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);

            // Set the colour to green
            glColor3f(0.0, 1.0, 0.0);

            // Draw the square for the body
            drawSquare(1.0);
        glPopMatrix();

		
         // Draw the 'torso'//////////////////////////
		glPushMatrix();
		//Translate down to under body
			glTranslatef(0, -BODY_LENGTH/2-ARM_WIDTH/2+UpDown, 0.0);

			glScalef(TORSO_LENGTH , ARM_WIDTH, 1.0);

			//Set the colour of the torso to red
			glColor3f(1.0, 0.0, 0.0);

			//Draw the torso
			drawSquare(1.0);
			
		glPopMatrix();

	
       // Draw the 'left leg'//////////////////////////
		glPushMatrix();
        // Move the arm to the joint hinge
			glTranslatef(-TORSO_LENGTH / 2 + ARM_WIDTH / 2, -BODY_LENGTH/2-ARM_WIDTH*0.4 + UpDown, 0.0);

			 

        // Rotate along the hinge
			 glRotatef(joint_rot, 0.0, 0.0, 1.0);
	
        // Scale the size of the leg
			 glScalef(ARM_WIDTH, ARM_LENGTH, 1.0);

        // Move to center location of leg, under previous rotation
			 glTranslatef(0.0, -0.4, 0.0);

		
		
        // Draw the square for the leg
			 glColor3f(0.0, 0.0, 1.0);
			 drawSquare(1.0);

		// Translate to place circle at joint and Descale back to size of 5
			 glTranslatef(0.0, 0.4, 0.0);
			 glScalef(5/ARM_WIDTH, 5/ARM_LENGTH, 1.0);

			 

		//Draw joint circle
			 glColor3f(1.0, 1.0, 1.0);
			 drawCircle(1.0);

		glPopMatrix();



		//Draw the 'right leg'////////////////////////////
		glPushMatrix();
		// Move the leg to the joint hinge
			glTranslatef(TORSO_LENGTH / 2 - ARM_WIDTH / 2, -BODY_LENGTH/2 - ARM_WIDTH*0.4 + UpDown, 0.0);

		// Rotate along the hinge
			glRotatef(joint_rightleg, 0.0, 0.0, 1.0);

		// Scale the size of the leg
			glScalef(ARM_WIDTH, ARM_LENGTH, 1.0);

		// Move to center location of leg, under previous rotation
			glTranslatef(0.0, -0.4, 0.0);

		// Draw the square for the leg
			glColor3f(0.0, 0.0, 1.0);
			drawSquare(1.0);


		// Translate to place circle at joint and Descale back to size of 5
			glTranslatef(0.0, 0.4, 0.0);
			glScalef(5 / ARM_WIDTH, 5 / ARM_LENGTH, 1.0);



		//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);

		glPopMatrix();






		//Draw the 'left arm'/////////////////////////////
		glPushMatrix();

			glTranslatef(-BODY_WIDTH/2+ARM_WIDTH*0.4,2.0+UpDown, 0.0);
			
			glRotatef(joint_leftarm, 0.0, 0.0, 1.0);
			
			
			glScalef(ARM_LENGTH, ARM_WIDTH, 1.0);
			glTranslatef(-0.4, 0.0, 0.0);
			
			glColor3f(0.1, 0.7, 0.7);
			drawSquare(1.0);


			// Translate to place circle at joint and Descale back to size of 5
			glTranslatef(0.4, 0.0, 0.0);
			glScalef(5 / ARM_LENGTH, 5 / ARM_WIDTH, 1.0);



			//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);

			//Translate back to get to the centre of arm
			glScalef(ARM_LENGTH / 5, ARM_WIDTH / 5, 1.0);
			glTranslatef(-0.4, 0.0, 0.0);
			
			//Draw the 'left Hand'///////////////////////

			//Scale back down to 1
			glScalef(1/ARM_LENGTH, 1/ARM_WIDTH, 1.0);
			//Translate to wrist joint
			glTranslatef(-ARM_LENGTH/2+ARM_WIDTH/2, 0, 0);

			//Rotate the wrist joint
			glRotatef(joint_leftwrist, 0.0, 0.0, 1.0);
			
			//hand scaling animation
			glScalef(ARM_LENGTH*arm_extension, ARM_WIDTH, 1.0);
			
			
		
			//Draw the square for the 'hand' add a scaling factor proportional to arm_extension so the wrist stays still
			//and it creates the illusion of elongation.
			
			glTranslatef(-0.4 + (1-arm_extension)*8/(ARM_LENGTH*arm_extension),0 ,0);

		
			
			
			glColor3f(1.0, 0.0, 1.0);
			drawSquare(1.0);

			//Translate to joint location

			glTranslatef(0.4 - (1 - arm_extension) * 8 / (ARM_LENGTH*arm_extension), 0.0, 0.0);
			//Scale back down to 5
			glScalef(5 / (ARM_LENGTH*arm_extension), 5 / ARM_WIDTH, 1.0);

			//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);
			
		

		glPopMatrix();

		//Draw the 'right arm'//////////////////////////
		glPushMatrix();

			glTranslatef(BODY_WIDTH/2-ARM_WIDTH*0.4, 2.0+UpDown, 0.0);

			glRotatef(joint_rightarm, 0.0, 0.0, 1.0);

			glScalef(ARM_LENGTH, ARM_WIDTH, 1.0);

			glTranslatef(0.4, 0, 0);

			//draw square for 'arm'
			glColor3f(0.1, 0.7, 0.7);

			drawSquare(1.0);


			// Translate to place circle at joint and Descale back to size of 5
			glTranslatef(-0.4, 0.0, 0.0);
			glScalef(5 / ARM_LENGTH, 5 / ARM_WIDTH, 1.0);



			//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);


			//Draw the 'right Hand'//////////////////////
			glScalef( ARM_LENGTH/5,  ARM_WIDTH/5, 1.0);
			glTranslatef(0.4, 0.0, 0.0);
			
			//scale back down to 1
			glScalef(1 / ARM_LENGTH, 1 / ARM_WIDTH, 1.0);

			//Translate to wrist joint
			glTranslatef(ARM_LENGTH/2 - ARM_WIDTH/2, 0, 0);

			//Rotate the wrist joint
			glRotatef(joint_rightwrist, 0.0, 0.0, 1.0);

			//hand scaling animation
			glScalef(ARM_LENGTH*arm_extension, ARM_WIDTH, 1.0);

			//Draw the square for the 'hand' add a scaling factor proportional to arm_extension so the wrist stays still
			//and it creates the illusion of elongation.

			glTranslatef(0.4 - (1 - arm_extension) * 8 / (ARM_LENGTH*arm_extension), 0, 0);

			glColor3f(1.0, 0.0, 1.0);
			drawSquare(1.0);

			//Translate to joint location

			glTranslatef(-0.4 + (1 - arm_extension) * 8 / (ARM_LENGTH*arm_extension), 0, 0);
			//Scale back down to 5
			glScalef(5 / (ARM_LENGTH*arm_extension), 5 / ARM_WIDTH, 1.0);

			//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);



		glPopMatrix();

		//Draw the Neck and Head//////////////////////////////////
		glPushMatrix();
			
			glTranslatef(0.0, BODY_LENGTH/2-ARM_WIDTH/2+UpDown, 0.0);
			glRotatef(joint_neck, 0.0, 0.0, 1.0);
			glScalef(ARM_WIDTH, ARM_LENGTH/1.25, 1.0);
			glTranslatef(0.0, 0.4, 0);
			glColor3f(0.1, 0.7, 0.7);
			drawSquare(1.0);


			// Translate to place circle at joint and Descale back to size of 5
			glTranslatef(0.0, -0.4, 0.0);
			glScalef(5 / ARM_WIDTH, 5 / (ARM_LENGTH/1.25), 1.0);

			//Draw joint circle
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(1.0);

			//Translate back to get to the centre of neck
			glScalef( ARM_WIDTH/5,  (ARM_LENGTH/1.25)/5, 1.0);
			glTranslatef(0.0, 0.4, 0.0);


			//Draw Head
			//Scale back down to 1
			
			glScalef(1 / ARM_WIDTH, 1 / (ARM_LENGTH/1.25), 1.0);
			glTranslatef(0.0, BODY_WIDTH-ARM_WIDTH/2, 0.0);
			glScalef(ARM_LENGTH, ARM_WIDTH*2.5, 1.0);

			glColor3f(1.0, 0.0, 1.0);
			drawSquare(1.0);
			//Draw the eyes
			//Scale down to 1
			glScalef(1/ARM_LENGTH,1/ (ARM_WIDTH*2.5), 1.0);
			
			//Draw the left eye
			glTranslatef(-ARM_WIDTH, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(10.0);
			drawCircle(6.0);
			//Draw the right eye
			glTranslatef(ARM_WIDTH * 2, 0.0, 0.0);
			glColor3f(1.0, 1.0, 1.0);
			drawCircle(10.0);
			drawCircle(6.0);
			
			



		glPopMatrix();

		glPushMatrix();
		glTranslatef(80, 80, 0);
		glRotatef(joint_leftarm, 0, 0, 1.0);
		glScalef(5, 4, 1);
		glColor3f(1.0, 1.0, 1.0);
		drawCircle(10.0);
		glPopMatrix();
		

		glPushMatrix();
		
		glRotatef(joint_leftarm, 0, 0, 1.0);
		glTranslatef(80, 80, 0);
		glScalef(5, 5, 1);
		glColor3f(1.0, 1.0, 1.0);
		drawCircle(10.0);
		glPopMatrix();


    // Retrieve the previous state of the transformation stack
    glPopMatrix();


    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}


void drawCircle(float radius){
	glBegin(GL_LINES);
	for (float i = 0; i < 360; i++){
		glVertex2d(radius*sin(i), radius*cos(i));
		
	}
	glEnd();
}