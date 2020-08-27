/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ERROR                            1       /* callback function: exitPanel */
#define  ERROR_PICTURERING                2       /* control type: pictRing, callback function: (none) */
#define  ERROR_PASS                       3       /* control type: string, callback function: EnterFunc */
#define  ERROR_NAME                       4       /* control type: string, callback function: EnterFunc */
#define  ERROR_FIX_SAVE                   5       /* control type: pictButton, callback function: CalBackErrorPanel */

#define  FEEDING                          2
#define  FEEDING_RAW                      2       /* control type: numeric, callback function: (none) */
#define  FEEDING_COUNTER                  3       /* control type: numeric, callback function: (none) */
#define  FEEDING_MOCT                     4       /* control type: numeric, callback function: (none) */
#define  FEEDING_HOUR                     5       /* control type: numeric, callback function: (none) */
#define  FEEDING_SEC                      6       /* control type: numeric, callback function: (none) */
#define  FEEDING_MIN                      7       /* control type: numeric, callback function: (none) */
#define  FEEDING_MTV                      8       /* control type: numeric, callback function: (none) */
#define  FEEDING_M0                       9       /* control type: numeric, callback function: (none) */
#define  FEEDING_M                        10      /* control type: numeric, callback function: (none) */
#define  FEEDING_TRANSLATOR               11      /* control type: textMsg, callback function: (none) */
#define  FEEDING_DURATION                 12      /* control type: numeric, callback function: (none) */
#define  FEEDING_PERCENT                  13      /* control type: numeric, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_19         14      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_2          15      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_15         16      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_14         17      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_13         18      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_12         19      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_17         20      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_16         21      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_18         22      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_11         23      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON            24      /* control type: pictButton, callback function: (none) */
#define  FEEDING_E_VIBRATOR               25      /* control type: LED, callback function: (none) */
#define  FEEDING_E_SCALE                  26      /* control type: LED, callback function: (none) */
#define  FEEDING_E_CONV                   27      /* control type: LED, callback function: (none) */
#define  FEEDING_E_SSM                    28      /* control type: LED, callback function: (none) */
#define  FEEDING_E_MSM                    29      /* control type: LED, callback function: (none) */
#define  FEEDING_W_SCALE                  30      /* control type: LED, callback function: (none) */
#define  FEEDING_W_VIBRATOR               31      /* control type: LED, callback function: (none) */
#define  FEEDING_W_CONV                   32      /* control type: LED, callback function: (none) */
#define  FEEDING_W_SSM                    33      /* control type: LED, callback function: (none) */
#define  FEEDING_W_MSM                    34      /* control type: LED, callback function: (none) */
#define  FEEDING_FINISH_OP_BUT            35      /* control type: pictButton, callback function: workingFunc */
#define  FEEDING_STOP_BUT                 36      /* control type: pictButton, callback function: workingFunc */
#define  FEEDING_FEED_BUT                 37      /* control type: pictButton, callback function: workingFunc */
#define  FEEDING_CONV_BUT                 38      /* control type: pictButton, callback function: workingFunc */
#define  FEEDING_VISUAL_PERCENT           39      /* control type: scale, callback function: (none) */
#define  FEEDING_FEEDING_TIMER            40      /* control type: timer, callback function: TimerFunc */
#define  FEEDING_CONST_KG_9               41      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_8               42      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_4               43      /* control type: textMsg, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_4          44      /* control type: pictButton, callback function: (none) */
#define  FEEDING_CONST_KG_5               45      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_3               46      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_7               47      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_6               48      /* control type: textMsg, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_10         49      /* control type: pictButton, callback function: (none) */
#define  FEEDING_ARROW                    50      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_3          51      /* control type: pictButton, callback function: (none) */
#define  FEEDING_CONST_KG_2               52      /* control type: textMsg, callback function: (none) */
#define  FEEDING_CONST_KG_1               53      /* control type: textMsg, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_7          54      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_9          55      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_8          56      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_6          57      /* control type: pictButton, callback function: (none) */
#define  FEEDING_PICTUREBUTTON_5          58      /* control type: pictButton, callback function: (none) */
#define  FEEDING_SPLITTER_2               59      /* control type: splitter, callback function: (none) */
#define  FEEDING_SPLITTER_5               60      /* control type: splitter, callback function: (none) */
#define  FEEDING_SPLITTER_4               61      /* control type: splitter, callback function: (none) */
#define  FEEDING_SPLITTER_3               62      /* control type: splitter, callback function: (none) */
#define  FEEDING_SPLITTER                 63      /* control type: splitter, callback function: (none) */

#define  HMI                              3       /* callback function: exitPanel */
#define  HMI_RAW                          2       /* control type: numeric, callback function: (none) */
#define  HMI_MOCT                         3       /* control type: numeric, callback function: (none) */
#define  HMI_MTV                          4       /* control type: numeric, callback function: (none) */
#define  HMI_DEFINE_CANVAS                5       /* control type: canvas, callback function: (none) */
#define  HMI_RING_B                       6       /* control type: pictButton, callback function: (none) */
#define  HMI_RING2                        7       /* control type: ring, callback function: productFunc */
#define  HMI_RING                         8       /* control type: ring, callback function: productFunc */
#define  HMI_FIX_SAVE                     9       /* control type: pictButton, callback function: Save_AND_BeginingFunc */
#define  HMI_TEXTMSG                      10      /* control type: textMsg, callback function: (none) */
#define  HMI_TRANSLATOR                   11      /* control type: textMsg, callback function: (none) */
#define  HMI_CONST_KG_5                   12      /* control type: textMsg, callback function: (none) */
#define  HMI_CONST_KG_14                  13      /* control type: textMsg, callback function: (none) */
#define  HMI_CONST_KG_13                  14      /* control type: textMsg, callback function: (none) */
#define  HMI_OFF_ON                       15      /* control type: textButton, callback function: offonFunc */
#define  HMI_PICTUREBUTTON_7              16      /* control type: pictButton, callback function: (none) */
#define  HMI_PICTUREBUTTON_8              17      /* control type: pictButton, callback function: (none) */
#define  HMI_PICTUREBUTTON_6              18      /* control type: pictButton, callback function: (none) */
#define  HMI_PICTUREBUTTON_5              19      /* control type: pictButton, callback function: (none) */
#define  HMI_CANVAS                       20      /* control type: canvas, callback function: (none) */
#define  HMI_COMMANDBUTTON                21      /* control type: command, callback function: function */

#define  MAINPANEL                        4       /* callback function: exitPanel */
#define  MAINPANEL_COMMANDBUTTON_UNO      2       /* control type: command, callback function: Connecting */
#define  MAINPANEL_COMMANDBUTTON_SCALE    3       /* control type: command, callback function: Connecting */
#define  MAINPANEL_ARDUINO_TEXTBOX        4       /* control type: textBox, callback function: (none) */
#define  MAINPANEL_WEIGHT_TEXTBOX         5       /* control type: textBox, callback function: (none) */
#define  MAINPANEL_STRING                 6       /* control type: string, callback function: (none) */
#define  MAINPANEL_Mtv_WEIGHT             7       /* control type: numeric, callback function: (none) */
#define  MAINPANEL_M0_WEIGHT              8       /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_7             9       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_35            10      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_74            11      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_73            12      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_69            13      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_68            14      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_40            15      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_47            16      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_48            17      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_61            18      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_46            19      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_72            20      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_62            21      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_45            22      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_64            23      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_44            24      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_43            25      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_52            26      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_51            27      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_50            28      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_49            29      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_42            30      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_38            31      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_41            32      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_32            33      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_PER                    34      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_PREV_PER               35      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_THEO_SSM_Factor_FILL   36      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_THEO_MSM_Factor_FILL   37      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_TRUE_VOL_MSM_CYC       38      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_TRUE_MSM_FACTOR_FILL   39      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_OPER_DURATION          40      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_33            41      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_34            42      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TRUE_VOL_SSM_CYC       43      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_23            44      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_26            45      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_M_Flow                 46      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_27            47      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_28            48      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TRUE_SSM_FACTOR_FILL   49      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_REQ_SSM_STEPS          50      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_VOL_Flow               51      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_15            52      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_70            53      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_67            54      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_16            55      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_71            56      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_REQ_MSM_STEPS          57      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_65            58      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_REQ_STEP_SECOND        59      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_21            60      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_Dynamic_Q              61      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_REQ_STEP_BOTH          62      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_PRACTICAL_VOL_RATIO    63      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_BOTH_VOL_FOR_PITCH     64      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_REQ_SECOND_VOL         65      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_22            66      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_REQ_BOTH_VOL           67      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_20            68      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_REQ_Vol_Total          69      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_54            70      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_55            71      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_75            72      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_66            73      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_53            74      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_19            75      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_14            76      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_MSM_Mass_Flow_Step     77      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_SSM_Mass_Flow_Step     78      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_37            79      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_36            80      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_76            81      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_12            82      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_5             83      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_57            84      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_56            85      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_11            86      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_10            87      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_9             88      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_8             89      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_77            90      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_MSM_Mass_Flow_Deg      91      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_SSM_Mass_Flow_Deg      92      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_CONST_KG_6             93      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_CONST_KG_4             94      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_WEIGHT                 95      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_S_RPM                  96      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_S_DURATION             97      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_STOP_MSM               98      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_START_MSM              99      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_STOP_SSM               100     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_START_SSM              101     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_M_DURATION             102     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_M_RPM                  103     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_STATE                  104     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_MSM_PRACT_MASS_FLOW    105     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_SSM_PRACT_MASS_FLOW    106     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_ARDUINO                107     /* control type: textButton, callback function: Connecting */
#define  MAINPANEL_ERROR                  108     /* control type: numeric, callback function: checkErrorFunc */
#define  MAINPANEL_Practic_MSM_VOL_PIT    109     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_Practic_SSM_VOL_PIT    110     /* control type: numeric, callback function: (none) */
#define  MAINPANEL_SPLITTER_6             111     /* control type: splitter, callback function: (none) */
#define  MAINPANEL_SPLITTER_4             112     /* control type: splitter, callback function: (none) */
#define  MAINPANEL_SPLITTER_5             113     /* control type: splitter, callback function: (none) */
#define  MAINPANEL_SPLITTER_3             114     /* control type: splitter, callback function: (none) */
#define  MAINPANEL_SPLITTER_2             115     /* control type: splitter, callback function: (none) */
#define  MAINPANEL_SPLITTER               116     /* control type: splitter, callback function: (none) */

#define  NEW                              5       /* callback function: exitPanel */
#define  NEW_CANVAS                       2       /* control type: canvas, callback function: (none) */
#define  NEW_NEW_FIX_NAME_STR             3       /* control type: string, callback function: writeFunc */
#define  NEW_NEW_FIX_SAVE                 4       /* control type: pictButton, callback function: writeFunc */
#define  NEW_NEW_FIX_CANCEL               5       /* control type: pictButton, callback function: writeFunc */
#define  NEW_NEW_FIX_OPERATION_NUM        6       /* control type: numeric, callback function: writeFunc */
#define  NEW_NEW_FIX_TARGET_NUM           7       /* control type: numeric, callback function: writeFunc */
#define  NEW_NEW_FIX_RAW_NUM              8       /* control type: numeric, callback function: writeFunc */
#define  NEW_CONST_KG_7                   9       /* control type: textMsg, callback function: (none) */
#define  NEW_CONST_KG_6                   10      /* control type: textMsg, callback function: (none) */
#define  NEW_CONST_KG_5                   11      /* control type: textMsg, callback function: (none) */
#define  NEW_PICTUREBUTTON_6              12      /* control type: pictButton, callback function: (none) */
#define  NEW_CONST_KG_8                   13      /* control type: textMsg, callback function: (none) */
#define  NEW_PICTUREBUTTON_7              14      /* control type: pictButton, callback function: (none) */
#define  NEW_PICTUREBUTTON_8              15      /* control type: pictButton, callback function: (none) */
#define  NEW_PICTUREBUTTON_5              16      /* control type: pictButton, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CalBackErrorPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK checkErrorFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Connecting(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK EnterFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK function(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK offonFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK productFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Save_AND_BeginingFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK workingFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK writeFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif