#pragma once

extern int windowWidth, windowHeight;
extern void myGlutDisplay();
extern void myGlutReshape(int w, int h);
extern void myGlutKeyboard(unsigned char Key, int x, int y);
extern void myGlutMouse(int button, int state, int x, int y);
extern void myGlutMouseWheel(int button, int dir, int x, int y);
extern void myGlutMotion(int x, int y);
extern void myGlutPassiveMotion(int x, int y);

class MyUiPanel
{
public:
	MyUiPanel();
	~MyUiPanel();

	static void GetViewport(int &x, int &y, int &w, int &h);
	static void InitGL(int w,int h);
	void AddUIs();

	static int main_window;

	/*** Magnifier ***/
	float UI_magnifier_radius;
	float UI_magnifier_scale;
	float UI_fishEye_focusRadiusRatio;

	/*** Components ***/
	int UI_drawTracks;
	int UI_drawTrackVol;
	int UI_drawMesh;
	int UI_drawContour;
	int UI_2DLegend;
	int UI_hoverOn3D;
	int UI_hoverOn2D;

	/*** Track Vis ***/
	float UI_trackDensityFilter;
	float UI_sampleRate;
	float UI_decayFactor;
	float UI_transparencyExponent;

	/*** Label ***/
	float UI_labelDrawRatio;
	int UI_labelSolid;

	/*** Regions ***/
	float UI_roiRenderRatio;
	float UI_linkDrawThreshold;

	/*** Bubble Sets ***/
	float UI_bubbleThreshold;
	float UI_bubbleMaxRadius;
	float UI_bubbleCoreRadius;
};

