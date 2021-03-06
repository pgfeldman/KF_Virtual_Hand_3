// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef handControl3_UI_h
#define handControl3_UI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "UI_cmd.h"
#include "KF_Hand_Window.h"
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>

class HandControlUI {
public:
  HandControlUI();
  Fl_Double_Window *mainWindow;
  KF_Hand_Window *handView;
  Fl_Value_Slider *greenSensitivity;
private:
  inline void cb_greenSensitivity_i(Fl_Value_Slider*, void*);
  static void cb_greenSensitivity(Fl_Value_Slider*, void*);
public:
  Fl_Value_Slider *redSensitivity;
private:
  inline void cb_redSensitivity_i(Fl_Value_Slider*, void*);
  static void cb_redSensitivity(Fl_Value_Slider*, void*);
public:
  Fl_Group *setup;
  Fl_Input *researcherNameInput;
  Fl_Input *subjectNameInput;
  Fl_Input *subjectAgeInput;
  Fl_Choice *subjectGender;
  static Fl_Menu_Item menu_subjectGender[];
  static Fl_Menu_Item *male;
  static Fl_Menu_Item *female;
  Fl_Value_Input *seedInput;
  Fl_Value_Input *sessionInput;
  Fl_Choice *testTypeCombo;
private:
  inline void cb_testTypeCombo_i(Fl_Choice*, void*);
  static void cb_testTypeCombo(Fl_Choice*, void*);
  static Fl_Menu_Item menu_testTypeCombo[];
public:
  static Fl_Menu_Item *noFeedback;
  static Fl_Menu_Item *hapticOnly;
  static Fl_Menu_Item *tactorOnly;
  static Fl_Menu_Item *hapticAndTactor;
  static Fl_Menu_Item *hapticAndTactorAndNone;
  static Fl_Menu_Item menu_[];
  static Fl_Menu_Item *saveSetupFile;
private:
  inline void cb_saveSetupFile_i(Fl_Menu_*, void*);
  static void cb_saveSetupFile(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *openFile;
private:
  inline void cb_openFile_i(Fl_Menu_*, void*);
  static void cb_openFile(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *singleSoundChoice;
private:
  inline void cb_singleSoundChoice_i(Fl_Menu_*, void*);
  static void cb_singleSoundChoice(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *closeAll;
private:
  inline void cb_closeAll_i(Fl_Menu_*, void*);
  static void cb_closeAll(Fl_Menu_*, void*);
public:
  Fl_Output *curOpenLoopOutput;
  Fl_Output *curHapticOutput;
  Fl_Output *curTactorOutput;
  Fl_Output *curHapticTactorOutput;
  Fl_Choice *soundChoice;
  Fl_Button *startButton;
private:
  inline void cb_startButton_i(Fl_Button*, void*);
  static void cb_startButton(Fl_Button*, void*);
public:
  Fl_Button *nextButton;
private:
  inline void cb_nextButton_i(Fl_Button*, void*);
  static void cb_nextButton(Fl_Button*, void*);
public:
  Fl_Output *testDirections;
  Fl_Group *constrainTargetCheck;
  Fl_Value_Input *xmin;
  Fl_Value_Input *ymin;
  Fl_Value_Input *zmin;
  Fl_Value_Input *xmax;
  Fl_Value_Input *ymax;
  Fl_Value_Input *zmax;
  Fl_Value_Input *targetSphereMin;
  Fl_Value_Input *targetSphereMax;
  Fl_Value_Input *goalBoxSize;
  Fl_Button *setEnvButton;
private:
  inline void cb_setEnvButton_i(Fl_Button*, void*);
  static void cb_setEnvButton(Fl_Button*, void*);
public:
  Fl_Check_Button *constrainTargetsCheck;
private:
  inline void cb_constrainTargetsCheck_i(Fl_Check_Button*, void*);
  static void cb_constrainTargetsCheck(Fl_Check_Button*, void*);
public:
  void show(int argc, char **argv);
private:
  UI_cmd *cmd; 
public:
  void setCmdPtr(UI_cmd *pCmd);
};
#endif
