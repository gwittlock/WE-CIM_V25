; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEWMSettingsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "EWMSettings.h"

ClassCount=3
Class1=CEWMSettingsApp
Class2=CEWMSettingsDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_EWMSETTINGS_DIALOG

[CLS:CEWMSettingsApp]
Type=0
HeaderFile=EWMSettings.h
ImplementationFile=EWMSettings.cpp
Filter=N

[CLS:CEWMSettingsDlg]
Type=0
HeaderFile=EWMSettingsDlg.h
ImplementationFile=EWMSettingsDlg.cpp
Filter=D
LastObject=ID_BUTTON_UPDATE
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=EWMSettingsDlg.h
ImplementationFile=EWMSettingsDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_EWMSETTINGS_DIALOG]
Type=1
Class=CEWMSettingsDlg
ControlCount=13
Control1=ID_BUTTON_UPDATE,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CHECK_ENABLE,button,1342242819
Control4=IDC_CHECK_FILTER,button,1342242819
Control5=IDC_CHECK_WARNINGS,button,1342242819
Control6=IDC_CHECK_USER,button,1342242819
Control7=IDC_CHECK_DIAGNOSTICS,button,1342242819
Control8=IDC_CHECK_PORTAL,button,1342242819
Control9=IDC_CHECK_UI,button,1342242819
Control10=IDC_CHECK_DAO,button,1342242819
Control11=IDC_CHECK_FILE,button,1342242819
Control12=IDC_CHECK_NESTING,button,1342242819
Control13=IDC_CHECK_SEEDS,button,1342242819

