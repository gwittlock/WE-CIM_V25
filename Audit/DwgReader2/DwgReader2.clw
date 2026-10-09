; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDwgReader2Dlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DwgReader2.h"

ClassCount=3
Class1=CDwgReader2App
Class2=CDwgReader2Dlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DWGREADER2_DIALOG

[CLS:CDwgReader2App]
Type=0
HeaderFile=DwgReader2.h
ImplementationFile=DwgReader2.cpp
Filter=N

[CLS:CDwgReader2Dlg]
Type=0
HeaderFile=DwgReader2Dlg.h
ImplementationFile=DwgReader2Dlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LAYER_LIST

[CLS:CAboutDlg]
Type=0
HeaderFile=DwgReader2Dlg.h
ImplementationFile=DwgReader2Dlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DWGREADER2_DIALOG]
Type=1
Class=CDwgReader2Dlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_DWG_PATH,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_DWG_BROWSE,button,1342242816
Control6=IDC_TEST,button,1342242819
Control7=IDC_ANALYZE,button,1342242819
Control8=IDC_LAYER_LIST,listbox,1352728841

