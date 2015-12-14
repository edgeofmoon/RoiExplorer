#pragma once

#include "SimpleSignal.h"

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
	static void AddUIs();

	static int main_window;

	/*** Event Signals ***/
	static Signal1< int > Signal_Event;
	static void SignalEvent(int event){
		Signal_Event(event);
	}

	/*** Camera Signals ***/
	static Signal1< int > Signal_SetViewAngle;
	static void SignalSetViewAngle(int viewAngle){
		Signal_SetViewAngle(viewAngle);
	}

	/*** Component Signals ***/
	static int mComponentToggle[4];
	static Signal1< int > Signal_EnableComponent;
	static Signal1< int > Signal_DisableComponent;
	static void SignalToggleComponents(int component){
		if (mComponentToggle[component]){
			Signal_EnableComponent(component);
		}
		else {
			Signal_DisableComponent(component);
		}
	}

	/*** Rendering Signals ***/
	static float mValuefs[2];
	static Signal2< int, float > Signal_SetValuef;
	static void SignalSetValues(int idx){
		Signal_SetValuef(idx, mValuefs[idx]);
	}
};

