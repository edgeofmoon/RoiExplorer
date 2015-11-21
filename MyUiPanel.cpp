#include "MyUiPanel.h"
#include "MyGLHeader.h"

int MyUiPanel::main_window = -1;

MyUiPanel::MyUiPanel()
{
	UI_trackDensityFilter = 0.1;
	UI_sampleRate = 1024;
	UI_decayFactor = 1024;
	UI_transparencyExponent = 16;
	UI_roiRenderRatio = 1;
	UI_linkDrawThreshold = 0.5;
	UI_bubbleThreshold = 0;
	UI_bubbleMaxRadius = 0.1;
	UI_bubbleCoreRadius = 0.025;
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
	main_window = glutCreateWindow("Cohort Brain Comparison");
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


	/*** Magnifier ***/
	GLUI_Rollout *magnifier_panel = new GLUI_Rollout(glui, "Magnifier", 0);
	magnifier_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(magnifier_panel, "Radius");
	GLUI_Scrollbar* magnifier_radiusSlider = new GLUI_Scrollbar
		(magnifier_panel, "Magnify Radius", GLUI_SCROLL_HORIZONTAL, &UI_magnifier_radius);
	magnifier_radiusSlider->set_int_limits(0, 600);
	magnifier_radiusSlider->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(magnifier_panel, "Scale");
	GLUI_Scrollbar* magnifier_scaleSlider = new GLUI_Scrollbar
		(magnifier_panel, "Magnify Scale", GLUI_SCROLL_HORIZONTAL, &UI_magnifier_scale);
	magnifier_scaleSlider->set_float_limits(1, 10);
	magnifier_scaleSlider->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(magnifier_panel, "Focus");
	GLUI_Scrollbar* magnifier_focusRatioSlider = new GLUI_Scrollbar
		(magnifier_panel, "Focus Radius", GLUI_SCROLL_HORIZONTAL, &UI_fishEye_focusRadiusRatio);
	magnifier_focusRatioSlider->set_float_limits(0, 1);
	magnifier_focusRatioSlider->set_alignment(GLUI_ALIGN_LEFT);


	/*** Components ***/
	GLUI_Rollout *component_panel = new GLUI_Rollout(glui, "Component", 0);
	component_panel->set_alignment(GLUI_ALIGN_LEFT);
	GLUI_Checkbox* trackCheckbox = new GLUI_Checkbox(component_panel, "Tracks", &UI_drawTracks);
	GLUI_Checkbox* trackVolCheckbox = new GLUI_Checkbox(component_panel, "Track Volume", &UI_drawTrackVol);
	GLUI_Checkbox* meshCheckbox = new GLUI_Checkbox(component_panel, "Cortex Mesh", &UI_drawMesh);
	GLUI_Checkbox* drawContour0Checkbox = new GLUI_Checkbox(component_panel, "Contour", &UI_drawContour);
	GLUI_Checkbox* Legend_Checkbox = new GLUI_Checkbox(component_panel, "Legend", &UI_2DLegend);
	GLUI_Checkbox* hoverOn3DCheckbox = new GLUI_Checkbox(component_panel, "Hover 3D", &UI_hoverOn3D);
	GLUI_Checkbox* hoverOn2DCheckbox = new GLUI_Checkbox(component_panel, "Hover 2D", &UI_hoverOn2D);

	/*** Track Vis ***/
	GLUI_Rollout* track_panel = new GLUI_Rollout(glui, "Tracks", 0);
	track_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(track_panel, "Density Filter");
	GLUI_Scrollbar* trackDensity_Slider = new GLUI_Scrollbar
		(track_panel, "Density Filter", GLUI_SCROLL_HORIZONTAL, &UI_trackDensityFilter);
	trackDensity_Slider->set_float_limits(0, 1);
	new GLUI_StaticText(track_panel, "Sample Rate");
	GLUI_Scrollbar* sampleRate_Slider = new GLUI_Scrollbar
		(track_panel, "Sample Rate", GLUI_SCROLL_HORIZONTAL, &UI_sampleRate);
	sampleRate_Slider->set_float_limits(8, 2048);
	new GLUI_StaticText(track_panel, "Decay Factor");
	GLUI_Scrollbar* decayFactor_Slider = new GLUI_Scrollbar
		(track_panel, "Decay Factor", GLUI_SCROLL_HORIZONTAL, &UI_decayFactor);
	decayFactor_Slider->set_float_limits(8, 2048);
	new GLUI_StaticText(track_panel, "Mesh Transparency");
	GLUI_Scrollbar* meshTransparency_Slider = new GLUI_Scrollbar
		(track_panel, "Mesh Transparency", GLUI_SCROLL_HORIZONTAL, &UI_transparencyExponent);
	meshTransparency_Slider->set_float_limits(0, 128);

	/*** Label ***/
	GLUI_Rollout* label_panel = new GLUI_Rollout(glui, "Label", 0);
	label_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_StaticText(label_panel, "Display Ratio");
	GLUI_Scrollbar* labelDrawRatioScrollbar = new GLUI_Scrollbar(label_panel, "Label Ratio", GLUI_SCROLL_HORIZONTAL, &UI_labelDrawRatio);
	labelDrawRatioScrollbar->set_float_limits(0, 1);
	GLUI_Checkbox* labelSolid_Checkbox = new GLUI_Checkbox(label_panel, "Solid", &UI_labelSolid);

	/*** Regions ***/
	GLUI_Rollout* roi_panel = new GLUI_Rollout(glui, "ROI", 0);
	new GLUI_StaticText(roi_panel, "Render Ratio");
	GLUI_Scrollbar* roiRenderRatio_Slider = new GLUI_Scrollbar
		(roi_panel, "Render Ratio", GLUI_SCROLL_HORIZONTAL, &UI_roiRenderRatio);
	roiRenderRatio_Slider->set_float_limits(0, 1);
	new GLUI_StaticText(roi_panel, "Link Threshold");
	GLUI_Scrollbar* LinkThreshold_Slider = new GLUI_Scrollbar
		(roi_panel, "Link Threshold", GLUI_SCROLL_HORIZONTAL, &UI_linkDrawThreshold);
	LinkThreshold_Slider->set_float_limits(0, 1);

	/*** Bubble Sets ***/
	GLUI_Rollout* bubble_panel = new GLUI_Rollout(glui, "BubbleSets", 0);
	new GLUI_StaticText(bubble_panel, "Contour Threshold");
	GLUI_Scrollbar* bubbleThreshold_Slider = new GLUI_Scrollbar
		(bubble_panel, "Contour Threshold", GLUI_SCROLL_HORIZONTAL, &UI_bubbleThreshold);
	roiRenderRatio_Slider->set_float_limits(0, 1);
	new GLUI_StaticText(bubble_panel, "Max Affected Radius");
	GLUI_Scrollbar* bubbleMaxRadius_Slider = new GLUI_Scrollbar
		(bubble_panel, "Max Affected Radius", GLUI_SCROLL_HORIZONTAL, &UI_bubbleMaxRadius);
	bubbleMaxRadius_Slider->set_float_limits(0, 0.5);
	new GLUI_StaticText(bubble_panel, "Core Affected Radius");
	GLUI_Scrollbar* bubbleCoreRadius_Slider = new GLUI_Scrollbar
		(bubble_panel, "Core Affected Radius", GLUI_SCROLL_HORIZONTAL, &UI_bubbleCoreRadius);
	bubbleCoreRadius_Slider->set_float_limits(0, 0.1);

	/*** Camera ***/
	GLUI_Rollout* viewpoint_panel = new GLUI_Rollout(glui, "Camera", 0);
	viewpoint_panel->set_alignment(GLUI_ALIGN_LEFT);
	new GLUI_Button(viewpoint_panel, "Superior_Up", 1);
	new GLUI_Button(viewpoint_panel, "Superior_Down", 2);
	new GLUI_Button(viewpoint_panel, "Right", 3);
	new GLUI_Button(viewpoint_panel, "Left", 4);
	new GLUI_Button(viewpoint_panel, "Posterior", 5);
	new GLUI_Button(viewpoint_panel, "Anterior", 6);

	glui->set_main_gfx_window(main_window);
}