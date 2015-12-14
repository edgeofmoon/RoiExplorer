#include "MyUiPanel.h"
#include "MyGLHeader.h"

int MyUiPanel::main_window = -1;

Signal1< int > MyUiPanel::Signal_SetViewAngle;
Signal1< int > MyUiPanel::Signal_EnableComponent;
Signal1< int > MyUiPanel::Signal_DisableComponent;
Signal2< int, float > MyUiPanel::Signal_SetValuef;
Signal1< int > MyUiPanel::Signal_Event;

int MyUiPanel::mComponentToggle[4] = { 1, 1, 1, 1 };
float MyUiPanel::mValuefs[2] = {5.f, 0.1f};

MyUiPanel::MyUiPanel()
{
}


MyUiPanel::~MyUiPanel()
{
}

void MyUiPanel::GetViewport(int &x, int &y, int &w, int &h){
	GLUI_Master.get_viewport_area(&x, &y, &w, &h);
}

void MyUiPanel::InitGL(int w, int h){
	char* argv[] = {"haha", "nothing"};
	int argc = 2;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(w, h);
	main_window = glutCreateWindow("Brain Cohort Visualization");
	glewInit();
	glutDisplayFunc(myGlutDisplay);
	GLUI_Master.set_glutDisplayFunc(myGlutDisplay);
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);
	GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);
	GLUI_Master.set_glutSpecialFunc(NULL);
	GLUI_Master.set_glutMouseFunc(myGlutMouse);
	glutMouseWheelFunc(myGlutMouseWheel);
	glutMotionFunc(myGlutMotion);
	glutPassiveMotionFunc(myGlutPassiveMotion);
}

void MyUiPanel::AddUIs(){
	printf("GLUI version: %3.2f\n", GLUI_Master.get_version());

	/*** Create the side subwindow ***/
	GLUI* glui = GLUI_Master.create_glui_subwindow(main_window,
		GLUI_SUBWINDOW_RIGHT);

	/*** Camera ***/
	GLUI_Panel* viewpoint_panel = new GLUI_Panel(glui, "Camera");
	viewpoint_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_Button(viewpoint_panel, "Superior", 1, SignalSetViewAngle);
	new GLUI_Button(viewpoint_panel, "Inferior", 2, SignalSetViewAngle);
	new GLUI_Button(viewpoint_panel, "Right", 3, SignalSetViewAngle);
	new GLUI_Button(viewpoint_panel, "Left", 4, SignalSetViewAngle);
	new GLUI_Button(viewpoint_panel, "Posterior", 5, SignalSetViewAngle);
	new GLUI_Button(viewpoint_panel, "Anterior", 6, SignalSetViewAngle);

	/*** Event ***/
	GLUI_Panel* Event_panel = new GLUI_Panel(glui, "Edit");
	Event_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_Button(Event_panel, "Disable ROI", 1, SignalEvent);
	new GLUI_Button(Event_panel, "Enable ROI", 2, SignalEvent);
	new GLUI_Button(Event_panel, "Clear Tree", 3, SignalEvent);
	new GLUI_Button(Event_panel, "Second Cohort", 4, SignalEvent);
	new GLUI_Button(Event_panel, "Change Tree", 5, SignalEvent);
	new GLUI_Button(Event_panel, "Create ROI", 6, SignalEvent);

	/*** Components ***/
	GLUI_Panel *component_panel = new GLUI_Panel(glui, "Component");
	component_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_Checkbox(component_panel, 
		"Tree Color", &mComponentToggle[0], 0, SignalToggleComponents);
	new GLUI_Checkbox(component_panel,
		"Tree Stats", &mComponentToggle[1], 1, SignalToggleComponents);
	new GLUI_Checkbox(component_panel,
		"Tree-Roi Lines", &mComponentToggle[2], 2, SignalToggleComponents);
	new GLUI_Checkbox(component_panel,
		"Roi-Roi Lines", &mComponentToggle[3], 3, SignalToggleComponents);

	/*** Rendering ***/
	GLUI_Panel* render_panel = new GLUI_Panel(glui, "Rendering");
	render_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(render_panel, "Transparency");
	GLUI_Scrollbar* slider = new GLUI_Scrollbar(render_panel,
		"Transparency", GLUI_SCROLL_HORIZONTAL, &mValuefs[0], 0, SignalSetValues);
	slider->set_float_limits(0, 64);
	new GLUI_StaticText(render_panel, "Track Threshold");
	slider = new GLUI_Scrollbar(render_panel,
		"Track Threshold", GLUI_SCROLL_HORIZONTAL, &mValuefs[1], 1, SignalSetValues);
	slider->set_float_limits(0, 1);

	glui->set_main_gfx_window(main_window);
}