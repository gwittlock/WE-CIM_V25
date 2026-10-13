Module Lcad

  Public Const LC_FALSE = 0
  Public Const LC_TRUE = 1
  Public Const LC_WS_HSCROLL = 1
  Public Const LC_WS_VSCROLL = 2
  Public Const LC_WS_BORDER = 4
  Public Const LC_WS_CLIENTEDGE = 8
  Public Const LC_WS_SUNKEN = 8
  Public Const LC_WS_STATICEDGE = 16
  Public Const LC_WS_VIEWTABS = 256
  Public Const LC_WS_RULERS = 512
  Public Const LC_WS_DEFAULT = 3
  Public Const LC_WS2_PROPWND = 1
  Public Const LC_WS2_RULERS = 2
  Public Const LC_WS2_SCROLLS = 4
  Public Const LC_WS2_STATBAR = 8
  Public Const LC_WS2_COORDS = 16
  Public Const LC_WS2_TOOLBAR = 32
  Public Const LC_WS2_DEFAULT = 47
  Public Const LC_MRU_IMAGE_W = 246
  Public Const LC_MRU_IMAGE_H = 190
  Public Const LC_PS_SOLID = 0
  Public Const LC_PS_DASH = 1
  Public Const LC_PS_DOT = 2
  Public Const LC_PS_DASHDOT = 3
  Public Const LC_PS_DASHDOTDOT = 4
  Public Const LC_LW_THIN = 0
  Public Const LC_LW_REAL = 1
  Public Const LC_LW_PIXEL = 2
  Public Const LC_FILL_SOLID = 0
  Public Const LC_FILL_BDIAGONAL = 1
  Public Const LC_FILL_CROSS = 2
  Public Const LC_FILL_DIAGCROSS = 3
  Public Const LC_FILL_FDIAGONAL = 4
  Public Const LC_FILL_HORIZONTAL = 5
  Public Const LC_FILL_VERTICAL = 6
  Public Const LC_FILL_NONE = 100
  Public Const LC_FILL_LINES = 101
  Public Const LC_FILL_HATCH = 102
  Public Const LC_IMGRES_BOX = 0
  Public Const LC_IMGRES_BILINEAR = 1
  Public Const LC_IMGRES_BSPLINE = 2
  Public Const LC_IMGRES_BICUBIC = 3
  Public Const LC_IMGRES_CATMULLROM = 4
  Public Const LC_IMGRES_LANCZOS3 = 5
  Public Const LC_GRIP_POINT = 0
  Public Const LC_GRIP_CENROT = 1
  Public Const LC_GRIP_ANGLE = 2
  Public Const LC_GRIP_ANGLE2 = 3
  Public Const LC_GRIP_BEZIER = 4
  Public Const LC_GRIP_BEZIER0 = 5
  Public Const LC_GRIP_ARCRAD = 6
  Public Const LC_BARTYPE_CODE39 = 0
  Public Const LC_BARTYPE_CODE93 = 1
  Public Const LC_BARTYPE_CODE128 = 6
  Public Const LC_BARTYPE_EAN13 = 7
  Public Const LC_BARTYPE_ITF = 8
  Public Const LC_BARTYPE_EAN8 = 9
  Public Const LC_BARTYPE_QR = 21
  Public Const LC_BARTYPE_MQR = 22
  Public Const LC_BARTYPE_DMATRIX = 23
  Public Const LC_BARTYPE_DM = 23
  Public Const LC_BARTYPE_DMATRIXR = 24
  Public Const LC_BARTYPE_DMR = 24
  Public Const LC_BARC_QRECL_L = 0
  Public Const LC_BARC_QRECL_M = 1
  Public Const LC_BARC_QRECL_Q = 2
  Public Const LC_BARC_QRECL_H = 3
  Public Const LC_LEADER_TCENTER = 0
  Public Const LC_LEADER_TLEFT = 1
  Public Const LC_LEADER_TRIGHT = 2
  Public Const LC_LEADER_VERT = 1
  Public Const LC_LEADER_CORNER = 2
  Public Const LC_VPL_CLEAR = 1
  Public Const LC_VPL_ADD = 2
  Public Const LC_VPL_DELETE = 3
  Public Const LC_VPL_PROP_BEGIN = 4
  Public Const LC_VPL_PROP_END = 5
  Public Const LC_RAD_TO_DEG = 57.2957795130823208768
  Public Const LC_DEG_TO_RAD = 0.01745329251994329577
  Public Const LC_PI = 3.14159265358979323846
  Public Const LC_PI2 = 1.57079632679489661923
  Public Const LC_PI4 = 0.78539816339744830962
  Public Const LC_2PI = 6.28318530717958647692
  Public Const LC_DEG1 = 0.01745329251994329577
  Public Const LC_DEG2 = 0.03490658503988659154
  Public Const LC_DEG3 = 0.05235987755982988731
  Public Const LC_DEG4 = 0.06981317007977318308
  Public Const LC_DEG5 = 0.08726646259971647885
  Public Const LC_DEG6 = 0.10471975511965977462
  Public Const LC_DEG7 = 0.12217304763960307038
  Public Const LC_DEG8 = 0.13962634015954636615
  Public Const LC_DEG9 = 0.15707963267948966192
  Public Const LC_DEG10 = 0.17453292519943295769
  Public Const LC_DEG20 = 0.34906585039886591538
  Public Const LC_DEG30 = 0.52359877559829887308
  Public Const LC_DEG40 = 0.69813170079773183077
  Public Const LC_DEG45 = 0.78539816339744830962
  Public Const LC_DEG50 = 0.87266462599716478846
  Public Const LC_DEG60 = 1.04719755119659774615
  Public Const LC_DEG70 = 1.22173047639603070385
  Public Const LC_DEG80 = 1.39626340159546366154
  Public Const LC_DEG90 = 1.57079632679489661923
  Public Const LC_DEG180 = 3.14159265358979323846
  Public Const LC_DEG270 = 4.71238898038468985769
  Public Const LC_DEG360 = 6.28318530717958647692
  Public Const LC_INSUNIT_UNDEFINED = 0
  Public Const LC_INSUNIT_INCHES = 1
  Public Const LC_INSUNIT_FEET = 2
  Public Const LC_INSUNIT_MILES = 3
  Public Const LC_INSUNIT_MILLIMETERS = 4
  Public Const LC_INSUNIT_CENTIMETERS = 5
  Public Const LC_INSUNIT_METERS = 6
  Public Const LC_INSUNIT_KILOMETERS = 7
  Public Const LC_INSUNIT_MICROINCHES = 8
  Public Const LC_INSUNIT_MILS = 9
  Public Const LC_INSUNIT_YARDS = 10
  Public Const LC_INSUNIT_ANGSTROMS = 11
  Public Const LC_INSUNIT_NANOMETERS = 12
  Public Const LC_INSUNIT_MICRONS = 13
  Public Const LC_INSUNIT_DECIMETERS = 14
  Public Const LC_INSUNIT_DEKAMETERS = 15
  Public Const LC_INSUNIT_HECTOMETERS = 16
  Public Const LC_INSUNIT_GIGAMETERS = 17
  Public Const LC_INSUNIT_ASTRONOMICAL = 18
  Public Const LC_INSUNIT_LIGHTYEARS = 19
  Public Const LC_INSUNIT_PARSECS = 20
  Public Const LC_LUNIT_SCIEN = 1
  Public Const LC_LUNIT_DECIM = 2
  Public Const LC_LUNIT_ENGIN = 3
  Public Const LC_LUNIT_ARCHI = 4
  Public Const LC_LUNIT_FRACT = 5
  Public Const LC_AUNIT_DEGREE = 0
  Public Const LC_AUNIT_DMS = 1
  Public Const LC_AUNIT_GRAD = 2
  Public Const LC_AUNIT_RADIAN = 3
  Public Const LC_AUNIT_SURVEY = 4
  Public Const LC_ANGLE_DEGREE = 0
  Public Const LC_ANGLE_DMS = 1
  Public Const LC_ANGLE_GRAD = 2
  Public Const LC_ANGLE_RADIAN = 3
  Public Const LC_ANGLE_SURVEY = 4
  Public Const LC_PAPER_CUSTOM = 0
  Public Const LC_PAPER_USER = 0
  Public Const LC_PAPER_A0 = 1
  Public Const LC_PAPER_A1 = 2
  Public Const LC_PAPER_A2 = 3
  Public Const LC_PAPER_A3 = 4
  Public Const LC_PAPER_A4 = 5
  Public Const LC_PAPER_A5 = 6
  Public Const LC_PAPER_A6 = 7
  Public Const LC_PAPER_B0 = 11
  Public Const LC_PAPER_B1 = 12
  Public Const LC_PAPER_B2 = 13
  Public Const LC_PAPER_B3 = 14
  Public Const LC_PAPER_B4 = 15
  Public Const LC_PAPER_B5 = 16
  Public Const LC_PAPER_B6 = 17
  Public Const LC_PAPER_C0 = 21
  Public Const LC_PAPER_C1 = 22
  Public Const LC_PAPER_C2 = 23
  Public Const LC_PAPER_C3 = 24
  Public Const LC_PAPER_C4 = 25
  Public Const LC_PAPER_C5 = 26
  Public Const LC_PAPER_C6 = 27
  Public Const LC_PAPER_ANSI_A = 31
  Public Const LC_PAPER_ANSI_B = 32
  Public Const LC_PAPER_ANSI_C = 33
  Public Const LC_PAPER_ANSI_D = 34
  Public Const LC_PAPER_ANSI_E = 35
  Public Const LC_PAPER_LETTER = 36
  Public Const LC_PAPER_LEGAL = 37
  Public Const LC_PAPER_EXECUTIVE = 38
  Public Const LC_PAPER_LEDGER = 39
  Public Const LC_PAPER_PORTRAIT = 0
  Public Const LC_PAPER_BOOK = 0
  Public Const LC_PAPER_LANDSCAPE = 1
  Public Const LC_PAPER_ALBUM = 1
  Public Const LC_TA_LEFBOT = 0
  Public Const LC_TA_CENBOT = 1
  Public Const LC_TA_RIGBOT = 2
  Public Const LC_TA_LEFCEN = 3
  Public Const LC_TA_CENTER = 4
  Public Const LC_TA_RIGCEN = 5
  Public Const LC_TA_LEFTOP = 6
  Public Const LC_TA_CENTOP = 7
  Public Const LC_TA_RIGTOP = 8
  Public Const LC_TA_ALIGNED = 11
  Public Const LC_TA_FIT = 12
  Public Const LC_TEXT_BACKWARD = 2
  Public Const LC_TEXT_UPDOWN = 4
  Public Const LC_ATA_LEFT = 0
  Public Const LC_ATA_CENTER = 1
  Public Const LC_ATA_RIGHT = 2
  Public Const LC_BTA_LEFT = 0
  Public Const LC_BTA_CENTER = 1
  Public Const LC_BTA_RIGHT = 2
  Public Const LC_PLFIT_BULGE = 0
  Public Const LC_PLFIT_NONE = 0
  Public Const LC_PLFIT_QUAD = 5
  Public Const LC_PLFIT_CUBIC = 6
  Public Const LC_PLFIT_BEZIER = 7
  Public Const LC_PLFIT_SPLINE = 99
  Public Const LC_PLFIT_ROUND = 101
  Public Const LC_PLFIT_LINQUAD = 102
  Public Const LC_POINT_PIXEL = 0
  Public Const LC_POINT_NONE = 1
  Public Const LC_POINT_PLUS = 2
  Public Const LC_POINT_X = 3
  Public Const LC_POINT_TICK = 4
  Public Const LC_POINT_CIRCLE = 32
  Public Const LC_POINT_SQUARE = 64
  Public Const LC_POINT_RHOMB = 128
  Public Const LC_POINT_FILLED = 256
  Public Const LC_POINT_BEAM0 = 10000
  Public Const LC_POINT_BEAM1 = 10001
  Public Const LC_BLK_ENT_RETAIN = 0
  Public Const LC_BLK_ENT_CONVERT = 1
  Public Const LC_BLK_ENT_DELETE = 2
  Public Const LC_LBUTTON = 1
  Public Const LC_RBUTTON = 2
  Public Const LC_MBUTTON = 4
  Public Const LC_SHIFT = 1
  Public Const LC_CTRL = 2
  Public Const LC_ALT = 4
  Public Const LC_KBD_QWERTY = 0
  Public Const LC_KBD_AZERTY = 1
  Public Const LC_KBD_QWERTZ = 2
  Public Const LC_CURSOR_NULL = 0
  Public Const LC_CURSOR_ARROW = 1
  Public Const LC_CURSOR_CROSS = 2
  Public Const LC_CURSOR_HAND = 3
  Public Const LC_CURSOR_HELP = 4
  Public Const LC_CURSOR_NO = 5
  Public Const LC_CURSOR_WAIT = 6
  Public Const LC_CURSOR_PAN1 = 7
  Public Const LC_CURSOR_PAN2 = 8
  Public Const LC_EXP_OUTLINE = 1
  Public Const LC_EXP_HATCH = 2
  Public Const LC_EXP_ALL = 3
  Public Const LC_EXP_MAXRESOL = 4
  Public Const LC_DRWEXP_PLINES = 0
  Public Const LC_DRWEXP_LINES = 1
  Public Const LC_DRWEXP_LARCS = 2
  Public Const LC_DRWEXP_LCA = 3
  Public Const LC_HELP_DG_PRINT = 1
  Public Const LC_HELP_DG_RASTER = 2
  Public Const LC_HELP_DG_GBR_APERS = 4
  Public Const LC_HELP_DG_GBR_BITMAP = 5
  Public Const LC_HELP_DG_COLOR = 6
  Public Const LC_HELP_DG_LAYERS = 7
  Public Const LC_HELP_DG_SELLTYPE = 8
  Public Const LC_HELP_DG_LINETYPES = 9
  Public Const LC_HELP_DG_LOADLINETYPE = 10
  Public Const LC_HELP_DG_TEXTSTYLES = 11
  Public Const LC_HELP_DG_PNTSTYLES = 12
  Public Const LC_HELP_DG_DIMSTYLES = 13
  Public Const LC_HELP_DG_HATSTYLES = 14
  Public Const LC_HELP_DG_SELBLOCK = 15
  Public Const LC_HELP_DG_SELTSTYLE = 16
  Public Const LC_HELP_DG_SELPTYPE = 17
  Public Const LC_HELP_DG_CREBLOCK = 18
  Public Const LC_HELP_DG_BLOCKS = 19
  Public Const LC_HELP_DG_LAYOUTS = 20
  Public Const LC_HELP_DG_PAGEPROP = 21
  Public Const LC_HELP_DG_IMAGES = 22
  Public Const LC_HELP_DG_IMAGEINSERT = 23
  Public Const LC_HELP_DG_SELFONT = 24
  Public Const LC_HELP_DG_TEXT = 25
  Public Const LC_HELP_DG_ARCTEXT = 26
  Public Const LC_HELP_DG_INPUTCOORD = 27
  Public Const LC_HELP_DG_WORKFIELD = 28
  Public Const LC_HELP_DG_CRBITMAP = 29
  Public Const LC_HELP_DG_INSERT = 30
  Public Const LC_HELP_DG_TSP = 31
  Public Const LC_HELP_DG_ARRAY = 32
  Public Const LC_HELP_DG_ARRAYARC = 33
  Public Const LC_HELP_DG_HATCH = 34
  Public Const LC_HELP_DG_GRID = 41
  Public Const LC_HELP_DG_PTRACK = 42
  Public Const LC_HELP_DG_OSNAP = 43
  Public Const LC_HELP_DG_KBMOVE = 44
  Public Const LC_HELP_DG_DRWPROPS = 45
  Public Const LC_HELP_DG_SYSPROPS = 46
  Public Const LC_HELP_DG_SELECTION = 47
  Public Const LC_UNDO_BEGIN = 0
  Public Const LC_UNDO_END = 1
  Public Const LC_ERR_OBJTYPE = 1
  Public Const LC_ERR_NOTAG = 2
  Public Const LC_ERR_USERCANCEL = 3
  Public Const LC_ERR_FILENAME = 4
  Public Const LC_ERR_FILELOAD = 5
  Public Const LC_ERR_FILESAVE = 6
  Public Const LC_ERR_UNRESBLOCKREF = 7
  Public Const LC_ERR_UNRESVIEWREF = 8
  Public Const LC_ERR_UNRESHATCH = 9
  Public Const LC_OBJ_LAYER = 1
  Public Const LC_OBJ_LINETYPE = 2
  Public Const LC_OBJ_TEXTSTYLE = 3
  Public Const LC_OBJ_DIMSTYLE = 4
  Public Const LC_OBJ_PNTSTYLE = 5
  Public Const LC_OBJ_IMAGE = 6
  Public Const LC_OBJ_MLSTYLE = 8
  Public Const LC_OBJ_FILLING = 9
  Public Const LC_OBJ_BLOCK = 10
  Public Const LC_OBJ_LAYOUT = 11
  Public Const LC_ENT_LINE = 101
  Public Const LC_ENT_POLYLINE = 102
  Public Const LC_ENT_CIRCLE = 103
  Public Const LC_ENT_ARC = 104
  Public Const LC_ENT_BLOCKREF = 105
  Public Const LC_ENT_POINT = 107
  Public Const LC_ENT_XLINE = 108
  Public Const LC_ENT_ELLIPSE = 109
  Public Const LC_ENT_TEXT = 110
  Public Const LC_ENT_TEXTWIN = 111
  Public Const LC_ENT_IMAGEREF = 115
  Public Const LC_ENT_VIEWPORT = 116
  Public Const LC_ENT_CLIPRECT = 117
  Public Const LC_ENT_RECT = 118
  Public Const LC_ENT_ATTRIB = 120
  Public Const LC_ENT_ECW = 121
  Public Const LC_ENT_MTEXT = 122
  Public Const LC_ENT_ARCTEXT = 123
  Public Const LC_ENT_HATCH = 124
  Public Const LC_ENT_MLINE = 127
  Public Const LC_ENT_DIMROT = 131
  Public Const LC_ENT_DIMLIN = 131
  Public Const LC_ENT_DIMALI = 132
  Public Const LC_ENT_DIMORD = 133
  Public Const LC_ENT_DIMRAD = 134
  Public Const LC_ENT_DIMDIA = 135
  Public Const LC_ENT_DIMANG = 136
  Public Const LC_ENT_LEADER = 137
  Public Const LC_ENT_RPLAN = 141
  Public Const LC_ENT_TIN = 142
  Public Const LC_ENT_BARCODE = 150
  Public Const LC_ENT_ARROW = 301
  Public Const LC_ENT_SPIRAL = 302
  Public Const LC_ENT_CAMERA = 303
  Public Const LC_ENT_CUSTOM = 1000
  Public Const LC_ENT_ALL = 32767
  Public Const LC_LWEIGHT_DEFAULT = -3
  Public Const LC_LWEIGHT_BYBLOCK = -2
  Public Const LC_LWEIGHT_BYLAYER = -1
  Public Const LC_LWIDTH_DEFAULT = -3
  Public Const LC_LWIDTH_BYBLOCK = -2
  Public Const LC_LWIDTH_BYLAYER = -1
  Public Const LC_COLOR_RGB = 0
  Public Const LC_COLOR_INDEX = 1
  Public Const LC_COLOR_RED = 1
  Public Const LC_COLOR_YELLOW = 2
  Public Const LC_COLOR_GREEN = 3
  Public Const LC_COLOR_CYAN = 4
  Public Const LC_COLOR_BLUE = 5
  Public Const LC_COLOR_MAGENTA = 6
  Public Const LC_COLOR_FOREGROUND = 7
  Public Const LC_COLOR_GRAY = 8
  Public Const LC_COLOR_LTGRAY = 9
  Public Const LC_COLOR_BYBLOCK = 0
  Public Const LC_COLOR_BYLAYER = 256
  Public Const LC_SELPEN_COLOR = 0
  Public Const LC_SELPEN_BYENT = 1
  Public Const LC_MLINE_TOP = 0
  Public Const LC_MLINE_MIDDLE = 1
  Public Const LC_MLINE_BOTTOM = 2
  Public Const LC_PLUG_IMPDRW = 2
  Public Const LC_PLUG_EXPDRW = 3
  Public Const LC_PLUG_IMGDIB = 4
  Public Const LC_FP_FLOAD = 0
  Public Const LC_FP_FSAVE = 1
  Public Const LC_FP_NITEMS = 2
  Public Const LC_FP_ITEM = 3
  Public Const LC_OSNAP_NULL = 0
  Public Const LC_OSNAP_NODE = 1
  Public Const LC_OSNAP_ENDPOINT = 2
  Public Const LC_OSNAP_MIDPOINT = 4
  Public Const LC_OSNAP_CENTER = 8
  Public Const LC_OSNAP_NEAREST = 16
  Public Const LC_OSNAP_INTER = 32
  Public Const LC_OSNAP_PERPEND = 64
  Public Const LC_OSNAP_TANGENT = 128
  Public Const LC_OSNAP_QUADRANT = 256
  Public Const LC_OSNAP_INSERT = 512
  Public Const LC_OSNAP_NONE = 1024
  Public Const LC_OSNAP_ALL = 1023
  Public Const LC_ATTRIB_HIDDEN = 1
  Public Const LC_ATTRIB_CONST = 2
  Public Const LC_ATTRIB_VERIFY = 4
  Public Const LC_ATTRIB_PRESET = 8
  Public Const LC_ATTRIB_LOCK = 16
  Public Const LC_ATTRIB_MTEXT = 32
  Public Const LC_BLOCK_OVERWRITENO = 0
  Public Const LC_BLOCK_OVERWRITEYES = 1
  Public Const LC_BLOCK_OVERWRITEDLG = 2
  Public Const LC_ARROW_CLOSEDF = 0
  Public Const LC_ARROW_CLOSEDB = 1
  Public Const LC_ARROW_CLOSED = 2
  Public Const LC_ARROW_DOT = 3
  Public Const LC_ARROW_ARCHTICK = 4
  Public Const LC_ARROW_OBLIQUE = 5
  Public Const LC_ARROW_OPEN = 6
  Public Const LC_ARROW_ORIGIN = 7
  Public Const LC_ARROW_ORIGIN2 = 8
  Public Const LC_ARROW_OPEN90 = 9
  Public Const LC_ARROW_OPEN30 = 10
  Public Const LC_ARROW_DOTSMALL = 11
  Public Const LC_ARROW_DOTB = 12
  Public Const LC_ARROW_DOTSMALLB = 13
  Public Const LC_ARROW_BOX = 14
  Public Const LC_ARROW_BOXF = 15
  Public Const LC_ARROW_DATUM = 16
  Public Const LC_ARROW_DATUMF = 17
  Public Const LC_ARROW_INTEGRAL = 18
  Public Const LC_ARROW_NONE = 19
  Public Const LC_CMD_FILESAVE = 1
  Public Const LC_CMD_FILESAVEAS = 2
  Public Const LC_CMD_PRINT = 3
  Public Const LC_CMD_RASTERIZE = 4
  Public Const LC_CMD_FILENEW = 5
  Public Const LC_CMD_FILEOPEN = 6
  Public Const LC_CMD_FILERECENT = 7
  Public Const LC_CMD_ZOOM_IN = 401
  Public Const LC_CMD_ZOOM_OUT = 402
  Public Const LC_CMD_ZOOM_EXT = 403
  Public Const LC_CMD_ZOOM_PREV = 405
  Public Const LC_CMD_PAN_LEFT = 407
  Public Const LC_CMD_PAN_RIGHT = 408
  Public Const LC_CMD_PAN_UP = 409
  Public Const LC_CMD_PAN_DOWN = 410
  Public Const LC_CMD_ZOOM_WIN = 421
  Public Const LC_CMD_ZOOM_RECT = 421
  Public Const LC_CMD_ZOOM_SEL = 422
  Public Const LC_CMD_PAGENEXT = 423
  Public Const LC_CMD_PAGEPREV = 424
  Public Const LC_CMD_PAGEMODEL = 425
  Public Const LC_CMD_PAGEVPORT = 426
  Public Const LC_CMD_VP_ACT = 427
  Public Const LC_CMD_VP_DEACT = 428
  Public Const LC_CMD_GRID = 101
  Public Const LC_CMD_OSNAP = 102
  Public Const LC_CMD_PTRACK = 103
  Public Const LC_CMD_SELOPTS = 104
  Public Const LC_CMD_QSELECT = 105
  Public Const LC_CMD_KBMOVE = 106
  Public Const LC_CMD_DIST = 111
  Public Const LC_CMD_AREA = 112
  Public Const LC_CMD_WORKFIELD = 113
  Public Const LC_CMD_WORKFIELDPRM = 114
  Public Const LC_CMD_JUMPLINES = 115
  Public Const LC_CMD_TSP2 = 116
  Public Const LC_CMD_HELP = 1001
  Public Const LC_CMD_RESET = 1003
  Public Const LC_CMD_POINT = 201
  Public Const LC_CMD_LINE = 202
  Public Const LC_LINE_SERIAL = 1
  Public Const LC_LINE_SEPARATE = 2
  Public Const LC_CMD_XLINE = 203
  Public Const LC_XLINE_BASE = 1
  Public Const LC_XLINE_ANG = 2
  Public Const LC_XLINE_SEP = 3
  Public Const LC_XLINE_RAY = 4
  Public Const LC_XLINE_XLINE = 5
  Public Const LC_CMD_POLYLINE = 205
  Public Const LC_CMD_PLINE = 205
  Public Const LC_CMD_RECT = 207
  Public Const LC_RECT_2P = 1
  Public Const LC_RECT_3P = 2
  Public Const LC_RECT_CSA = 3
  Public Const LC_CMD_CIRCLE = 208
  Public Const LC_CIRCLE_CR = 1
  Public Const LC_CIRCLE_2P = 2
  Public Const LC_CIRCLE_3P = 3
  Public Const LC_CMD_ARC = 209
  Public Const LC_ARC_SME = 1
  Public Const LC_ARC_SEM = 2
  Public Const LC_ARC_SEC = 3
  Public Const LC_ARC_SED = 4
  Public Const LC_ARC_CSE = 5
  Public Const LC_ARC_CONT = 6
  Public Const LC_CMD_ELLIPSE = 210
  Public Const LC_ELL_AX = 1
  Public Const LC_ELL_CEN = 2
  Public Const LC_ELL_RECT = 3
  Public Const LC_CMD_TEXT = 211
  Public Const LC_CMD_ATEXT = 212
  Public Const LC_CMD_ARCTEXT = 212
  Public Const LC_CMD_MTEXT = 213
  Public Const LC_CMD_INSERT = 214
  Public Const LC_CMD_BARCODE = 215
  Public Const LC_BARCODE_39 = 1
  Public Const LC_BARCODE_93 = 2
  Public Const LC_BARCODE_128 = 3
  Public Const LC_BARCODE_EAN13 = 4
  Public Const LC_BARCODE_EAN8 = 5
  Public Const LC_BARCODE_ITF = 6
  Public Const LC_BARCODE_MQR = 7
  Public Const LC_BARCODE_QR = 8
  Public Const LC_BARCODE_DMATRIX = 9
  Public Const LC_BARCODE_DM = 9
  Public Const LC_BARCODE_DMATRIXR = 10
  Public Const LC_BARCODE_DMR = 10
  Public Const LC_CMD_DIMLIN = 216
  Public Const LC_CMD_DIMALI = 217
  Public Const LC_CMD_DIMORD = 218
  Public Const LC_CMD_DIMRAD = 219
  Public Const LC_CMD_DIMDIA = 220
  Public Const LC_CMD_DIMANG = 221
  Public Const LC_CMD_LEADER = 222
  Public Const LC_CMD_ARROW = 223
  Public Const LC_CMD_SPIRAL = 224
  Public Const LC_CMD_HATCH = 225
  Public Const LC_CMD_ECW = 226
  Public Const LC_CMD_VPORT = 227
  Public Const LC_CMD_CLRECT = 231
  Public Const LC_CMD_CLRECT_DEL = 232
  Public Const LC_CMD_CLRECT_DIV = 233
  Public Const LC_CMD_CLRECT_CBCOPY = 234
  Public Const LC_CMD_CLRECT_BITMAP = 235
  Public Const LC_CMD_CLRECT_BITMAP2 = 236
  Public Const LC_CMD_CLRECT_HIDE = 237
  Public Const LC_CMD_SW_GRID = 1011
  Public Const LC_CMD_SW_GRIDSNAP = 1012
  Public Const LC_CMD_SW_OSNAP = 1014
  Public Const LC_CMD_SW_PTRACK = 1015
  Public Const LC_CMD_SW_POLAR = 1015
  Public Const LC_CMD_SW_ORTHO = 1016
  Public Const LC_CMD_CBCUT = 301
  Public Const LC_CMD_CBCOPY = 302
  Public Const LC_CMD_CBPASTE = 303
  Public Const LC_CMD_UNDO = 304
  Public Const LC_CMD_REDO = 305
  Public Const LC_CMD_COPY = 306
  Public Const LC_COPY_MOVE = 1
  Public Const LC_COPY_ROTATE = 2
  Public Const LC_COPY_SCALE = 3
  Public Const LC_COPY_MIRROR = 4
  Public Const LC_COPY_ARRRECT = 5
  Public Const LC_COPY_ARRCIRC = 6
  Public Const LC_CMD_ERASE = 307
  Public Const LC_CMD_MOVE = 308
  Public Const LC_CMD_ROTATE = 309
  Public Const LC_CMD_SCALE = 310
  Public Const LC_CMD_MIRROR = 311
  Public Const LC_CMD_EXPLODE = 312
  Public Const LC_CMD_JOIN = 314
  Public Const LC_CMD_DEP = 315
  Public Const LC_CMD_ORDER = 316
  Public Const LC_ORDER_FRONT = 1
  Public Const LC_ORDER_BACK = 2
  Public Const LC_ORDER_ABOVE = 3
  Public Const LC_ORDER_UNDER = 4
  Public Const LC_ORDER_SWAP = 5
  Public Const LC_ORDER_SEQUENCE = 6
  Public Const LC_CMD_ALIGN = 317
  Public Const LC_ALIGN_LEFT = 1
  Public Const LC_ALIGN_RIGHT = 2
  Public Const LC_ALIGN_TOP = 3
  Public Const LC_ALIGN_BOTTOM = 4
  Public Const LC_ALIGN_CENTER = 5
  Public Const LC_ALIGN_CENX = 6
  Public Const LC_ALIGN_CENY = 7
  Public Const LC_CMD_CLOSEBLOCK = 318
  Public Const LC_CMD_BASEPOINT = 319
  Public Const LC_CMD_TRIM = 320
  Public Const LC_CMD_EXTEND = 321
  Public Const LC_CMD_OFFSET = 322
  Public Const LC_CMD_OFFSET_POINT = 0
  Public Const LC_CMD_OFFSET_DIST = 1
  Public Const LC_CMD_BREAK = 323
  Public Const LC_CMD_STRETCH = 324
  Public Const LC_CMD_FILLET = 325
  Public Const LC_CMD_IMGRES = 343
  Public Const LC_CMD_IMGPOS = 344
  Public Const LC_CMD_LAYER = 501
  Public Const LC_CMD_LAYERORD = 502
  Public Const LC_CMD_COLOR = 503
  Public Const LC_CMD_FCOLOR = 504
  Public Const LC_CMD_LINETYPE = 505
  Public Const LC_CMD_TEXTSTYLE = 507
  Public Const LC_CMD_BLOCK = 508
  Public Const LC_CMD_CREBLOCK = 508
  Public Const LC_CMD_BLOCKS = 509
  Public Const LC_CMD_IMAGE = 510
  Public Const LC_CMD_PNTSTYLE = 511
  Public Const LC_CMD_DIMSTYLE = 512
  Public Const LC_CMD_MLSTYLE = 513
  Public Const LC_CMD_FILLING = 515
  Public Const LC_CMD_LAYOUT = 516
  Public Const LC_CMD_UNITS = 517
  Public Const LC_CMD_DRWPRM = 518
  Public Const LC_CMD_SYSPRM = 519
  Public Const LC_CMDBLOCK_EDIT = 32191
  Public Const LC_CMDBLOCK_ATT = 32192
  Public Const LC_CMD_GBR_APERTURES = 9001
  Public Const LC_CMD_GBR_BITMAP = 9002
  Public Const LC_CMD_CUSTOM = 30000
  Public Const LC_PROP_G_REGCODE = 1
  Public Const LC_PROP_G_VERSION = 2
  Public Const LC_PROP_G_MSGTITLE = 3
  Public Const LC_PROP_G_HELPFILE = 10
  Public Const LC_PROP_G_DIRDLL = 11
  Public Const LC_PROP_G_DIRFONTS = 13
  Public Const LC_PROP_G_DIRLNG = 14
  Public Const LC_PROP_G_DIRTPL = 16
  Public Const LC_PROP_G_DIRCFG = 17
  Public Const LC_PROP_G_SAVECFG = 18
  Public Const LC_PROP_G_ICON16 = 20
  Public Const LC_PROP_G_ICON32 = 21
  Public Const LC_PROP_G_RULERBMP = 22
  Public Const LC_PROP_G_DLGVAL = 23
  Public Const LC_PROP_G_STR = 24
  Public Const LC_PROP_G_PRNUSEBMP = 25
  Public Const LC_PROP_G_PRNBMPFILE = 26
  Public Const LC_PROP_G_PNTPIXSIZE = 27
  Public Const LC_PROP_G_GETDELENT = 28
  Public Const LC_PROP_G_SBARHEIGHT = 29
  Public Const LC_PROP_G_SOLIDBARC = 30
  Public Const LC_PROP_G_FILEPROGRESS = 31
  Public Const LC_PROP_G_GBRBMPFILE = 32
  Public Const LC_PROP_G_PICKADD = 51
  Public Const LC_PROP_G_PICKBYRECT = 52
  Public Const LC_PROP_G_PICKDRAG = 53
  Public Const LC_PROP_G_PICKBOXSIZE = 54
  Public Const LC_PROP_G_GRIPSIZE = 55
  Public Const LC_PROP_G_GRIPCOLORF = 56
  Public Const LC_PROP_G_GRIPCOLORB = 57
  Public Const LC_PROP_G_CAMERA_COUNT = 71
  Public Const LC_PROP_G_CAMERA_I = 72
  Public Const LC_PROP_G_CAMERA_NAME = 73
  Public Const LC_PROP_G_CAMERA_ON = 74
  Public Const LC_PROP_G_CAMERA_TIME = 75
  Public Const LC_PROP_G_CAMERA_WIDTH = 76
  Public Const LC_PROP_G_CAMERA_HEIGHT = 77
  Public Const LC_PROP_G_CAMERA_BITS = 78
  Public Const LC_PROP_G_CAMERA_BPROW = 79
  Public Const LC_PROP_G_WF_LEFT = 80
  Public Const LC_PROP_G_WF_BOTTOM = 81
  Public Const LC_PROP_G_WF_WIDTH = 82
  Public Const LC_PROP_G_WF_HEIGHT = 83
  Public Const LC_PROP_G_PTBUFNEWPTS = 131
  Public Const LC_PROP_G_PTBUFCLR = 132
  Public Const LC_PROP_G_MPGONCLR = 133
  Public Const LC_PROP_G_TEXT_ALIGN = 141
  Public Const LC_PROP_G_TEXT_H = 142
  Public Const LC_PROP_G_TEXT_WS = 143
  Public Const LC_PROP_G_TEXT_CSPACE = 144
  Public Const LC_PROP_G_TEXT_ANG = 145
  Public Const LC_PROP_G_TEXT_OBL = 146
  Public Const LC_PROP_G_TEXT_UPDOWN = 147
  Public Const LC_PROP_G_TEXT_BACK = 148
  Public Const LC_PROP_G_TEXT_FILL = 149
  Public Const LC_PROP_G_TEXT_BORDER = 150
  Public Const LC_PROP_G_BARC_CENTER = 161
  Public Const LC_PROP_G_BARC_ANGLE = 162
  Public Const LC_PROP_G_BARC_BELOW = 163
  Public Const LC_PROP_G_BARC_QZONE = 164
  Public Const LC_PROP_G_BARC_CHKSUM = 165
  Public Const LC_PROP_G_BARC_ECL = 166
  Public Const LC_PROP_G_BARC_INVERT = 167
  Public Const LC_PROP_G_BARC_WRATIO = 168
  Public Const LC_PROP_G_BARC_LINEW = 169
  Public Const LC_PROP_G_TOPO_MODE = 181
  Public Const LC_PROP_G_TOPO_PD = 182
  Public Const LC_PROP_G_TOPO_PS = 183
  Public Const LC_PROP_G_TOPO_SIZE = 184
  Public Const LC_PROP_G_TOPO_ZMIN = 185
  Public Const LC_PROP_G_TOPO_ZRANGE = 186
  Public Const LC_PROP_G_TOPO_ZMAX = 187
  Public Const LC_PROP_G_BEAMCOLOR0D = 191
  Public Const LC_PROP_G_BEAMCOLOR1D = 192
  Public Const LC_PROP_G_BEAMCOLOR2D = 193
  Public Const LC_PROP_G_BEAMCOLOR0U = 194
  Public Const LC_PROP_G_BEAMCOLOR1U = 195
  Public Const LC_PROP_G_BEAMCOLOR2U = 196
  Public Const LC_PROP_WND_ID = 301
  Public Const LC_PROP_WND_WIDTH = 302
  Public Const LC_PROP_WND_HEIGHT = 303
  Public Const LC_PROP_WND_PIXELSIZE = 304
  Public Const LC_PROP_WND_PICKBOXSIZE = 305
  Public Const LC_PROP_WND_CURSORX = 306
  Public Const LC_PROP_WND_CURX = 306
  Public Const LC_PROP_WND_CURSORY = 307
  Public Const LC_PROP_WND_CURY = 307
  Public Const LC_PROP_WND_CURLEF = 308
  Public Const LC_PROP_WND_CURBOT = 309
  Public Const LC_PROP_WND_CURRIG = 310
  Public Const LC_PROP_WND_CURTOP = 311
  Public Const LC_PROP_WND_XMIN = 312
  Public Const LC_PROP_WND_YMIN = 313
  Public Const LC_PROP_WND_XMAX = 314
  Public Const LC_PROP_WND_YMAX = 315
  Public Const LC_PROP_WND_XCEN = 316
  Public Const LC_PROP_WND_YCEN = 317
  Public Const LC_PROP_WND_DX = 318
  Public Const LC_PROP_WND_DY = 319
  Public Const LC_PROP_WND_RULERS = 320
  Public Const LC_PROP_WND_SELECT = 321
  Public Const LC_PROP_WND_SELBYRECT = 322
  Public Const LC_PROP_WND_VIEWBLOCK = 323
  Public Const LC_PROP_WND_BLOCK = 323
  Public Const LC_PROP_WND_DRW = 324
  Public Const LC_PROP_WND_HWND = 325
  Public Const LC_PROP_WND_HASFOCUS = 326
  Public Const LC_PROP_WND_WORKFIELD = 327
  Public Const LC_PROP_WND_CLRHIDE = 328
  Public Const LC_PROP_WND_JUMPLINES = 329
  Public Const LC_PROP_WND_COLORBG = 330
  Public Const LC_PROP_WND_COLORCURSOR = 331
  Public Const LC_PROP_WND_COLORFORE = 332
  Public Const LC_PROP_WND_COLORINFBG = 333
  Public Const LC_PROP_WND_COLORINFBORD = 334
  Public Const LC_PROP_WND_COLORINFTEXT = 335
  Public Const LC_PROP_WND_CURSORSYS = 338
  Public Const LC_PROP_WND_CURSORARROW = 338
  Public Const LC_PROP_WND_CURSORCROSS = 339
  Public Const LC_PROP_WND_CURSORSIZE = 340
  Public Const LC_PROP_WND_COORDS = 341
  Public Const LC_PROP_WND_LWMODE = 345
  Public Const LC_PROP_WND_LWSCALE = 346
  Public Const LC_PROP_WND_BREAKPOINTS = 347
  Public Const LC_PROP_WND_BREAKPTNUMS = 348
  Public Const LC_PROP_WND_ALPHABLEND = 350
  Public Const LC_PROP_WND_STDBLKFRAME = 351
  Public Const LC_PROP_WND_SIZE = 352
  Public Const LC_PROP_WND_DTIME = 353
  Public Const LC_PROP_WND_DRAWPAPER = 355
  Public Const LC_PROP_WND_FROZEN = 356
  Public Const LC_PROP_WND_COMMAND = 357
  Public Const LC_PROP_WND_CMD = 357
  Public Const LC_PROP_WND_CMDENT1 = 358
  Public Const LC_PROP_WND_OSNAP = 359
  Public Const LC_PROP_WND_GRIDSNAP = 370
  Public Const LC_PROP_WND_GRIDSHOW = 371
  Public Const LC_PROP_WND_GRIDDX = 372
  Public Const LC_PROP_WND_GRIDDY = 373
  Public Const LC_PROP_WND_GRIDX0 = 374
  Public Const LC_PROP_WND_GRIDY0 = 375
  Public Const LC_PROP_WND_GRIDBOLDX = 376
  Public Const LC_PROP_WND_GRIDBOLDY = 377
  Public Const LC_PROP_WND_GRIDCOLOR = 378
  Public Const LC_PROP_WND_GRIDDOTTED = 379
  Public Const LC_PROP_WND_GRIDCOLOR2 = 380
  Public Const LC_PROP_WND_GRIDDOTTED2 = 381
  Public Const LC_PROP_WND_PANSTEP = 390
  Public Const LC_PROP_WND_PANLW = 391
  Public Const LC_PROP_WND_PANIMAGE = 392
  Public Const LC_PROP_WND_PANFILL = 393
  Public Const LC_PROP_WND_PANPIXSZ = 394
  Public Const LC_PROP_WND_MEASCOLORPNT = 400
  Public Const LC_PROP_WND_MEASCOLORLINE = 401
  Public Const LC_PROP_WND_MEASLINESIZE = 402
  Public Const LC_PROP_WND_MEASFONTSIZE = 403
  Public Const LC_PROP_WND_MEASFILLAREA = 404
  Public Const LC_PROP_WND_GBRNLAYERS = 421
  Public Const LC_PROP_WND_GBRCLAYER = 422
  Public Const LC_PROP_WND_GBRFILENAME = 423
  Public Const LC_PROP_WND_XLINEANG = 424
  Public Const LC_PROP_FONT_FILENAME = 601
  Public Const LC_PROP_FONT_NAME = 602
  Public Const LC_PROP_FONT_LCF = 603
  Public Const LC_PROP_FONT_HEIGHT = 604
  Public Const LC_PROP_FONT_FILLED = 605
  Public Const LC_PROP_FONT_TTF = 606
  Public Const LC_PROP_FONT_NCHARS = 607
  Public Const LC_PROP_MPGON_XMIN = 631
  Public Const LC_PROP_MPGON_YMIN = 632
  Public Const LC_PROP_MPGON_XMAX = 633
  Public Const LC_PROP_MPGON_YMAX = 634
  Public Const LC_PROP_MPGON_XCEN = 635
  Public Const LC_PROP_MPGON_YCEN = 636
  Public Const LC_PROP_MPGON_W = 637
  Public Const LC_PROP_MPGON_H = 638
  Public Const LC_PROP_TIN_FILENAME = 1831
  Public Const LC_PROP_TIN_EMBEDDED = 1832
  Public Const LC_PROP_TIN_XMIN = 1835
  Public Const LC_PROP_TIN_XMAX = 1836
  Public Const LC_PROP_TIN_YMIN = 1837
  Public Const LC_PROP_TIN_YMAX = 1838
  Public Const LC_PROP_TIN_ZMIN = 1839
  Public Const LC_PROP_TIN_ZMAX = 1840
  Public Const LC_PROP_TIN_DX = 1841
  Public Const LC_PROP_TIN_DY = 1842
  Public Const LC_PROP_TIN_DZ = 1843
  Public Const LC_PROP_TIN_NPOINTS = 1844
  Public Const LC_PROP_TIN_NTRIANS = 1845
  Public Const LC_PROP_TIN_VIEWPT = 1851
  Public Const LC_PROP_TIN_VIEWPTN = 1852
  Public Const LC_PROP_TIN_VIEWPTI = 1853
  Public Const LC_PROP_TIN_VIEWPTZ = 1854
  Public Const LC_PROP_TIN_VIEWTR = 1855
  Public Const LC_PROP_TIN_VIEWTRF = 1856
  Public Const LC_PROP_TIN_VIEWTRI = 1857
  Public Const LC_PROP_TIN_VIEWTRV = 1858
  Public Const LC_PROP_TIN_VIEWISO = 1859
  Public Const LC_PROP_TIN_COLPNT = 1860
  Public Const LC_PROP_TIN_COLTR = 1861
  Public Const LC_PROP_TIN_COLTRI = 1862
  Public Const LC_PROP_TIN_COLTRV = 1863
  Public Const LC_PROP_TIN_COLISO = 1864
  Public Const LC_PROP_TIN_COLISOB = 1865
  Public Const LC_PROP_TIN_ISOSTEP = 1870
  Public Const LC_PROP_TIN_ISOBOLD = 1871
  Public Const LC_PROP_TIN_ISOBASE = 1872
  Public Const LC_PROP_GRID_XMIN = 1882
  Public Const LC_PROP_GRID_XMAX = 1883
  Public Const LC_PROP_GRID_YMIN = 1884
  Public Const LC_PROP_GRID_YMAX = 1885
  Public Const LC_PROP_GRID_W = 1886
  Public Const LC_PROP_GRID_H = 1887
  Public Const LC_PROP_GRID_NCELLX = 1888
  Public Const LC_PROP_GRID_NCELLY = 1889
  Public Const LC_PROP_CMD_OBJ = 2001
  Public Const LC_PROP_CMD_ID = 2002
  Public Const LC_PROP_CMD_STEP = 2003
  Public Const LC_PROP_CMD_LCWND = 2004
  Public Const LC_PROP_CMD_HWND = 2005
  Public Const LC_PROP_CMD_CURSORCROSS = 2006
  Public Const LC_PROP_DRW_UID = 3001
  Public Const LC_PROP_DRW_FILENAME = 3002
  Public Const LC_PROP_DRW_DESCR = 3003
  Public Const LC_PROP_DRW_COMMENT = 3003
  Public Const LC_PROP_DRW_READONLY = 3004
  Public Const LC_PROP_DRW_DIRTY = 3005
  Public Const LC_PROP_DRW_IDMAX = 3006
  Public Const LC_PROP_DRW_SYNCZOOM = 3011
  Public Const LC_PROP_DRW_LOCKSEL = 3012
  Public Const LC_PROP_DRW_HASALPHABLEND = 3013
  Public Const LC_PROP_DRW_COLORBACKM = 3021
  Public Const LC_PROP_DRW_COLORBACKP = 3022
  Public Const LC_PROP_DRW_COLORFOREM = 3023
  Public Const LC_PROP_DRW_COLORFOREP = 3024
  Public Const LC_PROP_DRW_COLORCURSORM = 3025
  Public Const LC_PROP_DRW_COLORCURSORP = 3026
  Public Const LC_PROP_DRW_COLORPAPER = 3027
  Public Const LC_PROP_DRW_LAYER = 3101
  Public Const LC_PROP_DRW_LINETYPE = 3102
  Public Const LC_PROP_DRW_TEXTSTYLE = 3103
  Public Const LC_PROP_DRW_DIMSTYLE = 3104
  Public Const LC_PROP_DRW_PNTSTYLE = 3105
  Public Const LC_PROP_DRW_MLSTYLE = 3106
  Public Const LC_PROP_DRW_BLOCK = 3107
  Public Const LC_PROP_DRW_VISBLOCK = 3108
  Public Const LC_PROP_DRW_FILLING = 3109
  Public Const LC_PROP_DRW_BARTYPE = 3110
  Public Const LC_PROP_DRW_LWMODE = 3111
  Public Const LC_PROP_DRW_LWSCALE = 3112
  Public Const LC_PROP_DRW_LWIDTH = 3113
  Public Const LC_PROP_DRW_LWDEFAULT = 3114
  Public Const LC_PROP_DRW_LTSCALE = 3115
  Public Const LC_PROP_DRW_COLOR = 3121
  Public Const LC_PROP_DRW_COLORBYLAYER = 3122
  Public Const LC_PROP_DRW_COLORBYBLOCK = 3123
  Public Const LC_PROP_DRW_COLORI = 3124
  Public Const LC_PROP_DRW_COLORT = 3125
  Public Const LC_PROP_DRW_FCOLOR = 3126
  Public Const LC_PROP_DRW_FCOLORBYLAYER = 3127
  Public Const LC_PROP_DRW_FCOLORBYBLOCK = 3128
  Public Const LC_PROP_DRW_FCOLORI = 3129
  Public Const LC_PROP_DRW_FCOLORT = 3130
  Public Const LC_PROP_DRW_RESOLARC = 3141
  Public Const LC_PROP_DRW_RESOLSPLINE = 3142
  Public Const LC_PROP_DRW_RESOLTEXT = 3143
  Public Const LC_PROP_DRW_EXPLODEARC = 3145
  Public Const LC_PROP_DRW_EXPPLINELA = 3146
  Public Const LC_PROP_DRW_ENABLEGRIPS = 3147
  Public Const LC_PROP_DRW_GRIPSLIMIT = 3148
  Public Const LC_PROP_DRW_GRIPCOLORF = 3149
  Public Const LC_PROP_DRW_GRIPCOLORB = 3150
  Public Const LC_PROP_DRW_SELPENMODE = 3151
  Public Const LC_PROP_DRW_SELCOLOR = 3152
  Public Const LC_PROP_DRW_SELCOLOR2 = 3153
  Public Const LC_PROP_DRW_SELBYLAYER = 3154
  Public Const LC_PROP_DRW_BLOCK_MODEL = 3160
  Public Const LC_PROP_DRW_LAYER_0 = 3161
  Public Const LC_PROP_DRW_LINETYPE_CONT = 3162
  Public Const LC_PROP_DRW_LINETYPE_BYLAY = 3163
  Public Const LC_PROP_DRW_LINETYPE_BYBLK = 3164
  Public Const LC_PROP_DRW_TEXTSTYLE_STD = 3165
  Public Const LC_PROP_DRW_TSTYLE_STD = 3165
  Public Const LC_PROP_DRW_PNTSTYLE_STD = 3166
  Public Const LC_PROP_DRW_DIMSTYLE_STD = 3167
  Public Const LC_PROP_DRW_MLSTYLE_STD = 3168
  Public Const LC_PROP_DRW_WF_LEFT = 3181
  Public Const LC_PROP_DRW_WF_BOTTOM = 3182
  Public Const LC_PROP_DRW_WF_WIDTH = 3183
  Public Const LC_PROP_DRW_WF_HEIGHT = 3184
  Public Const LC_PROP_DRW_WF_HIDE = 3185
  Public Const LC_PROP_DRW_WF_NUMCRECTS = 3186
  Public Const LC_PROP_DRW_WF_COLOR = 3187
  Public Const LC_PROP_DRW_WF_COLORCR = 3188
  Public Const LC_PROP_DRW_WF_TEXTSIZE = 3189
  Public Const LC_PROP_DRW_XDATASIZE = 3251
  Public Const LC_PROP_DRW_XDATA = 3252
  Public Const LC_PROP_DRW_INT0 = 3260
  Public Const LC_PROP_DRW_INT1 = 3261
  Public Const LC_PROP_DRW_INT2 = 3262
  Public Const LC_PROP_DRW_INT3 = 3263
  Public Const LC_PROP_DRW_INT4 = 3264
  Public Const LC_PROP_DRW_INT5 = 3265
  Public Const LC_PROP_DRW_INT6 = 3266
  Public Const LC_PROP_DRW_INT7 = 3267
  Public Const LC_PROP_DRW_INT8 = 3268
  Public Const LC_PROP_DRW_INT9 = 3269
  Public Const LC_PROP_DRW_FLOAT0 = 3270
  Public Const LC_PROP_DRW_FLOAT1 = 3271
  Public Const LC_PROP_DRW_FLOAT2 = 3272
  Public Const LC_PROP_DRW_FLOAT3 = 3273
  Public Const LC_PROP_DRW_FLOAT4 = 3274
  Public Const LC_PROP_DRW_FLOAT5 = 3275
  Public Const LC_PROP_DRW_FLOAT6 = 3276
  Public Const LC_PROP_DRW_FLOAT7 = 3277
  Public Const LC_PROP_DRW_FLOAT8 = 3278
  Public Const LC_PROP_DRW_FLOAT9 = 3279
  Public Const LC_PROP_DRW_STR0 = 3280
  Public Const LC_PROP_DRW_STR1 = 3281
  Public Const LC_PROP_DRW_STR2 = 3282
  Public Const LC_PROP_DRW_STR3 = 3283
  Public Const LC_PROP_DRW_STR4 = 3284
  Public Const LC_PROP_DRW_STR5 = 3285
  Public Const LC_PROP_DRW_STR6 = 3286
  Public Const LC_PROP_DRW_STR7 = 3287
  Public Const LC_PROP_DRW_STR8 = 3288
  Public Const LC_PROP_DRW_STR9 = 3289
  Public Const LC_PROP_TABLE_ID = 4001
  Public Const LC_PROP_TABLE_NAME = 4002
  Public Const LC_PROP_TABLE_DESC = 4003
  Public Const LC_PROP_TABLE_DESCR = 4003
  Public Const LC_PROP_TABLE_DRW = 4004
  Public Const LC_PROP_TABLE_DELETED = 4005
  Public Const LC_PROP_TABLE_ODHANDLE = 4010
  Public Const LC_PROP_TABLE_TYPE = 4011
  Public Const LC_PROP_TABLE_PRIORITY = 4012
  Public Const LC_PROP_TABLE_NREFS = 4013
  Public Const LC_PROP_TABLE_XDATASIZE = 4051
  Public Const LC_PROP_TABLE_XDATA = 4052
  Public Const LC_PROP_TABLE_XSTR = 4053
  Public Const LC_PROP_TABLE_INT0 = 4060
  Public Const LC_PROP_TABLE_INT1 = 4061
  Public Const LC_PROP_TABLE_INT2 = 4062
  Public Const LC_PROP_TABLE_INT3 = 4063
  Public Const LC_PROP_TABLE_INT4 = 4064
  Public Const LC_PROP_TABLE_FLOAT0 = 4070
  Public Const LC_PROP_TABLE_FLOAT1 = 4071
  Public Const LC_PROP_TABLE_FLOAT2 = 4072
  Public Const LC_PROP_TABLE_FLOAT3 = 4073
  Public Const LC_PROP_TABLE_FLOAT4 = 4074
  Public Const LC_PROP_LAYER_ID = 4001
  Public Const LC_PROP_LAYER_NAME = 4002
  Public Const LC_PROP_LAYER_DESC = 4003
  Public Const LC_PROP_LAYER_DESCR = 4003
  Public Const LC_PROP_LAYER_DRW = 4004
  Public Const LC_PROP_LAYER_DELETED = 4005
  Public Const LC_PROP_LAYER_COLOR = 4101
  Public Const LC_PROP_LAYER_COLORI = 4102
  Public Const LC_PROP_LAYER_COLORT = 4103
  Public Const LC_PROP_LAYER_FCOLOR = 4104
  Public Const LC_PROP_LAYER_FCOLORI = 4105
  Public Const LC_PROP_LAYER_FCOLORT = 4106
  Public Const LC_PROP_LAYER_LINETYPE = 4111
  Public Const LC_PROP_LAYER_LWEIGHT = 4112
  Public Const LC_PROP_LAYER_LWIDTH = 4112
  Public Const LC_PROP_LAYER_LOCKED = 4113
  Public Const LC_PROP_LAYER_NOPRINT = 4114
  Public Const LC_PROP_LAYER_VISIBLE = 4115
  Public Const LC_PROP_LAYER_0 = 4116
  Public Const LC_PROP_LAYER_NODLG = 4117
  Public Const LC_PROP_LINETYPE_ID = 4001
  Public Const LC_PROP_LINETYPE_NAME = 4002
  Public Const LC_PROP_LINETYPE_DESC = 4003
  Public Const LC_PROP_LINETYPE_DESCR = 4003
  Public Const LC_PROP_LINETYPE_DRW = 4004
  Public Const LC_PROP_LINETYPE_DELETED = 4005
  Public Const LC_PROP_LINETYPE_DATA = 4145
  Public Const LC_PROP_LINETYPE_SCALE = 4146
  Public Const LC_PROP_LINETYPE_CONTINUOUS = 4147
  Public Const LC_PROP_LINETYPE_BYLAYER = 4148
  Public Const LC_PROP_LINETYPE_BYBLOCK = 4149
  Public Const LC_PROP_LINETYPE_STD = 4150
  Public Const LC_PROP_LINETYPE_PATLEN = 4151
  Public Const LC_PROP_LINETYPE_NELEM = 4153
  Public Const LC_PROP_LINETYPE_IELEM = 4154
  Public Const LC_PROP_LTELEM_LEN = 4155
  Public Const LC_PROP_LTELEM_COMPLEX = 4156
  Public Const LC_PROP_LTELEM_SHAPE = 4157
  Public Const LC_PROP_LTELEM_TEXT = 4158
  Public Const LC_PROP_LTELEM_STYLE = 4159
  Public Const LC_PROP_LTELEM_FONTNAME = 4160
  Public Const LC_PROP_LTELEM_SCALE = 4161
  Public Const LC_PROP_LTELEM_ANGLE = 4162
  Public Const LC_PROP_LTELEM_ABSANGLE = 4163
  Public Const LC_PROP_LTELEM_X = 4164
  Public Const LC_PROP_LTELEM_Y = 4165
  Public Const LC_PROP_TSTYLE_ID = 4001
  Public Const LC_PROP_TSTYLE_NAME = 4002
  Public Const LC_PROP_TSTYLE_DESC = 4003
  Public Const LC_PROP_TSTYLE_DESCR = 4003
  Public Const LC_PROP_TSTYLE_DRW = 4004
  Public Const LC_PROP_TSTYLE_DELETED = 4005
  Public Const LC_PROP_TSTYLE_FONT = 4175
  Public Const LC_PROP_TSTYLE_HFONT = 4176
  Public Const LC_PROP_TSTYLE_HEIGHT = 4177
  Public Const LC_PROP_TSTYLE_WSCALE = 4178
  Public Const LC_PROP_TSTYLE_OBLIQUE = 4179
  Public Const LC_PROP_TSTYLE_ANGLE = 4180
  Public Const LC_PROP_TSTYLE_ALIGN = 4181
  Public Const LC_PROP_TSTYLE_UPDOWN = 4182
  Public Const LC_PROP_TSTYLE_BACKWARD = 4183
  Public Const LC_PROP_TSTYLE_LINESPACE = 4184
  Public Const LC_PROP_TSTYLE_CHARSPACE = 4185
  Public Const LC_PROP_TSTYLE_STANDARD = 4186
  Public Const LC_PROP_TSTYLE_SHAPES = 4187
  Public Const LC_PROP_TSTYLE_WINFONT = 4188
  Public Const LC_PROP_TSTYLE_SOLID = 4189
  Public Const LC_PROP_TSTYLE_BOLD = 4191
  Public Const LC_PROP_TSTYLE_ITALIC = 4192
  Public Const LC_PROP_TSTYLE_UNDERLINE = 4193
  Public Const LC_PROP_TSTYLE_STRIKEOUT = 4194
  Public Const LC_PROP_DIMST_ID = 4001
  Public Const LC_PROP_DIMST_NAME = 4002
  Public Const LC_PROP_DIMST_DESC = 4003
  Public Const LC_PROP_DIMST_DESCR = 4003
  Public Const LC_PROP_DIMST_DRW = 4004
  Public Const LC_PROP_DIMST_DELETED = 4005
  Public Const LC_PROP_DIMST_STANDARD = 4205
  Public Const LC_PROP_DIMST_ADEC = 4211
  Public Const LC_PROP_DIMST_ASZ = 4212
  Public Const LC_PROP_DIMST_AUNIT = 4213
  Public Const LC_PROP_DIMST_AZIN = 4214
  Public Const LC_PROP_DIMST_BLK1 = 4215
  Public Const LC_PROP_DIMST_BLK2 = 4216
  Public Const LC_PROP_DIMST_CEN = 4217
  Public Const LC_PROP_DIMST_CLRD = 4218
  Public Const LC_PROP_DIMST_CLRE = 4219
  Public Const LC_PROP_DIMST_CLRT = 4220
  Public Const LC_PROP_DIMST_DEC = 4221
  Public Const LC_PROP_DIMST_DSEP = 4222
  Public Const LC_PROP_DIMST_EXE = 4223
  Public Const LC_PROP_DIMST_EXO = 4224
  Public Const LC_PROP_DIMST_GAP = 4225
  Public Const LC_PROP_DIMST_LDRBLK = 4226
  Public Const LC_PROP_DIMST_LFAC = 4227
  Public Const LC_PROP_DIMST_LWD = 4228
  Public Const LC_PROP_DIMST_LWE = 4229
  Public Const LC_PROP_DIMST_POST = 4230
  Public Const LC_PROP_DIMST_RND = 4231
  Public Const LC_PROP_DIMST_SCALE = 4232
  Public Const LC_PROP_DIMST_TAD = 4233
  Public Const LC_PROP_DIMST_TIH = 4234
  Public Const LC_PROP_DIMST_TXT = 4235
  Public Const LC_PROP_DIMST_TXSTY = 4236
  Public Const LC_PROP_DIMST_TSTYLE = 4236
  Public Const LC_PROP_DIMST_LUNIT = 4237
  Public Const LC_PROP_DIMST_ZIN = 4238
  Public Const LC_PROP_PSTYLE_ID = 4001
  Public Const LC_PROP_PSTYLE_NAME = 4002
  Public Const LC_PROP_PSTYLE_DESC = 4003
  Public Const LC_PROP_PSTYLE_DESCR = 4003
  Public Const LC_PROP_PSTYLE_DRW = 4004
  Public Const LC_PROP_PSTYLE_DELETED = 4005
  Public Const LC_PROP_PSTYLE_STANDARD = 4265
  Public Const LC_PROP_PSTYLE_BLOCK = 4266
  Public Const LC_PROP_PSTYLE_BSCALE = 4267
  Public Const LC_PROP_PSTYLE_TSTYLE = 4268
  Public Const LC_PROP_PSTYLE_TH = 4269
  Public Const LC_PROP_PSTYLE_TWS = 4270
  Public Const LC_PROP_PSTYLE_PTMODE = 4271
  Public Const LC_PROP_PSTYLE_PTSIZE = 4272
  Public Const LC_PROP_PSTYLE_SNAP = 4273
  Public Const LC_PROP_PSTYLE_FIXED = 4274
  Public Const LC_PROP_MLSTYLE_ID = 4001
  Public Const LC_PROP_MLSTYLE_NAME = 4002
  Public Const LC_PROP_MLSTYLE_DESC = 4003
  Public Const LC_PROP_MLSTYLE_DESCR = 4003
  Public Const LC_PROP_MLSTYLE_DRW = 4004
  Public Const LC_PROP_MLSTYLE_DELETED = 4005
  Public Const LC_PROP_MLSTYLE_STANDARD = 4281
  Public Const LC_PROP_MLSTYLE_JOINTS = 4282
  Public Const LC_PROP_MLSTYLE_STARTLINE = 4283
  Public Const LC_PROP_MLSTYLE_STARTARC = 4284
  Public Const LC_PROP_MLSTYLE_ENDLINE = 4285
  Public Const LC_PROP_MLSTYLE_ENDARC = 4286
  Public Const LC_PROP_MLSTYLE_NLINES = 4287
  Public Const LC_PROP_MLSTYLE_ILINE = 4288
  Public Const LC_PROP_MLSTYLE_OFFSET = 4289
  Public Const LC_PROP_MLSTYLE_LTYPE = 4290
  Public Const LC_PROP_MLSTYLE_COLOR = 4291
  Public Const LC_PROP_IMAGE_ID = 4001
  Public Const LC_PROP_IMAGE_NAME = 4002
  Public Const LC_PROP_IMAGE_DESC = 4003
  Public Const LC_PROP_IMAGE_DESCR = 4003
  Public Const LC_PROP_IMAGE_DRW = 4004
  Public Const LC_PROP_IMAGE_DELETED = 4005
  Public Const LC_PROP_IMAGE_FILENAME = 4401
  Public Const LC_PROP_IMAGE_SIZE = 4402
  Public Const LC_PROP_IMAGE_WPIX = 4403
  Public Const LC_PROP_IMAGE_W = 4404
  Public Const LC_PROP_IMAGE_HPIX = 4405
  Public Const LC_PROP_IMAGE_H = 4406
  Public Const LC_PROP_IMAGE_CBIT = 4407
  Public Const LC_PROP_IMAGE_RGB = 4408
  Public Const LC_PROP_IMAGE_EMBEDDED = 4409
  Public Const LC_PROP_IMAGE_CREATED = 4410
  Public Const LC_PROP_IMAGE_COLORS = 4411
  Public Const LC_PROP_IMAGE_BITS = 4412
  Public Const LC_PROP_IMAGE_DIB = 4413
  Public Const LC_PROP_FILL_ID = 4001
  Public Const LC_PROP_FILL_NAME = 4002
  Public Const LC_PROP_FILL_DESC = 4003
  Public Const LC_PROP_FILL_DESCR = 4003
  Public Const LC_PROP_FILL_DRW = 4004
  Public Const LC_PROP_FILL_DELETED = 4005
  Public Const LC_PROP_FILL_NONE = 4501
  Public Const LC_PROP_FILL_SOLID = 4502
  Public Const LC_PROP_FILL_TYPE = 4503
  Public Const LC_PROP_BLOCK_ID = 4001
  Public Const LC_PROP_BLOCK_NAME = 4002
  Public Const LC_PROP_BLOCK_DESC = 4003
  Public Const LC_PROP_BLOCK_DESCR = 4003
  Public Const LC_PROP_BLOCK_DRW = 4004
  Public Const LC_PROP_BLOCK_DELETED = 4005
  Public Const LC_PROP_BLOCK_X = 4801
  Public Const LC_PROP_BLOCK_Y = 4802
  Public Const LC_PROP_BLOCK_UFSCALING = 4803
  Public Const LC_PROP_BLOCK_UNITS = 4804
  Public Const LC_PROP_BLOCK_UNITSCALE = 4810
  Public Const LC_PROP_BLOCK_MODEL = 4811
  Public Const LC_PROP_BLOCK_PAPER = 4812
  Public Const LC_PROP_BLOCK_LAYOUT = 4813
  Public Const LC_PROP_BLOCK_STANDARD = 4814
  Public Const LC_PROP_BLOCK_LAYOUTNAME = 4815
  Public Const LC_PROP_BLOCK_LAYOUTODHANDLE = 4816
  Public Const LC_PROP_BLOCK_LAYOUTORDER = 4817
  Public Const LC_PROP_BLOCK_HIDDEN = 4818
  Public Const LC_PROP_BLOCK_DIM = 4819
  Public Const LC_PROP_BLOCK_HATCH = 4820
  Public Const LC_PROP_BLOCK_NOBJ = 4821
  Public Const LC_PROP_BLOCK_NENTS = 4821
  Public Const LC_PROP_BLOCK_NSELOBJ = 4822
  Public Const LC_PROP_BLOCK_NSELENTS = 4822
  Public Const LC_PROP_BLOCK_ATTRIBS = 4827
  Public Const LC_PROP_BLOCK_XMIN = 4831
  Public Const LC_PROP_BLOCK_YMIN = 4832
  Public Const LC_PROP_BLOCK_XMAX = 4833
  Public Const LC_PROP_BLOCK_YMAX = 4834
  Public Const LC_PROP_BLOCK_XCEN = 4835
  Public Const LC_PROP_BLOCK_YCEN = 4836
  Public Const LC_PROP_BLOCK_DX = 4837
  Public Const LC_PROP_BLOCK_DY = 4838
  Public Const LC_PROP_BLOCK_VISLEF = 4839
  Public Const LC_PROP_BLOCK_VISBOT = 4840
  Public Const LC_PROP_BLOCK_VISRIG = 4841
  Public Const LC_PROP_BLOCK_VISTOP = 4842
  Public Const LC_PROP_BLOCK_SELXMIN = 4843
  Public Const LC_PROP_BLOCK_SELYMIN = 4844
  Public Const LC_PROP_BLOCK_SELXMAX = 4845
  Public Const LC_PROP_BLOCK_SELYMAX = 4846
  Public Const LC_PROP_BLOCK_SELXCEN = 4847
  Public Const LC_PROP_BLOCK_SELYCEN = 4848
  Public Const LC_PROP_PAPER_INCH = 4851
  Public Const LC_PROP_PAPER_X0 = 4852
  Public Const LC_PROP_PAPER_Y0 = 4853
  Public Const LC_PROP_PAPER_SIZE = 4854
  Public Const LC_PROP_PAPER_ORIENT = 4855
  Public Const LC_PROP_PAPER_W = 4856
  Public Const LC_PROP_PAPER_H = 4857
  Public Const LC_PROP_ENT_ID = 5001
  Public Const LC_PROP_ENT_KEY = 5002
  Public Const LC_PROP_ENT_COLOR = 5003
  Public Const LC_PROP_ENT_COLORI = 5004
  Public Const LC_PROP_ENT_COLORT = 5005
  Public Const LC_PROP_ENT_COLORBYLAYER = 5006
  Public Const LC_PROP_ENT_COLORBYBLOCK = 5007
  Public Const LC_PROP_ENT_FILLING = 5018
  Public Const LC_PROP_ENT_FILLED = 5019
  Public Const LC_PROP_ENT_SOLIDFILL = 5021
  Public Const LC_PROP_ENT_FCOLOR = 5008
  Public Const LC_PROP_ENT_FCOLORI = 5009
  Public Const LC_PROP_ENT_FCOLORT = 5010
  Public Const LC_PROP_ENT_FCOLORBYLAYER = 5011
  Public Const LC_PROP_ENT_FCOLORBYBLOCK = 5012
  Public Const LC_PROP_ENT_FALPHA = 5013
  Public Const LC_PROP_ENT_LAYER = 5014
  Public Const LC_PROP_ENT_LINETYPE = 5015
  Public Const LC_PROP_ENT_LTSCALE = 5016
  Public Const LC_PROP_ENT_LWEIGHT = 5017
  Public Const LC_PROP_ENT_LWIDTH = 5017
  Public Const LC_PROP_ENT_BLOCK = 5022
  Public Const LC_PROP_ENT_DRW = 5023
  Public Const LC_PROP_ENT_LOCKED = 5024
  Public Const LC_PROP_ENT_VISIBLE = 5025
  Public Const LC_PROP_ENT_HIDDEN = 5026
  Public Const LC_PROP_ENT_BLINK = 5027
  Public Const LC_PROP_ENT_TYPE = 5028
  Public Const LC_PROP_ENT_DELETED = 5029
  Public Const LC_PROP_ENT_IMMORTAL = 5030
  Public Const LC_PROP_ENT_SELECTED = 5031
  Public Const LC_PROP_ENT_PRIORITY = 5040
  Public Const LC_PROP_ENT_XDATASIZE = 5041
  Public Const LC_PROP_ENT_XDATA = 5042
  Public Const LC_PROP_ENT_XSTR = 5043
  Public Const LC_PROP_ENT_Z = 5045
  Public Const LC_PROP_ENT_XMIN = 5046
  Public Const LC_PROP_ENT_YMIN = 5047
  Public Const LC_PROP_ENT_XMAX = 5048
  Public Const LC_PROP_ENT_YMAX = 5049
  Public Const LC_PROP_POINT_STYLE = 5101
  Public Const LC_PROP_POINT_X = 5102
  Public Const LC_PROP_POINT_Y = 5103
  Public Const LC_PROP_POINT_SIZE = 5104
  Public Const LC_PROP_POINT_MODE = 5105
  Public Const LC_PROP_POINT_BANGLE = 5106
  Public Const LC_PROP_POINT_TDX = 5107
  Public Const LC_PROP_POINT_TDY = 5108
  Public Const LC_PROP_POINT_TANGLE = 5109
  Public Const LC_PROP_POINT_TEXT = 5110
  Public Const LC_PROP_POINT_TEXTLEN = 5111
  Public Const LC_PROP_LINE_X0 = 5121
  Public Const LC_PROP_LINE_Y0 = 5122
  Public Const LC_PROP_LINE_Z0 = 5123
  Public Const LC_PROP_LINE_X1 = 5124
  Public Const LC_PROP_LINE_Y1 = 5125
  Public Const LC_PROP_LINE_Z1 = 5126
  Public Const LC_PROP_LINE_DX = 5127
  Public Const LC_PROP_LINE_DY = 5128
  Public Const LC_PROP_LINE_DZ = 5129
  Public Const LC_PROP_LINE_ANG = 5130
  Public Const LC_PROP_LINE_LEN = 5131
  Public Const LC_PROP_XLINE_X0 = 5141
  Public Const LC_PROP_XLINE_Y0 = 5142
  Public Const LC_PROP_XLINE_ANG = 5144
  Public Const LC_PROP_XLINE_DIRX = 5145
  Public Const LC_PROP_XLINE_DIRY = 5146
  Public Const LC_PROP_XLINE_RAY = 5147
  Public Const LC_PROP_CIRCLE_X = 5201
  Public Const LC_PROP_CIRCLE_Y = 5202
  Public Const LC_PROP_CIRCLE_R = 5204
  Public Const LC_PROP_CIRCLE_RAD = 5204
  Public Const LC_PROP_CIRCLE_RADIUS = 5204
  Public Const LC_PROP_CIRCLE_DIAM = 5205
  Public Const LC_PROP_CIRCLE_LEN = 5206
  Public Const LC_PROP_CIRCLE_AREA = 5207
  Public Const LC_PROP_CIRCLE_ANG0 = 5208
  Public Const LC_PROP_CIRCLE_DIRCW = 5209
  Public Const LC_PROP_CIRCLE_RESOL = 5210
  Public Const LC_PROP_CIRC_X = 5201
  Public Const LC_PROP_CIRC_Y = 5202
  Public Const LC_PROP_CIRC_R = 5204
  Public Const LC_PROP_CIRC_RAD = 5204
  Public Const LC_PROP_CIRC_RADIUS = 5204
  Public Const LC_PROP_CIRC_DIAM = 5205
  Public Const LC_PROP_CIRC_LEN = 5206
  Public Const LC_PROP_CIRC_AREA = 5207
  Public Const LC_PROP_CIRC_ANG0 = 5208
  Public Const LC_PROP_CIRC_DIRCW = 5209
  Public Const LC_PROP_CIRC_RESOL = 5210
  Public Const LC_PROP_ARC_X = 5231
  Public Const LC_PROP_ARC_Y = 5232
  Public Const LC_PROP_ARC_R = 5234
  Public Const LC_PROP_ARC_RAD = 5234
  Public Const LC_PROP_ARC_RADIUS = 5234
  Public Const LC_PROP_ARC_ANG0 = 5235
  Public Const LC_PROP_ARC_ANGARC = 5236
  Public Const LC_PROP_ARC_ANGEND = 5237
  Public Const LC_PROP_ARC_X0 = 5238
  Public Const LC_PROP_ARC_Y0 = 5239
  Public Const LC_PROP_ARC_XEND = 5241
  Public Const LC_PROP_ARC_YEND = 5242
  Public Const LC_PROP_ARC_LEN = 5244
  Public Const LC_PROP_ARC_CHLEN = 5245
  Public Const LC_PROP_ARC_AREA = 5246
  Public Const LC_PROP_ARC_CCW = 5247
  Public Const LC_PROP_ARC_SECTOR = 5250
  Public Const LC_PROP_ARC_RESOL = 5251
  Public Const LC_PROP_ELL_X = 5261
  Public Const LC_PROP_ELL_Y = 5262
  Public Const LC_PROP_ELL_R1 = 5264
  Public Const LC_PROP_ELL_R2 = 5265
  Public Const LC_PROP_ELL_RATIO = 5266
  Public Const LC_PROP_ELL_ANGLE = 5267
  Public Const LC_PROP_ELL_ANG0 = 5268
  Public Const LC_PROP_ELL_ANGARC = 5269
  Public Const LC_PROP_ELL_ANGEND = 5270
  Public Const LC_PROP_ELL_X0 = 5271
  Public Const LC_PROP_ELL_Y0 = 5272
  Public Const LC_PROP_ELL_XEND = 5273
  Public Const LC_PROP_ELL_YEND = 5274
  Public Const LC_PROP_ELL_LEN = 5276
  Public Const LC_PROP_ELL_AREA = 5277
  Public Const LC_PROP_ELL_FULL = 5278
  Public Const LC_PROP_ELL_CCW = 5279
  Public Const LC_PROP_ELL_SECTOR = 5282
  Public Const LC_PROP_ELL_RESOL = 5283
  Public Const LC_PROP_PLINE_FIT = 5301
  Public Const LC_PROP_PLINE_CLOSED = 5302
  Public Const LC_PROP_PLINE_NVERS = 5303
  Public Const LC_PROP_PLINE_NPATHS = 5304
  Public Const LC_PROP_PLINE_WIDTH = 5305
  Public Const LC_PROP_PLINE_LEN = 5306
  Public Const LC_PROP_PLINE_AREA = 5307
  Public Const LC_PROP_PLINE_CW = 5308
  Public Const LC_PROP_PLINE_CCW = 5309
  Public Const LC_PROP_PLINE_Z = 5310
  Public Const LC_PROP_PLINE_CONSTZ = 5311
  Public Const LC_PROP_PLINE_RESOLA = 5312
  Public Const LC_PROP_PLINE_RESOLS = 5313
  Public Const LC_PROP_PLINE_RADIUS = 5314
  Public Const LC_PROP_PLINE_CHAMFER = 5315
  Public Const LC_PROP_PLINE_HASANG0 = 5316
  Public Const LC_PROP_PLINE_ANG0 = 5317
  Public Const LC_PROP_PLINE_HASANG2 = 5318
  Public Const LC_PROP_PLINE_ANG2 = 5319
  Public Const LC_PROP_MLINE_STYLE = 5351
  Public Const LC_PROP_MLINE_JUST = 5352
  Public Const LC_PROP_MLINE_SCALE = 5353
  Public Const LC_PROP_MLINE_NVERS = 5354
  Public Const LC_PROP_MLINE_CLOSED = 5355
  Public Const LC_PROP_MLINE_FIT = 5356
  Public Const LC_PROP_MLINE_LEN = 5357
  Public Const LC_PROP_MLINE_AREA = 5358
  Public Const LC_PROP_RECT_X = 5371
  Public Const LC_PROP_RECT_Y = 5372
  Public Const LC_PROP_RECT_W = 5374
  Public Const LC_PROP_RECT_H = 5375
  Public Const LC_PROP_RECT_ANGLE = 5376
  Public Const LC_PROP_RECT_R = 5377
  Public Const LC_PROP_RECT_RAD = 5377
  Public Const LC_PROP_RECT_RADIUS = 5377
  Public Const LC_PROP_RECT_CHAMFER = 5378
  Public Const LC_PROP_RECT_DIRCW = 5379
  Public Const LC_PROP_RECT_LEN = 5381
  Public Const LC_PROP_RECT_AREA = 5382
  Public Const LC_PROP_RECT_RESOL = 5383
  Public Const LC_PROP_CLRECT_ID = 5390
  Public Const LC_PROP_CLRECT_NAME = 5391
  Public Const LC_PROP_CLRECT_X = 5392
  Public Const LC_PROP_CLRECT_Y = 5393
  Public Const LC_PROP_CLRECT_W = 5394
  Public Const LC_PROP_CLRECT_H = 5395
  Public Const LC_PROP_CLRECT_ANGLE = 5396
  Public Const LC_PROP_CLRECT_LEN = 5398
  Public Const LC_PROP_CLRECT_AREA = 5399
  Public Const LC_PROP_TEXT_STYLE = 5401
  Public Const LC_PROP_TEXT_STR = 5402
  Public Const LC_PROP_TEXT_STRT = 5403
  Public Const LC_PROP_TEXT_LEN = 5404
  Public Const LC_PROP_TEXT_ALIGN = 5405
  Public Const LC_PROP_TEXT_H = 5406
  Public Const LC_PROP_TEXT_X = 5407
  Public Const LC_PROP_TEXT_Y = 5408
  Public Const LC_PROP_TEXT_DX = 5409
  Public Const LC_PROP_TEXT_DY = 5410
  Public Const LC_PROP_TEXT_WSCALE = 5412
  Public Const LC_PROP_TEXT_ANGLE = 5413
  Public Const LC_PROP_TEXT_OBLIQUE = 5414
  Public Const LC_PROP_TEXT_CHARSPACE = 5415
  Public Const LC_PROP_TEXT_WRECT = 5416
  Public Const LC_PROP_TEXT_X0 = 5417
  Public Const LC_PROP_TEXT_Y0 = 5418
  Public Const LC_PROP_TEXT_XFIT = 5419
  Public Const LC_PROP_TEXT_YFIT = 5420
  Public Const LC_PROP_TEXT_UPDOWN = 5421
  Public Const LC_PROP_TEXT_BACKWARD = 5422
  Public Const LC_PROP_TEXT_RESOL = 5423
  Public Const LC_PROP_MTEXT_STYLE = 5451
  Public Const LC_PROP_MTEXT_STR = 5452
  Public Const LC_PROP_MTEXT_LEN = 5453
  Public Const LC_PROP_MTEXT_ALIGN = 5454
  Public Const LC_PROP_MTEXT_X = 5455
  Public Const LC_PROP_MTEXT_Y = 5456
  Public Const LC_PROP_MTEXT_H = 5458
  Public Const LC_PROP_MTEXT_WSCALE = 5459
  Public Const LC_PROP_MTEXT_ANGLE = 5460
  Public Const LC_PROP_MTEXT_OBLIQUE = 5461
  Public Const LC_PROP_MTEXT_WRECT = 5462
  Public Const LC_PROP_MTEXT_HRECT = 5463
  Public Const LC_PROP_MTEXT_WRAPWIDTH = 5464
  Public Const LC_PROP_MTEXT_LINESPACE = 5465
  Public Const LC_PROP_MTEXT_CHARSPACE = 5466
  Public Const LC_PROP_MTEXT_MIRROR = 5467
  Public Const LC_PROP_MTEXT_RESOL = 5468
  Public Const LC_PROP_ATEXT_STYLE = 5481
  Public Const LC_PROP_ATEXT_STR = 5482
  Public Const LC_PROP_ATEXT_STRT = 5483
  Public Const LC_PROP_ATEXT_LEN = 5484
  Public Const LC_PROP_ATEXT_X = 5485
  Public Const LC_PROP_ATEXT_Y = 5486
  Public Const LC_PROP_ATEXT_R = 5487
  Public Const LC_PROP_ATEXT_RAD = 5487
  Public Const LC_PROP_ATEXT_RADIUS = 5487
  Public Const LC_PROP_ATEXT_ANGLE = 5488
  Public Const LC_PROP_ATEXT_ANGSTA = 5489
  Public Const LC_PROP_ATEXT_ANGEND = 5490
  Public Const LC_PROP_ATEXT_CW = 5491
  Public Const LC_PROP_ATEXT_H = 5492
  Public Const LC_PROP_ATEXT_WSCALE = 5493
  Public Const LC_PROP_ATEXT_CHARSPACE = 5494
  Public Const LC_PROP_ATEXT_ALIGN = 5495
  Public Const LC_PROP_ATEXT_RESOL = 5496
  Public Const LC_PROP_ATT_MODE = 5501
  Public Const LC_PROP_ATT_TSTYLE = 5502
  Public Const LC_PROP_ATT_TAG = 5503
  Public Const LC_PROP_ATT_PROMPT = 5504
  Public Const LC_PROP_ATT_DEFVAL = 5505
  Public Const LC_PROP_ATT_ALIGN = 5506
  Public Const LC_PROP_ATT_X = 5507
  Public Const LC_PROP_ATT_Y = 5508
  Public Const LC_PROP_ATT_Z = 5509
  Public Const LC_PROP_ATT_H = 5510
  Public Const LC_PROP_ATT_WSCALE = 5511
  Public Const LC_PROP_ATT_ANGLE = 5512
  Public Const LC_PROP_ATT_OBLIQUE = 5513
  Public Const LC_PROP_ATT_X0 = 5514
  Public Const LC_PROP_ATT_Y0 = 5515
  Public Const LC_PROP_ATT_XFIT = 5516
  Public Const LC_PROP_ATT_YFIT = 5517
  Public Const LC_PROP_ATT_UPDOWN = 5518
  Public Const LC_PROP_ATT_BACKWARD = 5519
  Public Const LC_PROP_ATT_POS = 5520
  Public Const LC_PROP_ATT_FIT = 5521
  Public Const LC_PROP_BLKREF_BLOCK = 5531
  Public Const LC_PROP_BLKREF_X = 5532
  Public Const LC_PROP_BLKREF_Y = 5533
  Public Const LC_PROP_BLKREF_SCALE = 5535
  Public Const LC_PROP_BLKREF_SCX = 5536
  Public Const LC_PROP_BLKREF_SCY = 5537
  Public Const LC_PROP_BLKREF_UFSCALE = 5539
  Public Const LC_PROP_BLKREF_ANGLE = 5540
  Public Const LC_PROP_BLKREF_ARNX = 5541
  Public Const LC_PROP_BLKREF_ARDX = 5542
  Public Const LC_PROP_BLKREF_ARNY = 5543
  Public Const LC_PROP_BLKREF_ARDY = 5544
  Public Const LC_PROP_BLKREF_ARANG = 5545
  Public Const LC_PROP_BLKREF_ATTRIBS = 5546
  Public Const LC_PROP_IMGREF_IMAGE = 5551
  Public Const LC_PROP_IMGREF_XC = 5554
  Public Const LC_PROP_IMGREF_YC = 5555
  Public Const LC_PROP_IMGREF_W = 5556
  Public Const LC_PROP_IMGREF_H = 5557
  Public Const LC_PROP_IMGREF_WPIX = 5558
  Public Const LC_PROP_IMGREF_HPIX = 5559
  Public Const LC_PROP_IMGREF_SCALE = 5560
  Public Const LC_PROP_IMGREF_PIXELSIZE = 5560
  Public Const LC_PROP_IMGREF_ANGLE = 5563
  Public Const LC_PROP_IMGREF_BORDER = 5565
  Public Const LC_PROP_IMGREF_TRANSP = 5566
  Public Const LC_PROP_IMGREF_TRCOLOR = 5567
  Public Const LC_PROP_IMGREF_TRALPHA = 5568
  Public Const LC_PROP_IMGREF_GREY = 5569
  Public Const LC_PROP_IMGREF_FLIPHOR = 5570
  Public Const LC_PROP_IMGREF_FLIPVER = 5571
  Public Const LC_PROP_IMGREF_PATH = 5573
  Public Const LC_PROP_IMGREF_GP_X = 5574
  Public Const LC_PROP_IMGREF_GP_Y = 5575
  Public Const LC_PROP_IMGREF_GP_COLOR = 5576
  Public Const LC_PROP_IMGREF_GP_GRAY = 5577
  Public Const LC_PROP_ECW_FILENAME = 5581
  Public Const LC_PROP_ECW_LOADED = 5582
  Public Const LC_PROP_ECW_WPIX = 5589
  Public Const LC_PROP_ECW_HPIX = 5590
  Public Const LC_PROP_ECW_CBIT = 5591
  Public Const LC_PROP_ECW_XMIN = 5583
  Public Const LC_PROP_ECW_YMIN = 5584
  Public Const LC_PROP_ECW_XMAX = 5585
  Public Const LC_PROP_ECW_YMAX = 5586
  Public Const LC_PROP_ECW_W = 5587
  Public Const LC_PROP_ECW_H = 5588
  Public Const LC_PROP_ECW_SCALEX = 5592
  Public Const LC_PROP_ECW_SCALEY = 5593
  Public Const LC_PROP_ECW_BORDER = 5594
  Public Const LC_PROP_CAMERA_X = 5601
  Public Const LC_PROP_CAMERA_Y = 5602
  Public Const LC_PROP_CAMERA_W = 5603
  Public Const LC_PROP_CAMERA_H = 5604
  Public Const LC_PROP_HATCH_NAME = 5631
  Public Const LC_PROP_HATCH_PATTERN = 5632
  Public Const LC_PROP_HATCH_SCALE = 5633
  Public Const LC_PROP_HATCH_ANGLE = 5634
  Public Const LC_PROP_HATCH_SIZE = 5635
  Public Const LC_PROP_HATCH_ASSOC = 5636
  Public Const LC_PROP_HATCH_SOLID = 5637
  Public Const LC_PROP_HATCH_CUSTOM = 5638
  Public Const LC_PROP_HATCH_NPT = 5639
  Public Const LC_PROP_HATCH_NLOOP = 5640
  Public Const LC_PROP_HATCH_NHPL = 5641
  Public Const LC_PROP_HATCH_IHPL = 5642
  Public Const LC_PROP_HPL_ANGLE = 5651
  Public Const LC_PROP_HPL_X0 = 5652
  Public Const LC_PROP_HPL_Y0 = 5653
  Public Const LC_PROP_HPL_DX = 5654
  Public Const LC_PROP_HPL_DY = 5655
  Public Const LC_PROP_HPL_NDASH = 5656
  Public Const LC_PROP_HPL_DASH1 = 5657
  Public Const LC_PROP_HPL_DASH2 = 5658
  Public Const LC_PROP_HPL_DASH3 = 5659
  Public Const LC_PROP_HPL_DASH4 = 5660
  Public Const LC_PROP_HPL_DASH5 = 5661
  Public Const LC_PROP_HPL_DASH6 = 5662
  Public Const LC_PROP_HPL_DASH7 = 5663
  Public Const LC_PROP_HPL_DASH8 = 5664
  Public Const LC_PROP_VPORT_LEF = 5703
  Public Const LC_PROP_VPORT_BOT = 5704
  Public Const LC_PROP_VPORT_RIG = 5705
  Public Const LC_PROP_VPORT_TOP = 5706
  Public Const LC_PROP_VPORT_BORDER = 5707
  Public Const LC_PROP_VPORT_W = 5711
  Public Const LC_PROP_VPORT_H = 5712
  Public Const LC_PROP_VPORT_VX = 5713
  Public Const LC_PROP_VPORT_VY = 5714
  Public Const LC_PROP_VPORT_VSCALE = 5715
  Public Const LC_PROP_VPORT_VANGLE = 5716
  Public Const LC_PROP_VPORT_FIXSCALE = 5717
  Public Const LC_PROP_BARC_XC = 5751
  Public Const LC_PROP_BARC_X = 5751
  Public Const LC_PROP_BARC_YC = 5752
  Public Const LC_PROP_BARC_Y = 5752
  Public Const LC_PROP_BARC_W = 5753
  Public Const LC_PROP_BARC_H = 5754
  Public Const LC_PROP_BARC_ANG = 5755
  Public Const LC_PROP_BARC_TYPE = 5756
  Public Const LC_PROP_BARC_CODE = 5757
  Public Const LC_PROP_BARC_TEXT = 5757
  Public Const LC_PROP_BARC_CHECKSUM = 5758
  Public Const LC_PROP_BARC_NCHARS = 5759
  Public Const LC_PROP_BARC_NUBITS = 5760
  Public Const LC_PROP_BARC_NARSIZE = 5761
  Public Const LC_PROP_BARC_WIDERATIO = 5762
  Public Const LC_PROP_BARC_QZONE = 5763
  Public Const LC_PROP_BARC_OFFSET = 5764
  Public Const LC_PROP_BARC_INVERT = 5765
  Public Const LC_PROP_BARC_HIDETEXT = 5766
  Public Const LC_PROP_BARC_TSTYLE = 5767
  Public Const LC_PROP_BARC_TEXTH = 5768
  Public Const LC_PROP_BARC_TEXTW = 5769
  Public Const LC_PROP_BARC_TEXTCS = 5770
  Public Const LC_PROP_BARC_TEXTALIGN = 5771
  Public Const LC_PROP_BARC_TEXTRES = 5772
  Public Const LC_PROP_BARC_ECLEVEL = 5773
  Public Const LC_PROP_BARC_MASKPAT = 5774
  Public Const LC_PROP_BARC_SIZE = 5775
  Public Const LC_PROP_BARC_NBARS = 5776
  Public Const LC_PROP_BARC_NMODX = 5776
  Public Const LC_PROP_BARC_NMODY = 5777
  Public Const LC_PROP_ARR_X0 = 5801
  Public Const LC_PROP_ARR_Y0 = 5802
  Public Const LC_PROP_ARR_X1 = 5803
  Public Const LC_PROP_ARR_Y1 = 5804
  Public Const LC_PROP_ARR_TIME = 5805
  Public Const LC_PROP_ARR_SHARP = 5806
  Public Const LC_PROP_SPIR_X = 5821
  Public Const LC_PROP_SPIR_Y = 5822
  Public Const LC_PROP_SPIR_R = 5823
  Public Const LC_PROP_SPIR_RADIUS = 5823
  Public Const LC_PROP_SPIR_R2 = 5824
  Public Const LC_PROP_SPIR_RATIO = 5825
  Public Const LC_PROP_SPIR_ANGLE = 5826
  Public Const LC_PROP_SPIR_TURNS = 5827
  Public Const LC_PROP_SPIR_DIRCW = 5829
  Public Const LC_PROP_SPIR_CLOSED = 5830
  Public Const LC_PROP_SPIR_DSTEP = 5831
  Public Const LC_PROP_SPIR_RESOL = 5832
  Public Const LC_PROP_DIM_STYLE = 6001
  Public Const LC_PROP_DIM_MEAS = 6003
  Public Const LC_PROP_DIM_TEXT = 6004
  Public Const LC_PROP_DIMANG_STYLE = 6001
  Public Const LC_PROP_DIMANG_MEAS = 6003
  Public Const LC_PROP_DIMANG_TEXT = 6004
  Public Const LC_PROP_DIMALI_STYLE = 6001
  Public Const LC_PROP_DIMALI_MEAS = 6003
  Public Const LC_PROP_DIMALI_TEXT = 6004
  Public Const LC_PROP_DIMDIA_STYLE = 6001
  Public Const LC_PROP_DIMDIA_MEAS = 6003
  Public Const LC_PROP_DIMDIA_TEXT = 6004
  Public Const LC_PROP_DIMRAD_STYLE = 6001
  Public Const LC_PROP_DIMRAD_MEAS = 6003
  Public Const LC_PROP_DIMRAD_TEXT = 6004
  Public Const LC_PROP_DIMORD_STYLE = 6001
  Public Const LC_PROP_DIMORD_MEAS = 6003
  Public Const LC_PROP_DIMORD_TEXT = 6004
  Public Const LC_PROP_DIMROT_STYLE = 6001
  Public Const LC_PROP_DIMROT_MEAS = 6003
  Public Const LC_PROP_DIMROT_TEXT = 6004
  Public Const LC_PROP_DIMLIN_STYLE = 6001
  Public Const LC_PROP_DIMLIN_MEAS = 6003
  Public Const LC_PROP_DIMLIN_TEXT = 6004
  Public Const LC_PROP_DIMANG_3PNT = 6011
  Public Const LC_PROP_DIMANG_2LINE = 6012
  Public Const LC_PROP_DIMANG_CPX = 6013
  Public Const LC_PROP_DIMANG_CPY = 6014
  Public Const LC_PROP_DIMANG_DP1X = 6015
  Public Const LC_PROP_DIMANG_DP1Y = 6016
  Public Const LC_PROP_DIMANG_DP2X = 6017
  Public Const LC_PROP_DIMANG_DP2Y = 6018
  Public Const LC_PROP_DIMANG_L1P1X = 6021
  Public Const LC_PROP_DIMANG_L1P1Y = 6022
  Public Const LC_PROP_DIMANG_L1P2X = 6023
  Public Const LC_PROP_DIMANG_L1P2Y = 6024
  Public Const LC_PROP_DIMANG_DP3X = 6023
  Public Const LC_PROP_DIMANG_DP3Y = 6024
  Public Const LC_PROP_DIMANG_L2P1X = 6025
  Public Const LC_PROP_DIMANG_L2P1Y = 6026
  Public Const LC_PROP_DIMANG_L2P2X = 6027
  Public Const LC_PROP_DIMANG_L2P2Y = 6028
  Public Const LC_PROP_DIMANG_DP4X = 6027
  Public Const LC_PROP_DIMANG_DP4Y = 6028
  Public Const LC_PROP_DIMANG_APX = 6029
  Public Const LC_PROP_DIMANG_APY = 6030
  Public Const LC_PROP_DIMANG_EXT1 = 6031
  Public Const LC_PROP_DIMANG_EXT2 = 6032
  Public Const LC_PROP_DIMANG_RAD = 6033
  Public Const LC_PROP_DIMANG_TPOS = 6034
  Public Const LC_PROP_DIMALI_DP1X = 6051
  Public Const LC_PROP_DIMALI_DP1Y = 6052
  Public Const LC_PROP_DIMALI_DP2X = 6053
  Public Const LC_PROP_DIMALI_DP2Y = 6054
  Public Const LC_PROP_DIMALI_TPX = 6055
  Public Const LC_PROP_DIMALI_TPY = 6056
  Public Const LC_PROP_DIMDIA_CPX = 6071
  Public Const LC_PROP_DIMDIA_CPY = 6072
  Public Const LC_PROP_DIMDIA_RPX = 6073
  Public Const LC_PROP_DIMDIA_RPY = 6074
  Public Const LC_PROP_DIMDIA_FPX = 6075
  Public Const LC_PROP_DIMDIA_FPY = 6076
  Public Const LC_PROP_DIMDIA_TPX = 6077
  Public Const LC_PROP_DIMDIA_TPY = 6078
  Public Const LC_PROP_DIMRAD_CPX = 6086
  Public Const LC_PROP_DIMRAD_CPY = 6087
  Public Const LC_PROP_DIMRAD_RPX = 6088
  Public Const LC_PROP_DIMRAD_RPY = 6089
  Public Const LC_PROP_DIMRAD_TPX = 6090
  Public Const LC_PROP_DIMRAD_TPY = 6091
  Public Const LC_PROP_DIMORD_XORD = 6101
  Public Const LC_PROP_DIMORD_DPX = 6102
  Public Const LC_PROP_DIMORD_DPY = 6103
  Public Const LC_PROP_DIMORD_TPX = 6104
  Public Const LC_PROP_DIMORD_TPY = 6105
  Public Const LC_PROP_DIMROT_ANGLE = 6121
  Public Const LC_PROP_DIMROT_DP1X = 6122
  Public Const LC_PROP_DIMROT_DP1Y = 6123
  Public Const LC_PROP_DIMROT_DP2X = 6124
  Public Const LC_PROP_DIMROT_DP2Y = 6125
  Public Const LC_PROP_DIMROT_TPX = 6126
  Public Const LC_PROP_DIMROT_TPY = 6127
  Public Const LC_PROP_DIMLIN_ANGLE = 6121
  Public Const LC_PROP_DIMLIN_DP1X = 6122
  Public Const LC_PROP_DIMLIN_DP1Y = 6123
  Public Const LC_PROP_DIMLIN_DP2X = 6124
  Public Const LC_PROP_DIMLIN_DP2Y = 6125
  Public Const LC_PROP_DIMLIN_TPX = 6126
  Public Const LC_PROP_DIMLIN_TPY = 6127
  Public Const LC_PROP_LEADER_STYLE = 6202
  Public Const LC_PROP_LEADER_TEXT = 6203
  Public Const LC_PROP_LEADER_ALIGN = 6204
  Public Const LC_PROP_LEADER_TPX = 6205
  Public Const LC_PROP_LEADER_TPY = 6206
  Public Const LC_PROP_LEADER_APX = 6207
  Public Const LC_PROP_LEADER_APY = 6208
  Public Const LC_PROP_LEADER_P1X = 6209
  Public Const LC_PROP_LEADER_P1Y = 6210
  Public Const LC_PROP_LEADER_P0X = 6211
  Public Const LC_PROP_LEADER_P0Y = 6212
  Public Const LC_PROP_LEADER_LDIST = 6213
  Public Const LC_PROP_LEADER_VERT = 6214
  Public Const LC_PROP_LEADER_CORNER = 6215
  Public Const LC_PROP_LEADER_TBW = 6216
  Public Const LC_PROP_LEADER_TBH = 6217
  Public Const LC_PROP_RPLAN_LEN = 6301
  Public Const LC_PROP_RPLAN_MARKARC = 6302
  Public Const LC_PROP_RPLAN_MARKSIZE = 6303
  Public Const LC_PROP_RPLAN_NVERS = 6311
  Public Const LC_PROP_RPLAN_IVER = 6312
  Public Const LC_PROP_RPVER_X = 6313
  Public Const LC_PROP_RPVER_Y = 6314
  Public Const LC_PROP_RPVER_ANGLE = 6315
  Public Const LC_PROP_RPVER_DIRANG = 6316
  Public Const LC_PROP_RPVER_R = 6317
  Public Const LC_PROP_RPVER_L1 = 6318
  Public Const LC_PROP_RPVER_L2 = 6319
  Public Const LC_PROP_RPVER_ANGL1 = 6320
  Public Const LC_PROP_RPVER_ANGARC = 6321
  Public Const LC_PROP_RPVER_ANGL2 = 6322
  Public Const LC_PROP_RPVER_BISEC = 6323
  Public Const LC_PROP_RPVER_ARCLEN = 6324
  Public Const LC_PROP_RPVER_CURLEN = 6325
  Public Const LC_PROP_RPVER_LINE1 = 6326
  Public Const LC_PROP_RPVER_T1 = 6327
  Public Const LC_PROP_RPVER_T2 = 6328
  Public Const LC_PROP_RPVER_LINE2 = 6329
  Public Const LC_PROP_RPVER_DIST1 = 6330
  Public Const LC_PROP_RPVER_DIST2 = 6331
  Public Const LC_PROP_VER_X = 10001
  Public Const LC_PROP_VER_Y = 10002
  Public Const LC_PROP_VER_Z = 10003
  Public Const LC_PROP_VER_ENDPATH = 10005
  Public Const LC_PROP_VER_FIX = 10006
  Public Const LC_PROP_VER_RADIUS = 10007
  Public Const LC_PROP_VER_WEIGHT = 10008
  Public Const LC_PROP_VER_INDEX = 10009
  Public Const LC_PROP_VER_FIRST = 10010
  Public Const LC_PROP_VER_LAST = 10011
  Public Const LC_PROP_VER_W0 = 10012
  Public Const LC_PROP_VER_W1 = 10013
  Public Const LC_PROP_VER_SEGDX = 10014
  Public Const LC_PROP_VER_SEGDY = 10015
  Public Const LC_PROP_VER_SEGANG = 10016
  Public Const LC_PROP_VER_SEGLEN = 10017
  Public Const LC_PROP_VER_BULGE = 10021
  Public Const LC_PROP_VER_SEGARCANG = 10022
  Public Const LC_PROP_VER_SEGARCH = 10023
  Public Const LC_PROP_VER_SEGARCLEN = 10024
  Public Const LC_PROP_VER_SEGARCRAD = 10025
  Public Const LC_PROP_PROPWND_ENABLE = 11002
  Public Const LC_PROP_PROPWND_DIVCOEF = 11003
  Public Const LC_PROP_EVENT_TYPE = 12001
  Public Const LC_PROP_EVENT_APPPRM1 = 12002
  Public Const LC_PROP_EVENT_APPPRM2 = 12003
  Public Const LC_PROP_EVENT_WND = 12004
  Public Const LC_PROP_EVENT_DRW = 12005
  Public Const LC_PROP_EVENT_BLOCK = 12006
  Public Const LC_PROP_EVENT_ENTITY = 12007
  Public Const LC_PROP_EVENT_ENT = 12007
  Public Const LC_PROP_EVENT_HCMD = 12008
  Public Const LC_PROP_EVENT_HDC = 12009
  Public Const LC_PROP_EVENT_INT1 = 12010
  Public Const LC_PROP_EVENT_INT2 = 12011
  Public Const LC_PROP_EVENT_INT3 = 12012
  Public Const LC_PROP_EVENT_INT4 = 12013
  Public Const LC_PROP_EVENT_INT5 = 12014
  Public Const LC_PROP_EVENT_INT6 = 12015
  Public Const LC_PROP_EVENT_FLOAT1 = 12016
  Public Const LC_PROP_EVENT_FLOAT2 = 12017
  Public Const LC_PROP_EVENT_FLOAT3 = 12018
  Public Const LC_PROP_EVENT_FLOAT4 = 12019
  Public Const LC_PROP_EVENT_FLOAT5 = 12020
  Public Const LC_PROP_EVENT_FLOAT6 = 12021
  Public Const LC_PROP_EVENT_STR1 = 12022
  Public Const LC_PROP_EVENT_STR2 = 12023
  Public Const LC_EVENT_HELP = 101
  Public Const LC_EVENT_PAINT = 102
  Public Const LC_EVENT_WNDVIEW = 103
  Public Const LC_EVENT_MOUSEMOVE = 105
  Public Const LC_EVENT_LBDOWN = 106
  Public Const LC_EVENT_LBUP = 107
  Public Const LC_EVENT_LBDBLCLK = 108
  Public Const LC_EVENT_RBDOWN = 109
  Public Const LC_EVENT_RBUP = 110
  Public Const LC_EVENT_KEYDOWN = 111
  Public Const LC_EVENT_VIEWBLOCK = 112
  Public Const LC_EVENT_EXTENTS = 113
  Public Const LC_EVENT_SNAP = 114
  Public Const LC_EVENT_FILE = 131
  Public Const LC_EVENT_ADDENTITY = 132
  Public Const LC_EVENT_ENTPROP = 133
  Public Const LC_EVENT_DRWPROP = 134
  Public Const LC_EVENT_WNDPROP = 135
  Public Const LC_EVENT_SELECT = 136
  Public Const LC_EVENT_PICKENT = 137
  Public Const LC_EVENT_SELENT1 = 161
  Public Const LC_EVENT_SELENTS = 162
  Public Const LC_EVENT_GRIPMOVE = 163
  Public Const LC_EVENT_GRIPDRAG = 164
  Public Const LC_EVENT_GRIPPAINT = 165
  Public Const LC_EVENT_DRAWCURSOR = 166
  Public Const LC_EVENT_RULERCORNER = 167
  Public Const LC_EVENT_ADDSTR = 168
  Public Const LC_EVENT_ADDCMD = 169
  Public Const LC_EVENT_ALPHABLEND = 170
  Public Const LC_EVENT_WNDTAB = 171
  Public Const LC_EVENT_CMDCREATE = 201
  Public Const LC_EVENT_CMDDESTROY = 202
  Public Const LC_EVENT_CMDSTART = 203
  Public Const LC_EVENT_CMDEND = 204
  Public Const LC_EVENT_CMDLBDOWN = 205
  Public Const LC_EVENT_CMDLBUP = 206
  Public Const LC_EVENT_CMDRBDOWN = 207
  Public Const LC_EVENT_CMDRBUP = 208
  Public Const LC_EVENT_CMDMOUSEMOVE = 209
  Public Const LC_EVENT_CMDPAINT = 210
  Public Const LC_EVENT_CMDSNAP = 211
  Public Const LC_EVENT_CMD1 = 212
  Public Const LC_EVENT_LAYERS = 303

  Public Delegate Sub F_LCEVENT (ByVal hEvent As Integer)

  Public Declare Sub EventSetProc Lib "Litecad.dll" Alias "lcEventSetProc" (ByVal EventType As Integer, pFunc As F_LCEVENT, ByVal Prm1 As Integer, ByVal Prm2 As Integer)

  Public Declare Sub EventReturnCode Lib "Litecad.dll" Alias "lcEventReturnCode" (ByVal code As Integer)

  Public Declare Function EventsEnable Lib "Litecad.dll" Alias "lcEventsEnable" (ByVal b As Integer) As Integer

  Public Declare Function Initialize Lib "Litecad.dll" Alias "lcInitialize" () As Integer

  Public Declare Function Uninitialize Lib "Litecad.dll" Alias "lcUninitialize" (ByVal bSaveConfig As Integer) As Integer

  Public Declare Unicode Function StrAdd Lib "Litecad.dll" Alias "lcStrAdd" (ByVal szTag As String, ByVal szText As String) As Integer

  Public Declare Unicode Function StrSet Lib "Litecad.dll" Alias "lcStrSet" (ByVal szTag As String, ByVal szText As String) As Integer

  Public Declare Unicode Function StrGet Lib "Litecad.dll" Alias "lcStrGet" (ByVal szTag As String) As Char

  Public Declare Unicode Function StrFileLoad Lib "Litecad.dll" Alias "lcStrFileLoad" (ByVal szFileName As String) As Integer

  Public Declare Unicode Function StrFileSave Lib "Litecad.dll" Alias "lcStrFileSave" (ByVal szFileName As String, ByVal szLanguage As String) As Integer

  Public Declare Function PropGetBool Lib "Litecad.dll" Alias "lcPropGetBool" (ByVal hObject As Integer, ByVal idProp As Integer) As Integer

  Public Declare Function PropGetInt Lib "Litecad.dll" Alias "lcPropGetInt" (ByVal hObject As Integer, ByVal idProp As Integer) As Integer

  Public Declare Function PropGetFloat Lib "Litecad.dll" Alias "lcPropGetFloat" (ByVal hObject As Integer, ByVal idProp As Integer) As Double

  Function PropGetStr (ByVal hObject As Integer, ByVal idProp As Integer) As String
    Dim Buf() As Char
    Dim nChars As Integer
    Dim ch As Integer

    nChars = Lcad.PropGetStr2(hObject, idProp)
    If nChars > 0 Then
      Buf = New Char(nChars) {}
      nChars = nChars - 1
      For i As Integer = 0 To nChars
        ch = Lcad.PropGetChar(i)
        Buf(i) = ChrW(ch)
      Next
      Return New String(Buf)
    End If
    Return ""
  End Function

  Public Declare Function PropGetStr2 Lib "Litecad.dll" Alias "lcPropGetStr2" (ByVal hObject As Integer, ByVal idProp As Integer) As Integer

  Public Declare Function PropGetChar Lib "Litecad.dll" Alias "lcPropGetChar" (ByVal iChar As Integer) As Integer

  Public Declare Function PropGetHandle Lib "Litecad.dll" Alias "lcPropGetHandle" (ByVal hObject As Integer, ByVal idProp As Integer) As Integer

  Public Declare Function PropPutBool Lib "Litecad.dll" Alias "lcPropPutBool" (ByVal hObject As Integer, ByVal idProp As Integer, ByVal bValue As Integer) As Integer

  Public Declare Function PropPutInt Lib "Litecad.dll" Alias "lcPropPutInt" (ByVal hObject As Integer, ByVal idProp As Integer, ByVal Value As Integer) As Integer

  Public Declare Function PropPutFloat Lib "Litecad.dll" Alias "lcPropPutFloat" (ByVal hObject As Integer, ByVal idProp As Integer, ByVal Value As Double) As Integer

  Public Declare Unicode Function PropPutStr Lib "Litecad.dll" Alias "lcPropPutStr" (ByVal hObject As Integer, ByVal idProp As Integer, ByVal szValue As String) As Integer

  Public Declare Function PropPutHandle Lib "Litecad.dll" Alias "lcPropPutHandle" (ByVal hObject As Integer, ByVal idProp As Integer, ByVal hValue As Integer) As Integer

  Public Declare Function CreateWindow Lib "Litecad.dll" Alias "lcCreateWindow" (ByVal hWndParent As Integer, ByVal Style As Integer) As Integer

  Public Declare Function DeleteWindow Lib "Litecad.dll" Alias "lcDeleteWindow" (ByVal hLcWnd As Integer) As Integer

  Public Declare Function WndResize Lib "Litecad.dll" Alias "lcWndResize" (ByVal hLcWnd As Integer, ByVal Left As Integer, ByVal Top As Integer, ByVal Width As Integer, ByVal Height As Integer) As Integer

  Public Declare Function WndRedraw Lib "Litecad.dll" Alias "lcWndRedraw" (ByVal hLcWnd As Integer) As Integer

  Public Declare Function WndSetFocus Lib "Litecad.dll" Alias "lcWndSetFocus" (ByVal hLcWnd As Integer) As Integer

  Public Declare Function WndSetExtents Lib "Litecad.dll" Alias "lcWndSetExtents" (ByVal hLcWnd As Integer, ByVal Xmin As Double, ByVal Ymin As Double, ByVal Xmax As Double, ByVal Ymax As Double) As Integer

  Public Declare Function WndZoomRect Lib "Litecad.dll" Alias "lcWndZoomRect" (ByVal hLcWnd As Integer, ByVal Left As Double, ByVal Bottom As Double, ByVal Right As Double, ByVal Top As Double) As Integer

  Public Declare Function WndZoomScale Lib "Litecad.dll" Alias "lcWndZoomScale" (ByVal hLcWnd As Integer, ByVal Scal As Double) As Integer

  Public Declare Function WndZoomMove Lib "Litecad.dll" Alias "lcWndZoomMove" (ByVal hLcWnd As Integer, ByVal DX As Double, ByVal DY As Double) As Integer

  Public Declare Function WndGetCursorCoord Lib "Litecad.dll" Alias "lcWndGetCursorCoord" (ByVal hLcWnd As Integer, ByRef pXwin As Integer, ByRef pYwin As Integer, ByRef pXdrw As Double, ByRef pYdrw As Double) As Integer

  Public Declare Function WndExeCommand Lib "Litecad.dll" Alias "lcWndExeCommand" (ByVal hLcWnd As Integer, ByVal Command As Integer, ByVal CmdParam As Integer) As Integer

  Public Declare Function WndOnClose Lib "Litecad.dll" Alias "lcWndOnClose" (ByVal hLcWnd As Integer) As Integer

  Public Declare Function WndSetBlock Lib "Litecad.dll" Alias "lcWndSetBlock" (ByVal hLcWnd As Integer, ByVal hBlock As Integer) As Integer

  Public Declare Function WndSetProps Lib "Litecad.dll" Alias "lcWndSetProps" (ByVal hLcWnd As Integer, ByVal hPropWnd As Integer) As Integer

  Public Declare Function WndToRaster Lib "Litecad.dll" Alias "lcWndToRaster" (ByVal hLcWnd As Integer, ByVal ImgW As Integer, ByVal ImgH As Integer, ByVal pBuffer As Integer, ByVal ResizeFilter As Integer) As Integer

  Public Declare Function WndGetEntByPoint Lib "Litecad.dll" Alias "lcWndGetEntByPoint" (ByVal hLcWnd As Integer, ByVal Xwin As Integer, ByVal Ywin As Integer) As Integer

  Public Declare Function WndGetEntByPoint2 Lib "Litecad.dll" Alias "lcWndGetEntByPoint2" (ByVal hLcWnd As Integer, ByVal X As Double, ByVal Y As Double, ByVal Delta As Double) As Integer

  Public Declare Function WndGetEntsByRect Lib "Litecad.dll" Alias "lcWndGetEntsByRect" (ByVal hLcWnd As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Rig As Double, ByVal Top As Double, ByVal bCross As Integer, ByVal nMaxEnts As Integer) As Integer

  Public Declare Function WndGetEntity Lib "Litecad.dll" Alias "lcWndGetEntity" (ByVal iEnt As Integer) As Integer

  Public Declare Function WndGetEntByID Lib "Litecad.dll" Alias "lcWndGetEntByID" (ByVal hLcWnd As Integer, ByVal Id As Integer) As Integer

  Public Declare Unicode Function WndGetEntByIDH Lib "Litecad.dll" Alias "lcWndGetEntByIDH" (ByVal hLcWnd As Integer, ByVal szId As String) As Integer

  Public Declare Function WndGetEntByKey Lib "Litecad.dll" Alias "lcWndGetEntByKey" (ByVal hLcWnd As Integer, ByVal Key As Integer) As Integer

  Public Declare Function WndCoordFromDrw Lib "Litecad.dll" Alias "lcWndCoordFromDrw" (ByVal hLcWnd As Integer, ByVal Xdrw As Double, ByVal Ydrw As Double, ByRef pXwin As Integer, ByRef pYwin As Integer) As Integer

  Public Declare Function WndCoordToDrw Lib "Litecad.dll" Alias "lcWndCoordToDrw" (ByVal hLcWnd As Integer, ByVal Xwin As Integer, ByVal Ywin As Integer, ByRef pXdrw As Double, ByRef pYdrw As Double) As Integer

  Public Declare Function CoordDrwToWnd Lib "Litecad.dll" Alias "lcCoordDrwToWnd" (ByVal hLcWnd As Integer, ByVal Xdrw As Double, ByVal Ydrw As Double, ByRef pXwnd As Integer, ByRef pYwnd As Integer) As Integer

  Public Declare Function CoordWndToDrw Lib "Litecad.dll" Alias "lcCoordWndToDrw" (ByVal hLcWnd As Integer, ByVal Xwnd As Integer, ByVal Ywnd As Integer, ByRef pXdrw As Double, ByRef pYdrw As Double) As Integer

  Public Declare Function FontGetFirst Lib "Litecad.dll" Alias "lcFontGetFirst" () As Integer

  Public Declare Function FontGetNext Lib "Litecad.dll" Alias "lcFontGetNext" (ByVal hFont As Integer) As Integer

  Public Declare Unicode Function FontAddRes Lib "Litecad.dll" Alias "lcFontAddRes" (ByVal szFontName As String, ByVal hModule As Integer, ByVal idResource As Integer) As Integer

  Public Declare Unicode Function FontAddFile Lib "Litecad.dll" Alias "lcFontAddFile" (ByVal szFontName As String, ByVal szFilename As String) As Integer

  Public Declare Unicode Function FontAddBin Lib "Litecad.dll" Alias "lcFontAddBin" (ByVal szFontName As String, ByVal hData As Integer) As Integer

  Public Declare Function FontGetChar Lib "Litecad.dll" Alias "lcFontGetChar" (ByVal hFont As Integer, ByVal CharCode As Integer) As Integer

  Public Declare Unicode Function ProgressCreate Lib "Litecad.dll" Alias "lcProgressCreate" (ByVal hLcWnd As Integer, ByVal W As Integer, ByVal H As Integer, ByVal szTitle As String) As Integer

  Public Declare Unicode Function ProgressSetText Lib "Litecad.dll" Alias "lcProgressSetText" (ByVal szText As String) As Integer

  Public Declare Function ProgressStart Lib "Litecad.dll" Alias "lcProgressStart" (ByVal MinVal As Integer, ByVal MaxVal As Integer) As Integer

  Public Declare Function ProgressSetPos Lib "Litecad.dll" Alias "lcProgressSetPos" (ByVal Val As Integer) As Integer

  Public Declare Function ProgressInc Lib "Litecad.dll" Alias "lcProgressInc" () As Integer

  Public Declare Function ProgressDelete Lib "Litecad.dll" Alias "lcProgressDelete" () As Integer

  Public Declare Function QuadCreate Lib "Litecad.dll" Alias "lcQuadCreate" () As Integer

  Public Declare Function QuadDelete Lib "Litecad.dll" Alias "lcQuadDelete" (ByVal hQuad As Integer) As Integer

  Public Declare Function QuadSet Lib "Litecad.dll" Alias "lcQuadSet" (ByVal hQuad As Integer, ByVal x0 As Double, ByVal y0 As Double, ByVal x1 As Double, ByVal y1 As Double, ByVal x2 As Double, ByVal y2 As Double, ByVal x3 As Double, ByVal y3 As Double) As Integer

  Public Declare Function QuadTransXYtoUV Lib "Litecad.dll" Alias "lcQuadTransXYtoUV" (ByVal hQuad As Integer, ByVal X As Double, ByVal Y As Double, ByRef pU As Double, ByRef pV As Double) As Integer

  Public Declare Function QuadTransUVtoXY Lib "Litecad.dll" Alias "lcQuadTransUVtoXY" (ByVal hQuad As Integer, ByVal U As Double, ByVal V As Double, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function QuadContains Lib "Litecad.dll" Alias "lcQuadContains" (ByVal hQuad As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function GridCreate Lib "Litecad.dll" Alias "lcGridCreate" () As Integer

  Public Declare Function GridDelete Lib "Litecad.dll" Alias "lcGridDelete" (ByVal hGrid As Integer) As Integer

  Public Declare Function GridSet Lib "Litecad.dll" Alias "lcGridSet" (ByVal hGrid As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal W As Double, ByVal H As Double, ByVal Nx As Integer, ByVal Ny As Integer) As Integer

  Public Declare Function GridSetDest Lib "Litecad.dll" Alias "lcGridSetDest" (ByVal hGrid As Integer, ByVal Ix As Integer, ByVal Iy As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function GridUpdate Lib "Litecad.dll" Alias "lcGridUpdate" (ByVal hGrid As Integer) As Integer

  Public Declare Function GridTrans Lib "Litecad.dll" Alias "lcGridTrans" (ByVal hGrid As Integer, ByVal X As Double, ByVal Y As Double, ByRef pXdest As Double, ByRef pYdest As Double) As Integer

  Public Declare Function GridGetNode Lib "Litecad.dll" Alias "lcGridGetNode" (ByVal hGrid As Integer, ByVal bDest As Integer, ByVal Ix As Integer, ByVal Iy As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function GridGetCell Lib "Litecad.dll" Alias "lcGridGetCell" (ByVal hGrid As Integer, ByVal X As Double, ByVal Y As Double, ByRef pIx As Integer, ByRef pIy As Integer) As Integer

  Public Declare Function GridSetView Lib "Litecad.dll" Alias "lcGridSetView" (ByVal hGrid As Integer, ByVal Mode As Integer, ByVal hLcWnd As Integer, ByVal ColLine As Integer, ByVal ColNode As Integer) As Integer

  Public Declare Function TIN_Create Lib "Litecad.dll" Alias "lcTIN_Create" (ByVal Id As Integer) As Integer

  Public Declare Function TIN_Delete Lib "Litecad.dll" Alias "lcTIN_Delete" (ByVal hTIN As Integer) As Integer

  Public Declare Unicode Function TIN_Load Lib "Litecad.dll" Alias "lcTIN_Load" (ByVal hTIN As Integer, ByVal szFileName As String, ByVal hWnd As Integer) As Integer

  Public Declare Function TIN_GetZ Lib "Litecad.dll" Alias "lcTIN_GetZ" (ByVal hTIN As Integer, ByVal X As Double, ByVal Y As Double, ByRef pZ As Double) As Integer

  Public Declare Function TIN_InterLine Lib "Litecad.dll" Alias "lcTIN_InterLine" (ByVal hTIN As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double) As Integer

  Public Declare Function TIN_InterGetPoint Lib "Litecad.dll" Alias "lcTIN_InterGetPoint" (ByVal hTIN As Integer, ByVal iPnt As Integer, ByRef pX As Double, ByRef pY As Double, ByRef pZ As Double) As Integer

  Public Declare Function CreateProps Lib "Litecad.dll" Alias "lcCreateProps" (ByVal hWndParent As Integer) As Integer

  Public Declare Function DeleteProps Lib "Litecad.dll" Alias "lcDeleteProps" (ByVal hPropWnd As Integer) As Integer

  Public Declare Function PropsResize Lib "Litecad.dll" Alias "lcPropsResize" (ByVal hPropWnd As Integer, ByVal Left As Integer, ByVal Top As Integer, ByVal Width As Integer, ByVal Height As Integer) As Integer

  Public Declare Function PropsUpdate Lib "Litecad.dll" Alias "lcPropsUpdate" (ByVal hPropWnd As Integer, ByVal bSelChanged As Integer) As Integer

  Public Declare Function CreateStatbar Lib "Litecad.dll" Alias "lcCreateStatbar" (ByVal hWndParent As Integer) As Integer

  Public Declare Function DeleteStatbar Lib "Litecad.dll" Alias "lcDeleteStatbar" (ByVal hStatbar As Integer) As Integer

  Public Declare Function StatbarResize Lib "Litecad.dll" Alias "lcStatbarResize" (ByVal hStatbar As Integer, ByVal Left As Integer, ByVal Top As Integer, ByVal Width As Integer, ByVal Height As Integer) As Integer

  Public Declare Function StatbarCell Lib "Litecad.dll" Alias "lcStatbarCell" (ByVal hStatbar As Integer, ByVal Id As Integer, ByVal Pos As Integer) As Integer

  Public Declare Unicode Function StatbarText Lib "Litecad.dll" Alias "lcStatbarText" (ByVal hStatbar As Integer, ByVal Id As Integer, ByVal szText As String) As Integer

  Public Declare Function StatbarRedraw Lib "Litecad.dll" Alias "lcStatbarRedraw" (ByVal hStatbar As Integer) As Integer

  Public Declare Unicode Function DgGetValue Lib "Litecad.dll" Alias "lcDgGetValue" (ByVal hWnd As Integer, ByVal Lef As Integer, ByVal Top As Integer, ByVal szTitle As String, ByVal szPrompt As String) As Integer

  Public Declare Unicode Function Help Lib "Litecad.dll" Alias "lcHelp" (ByVal szTopic As String) As Integer

  Public Declare Sub GetPolarPoint Lib "Litecad.dll" Alias "lcGetPolarPoint" (ByVal x0 As Double, ByVal y0 As Double, ByVal Angle As Double, ByVal Dist As Double, ByRef pOutX As Double, ByRef pOutY As Double)

  Public Declare Sub GetPolarPrm Lib "Litecad.dll" Alias "lcGetPolarPrm" (ByVal x1 As Double, ByVal y1 As Double, ByVal x2 As Double, ByVal y2 As Double, ByRef pAngle As Double, ByRef pDist As Double)

  Public Declare Function GetClientSize Lib "Litecad.dll" Alias "lcGetClientSize" (ByVal hWnd As Integer, ByRef pWidth As Integer, ByRef pHeight As Integer) As Integer

  Public Declare Function GetErrorCode Lib "Litecad.dll" Alias "lcGetErrorCode" () As Integer

  Public Declare Unicode Function GetErrorStr Lib "Litecad.dll" Alias "lcGetErrorStr" () As Char

  Public Declare Unicode Function CreateCommand Lib "Litecad.dll" Alias "lcCreateCommand" (ByVal hLcWnd As Integer, ByVal Id As Integer, ByVal szName As String, ByVal szTitle As String, ByVal hObj As Integer) As Integer

  Public Declare Function CmdExit Lib "Litecad.dll" Alias "lcCmdExit" (ByVal hCmd As Integer) As Integer

  Public Declare Unicode Function CmdCursorText Lib "Litecad.dll" Alias "lcCmdCursorText" (ByVal hCmd As Integer, ByVal szText As String) As Integer

  Public Declare Unicode Function CmdMessage Lib "Litecad.dll" Alias "lcCmdMessage" (ByVal hCmd As Integer, ByVal szText As String, ByVal IconType As Integer) As Integer

  Public Declare Function CmdResetLastPt Lib "Litecad.dll" Alias "lcCmdResetLastPt" (ByVal hCmd As Integer) As Integer

  Public Declare Unicode Function CameraConnect Lib "Litecad.dll" Alias "lcCameraConnect" (ByVal szName As String) As Integer

  Public Declare Function CameraDisconnect Lib "Litecad.dll" Alias "lcCameraDisconnect" () As Integer

  Public Declare Function CameraShot Lib "Litecad.dll" Alias "lcCameraShot" () As Integer

  Public Declare Function CreateDrawing Lib "Litecad.dll" Alias "lcCreateDrawing" () As Integer

  Public Declare Function DeleteDrawing Lib "Litecad.dll" Alias "lcDeleteDrawing" (ByVal hDrw As Integer) As Integer

  Public Declare Unicode Function DrwNew Lib "Litecad.dll" Alias "lcDrwNew" (ByVal hDrw As Integer, ByVal szFileName As String, ByVal hLcWnd As Integer) As Integer

  Public Declare Unicode Function DrwLoad Lib "Litecad.dll" Alias "lcDrwLoad" (ByVal hDrw As Integer, ByVal szFileName As String, ByVal hLcWnd As Integer) As Integer

  Public Declare Function DrwLoadMem Lib "Litecad.dll" Alias "lcDrwLoadMem" (ByVal hDrw As Integer, ByVal hMem As Integer, ByVal hLcWnd As Integer) As Integer

  Public Declare Unicode Function DrwInsert Lib "Litecad.dll" Alias "lcDrwInsert" (ByVal hDrw As Integer, ByVal szFileName As String, ByVal Overwrite As Integer, ByVal hLcWnd As Integer) As Integer

  Public Declare Function DrwCopy Lib "Litecad.dll" Alias "lcDrwCopy" (ByVal hDrw As Integer, ByVal hDrwSrc As Integer) As Integer

  Public Declare Unicode Function DrwSave Lib "Litecad.dll" Alias "lcDrwSave" (ByVal hDrw As Integer, ByVal szFileName As String, ByVal bBak As Integer, ByVal hLcWnd As Integer) As Integer

  Public Declare Function DrwSaveMem Lib "Litecad.dll" Alias "lcDrwSaveMem" (ByVal hDrw As Integer, ByVal hMem As Integer, ByVal MemSize As Integer) As Integer

  Public Declare Unicode Function DrwAddLayer Lib "Litecad.dll" Alias "lcDrwAddLayer" (ByVal hDrw As Integer, ByVal szName As String, ByVal szColor As String, ByVal hLtype As Integer, ByVal Lwidth As Integer) As Integer

  Public Declare Unicode Function DrwAddLinetype Lib "Litecad.dll" Alias "lcDrwAddLinetype" (ByVal hDrw As Integer, ByVal szName As String, ByVal szDefinition As String) As Integer

  Public Declare Unicode Function DrwAddLinetypeF Lib "Litecad.dll" Alias "lcDrwAddLinetypeF" (ByVal hDrw As Integer, ByVal szName As String, ByVal szFileName As String, ByVal szLtypeName As String) As Integer

  Public Declare Unicode Function DrwAddTextStyle Lib "Litecad.dll" Alias "lcDrwAddTextStyle" (ByVal hDrw As Integer, ByVal szName As String, ByVal szFontName As String, ByVal bWinFont As Integer) As Integer

  Public Declare Unicode Function DrwAddDimStyle Lib "Litecad.dll" Alias "lcDrwAddDimStyle" (ByVal hDrw As Integer, ByVal szName As String) As Integer

  Public Declare Unicode Function DrwAddMlineStyle Lib "Litecad.dll" Alias "lcDrwAddMlineStyle" (ByVal hDrw As Integer, ByVal szName As String) As Integer

  Public Declare Unicode Function DrwAddPntStyle Lib "Litecad.dll" Alias "lcDrwAddPntStyle" (ByVal hDrw As Integer, ByVal szName As String, ByVal hBlock As Integer, ByVal BlockScale As Double, ByVal hTStyle As Integer, ByVal TextHeight As Double, ByVal TextWidth As Double) As Integer

  Public Declare Unicode Function DrwAddImage Lib "Litecad.dll" Alias "lcDrwAddImage" (ByVal hDrw As Integer, ByVal szName As String, ByVal szFileName As String) As Integer

  Public Declare Unicode Function DrwAddImage2 Lib "Litecad.dll" Alias "lcDrwAddImage2" (ByVal hDrw As Integer, ByVal szName As String, ByVal Width As Integer, ByVal Height As Integer, ByVal nBits As Integer) As Integer

  Public Declare Unicode Function DrwAddImage3 Lib "Litecad.dll" Alias "lcDrwAddImage3" (ByVal hDrw As Integer, ByVal szName As String, ByVal hMem As Integer) As Integer

  Public Declare Unicode Function DrwAddBlock Lib "Litecad.dll" Alias "lcDrwAddBlock" (ByVal hDrw As Integer, ByVal szName As String, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Unicode Function DrwAddBlockFromFile Lib "Litecad.dll" Alias "lcDrwAddBlockFromFile" (ByVal hDrw As Integer, ByVal szName As String, ByVal szFileName As String, ByVal Overwrite As Integer, ByVal hwParent As Integer) As Integer

  Public Declare Unicode Function DrwAddBlockFromDrw Lib "Litecad.dll" Alias "lcDrwAddBlockFromDrw" (ByVal hDrw As Integer, ByVal szName As String, ByVal hDrw2 As Integer, ByVal Overwrite As Integer, ByVal hwParent As Integer) As Integer

  Public Declare Unicode Function DrwAddBlockFile Lib "Litecad.dll" Alias "lcDrwAddBlockFile" (ByVal hDrw As Integer, ByVal szName As String, ByVal szFileName As String, ByVal Overwrite As Integer, ByVal hwParent As Integer) As Integer

  Public Declare Unicode Function DrwAddBlockPaper Lib "Litecad.dll" Alias "lcDrwAddBlockPaper" (ByVal hDrw As Integer, ByVal szName As String, ByVal PaperSize As Integer, ByVal Orient As Integer, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Unicode Function DrwAddBlockCopy Lib "Litecad.dll" Alias "lcDrwAddBlockCopy" (ByVal hDrw As Integer, ByVal szName As String, ByVal hSrcBlock As Integer) As Integer

  Public Declare Function DrwDeleteObject Lib "Litecad.dll" Alias "lcDrwDeleteObject" (ByVal hDrw As Integer, ByVal hObject As Integer) As Integer

  Public Declare Function DrwDeleteUnused Lib "Litecad.dll" Alias "lcDrwDeleteUnused" (ByVal hDrw As Integer, ByVal ObjType As Integer) As Integer

  Public Declare Function DrwCountObjects Lib "Litecad.dll" Alias "lcDrwCountObjects" (ByVal hDrw As Integer, ByVal ObjType As Integer) As Integer

  Public Declare Function DrwSortObjects Lib "Litecad.dll" Alias "lcDrwSortObjects" (ByVal hDrw As Integer, ByVal ObjType As Integer) As Integer

  Public Declare Function DrwGetFirstObject Lib "Litecad.dll" Alias "lcDrwGetFirstObject" (ByVal hDrw As Integer, ByVal ObjType As Integer) As Integer

  Public Declare Function DrwGetNextObject Lib "Litecad.dll" Alias "lcDrwGetNextObject" (ByVal hDrw As Integer, ByVal hObject As Integer) As Integer

  Public Declare Function DrwGetObjectByID Lib "Litecad.dll" Alias "lcDrwGetObjectByID" (ByVal hDrw As Integer, ByVal ObjType As Integer, ByVal Id As Integer) As Integer

  Public Declare Unicode Function DrwGetObjectByIDH Lib "Litecad.dll" Alias "lcDrwGetObjectByIDH" (ByVal hDrw As Integer, ByVal ObjType As Integer, ByVal szId As String) As Integer

  Public Declare Unicode Function DrwGetObjectByName Lib "Litecad.dll" Alias "lcDrwGetObjectByName" (ByVal hDrw As Integer, ByVal ObjType As Integer, ByVal szName As String) As Integer

  Public Declare Function DrwGetEntByID Lib "Litecad.dll" Alias "lcDrwGetEntByID" (ByVal hDrw As Integer, ByVal Id As Integer) As Integer

  Public Declare Unicode Function DrwGetEntByIDH Lib "Litecad.dll" Alias "lcDrwGetEntByIDH" (ByVal hDrw As Integer, ByVal szId As String) As Integer

  Public Declare Function DrwGetEntByKey Lib "Litecad.dll" Alias "lcDrwGetEntByKey" (ByVal hDrw As Integer, ByVal Key As Integer) As Integer

  Public Declare Function DrwClearXData Lib "Litecad.dll" Alias "lcDrwClearXData" (ByVal hDrw As Integer, ByVal ObjType As Integer, ByVal Mode As Integer) As Integer

  Public Declare Function DrwPurge Lib "Litecad.dll" Alias "lcDrwPurge" (ByVal hDrw As Integer) As Integer

  Public Declare Function DrwExplode Lib "Litecad.dll" Alias "lcDrwExplode" (ByVal hDrw As Integer, ByVal Mode As Integer) As Integer

  Public Declare Function DrwUndoRecord Lib "Litecad.dll" Alias "lcDrwUndoRecord" (ByVal hDrw As Integer, ByVal Mode As Integer) As Integer

  Public Declare Function DrwUndo Lib "Litecad.dll" Alias "lcDrwUndo" (ByVal hDrw As Integer, ByVal bRedo As Integer) As Integer

  Public Declare Function BlockSetViewRect Lib "Litecad.dll" Alias "lcBlockSetViewRect" (ByVal hBlock As Integer, ByVal Xcen As Double, ByVal Ycen As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function BlockSetViewRect2 Lib "Litecad.dll" Alias "lcBlockSetViewRect2" (ByVal hBlock As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Rig As Double, ByVal Top As Double) As Integer

  Public Declare Function BlockSetPaperSize Lib "Litecad.dll" Alias "lcBlockSetPaperSize" (ByVal hBlock As Integer, ByVal PaperSize As Integer, ByVal Orient As Integer, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Unicode Function BlockRasterize Lib "Litecad.dll" Alias "lcBlockRasterize" (ByVal hBlock As Integer, ByVal szFileName As String, ByVal Xmin As Double, ByVal Ymin As Double, ByVal Xmax As Double, ByVal Ymax As Double, ByVal ImgW As Integer, ByVal ImgH As Integer) As Integer

  Public Declare Function BlockRasterizeMem Lib "Litecad.dll" Alias "lcBlockRasterizeMem" (ByVal hBlock As Integer, ByVal hMem As Integer, ByVal Xmin As Double, ByVal Ymin As Double, ByVal Xmax As Double, ByVal Ymax As Double, ByVal ImgW As Integer, ByVal ImgH As Integer) As Integer

  Public Declare Function BlockUpdate Lib "Litecad.dll" Alias "lcBlockUpdate" (ByVal hBlock As Integer, ByVal bUpdEnts As Integer, ByVal hNewEnt As Integer) As Integer

  Public Declare Function BlockScale Lib "Litecad.dll" Alias "lcBlockScale" (ByVal hBlock As Integer, ByVal Scale As Double) As Integer

  Public Declare Function BlockClear Lib "Litecad.dll" Alias "lcBlockClear" (ByVal hBlock As Integer, ByVal hLayer As Integer) As Integer

  Public Declare Function BlockPurge Lib "Litecad.dll" Alias "lcBlockPurge" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockSortEnts Lib "Litecad.dll" Alias "lcBlockSortEnts" (ByVal hBlock As Integer, ByVal bByLayers As Integer, ByVal hWnd As Integer) As Integer

  Public Declare Function BlockAddPoint Lib "Litecad.dll" Alias "lcBlockAddPoint" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function BlockAddPoint2 Lib "Litecad.dll" Alias "lcBlockAddPoint2" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal PtMode As Integer, ByVal PtSize As Double) As Integer

  Public Declare Function BlockAddPointTopo Lib "Litecad.dll" Alias "lcBlockAddPointTopo" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Z As Double) As Integer

  Public Declare Function BlockAddXline Lib "Litecad.dll" Alias "lcBlockAddXline" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Angle As Double, ByVal bRay As Integer) As Integer

  Public Declare Function BlockAddXline2P Lib "Litecad.dll" Alias "lcBlockAddXline2P" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal bRay As Integer) As Integer

  Public Declare Function BlockAddLine Lib "Litecad.dll" Alias "lcBlockAddLine" (ByVal hBlock As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function BlockAddLineDir Lib "Litecad.dll" Alias "lcBlockAddLineDir" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Angle As Double, ByVal Dist As Double) As Integer

  Public Declare Function BlockAddLineTan Lib "Litecad.dll" Alias "lcBlockAddLineTan" (ByVal hBlock As Integer, ByVal hEnt1 As Integer, ByVal hEnt2 As Integer, ByVal Mode As Integer) As Integer

  Public Declare Function BlockAddPolyline Lib "Litecad.dll" Alias "lcBlockAddPolyline" (ByVal hBlock As Integer, ByVal FitType As Integer, ByVal bClosed As Integer, ByVal bFilled As Integer) As Integer

  Public Declare Function BlockAddSpline Lib "Litecad.dll" Alias "lcBlockAddSpline" (ByVal hBlock As Integer, ByVal bClosed As Integer, ByVal bFilled As Integer) As Integer

  Public Declare Function BlockAddMline Lib "Litecad.dll" Alias "lcBlockAddMline" (ByVal hBlock As Integer, ByVal FitType As Integer, ByVal bClosed As Integer) As Integer

  Public Declare Function BlockAddRect Lib "Litecad.dll" Alias "lcBlockAddRect" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Width As Double, ByVal Height As Double, ByVal Angle As Double, ByVal bFilled As Integer) As Integer

  Public Declare Function BlockAddRect2 Lib "Litecad.dll" Alias "lcBlockAddRect2" (ByVal hBlock As Integer, ByVal Left As Double, ByVal Bottom As Double, ByVal Width As Double, ByVal Height As Double, ByVal Rad As Double, ByVal bFilled As Integer) As Integer

  Public Declare Function BlockAddCircle Lib "Litecad.dll" Alias "lcBlockAddCircle" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Radius As Double, ByVal bFilled As Integer) As Integer

  Public Declare Function BlockAddArc Lib "Litecad.dll" Alias "lcBlockAddArc" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Radius As Double, ByVal StartAngle As Double, ByVal ArcAngle As Double) As Integer

  Public Declare Function BlockAddArc3P Lib "Litecad.dll" Alias "lcBlockAddArc3P" (ByVal hBlock As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal X3 As Double, ByVal Y3 As Double) As Integer

  Public Declare Function BlockAddEllipse Lib "Litecad.dll" Alias "lcBlockAddEllipse" (ByVal hBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal R1 As Double, ByVal R2 As Double, ByVal RotAngle As Double, ByVal StartAngle As Double, ByVal ArcAngle As Double) As Integer

  Public Declare Unicode Function BlockAddText Lib "Litecad.dll" Alias "lcBlockAddText" (ByVal hBlock As Integer, ByVal szText As String, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Unicode Function BlockAddText2 Lib "Litecad.dll" Alias "lcBlockAddText2" (ByVal hBlock As Integer, ByVal szText As String, ByVal X As Double, ByVal Y As Double, ByVal Align As Integer, ByVal H As Double, ByVal WScale As Double, ByVal RotAngle As Double, ByVal Oblique As Double) As Integer

  Public Declare Unicode Function BlockAddText3 Lib "Litecad.dll" Alias "lcBlockAddText3" (ByVal hBlock As Integer, ByVal szText As String, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal Align As Integer, ByVal HW As Double, ByVal Oblique As Double) As Integer

  Public Declare Unicode Function BlockAddMText Lib "Litecad.dll" Alias "lcBlockAddMText" (ByVal hBlock As Integer, ByVal szText As String, ByVal X As Double, ByVal Y As Double, ByVal WrapWidth As Double, ByVal Align As Integer, ByVal RotAngle As Double, ByVal H As Double, ByVal WScale As Double) As Integer

  Public Declare Unicode Function BlockAddArcText Lib "Litecad.dll" Alias "lcBlockAddArcText" (ByVal hBlock As Integer, ByVal szText As String, ByVal X As Double, ByVal Y As Double, ByVal Radius As Double, ByVal StartAngle As Double, ByVal bClockwise As Integer, ByVal H As Double, ByVal WScale As Double, ByVal Align As Integer) As Integer

  Public Declare Function BlockAddBlockRef Lib "Litecad.dll" Alias "lcBlockAddBlockRef" (ByVal hBlock As Integer, ByVal hRefBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Function BlockAddBlockRefID Lib "Litecad.dll" Alias "lcBlockAddBlockRefID" (ByVal hBlock As Integer, ByVal idRefBlock As Integer, ByVal X As Double, ByVal Y As Double, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Unicode Function BlockAddBlockRefIDH Lib "Litecad.dll" Alias "lcBlockAddBlockRefIDH" (ByVal hBlock As Integer, ByVal szIdRefBlock As String, ByVal X As Double, ByVal Y As Double, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Function BlockAddImageRef Lib "Litecad.dll" Alias "lcBlockAddImageRef" (ByVal hBlock As Integer, ByVal hImage As Integer, ByVal X As Double, ByVal Y As Double, ByVal Width As Double, ByVal Height As Double, ByVal bBorder As Integer) As Integer

  Public Declare Unicode Function BlockAddEcw Lib "Litecad.dll" Alias "lcBlockAddEcw" (ByVal hBlock As Integer, ByVal szFileName As String) As Integer

  Public Declare Unicode Function BlockAddBarcode Lib "Litecad.dll" Alias "lcBlockAddBarcode" (ByVal hBlock As Integer, ByVal BarType As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Width As Double, ByVal Height As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddHatch Lib "Litecad.dll" Alias "lcBlockAddHatch" (ByVal hBlock As Integer, ByVal szFileName As String, ByVal szPatName As String, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Function BlockAddViewport Lib "Litecad.dll" Alias "lcBlockAddViewport" (ByVal hBlock As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Width As Double, ByVal Height As Double, ByVal DrwPntX As Double, ByVal DrwPntY As Double, ByVal DrwScale As Double, ByVal DrwAngle As Double) As Integer

  Public Declare Function BlockAddFace Lib "Litecad.dll" Alias "lcBlockAddFace" (ByVal hBlock As Integer, ByVal Flags As Integer, ByVal x0 As Double, ByVal y0 As Double, ByVal z0 As Double, ByVal x1 As Double, ByVal y1 As Double, ByVal z1 As Double, ByVal x2 As Double, ByVal y2 As Double, ByVal z2 As Double) As Integer

  Public Declare Function BlockAddFace4 Lib "Litecad.dll" Alias "lcBlockAddFace4" (ByVal hBlock As Integer, ByVal Flags As Integer, ByVal x0 As Double, ByVal y0 As Double, ByVal z0 As Double, ByVal x1 As Double, ByVal y1 As Double, ByVal z1 As Double, ByVal x2 As Double, ByVal y2 As Double, ByVal z2 As Double, ByVal x3 As Double, ByVal y3 As Double, ByVal z3 As Double) As Integer

  Public Declare Unicode Function BlockAddLeader Lib "Litecad.dll" Alias "lcBlockAddLeader" (ByVal hBlock As Integer, ByVal szText As String, ByVal Xt As Double, ByVal Yt As Double, ByVal LandDist As Double, ByVal Xa As Double, ByVal Ya As Double, ByVal Attach As Integer, ByVal Align As Integer) As Integer

  Public Declare Unicode Function BlockAddDimLin Lib "Litecad.dll" Alias "lcBlockAddDimLin" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal Xt As Double, ByVal Yt As Double, ByVal Angle As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimHor Lib "Litecad.dll" Alias "lcBlockAddDimHor" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal Yt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimVer Lib "Litecad.dll" Alias "lcBlockAddDimVer" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal Xt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimAli Lib "Litecad.dll" Alias "lcBlockAddDimAli" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal Xt As Double, ByVal Yt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimAli2 Lib "Litecad.dll" Alias "lcBlockAddDimAli2" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal Dt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimOrd Lib "Litecad.dll" Alias "lcBlockAddDimOrd" (ByVal hBlock As Integer, ByVal Xd As Double, ByVal Yd As Double, ByVal Xt As Double, ByVal Yt As Double, ByVal bX As Integer, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimRad Lib "Litecad.dll" Alias "lcBlockAddDimRad" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Xr As Double, ByVal Yr As Double, ByVal Xt As Double, ByVal Yt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimRad2 Lib "Litecad.dll" Alias "lcBlockAddDimRad2" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal Angle As Double, ByVal TextOff As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimDia Lib "Litecad.dll" Alias "lcBlockAddDimDia" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Xr As Double, ByVal Yr As Double, ByVal Xt As Double, ByVal Yt As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimDia2 Lib "Litecad.dll" Alias "lcBlockAddDimDia2" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal Angle As Double, ByVal TextOff As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimAng Lib "Litecad.dll" Alias "lcBlockAddDimAng" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal Xa As Double, ByVal Ya As Double, ByVal TextPos As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function BlockAddDimAng2 Lib "Litecad.dll" Alias "lcBlockAddDimAng2" (ByVal hBlock As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal X3 As Double, ByVal Y3 As Double, ByVal X4 As Double, ByVal Y4 As Double, ByVal Xa As Double, ByVal Ya As Double, ByVal TextPos As Double, ByVal szText As String) As Integer

  Public Declare Function BlockAddRPlan Lib "Litecad.dll" Alias "lcBlockAddRPlan" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockAddMpgon Lib "Litecad.dll" Alias "lcBlockAddMpgon" (ByVal hBlock As Integer, ByVal hMpgon As Integer) As Integer

  Public Declare Function BlockAddArrow Lib "Litecad.dll" Alias "lcBlockAddArrow" (ByVal hBlock As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function BlockAddSpiral Lib "Litecad.dll" Alias "lcBlockAddSpiral" (ByVal hBlock As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal Turns As Double, ByVal bDirCW As Integer, ByVal bClosed As Integer) As Integer

  Public Declare Function BlockAddCamview Lib "Litecad.dll" Alias "lcBlockAddCamview" (ByVal hBlock As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function BlockAddClone Lib "Litecad.dll" Alias "lcBlockAddClone" (ByVal hBlock As Integer, ByVal hEnt As Integer) As Integer

  Public Declare Function BlockGetFirstEnt Lib "Litecad.dll" Alias "lcBlockGetFirstEnt" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockGetNextEnt Lib "Litecad.dll" Alias "lcBlockGetNextEnt" (ByVal hBlock As Integer, ByVal hEnt As Integer) As Integer

  Public Declare Function BlockGetLastEnt Lib "Litecad.dll" Alias "lcBlockGetLastEnt" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockGetPrevEnt Lib "Litecad.dll" Alias "lcBlockGetPrevEnt" (ByVal hBlock As Integer, ByVal hEnt As Integer) As Integer

  Public Declare Function BlockGetEntByID Lib "Litecad.dll" Alias "lcBlockGetEntByID" (ByVal hBlock As Integer, ByVal Id As Integer) As Integer

  Public Declare Unicode Function BlockGetEntByIDH Lib "Litecad.dll" Alias "lcBlockGetEntByIDH" (ByVal hBlock As Integer, ByVal szId As String) As Integer

  Public Declare Function BlockGetEntByKey Lib "Litecad.dll" Alias "lcBlockGetEntByKey" (ByVal hBlock As Integer, ByVal Key As Integer) As Integer

  Public Declare Function BlockUnselect Lib "Litecad.dll" Alias "lcBlockUnselect" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockSelectEnt Lib "Litecad.dll" Alias "lcBlockSelectEnt" (ByVal hBlock As Integer, ByVal hEntity As Integer, ByVal bSelect As Integer) As Integer

  Public Declare Function BlockSelErase Lib "Litecad.dll" Alias "lcBlockSelErase" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockSelMove Lib "Litecad.dll" Alias "lcBlockSelMove" (ByVal hBlock As Integer, ByVal dX As Double, ByVal dY As Double, ByVal bCopy As Integer, ByVal bNewSelect As Integer) As Integer

  Public Declare Function BlockSelScale Lib "Litecad.dll" Alias "lcBlockSelScale" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal Scal As Double, ByVal bCopy As Integer, ByVal bNewSelect As Integer) As Integer

  Public Declare Function BlockSelRotate Lib "Litecad.dll" Alias "lcBlockSelRotate" (ByVal hBlock As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal Angle As Double, ByVal bCopy As Integer, ByVal bNewSelect As Integer) As Integer

  Public Declare Function BlockSelMirror Lib "Litecad.dll" Alias "lcBlockSelMirror" (ByVal hBlock As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal bCopy As Integer, ByVal bNewSelect As Integer) As Integer

  Public Declare Function BlockSelExplode Lib "Litecad.dll" Alias "lcBlockSelExplode" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockSelJoin Lib "Litecad.dll" Alias "lcBlockSelJoin" (ByVal hBlock As Integer, ByVal Delta As Double) As Integer

  Public Declare Function BlockSelAlign Lib "Litecad.dll" Alias "lcBlockSelAlign" (ByVal hBlock As Integer, ByVal Mode As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Unicode Function BlockSelBlock Lib "Litecad.dll" Alias "lcBlockSelBlock" (ByVal hBlock As Integer, ByVal szName As String, ByVal X As Double, ByVal Y As Double, ByVal Mode As Integer, ByVal bOverwrite As Integer) As Integer

  Public Declare Function BlockGetFirstSel Lib "Litecad.dll" Alias "lcBlockGetFirstSel" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockGetNextSel Lib "Litecad.dll" Alias "lcBlockGetNextSel" (ByVal hBlock As Integer) As Integer

  Public Declare Function BlockOrderByLayers Lib "Litecad.dll" Alias "lcBlockOrderByLayers" (ByVal hBlock As Integer, ByVal hWnd As Integer) As Integer

  Public Declare Function BlockSortTSP Lib "Litecad.dll" Alias "lcBlockSortTSP" (ByVal hBlock As Integer, ByVal hLayer As Integer, ByVal hWnd As Integer) As Integer

  Public Declare Function BlockGetJumpsLen Lib "Litecad.dll" Alias "lcBlockGetJumpsLen" (ByVal hBlock As Integer, ByVal hLayer As Integer, ByVal hWnd As Integer) As Double

  Public Declare Function LayerClear Lib "Litecad.dll" Alias "lcLayerClear" (ByVal hLayer As Integer, ByVal hBlock As Integer) As Integer

  Public Declare Function EntErase Lib "Litecad.dll" Alias "lcEntErase" (ByVal hEnt As Integer, ByVal bErase As Integer) As Integer

  Public Declare Function EntMove Lib "Litecad.dll" Alias "lcEntMove" (ByVal hEnt As Integer, ByVal dX As Double, ByVal dY As Double) As Integer

  Public Declare Function EntScale Lib "Litecad.dll" Alias "lcEntScale" (ByVal hEnt As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal Scal As Double) As Integer

  Public Declare Function EntRotate Lib "Litecad.dll" Alias "lcEntRotate" (ByVal hEnt As Integer, ByVal X0 As Double, ByVal Y0 As Double, ByVal Angle As Double) As Integer

  Public Declare Function EntMirror Lib "Litecad.dll" Alias "lcEntMirror" (ByVal hEnt As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function EntExplode Lib "Litecad.dll" Alias "lcEntExplode" (ByVal hEnt As Integer, ByVal bSelect As Integer) As Integer

  Public Declare Function EntOffset Lib "Litecad.dll" Alias "lcEntOffset" (ByVal hEnt As Integer, ByVal Dist As Double) As Integer

  Public Declare Function EntToTop Lib "Litecad.dll" Alias "lcEntToTop" (ByVal hEnt As Integer) As Integer

  Public Declare Function EntToBottom Lib "Litecad.dll" Alias "lcEntToBottom" (ByVal hEnt As Integer) As Integer

  Public Declare Function EntToAbove Lib "Litecad.dll" Alias "lcEntToAbove" (ByVal hEnt As Integer, ByVal hEnt2 As Integer) As Integer

  Public Declare Function EntToUnder Lib "Litecad.dll" Alias "lcEntToUnder" (ByVal hEnt As Integer, ByVal hEnt2 As Integer) As Integer

  Public Declare Function EntGetGrip Lib "Litecad.dll" Alias "lcEntGetGrip" (ByVal hEnt As Integer, ByVal iGrip As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function EntPutGrip Lib "Litecad.dll" Alias "lcEntPutGrip" (ByVal hEnt As Integer, ByVal iGrip As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function EntUpdate Lib "Litecad.dll" Alias "lcEntUpdate" (ByVal hEnt As Integer) As Integer

  Public Declare Function EntCopyBase Lib "Litecad.dll" Alias "lcEntCopyBase" (ByVal hEnt As Integer, ByVal hEntFrom As Integer) As Integer

  Public Declare Function Intersection Lib "Litecad.dll" Alias "lcIntersection" (ByVal hEnt As Integer, ByVal hEnt2 As Integer) As Integer

  Public Declare Function InterGetPoint Lib "Litecad.dll" Alias "lcInterGetPoint" (ByVal iPoint As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function PlineAddVer Lib "Litecad.dll" Alias "lcPlineAddVer" (ByVal hPline As Integer, ByVal hVer As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function PlineAddVer2 Lib "Litecad.dll" Alias "lcPlineAddVer2" (ByVal hPline As Integer, ByVal hVer As Integer, ByVal X As Double, ByVal Y As Double, ByVal Param As Double, ByVal W0 As Double, ByVal W1 As Double) As Integer

  Public Declare Function PlineAddVerDir Lib "Litecad.dll" Alias "lcPlineAddVerDir" (ByVal hPline As Integer, ByVal hVer As Integer, ByVal Ang As Double, ByVal Length As Double) As Integer

  Public Declare Function PlineEnd Lib "Litecad.dll" Alias "lcPlineEnd" (ByVal hPline As Integer) As Integer

  Public Declare Function PlineDeleteVer Lib "Litecad.dll" Alias "lcPlineDeleteVer" (ByVal hPline As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function PlineGetFirstVer Lib "Litecad.dll" Alias "lcPlineGetFirstVer" (ByVal hPline As Integer) As Integer

  Public Declare Function PlineGetNextVer Lib "Litecad.dll" Alias "lcPlineGetNextVer" (ByVal hPline As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function PlineGetLastVer Lib "Litecad.dll" Alias "lcPlineGetLastVer" (ByVal hPline As Integer) As Integer

  Public Declare Function PlineGetPrevVer Lib "Litecad.dll" Alias "lcPlineGetPrevVer" (ByVal hPline As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function PlineGetVer Lib "Litecad.dll" Alias "lcPlineGetVer" (ByVal hPline As Integer, ByVal Index As Integer) As Integer

  Public Declare Function PlineGetVerPt Lib "Litecad.dll" Alias "lcPlineGetVerPt" (ByVal hPline As Integer, ByVal X As Double, ByVal Y As Double, ByVal Delta As Double) As Integer

  Public Declare Function PlineGetSeg Lib "Litecad.dll" Alias "lcPlineGetSeg" (ByVal hPline As Integer, ByVal X As Double, ByVal Y As Double, ByVal Delta As Double) As Integer

  Public Declare Function PlineReverse Lib "Litecad.dll" Alias "lcPlineReverse" (ByVal hPline As Integer) As Integer

  Public Declare Function PlineContainPoint Lib "Litecad.dll" Alias "lcPlineContainPoint" (ByVal hPline As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function PlineGetRoundPrm Lib "Litecad.dll" Alias "lcPlineGetRoundPrm" (ByVal hPline As Integer, ByVal hVer As Integer, ByRef px0 As Double, ByRef py0 As Double, ByRef pBulge As Double, ByRef px1 As Double, ByRef py1 As Double) As Integer

  Public Declare Function XlinePutDir Lib "Litecad.dll" Alias "lcXlinePutDir" (ByVal hXline As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function RectGetPolyline Lib "Litecad.dll" Alias "lcRectGetPolyline" (ByVal hRect As Integer, ByRef pX As Double, ByRef pY As Double, ByRef pBulge As Double) As Integer

  Public Declare Function ImgRefGetPixel Lib "Litecad.dll" Alias "lcImgRefGetPixel" (ByVal hImgRef As Integer, ByVal iX As Integer, ByVal iY As Integer, ByRef pX As Double, ByRef pY As Double, ByRef pColor As Integer) As Integer

  Public Declare Function ImgRefResize Lib "Litecad.dll" Alias "lcImgRefResize" (ByVal hImgRef As Integer, ByVal NewWidth As Integer, ByVal NewHeight As Integer, ByVal Method As Integer) As Integer

  Public Declare Unicode Function HatchSetPattern Lib "Litecad.dll" Alias "lcHatchSetPattern" (ByVal hHatch As Integer, ByVal szFileName As String, ByVal szPatName As String, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Function HatchBoundStart Lib "Litecad.dll" Alias "lcHatchBoundStart" (ByVal hHatch As Integer) As Integer

  Public Declare Function HatchBoundPoint Lib "Litecad.dll" Alias "lcHatchBoundPoint" (ByVal hHatch As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function HatchBoundEntity Lib "Litecad.dll" Alias "lcHatchBoundEntity" (ByVal hHatch As Integer, ByVal hEnt As Integer) As Integer

  Public Declare Function HatchBoundEndLoop Lib "Litecad.dll" Alias "lcHatchBoundEndLoop" (ByVal hHatch As Integer) As Integer

  Public Declare Function HatchBoundEnd Lib "Litecad.dll" Alias "lcHatchBoundEnd" (ByVal hHatch As Integer) As Integer

  Public Declare Function HatchPatStart Lib "Litecad.dll" Alias "lcHatchPatStart" (ByVal hHatch As Integer) As Integer

  Public Declare Function HatchPatLine Lib "Litecad.dll" Alias "lcHatchPatLine" (ByVal hHatch As Integer, ByVal Angle As Double, ByVal x0 As Double, ByVal y0 As Double, ByVal dx As Double, ByVal dy As Double, ByVal nDash As Integer, ByVal L0 As Double, ByVal L1 As Double, ByVal L2 As Double, ByVal L3 As Double, ByVal L4 As Double, ByVal L5 As Double, ByVal L6 As Double, ByVal L7 As Double) As Integer

  Public Declare Function HatchPatEnd Lib "Litecad.dll" Alias "lcHatchPatEnd" (ByVal hHatch As Integer) As Integer

  Public Declare Function HatchGetLoopSize Lib "Litecad.dll" Alias "lcHatchGetLoopSize" (ByVal hHatch As Integer, ByVal iLoop As Integer) As Integer

  Public Declare Function HatchGetPoint Lib "Litecad.dll" Alias "lcHatchGetPoint" (ByVal hHatch As Integer, ByVal iPnt As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function RPlanAddVer Lib "Litecad.dll" Alias "lcRPlanAddVer" (ByVal hRPlan As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function RPlanSetCurve Lib "Litecad.dll" Alias "lcRPlanSetCurve" (ByVal hVer As Integer, ByVal Radius As Double, ByVal LenClot1 As Double, ByVal LenClot2 As Double) As Integer

  Public Declare Function RPlanSetPos Lib "Litecad.dll" Alias "lcRPlanSetPos" (ByVal hVer As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function RPlanDeleteVer Lib "Litecad.dll" Alias "lcRPlanDeleteVer" (ByVal hRPlan As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function RPlanGetFirstVer Lib "Litecad.dll" Alias "lcRPlanGetFirstVer" (ByVal hRPlan As Integer) As Integer

  Public Declare Function RPlanGetNextVer Lib "Litecad.dll" Alias "lcRPlanGetNextVer" (ByVal hRPlan As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function RPlanGetLastVer Lib "Litecad.dll" Alias "lcRPlanGetLastVer" (ByVal hRPlan As Integer) As Integer

  Public Declare Function RPlanGetPrevVer Lib "Litecad.dll" Alias "lcRPlanGetPrevVer" (ByVal hRPlan As Integer, ByVal hVer As Integer) As Integer

  Public Declare Function RPlanGetVer Lib "Litecad.dll" Alias "lcRPlanGetVer" (ByVal hRPlan As Integer, ByVal Index As Integer) As Integer

  Public Declare Function RPlanGetPoint Lib "Litecad.dll" Alias "lcRPlanGetPoint" (ByVal hRPlan As Integer, ByVal Dist As Double, ByRef pX As Double, ByRef pY As Double, ByRef pAngle As Double, ByRef pSide As Integer) As Integer

  Public Declare Function RPlanGetDist Lib "Litecad.dll" Alias "lcRPlanGetDist" (ByVal hRPlan As Integer, ByVal X As Double, ByVal Y As Double, ByRef pX2 As Double, ByRef pY2 As Double, ByRef pDist As Double, ByRef pOffset As Double) As Integer

  Public Declare Function VportSetView Lib "Litecad.dll" Alias "lcVportSetView" (ByVal hVport As Integer, ByVal Xcen As Double, ByVal Ycen As Double, ByVal Scal As Double, ByVal Angle As Double) As Integer

  Public Declare Function VportLayerDlg Lib "Litecad.dll" Alias "lcVportLayerDlg" (ByVal hVport As Integer, ByVal hLcWnd As Integer) As Integer

  Public Declare Function VportLayerCmd Lib "Litecad.dll" Alias "lcVportLayerCmd" (ByVal hVport As Integer, ByVal Cmd As Integer, ByVal hLayer As Integer) As Integer

  Public Declare Unicode Function ColorIsRGB Lib "Litecad.dll" Alias "lcColorIsRGB" (ByVal szColor As String) As Integer

  Public Declare Unicode Function ColorGetRed Lib "Litecad.dll" Alias "lcColorGetRed" (ByVal szColor As String) As Integer

  Public Declare Unicode Function ColorGetGreen Lib "Litecad.dll" Alias "lcColorGetGreen" (ByVal szColor As String) As Integer

  Public Declare Unicode Function ColorGetBlue Lib "Litecad.dll" Alias "lcColorGetBlue" (ByVal szColor As String) As Integer

  Public Declare Unicode Function ColorGetIndex Lib "Litecad.dll" Alias "lcColorGetIndex" (ByVal szColor As String) As Integer

  Public Declare Unicode Function ColorToVal Lib "Litecad.dll" Alias "lcColorToVal" (ByVal szColor As String, ByRef pbRGB As Integer, ByRef pIndex As Integer, ByRef pR As Integer, ByRef pG As Integer, ByRef pB As Integer) As Integer

  Public Declare Function ColorSetPalette Lib "Litecad.dll" Alias "lcColorSetPalette" (ByVal Index As Integer, ByVal R As Integer, ByVal G As Integer, ByVal B As Integer) As Integer

  Public Declare Function ColorGetPalette Lib "Litecad.dll" Alias "lcColorGetPalette" (ByVal Index As Integer, ByRef pR As Integer, ByRef pG As Integer, ByRef pB As Integer) As Integer

  Public Declare Function ColorSetTopo Lib "Litecad.dll" Alias "lcColorSetTopo" (ByVal Index As Integer, ByVal R As Integer, ByVal G As Integer, ByVal B As Integer) As Integer

  Public Declare Function ImageSetPixelRGB Lib "Litecad.dll" Alias "lcImageSetPixelRGB" (ByVal hImage As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal Red As Integer, ByVal Green As Integer, ByVal Blue As Integer) As Integer

  Public Declare Function ImageSetPixelI Lib "Litecad.dll" Alias "lcImageSetPixelI" (ByVal hImage As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal iColor As Integer) As Integer

  Public Declare Function ImageGetPixelRGB Lib "Litecad.dll" Alias "lcImageGetPixelRGB" (ByVal hImage As Integer, ByVal X As Integer, ByVal Y As Integer, ByRef pRed As Integer, ByRef pGreen As Integer, ByRef pBlue As Integer) As Integer

  Public Declare Function ImageGetPixelI Lib "Litecad.dll" Alias "lcImageGetPixelI" (ByVal hImage As Integer, ByVal X As Integer, ByVal Y As Integer, ByRef piColor As Integer) As Integer

  Public Declare Function ImageSetPalColor Lib "Litecad.dll" Alias "lcImageSetPalColor" (ByVal hImage As Integer, ByVal iColor As Integer, ByVal Red As Integer, ByVal Green As Integer, ByVal Blue As Integer) As Integer

  Public Declare Function ImageGetPalColor Lib "Litecad.dll" Alias "lcImageGetPalColor" (ByVal hImage As Integer, ByVal iColor As Integer, ByRef pRed As Integer, ByRef pGreen As Integer, ByRef pBlue As Integer) As Integer

  Public Declare Unicode Function ImageLoad Lib "Litecad.dll" Alias "lcImageLoad" (ByVal hImage As Integer, ByVal szFilename As String, ByVal hWnd As Integer) As Integer

  Public Declare Function ImageLoadDIB Lib "Litecad.dll" Alias "lcImageLoadDIB" (ByVal hImage As Integer, ByVal hDib2 As Integer) As Integer

  Public Declare Function ImageCopyQuad Lib "Litecad.dll" Alias "lcImageCopyQuad" (ByVal hImage As Integer, ByVal hImageSrc As Integer, ByVal W As Integer, ByVal H As Integer, ByVal x0 As Double, ByVal y0 As Double, ByVal x1 As Double, ByVal y1 As Double, ByVal x2 As Double, ByVal y2 As Double, ByVal x3 As Double, ByVal y3 As Double) As Integer

  Public Declare Function FilletSetLines Lib "Litecad.dll" Alias "lcFilletSetLines" (ByVal L1x0 As Double, ByVal L1y0 As Double, ByVal L1x1 As Double, ByVal L1y1 As Double, ByVal L2x0 As Double, ByVal L2y0 As Double, ByVal L2x1 As Double, ByVal L2y1 As Double) As Integer

  Public Declare Function Fillet Lib "Litecad.dll" Alias "lcFillet" (ByVal Rad As Double, ByVal Bis As Double, ByVal Tang As Double) As Integer

  Public Declare Function FilletGetPoint Lib "Litecad.dll" Alias "lcFilletGetPoint" (ByVal iPnt As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function WFieldSet Lib "Litecad.dll" Alias "lcWFieldSet" (ByVal hDrw As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function WFieldClear Lib "Litecad.dll" Alias "lcWFieldClear" (ByVal hDrw As Integer) As Integer

  Public Declare Function WFieldDivide Lib "Litecad.dll" Alias "lcWFieldDivide" (ByVal hDrw As Integer, ByVal NumX As Integer, ByVal NumY As Integer, ByVal bClearExist As Integer) As Integer

  Public Declare Function WFieldAddCR Lib "Litecad.dll" Alias "lcWFieldAddCR" (ByVal hDrw As Integer, ByVal ID As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function WFieldSetCR Lib "Litecad.dll" Alias "lcWFieldSetCR" (ByVal hDrw As Integer, ByVal ID As Integer, ByVal Lef As Double, ByVal Bot As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function WFieldDeleteCR Lib "Litecad.dll" Alias "lcWFieldDeleteCR" (ByVal hDrw As Integer, ByVal ID As Integer) As Integer

  Public Declare Function WFieldActiveCR Lib "Litecad.dll" Alias "lcWFieldActiveCR" (ByVal hDrw As Integer, ByVal ID As Integer) As Integer

  Public Declare Unicode Function WFieldSave Lib "Litecad.dll" Alias "lcWFieldSave" (ByVal hDrw As Integer, ByVal szFileName As String) As Integer

  Public Declare Unicode Function WFieldLoad Lib "Litecad.dll" Alias "lcWFieldLoad" (ByVal hDrw As Integer, ByVal szFileName As String) As Integer

  Public Declare Function WFieldFirstCR Lib "Litecad.dll" Alias "lcWFieldFirstCR" (ByVal hDrw As Integer) As Integer

  Public Declare Function WFieldNextCR Lib "Litecad.dll" Alias "lcWFieldNextCR" (ByVal hDrw As Integer, ByVal hEnt As Integer) As Integer

  Public Declare Function ExpEntity Lib "Litecad.dll" Alias "lcExpEntity" (ByVal hEnt As Integer, ByVal Flags As Integer, ByVal bUnrotate As Integer) As Integer

  Public Declare Function ExpGetPline Lib "Litecad.dll" Alias "lcExpGetPline" (ByVal Delta As Double) As Integer

  Public Declare Function ExpGetVertex Lib "Litecad.dll" Alias "lcExpGetVertex" (ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Unicode Function GbrLoad Lib "Litecad.dll" Alias "lcGbrLoad" (ByVal hLcWnd As Integer, ByVal szFileName0 As String) As Integer

  Public Declare Function GbrClose Lib "Litecad.dll" Alias "lcGbrClose" (ByVal hLcWnd As Integer) As Integer

  Public Declare Unicode Function PlugGetOption Lib "Litecad.dll" Alias "lcPlugGetOption" (ByVal szFileName As String, ByVal szKey As String) As Char

  Public Declare Unicode Function PlugGetOption2 Lib "Litecad.dll" Alias "lcPlugGetOption2" (ByVal szFileName As String, ByVal szKey As String) As Integer

  Public Declare Unicode Function PlugSetOption Lib "Litecad.dll" Alias "lcPlugSetOption" (ByVal szFileName As String, ByVal szKey As String, ByVal szValue As String, ByVal bSave As Integer) As Integer

  Public Declare Unicode Function Mru_EnableFileExt Lib "Litecad.dll" Alias "lcMru_EnableFileExt" (ByVal szFileExt As String) As Integer

  Public Declare Unicode Function Mru_Load Lib "Litecad.dll" Alias "lcMru_Load" (ByVal szFileName As String) As Integer

  Public Declare Function Mru_Save Lib "Litecad.dll" Alias "lcMru_Save" () As Integer

  Public Declare Unicode Function Mru_AddFile Lib "Litecad.dll" Alias "lcMru_AddFile" (ByVal szFileName As String, ByVal bFileHasView As Integer) As Integer

  Public Declare Unicode Function Mru_SetImage Lib "Litecad.dll" Alias "lcMru_SetImage" (ByVal szFileName As String, ByVal pImgBuf As Integer, ByVal ImgSize As Integer) As Integer

  Public Declare Unicode Function Mru_HasImage Lib "Litecad.dll" Alias "lcMru_HasImage" (ByVal szFileName As String) As Integer

  Public Declare Unicode Function Mru_SetViewRect Lib "Litecad.dll" Alias "lcMru_SetViewRect" (ByVal szFileName As String, ByVal Xmin As Double, ByVal Ymin As Double, ByVal Xmax As Double, ByVal Ymax As Double) As Integer

  Public Declare Unicode Function Mru_GetViewRect Lib "Litecad.dll" Alias "lcMru_GetViewRect" (ByVal szFileName As String, ByRef pXmin As Double, ByRef pYmin As Double, ByRef pXmax As Double, ByRef pYmax As Double) As Integer

  Public Declare Function Mru_Dialog Lib "Litecad.dll" Alias "lcMru_Dialog" (ByVal hWnd As Integer, ByVal hIcon As Integer, ByVal szFileName As Integer) As Integer

  Public Declare Function WndTabClear Lib "Litecad.dll" Alias "lcWndTabClear" (ByVal hLcWnd As Integer) As Integer

  Public Declare Unicode Function WndTabAdd Lib "Litecad.dll" Alias "lcWndTabAdd" (ByVal hLcWnd As Integer, ByVal TabID As Integer, ByVal szLabel As String, ByVal szTipText As String, ByVal hObject As Integer) As Integer

  Public Declare Function WndTabSelect Lib "Litecad.dll" Alias "lcWndTabSelect" (ByVal hLcWnd As Integer, ByVal TabID As Integer) As Integer

  Public Declare Function WndPaperEnable Lib "Litecad.dll" Alias "lcWndPaperEnable" (ByVal hLcWnd As Integer, ByVal bEnable As Integer) As Integer

  Public Declare Function WndPaperSetSize Lib "Litecad.dll" Alias "lcWndPaperSetSize" (ByVal hLcWnd As Integer, ByVal Size As Integer, ByVal Orient As Integer) As Integer

  Public Declare Function WndPaperSetSize2 Lib "Litecad.dll" Alias "lcWndPaperSetSize2" (ByVal hLcWnd As Integer, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function WndPaperSetPos Lib "Litecad.dll" Alias "lcWndPaperSetPos" (ByVal hLcWnd As Integer, ByVal Left As Double, ByVal Bottom As Double) As Integer

  Public Declare Function GripClear Lib "Litecad.dll" Alias "lcGripClear" (ByVal hLcWnd As Integer) As Integer

  Public Declare Function GripAdd Lib "Litecad.dll" Alias "lcGripAdd" (ByVal hLcWnd As Integer, ByVal hObj As Integer, ByVal iGrip As Integer, ByVal Typ As Integer, ByVal X As Double, ByVal Y As Double, ByVal Ang As Double, ByVal X0 As Double, ByVal Y0 As Double) As Integer

  Public Declare Function GripSet Lib "Litecad.dll" Alias "lcGripSet" (ByVal hLcWnd As Integer, ByVal hObj As Integer, ByVal iGrip As Integer, ByVal X As Double, ByVal Y As Double, ByVal Ang As Double, ByVal X0 As Double, ByVal Y0 As Double) As Integer

  Public Declare Function Paint_PenCreate Lib "Litecad.dll" Alias "lcPaint_PenCreate" (ByVal hLcWnd As Integer, ByVal Id As Integer, ByVal Color As Integer, ByVal Width As Double, ByVal PenStyle As Integer) As Integer

  Public Declare Function Paint_PenSelect Lib "Litecad.dll" Alias "lcPaint_PenSelect" (ByVal hLcWnd As Integer, ByVal hPen As Integer) As Integer

  Public Declare Function Paint_PenSelectID Lib "Litecad.dll" Alias "lcPaint_PenSelectID" (ByVal hLcWnd As Integer, ByVal IdPen As Integer) As Integer

  Public Declare Function Paint_BrushCreate Lib "Litecad.dll" Alias "lcPaint_BrushCreate" (ByVal hLcWnd As Integer, ByVal Id As Integer, ByVal Color As Integer, ByVal Pattern As Integer, ByVal Alpha As Integer) As Integer

  Public Declare Function Paint_BrushSelect Lib "Litecad.dll" Alias "lcPaint_BrushSelect" (ByVal hLcWnd As Integer, ByVal hBrush As Integer) As Integer

  Public Declare Function Paint_BrushSelectID Lib "Litecad.dll" Alias "lcPaint_BrushSelectID" (ByVal hLcWnd As Integer, ByVal IdBrush As Integer) As Integer

  Public Declare Function Paint_DrawPtbuf Lib "Litecad.dll" Alias "lcPaint_DrawPtbuf" (ByVal hLcWnd As Integer, ByVal hPtbuf As Integer, ByVal bClosed As Integer) As Integer

  Public Declare Function Paint_DrawMpgon Lib "Litecad.dll" Alias "lcPaint_DrawMpgon" (ByVal hLcWnd As Integer, ByVal hMpgon As Integer, ByVal bFill As Integer, ByVal bBorder As Integer) As Integer

  Public Declare Function Paint_DrawImage Lib "Litecad.dll" Alias "lcPaint_DrawImage" (ByVal hLcWnd As Integer, ByVal hImage As Integer, ByVal X As Double, ByVal Y As Double, ByVal PixelSize As Double, ByVal Transp As Integer, ByVal TVal As Integer, ByVal hPtbuf As Integer) As Integer

  Public Declare Function Paint_DrawImage2 Lib "Litecad.dll" Alias "lcPaint_DrawImage2" (ByVal hLcWnd As Integer, ByVal hImage As Integer, ByVal X As Double, ByVal Y As Double, ByVal W As Double, ByVal H As Double, ByVal Transp As Integer, ByVal TVal As Integer, ByVal hPtbuf As Integer) As Integer

  Public Declare Unicode Function Paint_DrawText Lib "Litecad.dll" Alias "lcPaint_DrawText" (ByVal hLcWnd As Integer, ByVal X As Double, ByVal Y As Double, ByVal szText As String) As Integer

  Public Declare Unicode Function Paint_DrawText2 Lib "Litecad.dll" Alias "lcPaint_DrawText2" (ByVal hLcWnd As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal Align As Integer, ByVal szText As String) As Integer

  Public Declare Unicode Function Paint_DrawTextBC Lib "Litecad.dll" Alias "lcPaint_DrawTextBC" (ByVal hLcWnd As Integer, ByVal hMpgon As Integer, ByVal Gap As Double, ByVal Height As Double, ByVal Align As Integer, ByVal szText As String) As Integer

  Public Declare Unicode Function Paint_DrawArcText Lib "Litecad.dll" Alias "lcPaint_DrawArcText" (ByVal hLcWnd As Integer, ByVal szText As String, ByVal X As Double, ByVal Y As Double, ByVal Rad As Double, ByVal Ang0 As Double, ByVal bCW As Integer, ByVal H As Double, ByVal WScale As Double, ByVal Align As Integer) As Integer

  Public Declare Function Paint_DrawHatch Lib "Litecad.dll" Alias "lcPaint_DrawHatch" (ByVal hLcWnd As Integer, ByVal hHatch As Integer) As Integer

  Public Declare Function Paint_DrawPoint Lib "Litecad.dll" Alias "lcPaint_DrawPoint" (ByVal hLcWnd As Integer, ByVal X As Double, ByVal Y As Double, ByVal PtMode As Integer, ByVal PtSize As Double) As Integer

  Public Declare Function Paint_DrawLine Lib "Litecad.dll" Alias "lcPaint_DrawLine" (ByVal hLcWnd As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function Paint_DrawXline Lib "Litecad.dll" Alias "lcPaint_DrawXline" (ByVal hLcWnd As Integer, ByVal X As Double, ByVal Y As Double, ByVal Angle As Double, ByVal bRay As Integer) As Integer

  Public Declare Function Paint_DrawRect Lib "Litecad.dll" Alias "lcPaint_DrawRect" (ByVal hLcWnd As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Width As Double, ByVal Height As Double) As Integer

  Public Declare Function Paint_DrawRect2 Lib "Litecad.dll" Alias "lcPaint_DrawRect2" (ByVal hLcWnd As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function Paint_DrawTIN Lib "Litecad.dll" Alias "lcPaint_DrawTIN" (ByVal hLcWnd As Integer, ByVal hTIN As Integer) As Integer

  Public Declare Function Paint_DrawGrid Lib "Litecad.dll" Alias "lcPaint_DrawGrid" (ByVal hLcWnd As Integer, ByVal hGrid As Integer, ByVal bDest As Integer, ByVal ColLine As Integer, ByVal ColNode As Integer) As Integer

  Public Declare Unicode Function Paint_DrawCPrompt Lib "Litecad.dll" Alias "lcPaint_DrawCPrompt" (ByVal hLcWnd As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal Align As Integer, ByVal szText As String) As Integer

  Public Declare Sub Paint_SetPixel Lib "Litecad.dll" Alias "lcPaint_SetPixel" (ByVal hDC As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal Color As Integer)

  Public Declare Function Paint_CreatePtbuf Lib "Litecad.dll" Alias "lcPaint_CreatePtbuf" () As Integer

  Public Declare Function Paint_DeletePtbuf Lib "Litecad.dll" Alias "lcPaint_DeletePtbuf" (ByVal hPtbuf As Integer) As Integer

  Public Declare Function Paint_PtbufClear Lib "Litecad.dll" Alias "lcPaint_PtbufClear" (ByVal hPtbuf As Integer) As Integer

  Public Declare Function Paint_PtbufAddPoint Lib "Litecad.dll" Alias "lcPaint_PtbufAddPoint" (ByVal hPtbuf As Integer, ByVal X As Double, ByVal Y As Double, ByVal Prm1 As Double, ByVal Prm2 As Double, ByVal IntPrm As Integer) As Integer

  Public Declare Function Paint_PtbufAddPoint2 Lib "Litecad.dll" Alias "lcPaint_PtbufAddPoint2" (ByVal hPtbuf As Integer, ByVal X As Double, ByVal Y As Double) As Integer

  Public Declare Function Paint_PtbufAddPointP Lib "Litecad.dll" Alias "lcPaint_PtbufAddPointP" (ByVal hPtbuf As Integer, ByVal Angle As Double, ByVal Dist As Double) As Integer

  Public Declare Function Paint_PtbufAddLine Lib "Litecad.dll" Alias "lcPaint_PtbufAddLine" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function Paint_PtbufAddLineP Lib "Litecad.dll" Alias "lcPaint_PtbufAddLineP" (ByVal hPtbuf As Integer, ByVal X As Double, ByVal Y As Double, ByVal Angle As Double, ByVal Dist As Double) As Integer

  Public Declare Function Paint_PtbufAddCircle Lib "Litecad.dll" Alias "lcPaint_PtbufAddCircle" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddCircle2 Lib "Litecad.dll" Alias "lcPaint_PtbufAddCircle2" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddCircle3 Lib "Litecad.dll" Alias "lcPaint_PtbufAddCircle3" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal X3 As Double, ByVal Y3 As Double, ByVal bInside As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArc Lib "Litecad.dll" Alias "lcPaint_PtbufAddArc" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal StartAngle As Double, ByVal ArcAngle As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArc3p Lib "Litecad.dll" Alias "lcPaint_PtbufAddArc3p" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal X3 As Double, ByVal Y3 As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSDE Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSDE" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal DirAng As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSDAR Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSDAR" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal DirAng As Double, ByVal AngArc As Double, ByVal R As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSER Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSER" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal Radius As Double, ByVal bClockwise As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSEL Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSEL" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal ArcLen As Double, ByVal bClockwise As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSEA Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSEA" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal AngArc As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcSEB Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcSEB" (ByVal hPtbuf As Integer, ByVal Xs As Double, ByVal Ys As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal Bulge As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcCSE Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcCSE" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Xs As Double, ByVal Ys As Double, ByVal Xe As Double, ByVal Ye As Double, ByVal bClockwise As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcCSA Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcCSA" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Xs As Double, ByVal Ys As Double, ByVal AngArc As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcCSL Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcCSL" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Xs As Double, ByVal Ys As Double, ByVal ChordLen As Double, ByVal bClockwise As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddArcCRAA Lib "Litecad.dll" Alias "lcPaint_PtbufAddArcCRAA" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal R As Double, ByVal AngStart As Double, ByVal AngEnd As Double, ByVal bClockwise As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddEllipse Lib "Litecad.dll" Alias "lcPaint_PtbufAddEllipse" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Rmaj As Double, ByVal Rmin As Double, ByVal RotAng As Double, ByVal StartAng As Double, ByVal ArcAng As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddEllipse2 Lib "Litecad.dll" Alias "lcPaint_PtbufAddEllipse2" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddRect Lib "Litecad.dll" Alias "lcPaint_PtbufAddRect" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal W As Double, ByVal H As Double, ByVal Angle As Double, ByVal R As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddRect2 Lib "Litecad.dll" Alias "lcPaint_PtbufAddRect2" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal R As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddRect3 Lib "Litecad.dll" Alias "lcPaint_PtbufAddRect3" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal W As Double, ByVal Align As Integer, ByVal R As Double, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddWline Lib "Litecad.dll" Alias "lcPaint_PtbufAddWline" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double, ByVal W As Double, ByVal Align As Integer, ByVal bArc As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufAddPtbuf Lib "Litecad.dll" Alias "lcPaint_PtbufAddPtbuf" (ByVal hPtbuf As Integer, ByVal hPtbuf2 As Integer) As Integer

  Public Declare Function Paint_PtbufGetPoint Lib "Litecad.dll" Alias "lcPaint_PtbufGetPoint" (ByVal hPtbuf As Integer, ByVal Mode As Integer, ByRef pX As Double, ByRef pY As Double) As Integer

  Public Declare Function Paint_PtbufGetPoint2 Lib "Litecad.dll" Alias "lcPaint_PtbufGetPoint2" (ByVal hPtbuf As Integer, ByVal Mode As Integer, ByRef pX As Double, ByRef pY As Double, ByRef pPrm1 As Double, ByRef pPrm2 As Double, ByRef pIntPrm As Integer) As Integer

  Public Declare Function Paint_PtbufInterpolate Lib "Litecad.dll" Alias "lcPaint_PtbufInterpolate" (ByVal hPtbuf As Integer, ByVal bClosed As Integer, ByVal hPtbufDest As Integer, ByVal Mode As Integer, ByVal Resol As Integer) As Integer

  Public Declare Function Paint_PtbufMove Lib "Litecad.dll" Alias "lcPaint_PtbufMove" (ByVal hPtbuf As Integer, ByVal dx As Double, ByVal dy As Double) As Integer

  Public Declare Function Paint_PtbufRotate Lib "Litecad.dll" Alias "lcPaint_PtbufRotate" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Angle As Double) As Integer

  Public Declare Function Paint_PtbufScale Lib "Litecad.dll" Alias "lcPaint_PtbufScale" (ByVal hPtbuf As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal ScaleX As Double, ByVal ScaleY As Double) As Integer

  Public Declare Function Paint_PtbufMirror Lib "Litecad.dll" Alias "lcPaint_PtbufMirror" (ByVal hPtbuf As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function Paint_PtbufCopy Lib "Litecad.dll" Alias "lcPaint_PtbufCopy" (ByVal hPtbuf As Integer, ByVal hPtbufDest As Integer) As Integer

  Public Declare Function Paint_CreateMpgon Lib "Litecad.dll" Alias "lcPaint_CreateMpgon" () As Integer

  Public Declare Function Paint_DeleteMpgon Lib "Litecad.dll" Alias "lcPaint_DeleteMpgon" (ByVal hMpgon As Integer) As Integer

  Public Declare Function Paint_MpgonClear Lib "Litecad.dll" Alias "lcPaint_MpgonClear" (ByVal hMpgon As Integer) As Integer

  Public Declare Function Paint_MpgonAddPgon Lib "Litecad.dll" Alias "lcPaint_MpgonAddPgon" (ByVal hMpgon As Integer, ByVal hPtbuf As Integer) As Integer

  Public Declare Unicode Function Paint_MpgonAddText Lib "Litecad.dll" Alias "lcPaint_MpgonAddText" (ByVal hMpgon As Integer, ByVal hFont As Integer, ByVal X As Double, ByVal Y As Double, ByVal szText As String, ByVal Resol As Integer) As Integer

  Public Declare Unicode Function Paint_MpgonAddBarcode Lib "Litecad.dll" Alias "lcPaint_MpgonAddBarcode" (ByVal hMpgon As Integer, ByVal BarType As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Width As Double, ByVal Height As Double, ByVal szText As String) As Integer

  Public Declare Function Paint_MpgonMove Lib "Litecad.dll" Alias "lcPaint_MpgonMove" (ByVal hMpgon As Integer, ByVal dx As Double, ByVal dy As Double) As Integer

  Public Declare Function Paint_MpgonRotate Lib "Litecad.dll" Alias "lcPaint_MpgonRotate" (ByVal hMpgon As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal Angle As Double) As Integer

  Public Declare Function Paint_MpgonScale Lib "Litecad.dll" Alias "lcPaint_MpgonScale" (ByVal hMpgon As Integer, ByVal Xc As Double, ByVal Yc As Double, ByVal ScaleX As Double, ByVal ScaleY As Double) As Integer

  Public Declare Function Paint_MpgonMirror Lib "Litecad.dll" Alias "lcPaint_MpgonMirror" (ByVal hMpgon As Integer, ByVal X1 As Double, ByVal Y1 As Double, ByVal X2 As Double, ByVal Y2 As Double) As Integer

  Public Declare Function Paint_MpgonCopy Lib "Litecad.dll" Alias "lcPaint_MpgonCopy" (ByVal hMpgon As Integer, ByVal hMpgonDest As Integer) As Integer

  Public Declare Function Paint_HatchGen Lib "Litecad.dll" Alias "lcPaint_HatchGen" (ByVal hMpgon As Integer, ByVal hHatch As Integer, ByVal Dist As Double, ByVal Angle As Double, ByVal W As Double, ByVal Beamc1 As Double, ByVal Beamc2 As Double, ByVal StartOff As Double, ByVal EndOff As Double) As Integer

  Public Declare Function Paint_ImageAdd Lib "Litecad.dll" Alias "lcPaint_ImageAdd" (ByVal Id As Integer) As Integer

  Public Declare Function Paint_ImageDelete Lib "Litecad.dll" Alias "lcPaint_ImageDelete" (ByVal hImage As Integer) As Integer

  Public Declare Function Paint_ImageGetFirst Lib "Litecad.dll" Alias "lcPaint_ImageGetFirst" () As Integer

  Public Declare Function Paint_ImageGetNext Lib "Litecad.dll" Alias "lcPaint_ImageGetNext" (ByVal hImage As Integer) As Integer

  Public Declare Function Paint_ImageGetByID Lib "Litecad.dll" Alias "lcPaint_ImageGetByID" (ByVal Id As Integer) As Integer

  Public Declare Unicode Function Paint_ImageLoad Lib "Litecad.dll" Alias "lcPaint_ImageLoad" (ByVal hImage As Integer, ByVal szFileName As String) As Integer

  Public Declare Function Paint_ImageCopy Lib "Litecad.dll" Alias "lcPaint_ImageCopy" (ByVal hImage As Integer, ByVal hImageDest As Integer) As Integer

  Public Declare Function Paint_ImageCreate Lib "Litecad.dll" Alias "lcPaint_ImageCreate" (ByVal hImage As Integer, ByVal Width As Integer, ByVal Height As Integer) As Integer

  Public Declare Function Paint_ImageSetPixel Lib "Litecad.dll" Alias "lcPaint_ImageSetPixel" (ByVal hImage As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal R As Integer, ByVal G As Integer, ByVal B As Integer) As Integer

  Public Declare Function Paint_ImageFlip Lib "Litecad.dll" Alias "lcPaint_ImageFlip" (ByVal hImage As Integer, ByVal bHor As Integer, ByVal bVert As Integer) As Integer

  Public Declare Function Paint_ImageRotate Lib "Litecad.dll" Alias "lcPaint_ImageRotate" (ByVal hImage As Integer, ByVal Angle As Double) As Integer

  Public Declare Function Paint_ImageGray Lib "Litecad.dll" Alias "lcPaint_ImageGray" (ByVal hImage As Integer) As Integer

  Public Declare Function Paint_ImageResize Lib "Litecad.dll" Alias "lcPaint_ImageResize" (ByVal hImage As Integer, ByVal NewWidth As Integer, ByVal NewHeight As Integer, ByVal ResizeFilter As Integer) As Integer

  Public Declare Function Paint_ImageGetPtbuf Lib "Litecad.dll" Alias "lcPaint_ImageGetPtbuf" (ByVal hImage As Integer, ByVal RotAngle As Double) As Integer

  Public Declare Unicode Function Paint_FontOpenLC Lib "Litecad.dll" Alias "lcPaint_FontOpenLC" (ByVal szFontName As String) As Integer

  Public Declare Unicode Function Paint_FontOpenTT Lib "Litecad.dll" Alias "lcPaint_FontOpenTT" (ByVal szFontName As String, ByVal bBold As Integer, ByVal bItalic As Integer) As Integer

  Public Declare Function Paint_FontClose Lib "Litecad.dll" Alias "lcPaint_FontClose" (ByVal hFont As Integer) As Integer

  Public Declare Function Paint_FontSelect Lib "Litecad.dll" Alias "lcPaint_FontSelect" (ByVal hLcWnd As Integer, ByVal hFont As Integer) As Integer


End Module
