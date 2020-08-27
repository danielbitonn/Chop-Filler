#include "toolbox.h"
#include <utility.h>
#include <analysis.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include "ChopFiller.h"



#define Qmin 90.00
#define Qmax 98.00
#define MaxTargetVal 200.00
#define MinTargetVal 20.00
#define KD_FACTOR 0.005
#define PI_FACTOR 5.0


/************************************************** PANELS **************************************************************/
/************************************************************************************************************************/
static int panelHandle,panelHMI,panelNEW,panelERROR,panelFEEDING;         

int  C_NOW_Month, C_NOW_Day, C_NOW_Year, C_NOW_Minute, C_NOW_Hour;
double C_DateTimeNow,C_NOW_Second,C_NOW_Duration;
#define START 0
#define STOP  1
#define OPERATION 0
#define FEEDING_SSM 1
#define FEEDING_MSM 2
#define START 0
#define STOP 1
#define DT 2


/******************************************* Communication Defines ******************************************************/
/************************************************************************************************************************/
/***Parity:	none 0	|	odd 1	|	even 2	|	mark 3	|	space 4	 ***/	

/*Weight Scale*/
#define SCALECOM 12
#define SCL_BaoudRate 9600
#define SCL_Parity 0
#define SCL_DataB 8
#define SCL_StopB 1
#define SCL_InSize 512
#define SCL_OutSize 512

/*Cotroller - Arduino Mega*/
#define ARDUINOCOM 10
#define ARD_BaoudRate 1000000
#define ARD_Parity 0
#define ARD_DataB 8
#define ARD_StopB 1
#define ADR_InSize 1002
#define ADR_OutSize 1002

/* S/U T/S . N/G T/S [0] [0] [.] [0] [0] [0]  */ 
/* ___ ___ . ___ ___  _   _   .   _   _   _	  */
//double ST_NT=0.00; int st_nt=0;			/*	ST.NT -> SteadyState   		 Net	*/	 
//double US_NT=0.00; int us_nt=0;			/*	US.NT -> UnSteadyState 		 Net 	*/	  
//double ST_GS=0.00; int st_gs=0;			/*	ST.GS -> SteadyState Of 	 ZERO	*/
//double US_GS=0.00; int us_gs=0;			/*	US.GS -> SteadyState Around  ZERO	*/	  

#define MainFeed 	 0.95 
#define SubFeed  	 0.05
#define MaxFeedError 1.02
#define AVG_AMOUNT 1
#define STEADY_STATE_FLAG 10
#define Q 90.0
#define STEPPER_FACTOR 5.625
#define STEP_FACTOR_I 8
#define STEP_FACTOR_D 8.0


/*******************************************  ERROR PANEL DISPLAY  ******************************************************/
/************************************************************************************************************************/
#define WAIT 1
#define ENTERANCE 7
#define FINISH_OPER 8

int newFlag=1,RingCounter=0,checkError=0,flag=0;
char try[10][50],bufftry[50];

/*******************************************  Theroretical Facros  ******************************************************/
/************************************************************************************************************************/
/* MSM >>> Main Spiral axis */
#define Theor_MSM_VOL_FLOW_PITCH 750.00 									/*by 10^-6 			[m^3/Cyc] */
#define MSM_Factor_FILL 0.25												/*By Design Theory 	[%]   */
double  Practicl_MSM_VOL_PITCH=0.00, 										/*by 10^-6 			[m^3] */
		MSM_Mass_Flow_Pitch=0.00, 
		MSM_Mass_Flow_Deg=0.00, 
		MSM_Mass_Flow_Step=0.00;

/* SSM >>> Second Spiral axis */
#define Theor_SSM_VOL_FLOW_PITCH 7.50  										/*by 10^-6 			[m^3/Cyc] */
#define SSM_Factor_FILL 0.25												/*By Design Theory 	[%]   */
double  Practicl_SSM_VOL_PITCH=0.00, 										/*by 10^-6 			[m^3] */
	    SSM_Mass_Flow_Pitch=0.00, 
		SSM_Mass_Flow_Deg=0.00, 
		SSM_Mass_Flow_Step=0.00;

double PRACTICAL_VOL_RATIO_MSM_SSM=0.00;
double REQ_Vol_Total=0.00;
double Dynamic_Q=0.00;
double REQ_BOTH_VOL=0.00;
double REQ_SECOND_VOL=0.00;
double BOTH_VOL_FOR_PITCH=0.00;
double REQ_STEP_BOTH=0.00;
double REQ_STEP_SECOND=0.00;
double REQ_MSM_STEPS=0.00;
double REQ_SSM_STEPS=0.00;

typedef struct {
				char
					date[50],
					productName[100];
				int
					day,
					month,
					year,
					ProductNo,
					operationWeight,  // Max 350000 gram
		/*N..Used*/	amount,			  // Equal to ROUNDUP[(operationWeight)/(targetVal)]
		/*N.Used*/	MaxReqfeedTime,	  // Max 12000 Sec When {tagetVal=300 & productRaw=1600, & }
					productRaw,		  // 500 To 1600
					targetVal,		  // Between 300 To 20,000
					LastTotalError,	  // 
					NewTotalError,
					ServingsCounter,
					MSM_stepCounter,
					SSM_stepCounter,
					Arduino_SSM_Encoder,
					i_kp,
					i_ki,
					i_kdd,
					i_kd,
					i_Q;
				
				double
					DateTimeElements,
					feedSecDuration_MSM,
	   				feedSecDuration_SSM,
					d_kp,
					d_ki,
					d_kdd,
					d_kd,
					d_Q,
					productID;
	}S_PRODUCT;
S_PRODUCT product[30];		//Data Of All the Products of the production line
S_PRODUCT ProductInProcess; //The Product In Process






void PreTheoryCalcFunc()
{
/*MSM*/
Practicl_MSM_VOL_PITCH = Theor_MSM_VOL_FLOW_PITCH*MSM_Factor_FILL;	/* By10^-6 [m^3/Cyc] */	SetCtrlVal (panelHandle, MAINPANEL_Practic_MSM_VOL_PIT,Practicl_MSM_VOL_PITCH);
/*SSM*/
Practicl_SSM_VOL_PITCH = Theor_SSM_VOL_FLOW_PITCH*SSM_Factor_FILL;	/* By10^-6 [m^3/Cyc] */	SetCtrlVal (panelHandle, MAINPANEL_Practic_SSM_VOL_PIT,Practicl_SSM_VOL_PITCH );
}

/**************************************   Pre Calculations   ********************************************/
/********************************************************************************************************/
double MapFunction(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void PreCalcFunc()
{
/*MSM*/
MSM_Mass_Flow_Pitch = (double)ProductInProcess.productRaw*Practicl_MSM_VOL_PITCH;    /* By10^-6 [kg/Cyc]  */	SetCtrlVal (panelHandle, MAINPANEL_MSM_PRACT_MASS_FLOW, MSM_Mass_Flow_Pitch);
MSM_Mass_Flow_Deg   = MSM_Mass_Flow_Pitch/360.00;									 /* By10^-6 [kg/Deg]  */	SetCtrlVal (panelHandle, MAINPANEL_MSM_Mass_Flow_Deg ,MSM_Mass_Flow_Deg);
MSM_Mass_Flow_Step  = MSM_Mass_Flow_Deg*STEPPER_FACTOR;								 /* By10^-6 [kg/Step] */	SetCtrlVal (panelHandle, MAINPANEL_MSM_Mass_Flow_Step ,MSM_Mass_Flow_Step);

/*SSM*/
SSM_Mass_Flow_Pitch=(double)ProductInProcess.productRaw*Practicl_SSM_VOL_PITCH;	 	 /* By10^-6 [kg/Cyc]  */	SetCtrlVal (panelHandle, MAINPANEL_SSM_PRACT_MASS_FLOW, SSM_Mass_Flow_Pitch);
SSM_Mass_Flow_Deg = SSM_Mass_Flow_Pitch/360.00;										 /* By10^-6 [kg/Deg]  */	SetCtrlVal (panelHandle, MAINPANEL_SSM_Mass_Flow_Deg ,SSM_Mass_Flow_Deg);
SSM_Mass_Flow_Step  = SSM_Mass_Flow_Deg*STEPPER_FACTOR;								 /* By10^-6 [kg/Step] */	SetCtrlVal (panelHandle, MAINPANEL_SSM_Mass_Flow_Step ,SSM_Mass_Flow_Step);

PRACTICAL_VOL_RATIO_MSM_SSM = Practicl_MSM_VOL_PITCH/Practicl_SSM_VOL_PITCH;
SetCtrlVal (panelHandle, MAINPANEL_PRACTICAL_VOL_RATIO ,PRACTICAL_VOL_RATIO_MSM_SSM);

REQ_Vol_Total = ((double)(ProductInProcess.targetVal)/((double)(ProductInProcess.productRaw)*1000.00))*pow(10.00,6.00);
SetCtrlVal (panelHandle, MAINPANEL_REQ_Vol_Total ,REQ_Vol_Total);


Dynamic_Q = MapFunction((double)ProductInProcess.targetVal,MinTargetVal,MaxTargetVal,Qmin,Qmax);
SetCtrlVal (panelHandle, MAINPANEL_Dynamic_Q ,Dynamic_Q);

REQ_BOTH_VOL=(REQ_Vol_Total*Dynamic_Q)/100.0;
SetCtrlVal (panelHandle, MAINPANEL_REQ_BOTH_VOL ,REQ_BOTH_VOL);

REQ_SECOND_VOL=REQ_Vol_Total-REQ_BOTH_VOL;
SetCtrlVal (panelHandle, MAINPANEL_REQ_SECOND_VOL ,REQ_SECOND_VOL);

BOTH_VOL_FOR_PITCH = (1.00+PRACTICAL_VOL_RATIO_MSM_SSM)*Practicl_SSM_VOL_PITCH;
SetCtrlVal (panelHandle, MAINPANEL_BOTH_VOL_FOR_PITCH ,BOTH_VOL_FOR_PITCH);

REQ_STEP_BOTH=(REQ_BOTH_VOL)/(BOTH_VOL_FOR_PITCH);
SetCtrlVal (panelHandle, MAINPANEL_REQ_STEP_BOTH ,REQ_STEP_BOTH);

REQ_STEP_SECOND=(REQ_SECOND_VOL)/(Practicl_SSM_VOL_PITCH);
SetCtrlVal (panelHandle, MAINPANEL_REQ_STEP_SECOND ,REQ_STEP_SECOND);

REQ_MSM_STEPS=REQ_STEP_BOTH;
SetCtrlVal (panelHandle, MAINPANEL_REQ_MSM_STEPS ,REQ_MSM_STEPS);

REQ_SSM_STEPS=REQ_STEP_BOTH+REQ_STEP_SECOND;
SetCtrlVal (panelHandle, MAINPANEL_REQ_SSM_STEPS ,REQ_SSM_STEPS);
}


/*****************************************************************************************/
/**************************************** 	Parameters 	**********************************/
int
Scale_Counter_Box, Arduino_Counter_Box,
state=0,
DENS=0,
TM=0,
read=0,
SPEED=0,
productChose,
BOX_M0=0,
inputStrIndex=1,
ProductInOperation=-1,
ans=0,
runFlag=0,	startFlag=0,	weightFlag=0,	arduinoFlag=0,	foundFlag=0,	readFlag=0,	ErrorFlag=0,	offonFlag=0,	convFlag=0,
getting_MSM_Duration_Flag=0,arrowFlag=0;

double 
weight=0.00,
READ_weight=0.00,
Per=0.00,
prev_per=0.00,
RPM=0.00,
S_RPM=0.00,
M_RPM=0.00;

char
sendStr [30],
strToArduino [100],
strToScale[100],
productChoseName[50],
miniBuff[100],	miniBuff1[100],	miniBuff2[100],	miniBuff3[100];

#define START_OPER 0
#define END_OPER 1
#define START_FEED 2
#define END_FEED 3
#define NOW 4
#define DATE 6

typedef struct
	{
	int  
	 Month[4],
	 Day[4],
	 Year[4],
	 Minute[4],
	 Hour[4],
	 NOW_Month, 
	 NOW_Day, 
	 NOW_Year, 
	 NOW_Minute, 
	 NOW_Hour;
	 
	double 
	   NOW_Second,
	   Second[4],
	   DateTimeStart,
	   DateTimeStop,
	   DateTimeDuration, 
	   DateTimeNow[5] ,		//0:
	   feedSecDuration[4],	//0:??? || 1:Delta Of Oper Using END_OPER || 2:???? || 3: Delta of Feed Using END_FEED
	   feedSecDuration_MSM,
	   feedSecDuration_SSM;	
	}
S_FEED;
S_FEED FEED; 

/*
OPERATION 0	|	FEEDING_SSM 1 (All Feeding)	|	FEEDING_MSM 2	|	
START = 0 	| STOP = 1  |	DT = 2
Example: timer_arr[START_OPER][START] >>> currect Starting Operation By: "GetCurrentDateTime ();"
		 timer_arr[START_OPER][STOP]  >>> Cuurect Finish Operation	 By: "GetCurrentDateTime ();"
Then Using "DeltaTimeFunc" to get DT to..
		 timer_arr[START_OPER][DT]	   >>> Delta Time				 By: "GetDeltaTimeFunc(double Start, double End);"
*/
double timer_arr[5][3];
double GetDeltaTimeFunc(double Start, double End)
{
//double duration=0.00;
C_DateTimeNow=End-Start;
GetDateTimeElements (C_DateTimeNow, &C_NOW_Hour, &C_NOW_Minute,&C_NOW_Second, &C_NOW_Month,&C_NOW_Day, &C_NOW_Year);
C_NOW_Duration=(double)C_NOW_Hour*3600.0+(double)C_NOW_Minute*60.0+C_NOW_Second;
return C_NOW_Duration;
}

/*
START_OPER 0	|	END_OPER 1	|	START_FEED 2	|	END_FEED 3	|	NOW 4 |
*/

void GetTimeFunc(int event)
{
switch (event)
{
case 4: //TIMER FUNC
GetCurrentDateTime(&FEED.DateTimeNow[4]);
GetDateTimeElements (FEED.DateTimeNow[4], &FEED.NOW_Hour, &FEED.NOW_Minute,&FEED.NOW_Second, &FEED.NOW_Month,&FEED.NOW_Day, &FEED.NOW_Year);
ProductInProcess.productID=FEED.DateTimeNow[4];
break;

//case 0: //Start Operation USING "START_OPER" 	/*UnUSE*/
//GetCurrentDateTime(&FEED.DateTimeNow[0]);
//GetDateTimeElements (FEED.DateTimeNow[0], &FEED.Hour[0], &FEED.Minute[0],&FEED.Second[0], &FEED.Month[0],&FEED.Day[0], &FEED.Year[0]);
//break;

//case 1://End Operation USING "END_OPER"			/*UnUSE*/
//GetCurrentDateTime(&FEED.DateTimeNow[1]);
//GetDateTimeElements (FEED.DateTimeNow[1], &FEED.Hour[1], &FEED.Minute[1],&FEED.Second[1], &FEED.Month[1],&FEED.Day[1], &FEED.Year[1]);	
//FEED.feedSecDuration[1]=FEED.Second[1]-FEED.Second[0];
//break;

//case 2://Start Feeding uniq USING "START_FEED"	/*UnUSE*/
//GetCurrentDateTime(&FEED.DateTimeNow[2]);
//GetDateTimeElements (FEED.DateTimeNow[2], &FEED.Hour[2], &FEED.Minute[2],&FEED.Second[2], &FEED.Month[2],&FEED.Day[2], &FEED.Year[2]);	
//break;

//case 3://End Feeding uniq USING "END_FEED"		/*UnUSE*/
//GetCurrentDateTime(&FEED.DateTimeNow[3]);
//GetDateTimeElements (FEED.DateTimeNow[3], &FEED.Hour[3], &FEED.Minute[3],&FEED.Second[3], &FEED.Month[3],&FEED.Day[3], &FEED.Year[3]);	
//FEED.feedSecDuration[3]=FEED.Second[3]-FEED.Second[2];
//break;



//case 5: //FOR Delta time Func					
//GetCurrentDateTime(&C_DateTimeNow);	
//GetDateTimeElements (C_DateTimeNow, &C_NOW_Hour, &C_NOW_Minute,&C_NOW_Second, &C_NOW_Month,&C_NOW_Day, &C_NOW_Year);
//break;
}
}



/*
double DeltaTimeFunc(int k)
{
switch (k)
{
	case 0:
		C_DateTimeNow=FEED.DateTimeNow[4];
		break;
	case 1:
		C_DateTimeNow=FEED.DateTimeNow[4]-C_DateTimeNow;
		GetDateTimeElements (C_DateTimeNow, &C_NOW_Hour, &C_NOW_Minute,&C_NOW_Second, &C_NOW_Month,&C_NOW_Day, &C_NOW_Year);
		C_NOW_Duration=(double)C_NOW_Hour*3600.0+(double)C_NOW_Minute*60.0+C_NOW_Second;
	return C_NOW_Duration;
}
return 0.0;
}
*/


/*********************************************************************************************************************************************************************************/
/****************************		 File Parameters 	**********************************/
#define READ   0																		  //dataFunc(READ); 
#define APPEND 1		  																  //dataFunc(APPEND); 

FILE *fp;
int
	AmountOfLines;

char 
	filename[50],
	buffer[500],
	productCol[500];
	


#define Amount_Of_Feeds_In_Operation 20
typedef struct {
				char
					date[Amount_Of_Feeds_In_Operation][30],
					productName[Amount_Of_Feeds_In_Operation][50];
				int
					ProductNo,
					operationWeight,  // Max 350000 gram
		/*N..Used*/	amount,			  // Equal to ROUNDUP[(operationWeight)/(targetVal)]
		/*N.Used*/	MaxReqfeedTime,	  // Max 12000 Sec When {tagetVal=300 & productRaw=1600, & }
					LastTotalError;	  
				int
					day[Amount_Of_Feeds_In_Operation],
					month[Amount_Of_Feeds_In_Operation],
					year[Amount_Of_Feeds_In_Operation],
					productRaw[Amount_Of_Feeds_In_Operation],		  
					targetVal[Amount_Of_Feeds_In_Operation],
					NewTotalError[Amount_Of_Feeds_In_Operation],
					ServingsCounter[Amount_Of_Feeds_In_Operation],
					MSM_stepCounter[Amount_Of_Feeds_In_Operation],
					SSM_stepCounter[Amount_Of_Feeds_In_Operation],
					FinalWeight[Amount_Of_Feeds_In_Operation],
					Arduino_SSM_Encoder[Amount_Of_Feeds_In_Operation],
					i_kp[Amount_Of_Feeds_In_Operation],
					i_ki[Amount_Of_Feeds_In_Operation],
					i_kdd[Amount_Of_Feeds_In_Operation],
					i_kd[Amount_Of_Feeds_In_Operation],
					i_Q[Amount_Of_Feeds_In_Operation];
				double
					DateTimeElements[Amount_Of_Feeds_In_Operation],
					Error_Percent[Amount_Of_Feeds_In_Operation],
					Prev_Error_Percent[Amount_Of_Feeds_In_Operation],
					feedSecDuration_MSM[Amount_Of_Feeds_In_Operation],
	   				feedSecDuration_SSM[Amount_Of_Feeds_In_Operation],
					M_RPM[Amount_Of_Feeds_In_Operation],
					S_RPM[Amount_Of_Feeds_In_Operation],
					d_kp[Amount_Of_Feeds_In_Operation],
					d_ki[Amount_Of_Feeds_In_Operation],
					d_kdd[Amount_Of_Feeds_In_Operation],
					d_kd[Amount_Of_Feeds_In_Operation],
					d_Q[Amount_Of_Feeds_In_Operation];
					
/*For CalculationRunningStatitics*/				
					double
/*kg/sec*/			M_Flow[Amount_Of_Feeds_In_Operation],
/*m^3/sec*/			VOL_Flow[Amount_Of_Feeds_In_Operation],
					Calc_Total_VOL_Pitch[Amount_Of_Feeds_In_Operation],
					TRUE_VOL_SSM_CYC[Amount_Of_Feeds_In_Operation],
					TRUE_VOL_MSM_CYC[Amount_Of_Feeds_In_Operation],
					TRUE_SSM_FACTOR_FILL[Amount_Of_Feeds_In_Operation],
					TRUE_MSM_FACTOR_FILL[Amount_Of_Feeds_In_Operation],
					productID[Amount_Of_Feeds_In_Operation];
					
	}S_OPERATION;
S_OPERATION OperationInProccess;



/*******************************	 Simple Moving Average 		**************************/			
/*****************************************************************************************/			
int 
  arrNumbers[AVG_AMOUNT] = {0},
  pos = 0,
  newAvg = 0,
  prevAvg=0,
  ST_counter=0;
double 
d_newAvg=0.0,d_prevAvg=0.0;
long 
  sum = 0;
  
int SMA_Func(int *ptrArrNum,long *ptrSum,int pos,int len,int nextNum)
			{
			/*Subtract the oldest number from the prev sum, add the new number*/
			*ptrSum = *ptrSum - ptrArrNum[pos] + nextNum;
			/*Assign the nextNum to the position in the array*/
			ptrArrNum[pos] = nextNum;
			//return the average
			return *ptrSum / len;
			}
void CalculationRunningStatitics()
{
/*kg/sec*/	OperationInProccess.M_Flow[ProductInProcess.ServingsCounter] 				= ((double)OperationInProccess.FinalWeight[ProductInProcess.ServingsCounter]*pow(10.0,-3.0))/(OperationInProccess.feedSecDuration_SSM[ProductInProcess.ServingsCounter]); 													
SetCtrlVal (panelHandle, MAINPANEL_M_Flow, OperationInProccess.M_Flow[ProductInProcess.ServingsCounter]*pow (10.00, 6.00));
/*m^3/sec*/	OperationInProccess.VOL_Flow[ProductInProcess.ServingsCounter]  			= OperationInProccess.M_Flow[ProductInProcess.ServingsCounter]/(double)ProductInProcess.productRaw;																																		
SetCtrlVal (panelHandle, MAINPANEL_VOL_Flow, OperationInProccess.VOL_Flow[ProductInProcess.ServingsCounter]*pow (10.00, 6.00));	

/*	
For Now The math Is :
M_Flow	=	RAW	* [ { (MAIN_Vol_Flow_S) + (MAIN_Vol_Flow_M) } + { SUB_Vol_Flow_S }]
>>>	RAW * [	{TRUE_VOL_SSM_CYC*MAIN_RPM} * {(1/60.0) + (RATIO_FACTOR/60.0)}	+	{TRUE_VOL_SSM_CYC*SUB_RPM}*{(1/60.0)} ]
>>> [RAW * TRUE_VOL_SSM_CYC / 60.0] * [{MAIN_RPM*(1+RATIO_FACTOR)} + {SUB_RPM}]
KNOWN	:	MAIN_RPM == MSM_RPM
KNOWN	:	SUB_RPM  == SSM_RPM - MSM_RPM
KNOWN	:	PRACTICAL_VOL_RATIO_MSM_SSM
From That Matj i can extract the "TRUE_VOL_SSM_CYC"
By the Following

TRUE_VOL_SSM_CYC = {M_Flow * 60.00} / {RAW * [(MAIN_RPM*(1+RATIO_FACTOR))+(SUB_RPM)]}
TRUE_VOL_MSM_CYC = RATIO_FACTOR * TRUE_VOL_SSM_CYC;
*/
/*****	SSM	********/
/* [m^3/Cyc] */OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter]=( OperationInProccess.M_Flow[ProductInProcess.ServingsCounter]*60.00 ) / ( ((double)OperationInProccess.productRaw[ProductInProcess.ServingsCounter])*((1.00 + PRACTICAL_VOL_RATIO_MSM_SSM)*(OperationInProccess.M_RPM[ProductInProcess.ServingsCounter])+(OperationInProccess.S_RPM[ProductInProcess.ServingsCounter]-OperationInProccess.M_RPM[ProductInProcess.ServingsCounter])) );
SetCtrlVal (panelHandle, MAINPANEL_TRUE_VOL_SSM_CYC, OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter]*pow (10.00, 6.00));	

/* <0-1> */OperationInProccess.TRUE_SSM_FACTOR_FILL[ProductInProcess.ServingsCounter] = (OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter]) / (Theor_SSM_VOL_FLOW_PITCH*pow (10.00, -6.00));
SetCtrlVal (panelHandle, MAINPANEL_TRUE_SSM_FACTOR_FILL, OperationInProccess.TRUE_SSM_FACTOR_FILL[ProductInProcess.ServingsCounter]);	

/*****	MSM	********/
/* [m^3/Cyc] */OperationInProccess.TRUE_VOL_MSM_CYC[ProductInProcess.ServingsCounter]=PRACTICAL_VOL_RATIO_MSM_SSM * OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter];
SetCtrlVal (panelHandle, MAINPANEL_TRUE_VOL_MSM_CYC, OperationInProccess.TRUE_VOL_MSM_CYC[ProductInProcess.ServingsCounter]*pow (10.00, 6.00));	

/* <0-1> */OperationInProccess.TRUE_MSM_FACTOR_FILL[ProductInProcess.ServingsCounter] = (OperationInProccess.TRUE_VOL_MSM_CYC[ProductInProcess.ServingsCounter]) / (Theor_MSM_VOL_FLOW_PITCH*pow (10.00, -6.00));
SetCtrlVal (panelHandle, MAINPANEL_TRUE_MSM_FACTOR_FILL, OperationInProccess.TRUE_MSM_FACTOR_FILL[ProductInProcess.ServingsCounter]);	


SetCtrlVal (panelHandle, MAINPANEL_THEO_MSM_Factor_FILL,MSM_Factor_FILL);	
SetCtrlVal (panelHandle, MAINPANEL_THEO_SSM_Factor_FILL,SSM_Factor_FILL);	

}

/*******************************	 Store Parameters 		******************************/			
/*****************************************************************************************/
void StorageFeedsFunc()
{
/*int*/					
	OperationInProccess.productRaw[ProductInProcess.ServingsCounter]			=	ProductInProcess.productRaw;		  
	OperationInProccess.targetVal[ProductInProcess.ServingsCounter]				=	ProductInProcess.targetVal;
	OperationInProccess.NewTotalError[ProductInProcess.ServingsCounter]			=	ProductInProcess.NewTotalError;
	OperationInProccess.ServingsCounter[ProductInProcess.ServingsCounter]		=	ProductInProcess.ServingsCounter;
	OperationInProccess.MSM_stepCounter[ProductInProcess.ServingsCounter]		=	ProductInProcess.MSM_stepCounter/STEP_FACTOR_I;
	OperationInProccess.SSM_stepCounter[ProductInProcess.ServingsCounter]		=	ProductInProcess.SSM_stepCounter/STEP_FACTOR_I;
	OperationInProccess.FinalWeight[ProductInProcess.ServingsCounter]			=	prevAvg;	 	 					  /*From Case03*/
	OperationInProccess.Arduino_SSM_Encoder[ProductInProcess.ServingsCounter]	=	ProductInProcess.Arduino_SSM_Encoder;
	OperationInProccess.i_kp[ProductInProcess.ServingsCounter]					= 	ProductInProcess.i_kp;
	OperationInProccess.i_ki[ProductInProcess.ServingsCounter]					= 	ProductInProcess.i_ki;
	OperationInProccess.i_kdd[ProductInProcess.ServingsCounter]					= 	ProductInProcess.i_kdd;
	OperationInProccess.i_Q[ProductInProcess.ServingsCounter]					= 	ProductInProcess.i_Q;

/*double*/
	OperationInProccess.Error_Percent[ProductInProcess.ServingsCounter]			=	Per; 								  /*From Case03*/
	OperationInProccess.Prev_Error_Percent[ProductInProcess.ServingsCounter]	=	prev_per;  							  /*From Case03*/
	OperationInProccess.feedSecDuration_MSM[ProductInProcess.ServingsCounter]	=	ProductInProcess.feedSecDuration_MSM; /*From Case0*/
	OperationInProccess.feedSecDuration_SSM[ProductInProcess.ServingsCounter]	=	ProductInProcess.feedSecDuration_SSM; /*From Case0*/
	OperationInProccess.M_RPM[ProductInProcess.ServingsCounter]					=	M_RPM;								  /*Cyc/Sec from case0*/
	OperationInProccess.S_RPM[ProductInProcess.ServingsCounter]					=	S_RPM;								  /*Cyc/Sec from case0*/
	OperationInProccess.d_kd[ProductInProcess.ServingsCounter]					=	(double)ProductInProcess.i_kdd*KD_FACTOR;
	OperationInProccess.DateTimeElements[ProductInProcess.ServingsCounter] 		= 	FEED.DateTimeNow[NOW];
 	OperationInProccess.day[ProductInProcess.ServingsCounter]					=	FEED.NOW_Day;
	OperationInProccess.month[ProductInProcess.ServingsCounter]					=	FEED.NOW_Month;
	OperationInProccess.year[ProductInProcess.ServingsCounter]					=	FEED.NOW_Year;
	OperationInProccess.productID[ProductInProcess.ServingsCounter]				=	ProductInProcess.productID;
	sprintf(OperationInProccess.productName[ProductInProcess.ServingsCounter], "%s"	,ProductInProcess.productName);
	sprintf(OperationInProccess.date[ProductInProcess.ServingsCounter],		   "%s"	,ProductInProcess.date);	

CalculationRunningStatitics();
}

void InitializeFeedFunc()
{
ProductInProcess.NewTotalError=0;
ProductInProcess.MSM_stepCounter=0;
ProductInProcess.SSM_stepCounter=0;
prevAvg=0;
ProductInProcess.Arduino_SSM_Encoder=0;

Per=0.00;
prev_per=0.00;
ProductInProcess.feedSecDuration_MSM=0.00;
ProductInProcess.feedSecDuration_SSM=0.00;
timer_arr[FEEDING_MSM][STOP]=0.00;
}


/*******************************	 DataStorage 		**********************************/			//Case data=0 -> READ
/*****************************************************************************************/			//Case data=1 -> APPEND
void dataFunc(int data)																	  			//dataFunc(int data);
{
	char FileName[50],buffer[1000];
	int k=0,i=0;
	
switch (data)
	  	{
		/****	Reading		****/   
		/***********************/
		case 0:
				{
					sprintf(FileName,"data\\main_productdata.csv");	
					fp = fopen (FileName, "r");
				while(fgets ( buffer,999, fp)!=0) 
					 {
						 switch (k>=1)
						 {
							 case 0:
							 sscanf( buffer,"%s\n",productCol);
							 break;
						 
							 case 1:
								sscanf( buffer,"%[^,],%d,%[^,],%d,%d,%d,%d,%d,%d,%lf\n",
							 product[k].date,				&product[k].ProductNo,
							 product[k].productName,		&product[k].productRaw,	
							 &product[k].operationWeight, 	&product[k].targetVal,
							 &product[k].amount,			&product[k].MaxReqfeedTime,
							 &product[k].LastTotalError,	&product[i].productID);
							 buffer[0]='\0'; 
							 break;
						 }
						 k++;
						
					}
					AmountOfLines=k;
					fclose (fp);	 
				  }
																										break;
		/****	Append		****/   
		/***********************/
		case 1: 
				{		//sprintf(FileName,"data\\main_productdata.csv");
					   	sprintf(FileName,"data\\main_productdataCHECK.csv");
						fp = fopen (FileName,"w");
						for(i=0;i<1;i++)
						{
						fprintf(fp,"Date, No.Product, Product, Raw, OperationWeight[gram], TargetWeight[geam], Amount[=E/F],	MaxReqFeedTime[msec][=FLOOR.Math(E/14400)],	LastTotalError\n");
						}
						for (i=1;i<=AmountOfLines;i++)
						{
						 fprintf (fp,"%s,%d,%s,%d,%d,%d,%d,%d,%d,%d\n",
						 product[i].date,				product[i].ProductNo,
						 product[i].productName,		product[i].productRaw,	
						 product[i].operationWeight, 	product[i].targetVal,
						 product[i].amount,				product[i].MaxReqfeedTime,
						 product[i].LastTotalError),	product[i].productID;
						
						 }
				fclose (fp);
				}																						break;

			}

}			



/***********************	LookingForFunc In DataBase		******************************/
/*****************************************************************************************/
int LookingForFunc(char* pName, int pNumber)
			{
			int cmp=1;
			   for(int i=1;i<=AmountOfLines;i++)
				{
				 cmp=strcmp(pName, product[i].productName);
				 if (pNumber==product[i].ProductNo || cmp==0)
				 	{
				 	 ProductInProcess.amount			=	product[i].amount;
					 ProductInProcess.MaxReqfeedTime	=	product[i].MaxReqfeedTime;
					 ProductInProcess.operationWeight	=	product[i].operationWeight;
					 ProductInProcess.ProductNo			=	product[i].ProductNo;
					 ProductInProcess.productRaw		=	product[i].productRaw;
					 ProductInProcess.targetVal			=	product[i].targetVal;
					 ProductInProcess.LastTotalError	=	product[i].LastTotalError;
					 ProductInProcess.productID 		= 	product[i].productID;
					 sprintf(ProductInProcess.date,"%s",product[i].date);
					 sprintf(ProductInProcess.productName,"%s",product[i].productName);
					 ProductInOperation=i;
				 	return 1;
					}
					else {ProductInOperation=AmountOfLines+1;}
				}
			return 0;
			}



/*****************************************************************************************/
/**************************************** PANEL DISPLAY **********************************/			//panelDidplayMachine(int panel_on);
/* 	MAINPANEL		|		HMI		|		NEW		|		ERROR		|		FEEDING		*/
void panelDidplayMachine(int panel_on)
{	
switch (panel_on)
	{
		case MAINPANEL:	/*>>>*/		 HidePanel (panelERROR);	HidePanel (panelNEW);		HidePanel (panelHMI);		HidePanel (panelFEEDING);
		DisplayPanel (panelHandle);	 break;
		
		case ERROR:		/*>>>*/		 HidePanel (panelNEW);		HidePanel (panelHMI);		HidePanel (panelFEEDING);
		DisplayPanel (panelERROR);	 break;
		
		case NEW:		/*>>>*/		 HidePanel (panelERROR);	HidePanel (panelHMI);		HidePanel (panelFEEDING);			
		DisplayPanel (panelNEW);	 break;
		
		case HMI:		/*>>>*/		 HidePanel (panelERROR);	HidePanel (panelNEW);		HidePanel (panelFEEDING);
		DisplayPanel (panelHMI);	 break;
		
		case FEEDING:	/*>>>*/		 HidePanel (panelERROR);	HidePanel (panelNEW);		HidePanel (panelHMI);
		DisplayPanel (panelFEEDING); break;
	}

}

void MessagePanelFunc(int k){
	SetCtrlVal (panelERROR, ERROR_PICTURERING, k);
	//panelDidplayMachine(ERROR);
	DisplayPanel (panelERROR);
	
	/*Keys*/
	/*
	k=1 >>> thinking 01
 	k=2 >>> thinking 02
	k=3 >>> thinking 03
	k=4 >>> Written exsit Product
	k=5 >>> MSM Problem
	k=6 >>> SSM Problem
	k=7 >>> Enterance
	k=8 >>> Finish Operation Note
	*/

}

/*****************************************************************************************/
/******************************* Start Running  ******************************************/
/*****************************************************************************************/

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle 	= LoadPanel 	(0, "ChopFiller.uir", MAINPANEL)) 	< 0) 	return -1;
	if ((panelHMI 		= LoadPanel 	(0, "ChopFiller.uir", HMI)) 		< 0)	return -1;
	if ((panelNEW 		= LoadPanel 	(0, "ChopFiller.uir", NEW)) 		< 0)	return -1;
	if ((panelERROR 	= LoadPanel 	(0, "ChopFiller.uir", ERROR)) 		< 0)	return -1;
	if ((panelFEEDING 	= LoadPanel 	(0, "ChopFiller.uir", FEEDING)) 	< 0)	return -1;

/*****************************	Prepare Running		**************************************/
/*****************************************************************************************/
/*Read Data*/								dataFunc(READ);	

/*Pre Theoretical Factor Calculation*/		PreTheoryCalcFunc();

/*Define Password Ctrl*/					SetCtrlAttribute (panelERROR, ERROR_PASS, ATTR_ENABLE_CHARACTER_MASKING, 1);
											SetCtrlAttribute (panelERROR, ERROR_PASS, ATTR_MASK_CHARACTER, '*');
/*Define Ctrl To Hebrew*/					SetCtrlAttribute (panelHMI, HMI_RING, ATTR_TEXT_CHARACTER_SET, VAL_HEBREW_CHARSET);
											SetCtrlAttribute (panelHMI, HMI_TRANSLATOR, ATTR_TEXT_CHARACTER_SET, VAL_HEBREW_CHARSET);
/***********************************	Run		******************************************/
/*****************************************************************************************/
DisplayPanel (panelHandle);
RunUserInterface ();
/***********************************	Close	******************************************/
/*****************************************************************************************/
	DiscardPanel (panelHandle);
	DiscardPanel (panelHMI);
	DiscardPanel (panelNEW);
	DiscardPanel (panelERROR);
	DiscardPanel (panelFEEDING);
	
	CloseCom (SCALECOM);
	CloseCom (ARDUINOCOM);
	return 0;
}


/**************************************	   EXIT Panels  **********************************/
/*****************************************************************************************/
int CVICALLBACK exitPanel (int panel, int event, void *callbackData,int eventData1, int eventData2)
{	
switch (event)
	{
		case EVENT_GOT_FOCUS: 							break;
		case EVENT_LOST_FOCUS:							break;
		case EVENT_CLOSE:	  
			
		if(panel==panelHandle)	   { QuitUserInterface (0);								 }
		else if(panel==panelHMI)   { HidePanel (panelHMI);	 DisplayPanel (panelHandle); }
		else if(panel==panelNEW)   { HidePanel (panelNEW);	 DisplayPanel (panelHMI);    }
		else if(panel==panelERROR) { HidePanel (panelERROR); DisplayPanel (panelHMI);    }
														break;
		}
return 0;
}

void ThinkingFunc()
{
SetCtrlAttribute (panelERROR, ERROR_FIX_SAVE, ATTR_VISIBLE,0);
/*Round*/
		MessagePanelFunc(1);
		Delay(0.3);
		MessagePanelFunc(2);
		Delay(0.3);
		MessagePanelFunc(3);
		Delay(0.3);
		/**/
		
		/*Round*/
		MessagePanelFunc(1);
		Delay(0.3);
		MessagePanelFunc(2);
		Delay(0.3);
		MessagePanelFunc(3);
		Delay(0.3);
		/**/
		
		/*Round*/
		MessagePanelFunc(1);
		Delay(0.3);
		MessagePanelFunc(2);
		Delay(0.3);
		MessagePanelFunc(3);
		Delay(0.3);
		/**/
HidePanel(panelERROR);
SetCtrlAttribute (panelERROR, ERROR_FIX_SAVE, ATTR_VISIBLE,1);
}

void EnteranceFunc()
{
SetCtrlAttribute (panelERROR, ERROR_PASS, ATTR_VISIBLE,1);
SetCtrlAttribute (panelERROR, ERROR_NAME, ATTR_VISIBLE,1);
SetActiveCtrl (panelERROR, ERROR_NAME);
MessagePanelFunc(ENTERANCE);
}


void definePanelFunc(int i)
			{
				switch (i)
				{
					case 0:	//New Feeding >>>> Like InitializeFeedFunc()!!!!!!!!!!
						prev_per=Per=0.0;
						DefaultCtrl (panelFEEDING, FEEDING_M0);
						DefaultCtrl (panelFEEDING, FEEDING_M);
						DefaultCtrl (panelFEEDING, FEEDING_PERCENT);
						DefaultCtrl (panelFEEDING, FEEDING_VISUAL_PERCENT);
						DefaultCtrl (panelFEEDING, FEEDING_DURATION);
						break;
					case 1://Conveyor is Running
						SetCtrlVal(panelFEEDING,FEEDING_W_CONV,1);
						SetCtrlAttribute (panelFEEDING, FEEDING_FEED_BUT, ATTR_DIMMED,1);
						SetCtrlAttribute (panelFEEDING, FEEDING_STOP_BUT, ATTR_DIMMED,1);
						arrowFlag=0;
						prev_per=Per=0.0;
						DefaultCtrl (panelFEEDING, FEEDING_M0);
						DefaultCtrl (panelFEEDING, FEEDING_M);
						DefaultCtrl (panelFEEDING, FEEDING_PERCENT);
						DefaultCtrl (panelFEEDING, FEEDING_VISUAL_PERCENT);
						DefaultCtrl (panelFEEDING, FEEDING_DURATION);
						break;
				case 2:	//Restart
					prev_per=Per=0.0;
					DefaultPanel(panelHMI);
					DefaultPanel(panelFEEDING);
					DefaultPanel(panelERROR);
				break;
				
				}
			
			}
/*****************************************************************************************/
/**************************************** Dimmed Func 	**********************************/
void dimmedFunc(int onoff)
{
	//SetCtrlAttribute (panelHMI, 	HMI_RING, 			ATTR_DIMMED, onoff);
	//SetCtrlAttribute (panelHMI, 	HMI_FIX_SAVE, 		ATTR_DIMMED, onoff);
}

/*****************************************************************************************/
/********************************** 	HMI Func 	**************************************/
void UpdateHMIfunc()
{
/*HMI PANEL*/
//SetCtrlVal(panelHMI,HMI_FEEDING,(double)prevAvg);
//SetCtrlVal(panelHMI,HMI_PERCENT,Per);
//SetCtrlVal(panelHMI,HMI_COUNTER,ProductInProcess.ServingsCounter);

/*FEEDING PANEL*/
SetCtrlVal(panelFEEDING,FEEDING_M,prevAvg);
SetCtrlVal(panelFEEDING,FEEDING_PERCENT,Per);
SetCtrlVal(panelFEEDING,FEEDING_VISUAL_PERCENT,Per);
SetCtrlVal(panelFEEDING,FEEDING_COUNTER,ProductInProcess.ServingsCounter);

}


/*****************************************************************************************/
/**************************************** State Machine **********************************/
char InherenticData[1100];

void PrintToTxtfile(char* inputStr)
{

char InherenticDataFileName[50];

	sprintf(InherenticDataFileName,"data\\InherenticData.csv");
	fp = fopen (InherenticDataFileName,"a");
	fprintf (fp,"%s\n",inputStr);
fclose (fp);
}

void case5Func(char* inputScaleStr)
{
	char buff_arr[1000];
sscanf(inputScaleStr,"$: %s\n",buff_arr);
sprintf(InherenticData,"Id:%lf,%s,%s",ProductInProcess.productID,ProductInProcess.productName,buff_arr);
PrintToTxtfile(InherenticData);
}




void case4Func()
{
state=4;
sprintf(strToArduino,">%d",state);	
ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
ThinkingFunc();
//convFlag=1;
}

/*** State #03 ***/ 
void case3Func(char* inputScaleStr)
{
if(state==3)
		{
		//SetCtrlAttribute (panelFEEDING, FEEDING_TIMER, ATTR_ENABLED,1);
		
		static char strw[50];
		sscanf(inputScaleStr,"%s \t %lf %s",strw,&weight,strw);
		newAvg=weight*1000;
		if(prevAvg<=(newAvg-BOX_M0) && flag!=1)
			{
				prevAvg=newAvg-BOX_M0;
				Per=100.0*((double)prevAvg/(double)ProductInProcess.targetVal);
				sprintf(strToArduino,"#%d,%d",prevAvg,(int)Per);
				ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
				UpdateHMIfunc();
			}
			else if(ProductInProcess.targetVal<=newAvg-BOX_M0)
			{
				prevAvg=newAvg-BOX_M0;
				Per=100.0*((double)prevAvg/(double)ProductInProcess.targetVal);
				sprintf(strToArduino,"#%d,%d",prevAvg,(int)Per);
				ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
				UpdateHMIfunc();
				flag=1;
			}
			
		if(timer_arr[FEEDING_MSM][STOP]==0.00 && Per>=Q)
				{ 
				GetCurrentDateTime (&timer_arr[FEEDING_MSM][STOP]);
				SetCtrlVal (panelHandle,MAINPANEL_STOP_MSM, timer_arr[FEEDING_MSM][STOP]);
				newFlag=1;
				}	
			}
		
		SetCtrlVal (panelHandle, MAINPANEL_WEIGHT,newAvg);
		}



/*** State #02 - Restart Feeding***/ 
void case2Func(char* inputScaleStr)
{
	if(state==2)
	{
	//newFlag=0;
	static char strw[50];
		ST_counter++;
			if(ST_counter>=STEADY_STATE_FLAG)
			{
			ST_counter=0;	
			sscanf(inputScaleStr,"%s \t %lf %s",strw,&weight,strw);
			newAvg=weight*1000;
			
			SetCtrlVal (panelHandle, MAINPANEL_M0_WEIGHT,newAvg);
			SetCtrlVal (panelFEEDING, FEEDING_M0,newAvg);
			
			BOX_M0=newAvg;
			sprintf(strToArduino,"BOX_M0>>>%d",BOX_M0);
			ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
			state=3;
			
			Delay(1.0);
			Per=0.00;
			sprintf(strToArduino,"#%d,%d",(newAvg-BOX_M0),(int)Per);
			ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
					
			GetCurrentDateTime (&timer_arr[FEEDING_SSM][START]);
			SetCtrlVal(panelHandle,MAINPANEL_START_SSM,timer_arr[FEEDING_SSM][START]);
			GetCurrentDateTime (&timer_arr[FEEDING_MSM][START]);
			SetCtrlVal(panelHandle,MAINPANEL_START_MSM,timer_arr[FEEDING_MSM][START]);
			}
		}

}


/*** State #01  - Activate STATE & TargetVal Mass	***/ 
void case1Func()
{
	state=1;
	sprintf(strToArduino,">%d,%d,%d",state,ProductInProcess.targetVal,ProductInProcess.productRaw);
	ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
	state=2;
	//case2Func();

}

int RPM_Calc_Func(int steps, double sec)
{
//STEPPER_FACTOR 5.625
//RPM=((((double)steps*STEPPER_FACTOR)/360.0)*sec)*60.0>>>
//>>>
//>>>
	double divide_step=(double)steps/STEP_FACTOR_D;
	RPM=(divide_step*STEPPER_FACTOR)/(sec*6.0);	
	
	//RPM=((double)steps*STEPPER_FACTOR)/(sec*6.0);	
	return RPM;
}


/*** State #00  -  Get Activation From Arduino To HMI	***/ 
void case0Func(char* inputStr)
{
  /*Says Arduino finish to feeding ProductInProcess.targetVal serving*/
  if(inputStr[0]=='F') {
						//state=0;
	  					startFlag=0;
						
						GetCurrentDateTime (&timer_arr[FEEDING_SSM][STOP]);
						SetCtrlVal(panelHandle,MAINPANEL_STOP_SSM,timer_arr[FEEDING_SSM][STOP]);
						
						timer_arr[FEEDING_SSM][DT]=GetDeltaTimeFunc(timer_arr[FEEDING_SSM][START], timer_arr[FEEDING_SSM][STOP]);
						ProductInProcess.feedSecDuration_SSM=timer_arr[FEEDING_SSM][DT];
						
						SetCtrlVal (panelFEEDING, FEEDING_DURATION, ProductInProcess.feedSecDuration_SSM);
  						sscanf(inputStr,"Finaly %d Serving, %d[step], %d[step], %d[g], %d[Encoder], kp %d, ki %d, kdd %d, Q %d",
							   											 &ProductInProcess.ServingsCounter,
                                                                         &ProductInProcess.MSM_stepCounter,
                                                                         &ProductInProcess.SSM_stepCounter,  
                                                                         &ProductInProcess.NewTotalError,
							  											 &ProductInProcess.Arduino_SSM_Encoder,
																	 	 &ProductInProcess.i_kp,
																	 	 &ProductInProcess.i_ki,
																	 	 &ProductInProcess.i_kdd,
																	 	 &ProductInProcess.i_Q); 
						
						S_RPM = RPM_Calc_Func(ProductInProcess.SSM_stepCounter,ProductInProcess.feedSecDuration_SSM);
						SetCtrlVal (panelHandle, MAINPANEL_S_RPM,S_RPM );
						SetCtrlVal (panelHandle, MAINPANEL_S_DURATION,timer_arr[FEEDING_SSM][DT]);
						
						timer_arr[FEEDING_MSM][DT] = GetDeltaTimeFunc(timer_arr[FEEDING_MSM][START], timer_arr[FEEDING_MSM][STOP]);
						ProductInProcess.feedSecDuration_MSM = timer_arr[FEEDING_MSM][DT]; 
						M_RPM=RPM_Calc_Func(ProductInProcess.MSM_stepCounter,ProductInProcess.feedSecDuration_MSM);
						SetCtrlVal (panelHandle, MAINPANEL_M_RPM, M_RPM);
						SetCtrlVal (panelHandle, MAINPANEL_M_DURATION,timer_arr[FEEDING_MSM][DT]);
						
						StorageFeedsFunc();
						
						InitializeFeedFunc();
						
						state=0; 
						arrowFlag=1;
						}
  
/**/
//	else if(inputStr[0]=='$'){ case5Func(inputStr)}
/*SROP Feeding*/
	else if(inputStr[0]=='!') { state=0;  readFlag=0;	startFlag=0;}
/*Conv Function >>> Statr 4 in Arduino*/
	else if(inputStr[0]=='C') { state=0; /*MessagePopup("vf","fsd");*/ 
								SetCtrlAttribute (panelFEEDING, FEEDING_ARROW, ATTR_VISIBLE,0);
								SetCtrlAttribute (panelFEEDING, FEEDING_FEED_BUT, ATTR_DIMMED,0);
								SetCtrlAttribute (panelFEEDING, FEEDING_STOP_BUT, ATTR_DIMMED,0);
							   }
}

/*****************************************************************************************/
							/*End Of State Machine*/
 
/*****************************************************************************************/
/**************************************** Communication **********************************/	//Scale & Arduino defines communication
/*****************************************************************************************/

/*****************************************************************************************/
/*** Scale Communication ***/
void CVICALLBACK SerialScale (int COM, int eventMask, void *callbackData)
{
	int bytesRead;
	char strScale[50];

if(readFlag){
		while (GetInQLen (SCALECOM) > 2)
		{
			bytesRead=ComRdTerm (SCALECOM, strScale, 50,'\n');
			strScale[bytesRead-1]='\0';
			InsertTextBoxLine (panelHandle, MAINPANEL_WEIGHT_TEXTBOX, -1, strScale);
//			InsertTextBoxLine (panelHandle, MAINPANEL_WEIGHT_TEXTBOX, -1, strToArduino);

		if( (startFlag==1) && (strScale[0]=='S' || strScale[0]=='U'))	
				{
				switch (state){
				case 3:
				case3Func(strScale); /*Get From SCALE To ARDUINO ACTIVATION*/
				break;
				case 2:
				case2Func(strScale);
				break;
				}
			}
			SetCtrlAttribute (panelHandle, MAINPANEL_WEIGHT_TEXTBOX, ATTR_FIRST_VISIBLE_LINE, Scale_Counter_Box);
			Scale_Counter_Box++; 
			
		}
	}
	
}

/*****************************************************************************************/
/*** Arduino Communication ***/ 
void CVICALLBACK SerialArduino (int portNumber, int eventMask, void *callbackData)
{
	int bytesRead;
	char strArduino[1000];
if(newFlag){
	while (GetInQLen (ARDUINOCOM) > 0)
	{
		bytesRead=ComRdTerm (ARDUINOCOM, strArduino, 999, '\n');	
		if(bytesRead>=1) strArduino[bytesRead-1]='\0';
		InsertTextBoxLine (panelHandle, MAINPANEL_ARDUINO_TEXTBOX, -1, strArduino);

		if( (startFlag==1) && (strArduino[0]=='F' || strArduino[0]=='C') )	
			{
			case0Func(strArduino); /*Get From ARDUINO*/
			}
	else if ((startFlag==0) && (strArduino[0]=='$')) //In the beggining of "Case0Func" The StarFlag is Change to "0" !!!!
			{case5Func(strArduino);}
			
		SetCtrlAttribute (panelHandle, MAINPANEL_ARDUINO_TEXTBOX, ATTR_FIRST_VISIBLE_LINE, Arduino_Counter_Box);
		Arduino_Counter_Box++; 
	}
 }
}

/*****************************************************************************************/
/*** Creating Communication ***/
int CVICALLBACK Connecting (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control)
			{
				case MAINPANEL_ARDUINO:
					int connection_flag;
					GetCtrlVal (panelHandle, MAINPANEL_ARDUINO, &connection_flag);
					switch(connection_flag)
					{
						case 0:// Disconnect
							if(arduinoFlag==1){CloseCom(ARDUINOCOM);}
							break;
						case 1: //Connect
							DisableBreakOnLibraryErrors();
							SetCtrlAttribute (panelHandle, MAINPANEL_COMMANDBUTTON_SCALE, ATTR_DIMMED, 0);
							OpenComConfig (ARDUINOCOM, "", ARD_BaoudRate, ARD_Parity, ARD_DataB, ARD_StopB, ADR_InSize, ADR_OutSize);
							FlushInQ (ARDUINOCOM);
							InstallComCallback (ARDUINOCOM, LWRS_RXFLAG, 0, '\n', SerialArduino, 0);
							arduinoFlag=1;
							EnableBreakOnLibraryErrors(); 
							break;
					}
				break;
				
				case MAINPANEL_COMMANDBUTTON_SCALE:
					DisableBreakOnLibraryErrors();
					OpenComConfig (SCALECOM, "", SCL_BaoudRate, SCL_Parity, SCL_DataB, SCL_StopB, SCL_InSize, SCL_OutSize);
					FlushInQ (SCALECOM);
					InstallComCallback (SCALECOM, LWRS_RXFLAG, 0, '\n', SerialScale, 0);
					weightFlag=1;
					EnableBreakOnLibraryErrors(); 
				break;
				case MAINPANEL_COMMANDBUTTON_UNO:
					/**/
				
					/**/
					DisableBreakOnLibraryErrors();
					SetCtrlAttribute (panelHandle, MAINPANEL_COMMANDBUTTON_SCALE, ATTR_DIMMED, 0);
					OpenComConfig (ARDUINOCOM, "", ARD_BaoudRate, ARD_Parity, ARD_DataB, ARD_StopB, ADR_InSize, ADR_OutSize);
					FlushInQ (ARDUINOCOM);
					InstallComCallback (ARDUINOCOM, LWRS_RXFLAG, 0, '\n', SerialArduino, 0);
					arduinoFlag=1;
					EnableBreakOnLibraryErrors(); 
				break;
			}
		
		if(arduinoFlag==1 && weightFlag==1)
		{	DisplayPanel (panelHMI);	}
		
	break;
	}
 return 0;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
							/*End Of Communication*/

int CVICALLBACK Save_AND_BeginingFunc (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{   switch (event)
	{
		case EVENT_COMMIT:
		/*Turn On Operation*/	
		SetCtrlAttribute (panelHMI, HMI_OFF_ON, ATTR_DIMMED, 0); 
					
					GetCtrlVal (panelHMI, HMI_TRANSLATOR,ProductInProcess.productName);
					SetCtrlVal (panelFEEDING, FEEDING_TRANSLATOR, ProductInProcess.productName);
					
					GetCtrlVal (panelHMI, HMI_RAW,&ProductInProcess.productRaw);
					SetCtrlVal (panelFEEDING, FEEDING_RAW,ProductInProcess.productRaw);
										
					GetCtrlVal (panelHMI, HMI_MTV,&ProductInProcess.targetVal);
					SetCtrlVal (panelFEEDING, FEEDING_MTV,(int)ProductInProcess.targetVal);
					SetCtrlVal (panelHandle, MAINPANEL_Mtv_WEIGHT, ProductInProcess.targetVal);
				
					GetCtrlVal (panelHMI, HMI_MOCT,&ProductInProcess.operationWeight);
					SetCtrlVal (panelFEEDING, FEEDING_MOCT,ProductInProcess.operationWeight);
			break;
	}
	return 0;
}

int CVICALLBACK offonFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{   
	switch (event)
	{
		case EVENT_COMMIT:
						PreCalcFunc();		/*** Pre Operation Calculate ***/
						GetCtrlVal (panelHMI, HMI_OFF_ON, &offonFlag);
						SetCtrlAttribute (panelFEEDING, FEEDING_FEEDING_TIMER, ATTR_ENABLED, offonFlag);
						GetCurrentDateTime (&timer_arr[OPERATION][START]);
						int len = strlen(ProductInProcess.date);
						if(len<1){
								 GetTimeFunc(NOW);	sprintf(ProductInProcess.date,"%.2d/%.2d/%d",FEED.NOW_Day,FEED.NOW_Month,FEED.NOW_Year);
								 }
						DefaultPanel(panelHMI);
						HidePanel(panelHMI);
						DisplayPanel(panelFEEDING);
						
						sprintf(strToArduino,"Z");
						ComWrt (SCALECOM, strToScale, strlen(strToScale)+1);
						readFlag=offonFlag;
			break;
	}
	return 0;
}


int CVICALLBACK productFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHMI, HMI_RING, &productChose);
			GetLabelFromIndex (panelHMI, HMI_RING,productChose ,productChoseName);																				   
			if (productChose==0)	{ dimmedFunc(0);	DisplayPanel (panelNEW);  SetActiveCtrl (panelNEW, NEW_NEW_FIX_NAME_STR);} 
			else 					{ foundFlag=LookingForFunc(productChoseName,productChose);}
			if(foundFlag==1)
				{
				SetCtrlVal (panelHMI, HMI_TRANSLATOR, ProductInProcess.productName);
				SetCtrlVal (panelHMI, HMI_RAW, ProductInProcess.productRaw);
				SetCtrlVal (panelHMI, HMI_MTV, ProductInProcess.targetVal);
				SetCtrlVal (panelHMI, HMI_MOCT,ProductInProcess.operationWeight);
				}
		break;
	}
	return 0;
}


void DataBaseFunc_Feeds()
{
//char HeadingArr[1000],Headers[1000];


/*Headers*/
//sprintf(Headers,"productID,Date,ProductName,Raw[kg/m^3],TargetWeight[gram],FinalWeight[gram],Per_Error[%c],ServingCounter,MainSteeperMotor_STEPS[step],SubStepperMotor_STEPS[step],StepperCountFactor[degrees/step],StepperDataSheetsFactor[degrees/step],ARD_Steps[step],QFactor[%c],Kp,Ki,Kd,kdd,KD_FACTOR,PI_FACTOR,FeedPercentage[%c],Prev_Error_Per[%c],MainSteeperMotor_Duration[sec],MainSteeperMotor_RPM[rev/min],SubSteeperMotor_Duration[sec],SubSteeperMotor_RPM[rev/min],DateTimeElements,day,month,year,",'%','%','%','%');									
//sprintf(HeadingArr,"MSM_Mass_Flow_Pitch[kg/Cyc],MSM_Mass_Flow_Deg[kg/Deg],MSM_Mass_Flow_Step[kg/Step],SSM_Mass_Flow_Pitch[kg/Cyc],SSM_Mass_Flow_Deg[kg/Deg],SSM_Mass_Flow_Step[kg/Step],PRACTICAL_VOL_RATIO_MSM_SSM,REQ_Vol_Total[kg/Cyc],Dynamic_Q,REQ_BOTH_VOL[kg/m^3],REQ_SECOND_VOL[kg/m^3],BOTH_VOL_FOR_PITCH[kg/m^3],REQ_STEP_BOTH[step],REQ_STEP_SECOND[step],REQ_MSM_STEPS[step],REQ_SSM_STEPS[step],M_Flow,V_Flow,TRUE_VOL_SSM_CYC[m^3/Cyc],TRUE_SSM_FACTOR_FILL<0-1>,TRUE_VOL_MSM_CYC[m^3/Cyc],TRUE_MSM_FACTOR_FILL<0-1>,MSM_Factor_FILL,SSM_Factor_FILL,Practicl_MSM_VOL_PITCH[m^3/Cyc],Practicl_SSM_VOL_PITCH[m^3/Cyc]");



char DataBaseFileName[50];
	sprintf(DataBaseFileName,"data\\FeedsDataBase.csv");
	fp = fopen (DataBaseFileName,"a");
	//fprintf(fp,"%s%s\n",Headers,HeadingArr);
	
	
	
/********************************	DON'T DELET!!!			*****************************************************************/
/*Header*/			
	/*
	fprintf(fp,"
				Date, 									ProductName, 
				Raw[kg/m^3], 							TargetWeight[gram],
			 	FinalWeight[gram],						Per_Error[%c],
				ServingCounter,							MainSteeperMotor_STEPS[step],
				SubStepperMotor_STEPS[step],			StepperCountFactor[degrees/step],
			 	StepperDataSheetsFactor[degrees/step], 	ARD_Steps[step],
			 	QFactor[%c], 							Kp,
			 	Ki, 									Kd,
				kdd, 									KD_FACTOR,
			 	PI_FACTOR, 								FeedPercentage[%c],
			 	Prev_Error_Per[%c], 					MainSteeperMotor_Duration[sec],
			 	MainSteeperMotor_RPM[rev/min], 			SubSteeperMotor_Duration[sec],
				SubSteeperMotor_RPM[rev/min], 			DateTimeElements,
			 	day, 									month,
			 	year
																	\n",'%','%','%','%');									
	*/
	/*Data*/		for (int i=1;i<=ProductInProcess.ServingsCounter;i++)
						{
						 fprintf (fp,"%lf ,%s,%s,%d,%d ,%d,%d,%d,%d ,%d,%d,%lf,%d ,%d,%lf,%lf ,%lf,%lf,%lf,%lf ,%lf,%d,%d,%d, %.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.8lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf\n",
									OperationInProccess.productID[i],
								  	OperationInProccess.date[i],				OperationInProccess.productName[i],
									OperationInProccess.productRaw[i],			OperationInProccess.targetVal[i],
									
									OperationInProccess.FinalWeight[i],			OperationInProccess.NewTotalError[i],
									OperationInProccess.ServingsCounter[i],		OperationInProccess.MSM_stepCounter[i],
									
									OperationInProccess.SSM_stepCounter[i],		STEP_FACTOR_I,
									STEPPER_FACTOR,								OperationInProccess.Arduino_SSM_Encoder[i],
									
									OperationInProccess.i_Q[i],					
									OperationInProccess.Error_Percent[i],		OperationInProccess.Prev_Error_Percent[i],
									
									OperationInProccess.feedSecDuration_MSM[i],	OperationInProccess.M_RPM[i],
									OperationInProccess.feedSecDuration_SSM[i],	OperationInProccess.S_RPM[i],
									
									OperationInProccess.DateTimeElements[i],	OperationInProccess.day[i],
									OperationInProccess.month[i],				OperationInProccess.year[i],
									
			MSM_Mass_Flow_Pitch*pow(10.0,-6.0),	MSM_Mass_Flow_Deg*pow(10.0,-6.0),	MSM_Mass_Flow_Step*pow(10.0,-6.0),
			SSM_Mass_Flow_Pitch*pow(10.0,-6.0),	SSM_Mass_Flow_Deg*pow(10.0,-6.0),	SSM_Mass_Flow_Step*pow(10.0,-6.0),
			PRACTICAL_VOL_RATIO_MSM_SSM,	REQ_Vol_Total*pow(10.0,-6.0),	Dynamic_Q,
			REQ_BOTH_VOL*pow(10.0,-6.0),	REQ_SECOND_VOL*pow(10.0,-6.0),	BOTH_VOL_FOR_PITCH*pow(10.0,-6.0),
			REQ_STEP_BOTH,	REQ_STEP_SECOND,REQ_MSM_STEPS,REQ_SSM_STEPS,
			OperationInProccess.M_Flow[ProductInProcess.ServingsCounter],
			OperationInProccess.VOL_Flow[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_SSM_FACTOR_FILL[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_VOL_MSM_CYC[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_MSM_FACTOR_FILL[ProductInProcess.ServingsCounter],
			MSM_Factor_FILL,	SSM_Factor_FILL,
			Practicl_MSM_VOL_PITCH,	Practicl_SSM_VOL_PITCH);
					 	}
				fclose (fp);
}															

int CVICALLBACK workingFunc (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control)
			{
			case FEEDING_FEED_BUT:
			timer_arr[FEEDING_MSM][START]=0.00;	
			timer_arr[FEEDING_MSM][STOP]=0.00;
			//productID[Amount_Of_Feeds_In_Operation];
			readFlag=1;		startFlag=1;	arrowFlag=0;
			ProductInProcess.ServingsCounter++;
			definePanelFunc(0);
			case1Func();	
			break;
			
			case FEEDING_STOP_BUT:
			timer_arr[FEEDING_MSM][STOP]=0.00;
			state=0;	sprintf(strToArduino,">%d,%lf",state,(double)ProductInProcess.targetVal);
			ComWrt (ARDUINOCOM, strToArduino, strlen(strToArduino)+1);
			ProductInProcess.ServingsCounter--;
			definePanelFunc(0);
			case0Func("!STOP");
			break;
			
			case FEEDING_CONV_BUT:
			definePanelFunc(1);
			case4Func();
			startFlag=1;
			break;	
			
			case FEEDING_FINISH_OP_BUT:
			GetCurrentDateTime (&timer_arr[OPERATION][STOP]);
			SetCtrlVal(panelHandle,MAINPANEL_OPER_DURATION,FEED.feedSecDuration[1]);
			MessagePanelFunc(FINISH_OPER); /*FINISH_OPER 8*/		
			DataBaseFunc_Feeds();
			/*BONUS - Add Default all parameters and state to Initial Operation*/
			break;	
			
			}
			break;
	}
	return 0;
}

void UpdateRingProducts()
{
SetCtrlAttribute (panelHMI, HMI_RING, ATTR_TEXT_CHARACTER_SET, VAL_HEBREW_CHARSET);
GetNumListItems (panelHMI, HMI_RING,&RingCounter);
InsertListItem (panelHMI, HMI_RING, RingCounter,ProductInProcess.productName , RingCounter);
GetLabelFromIndex (panelHMI, HMI_RING,RingCounter ,bufftry );
SetCtrlIndex (panelHMI, HMI_RING, RingCounter);
//SetCtrlVal (panelHMI, HMI_TRY, bufftry);
}

int CVICALLBACK writeFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			switch (control)
			  {
				case NEW_NEW_FIX_NAME_STR:
					GetCtrlVal (panelNEW, NEW_NEW_FIX_NAME_STR, miniBuff);
					if(LookingForFunc(miniBuff,-1))	{MessagePanelFunc(4); foundFlag=0;}
					else {GetCtrlVal (panelNEW, NEW_NEW_FIX_NAME_STR,ProductInProcess.productName);				
					SetActiveCtrl (panelNEW, NEW_NEW_FIX_RAW_NUM); foundFlag=1;	 } break;
																																										   
				case NEW_NEW_FIX_RAW_NUM:
					if(foundFlag) { GetCtrlVal (panelNEW,NEW_NEW_FIX_RAW_NUM,&ProductInProcess.productRaw);		
					SetActiveCtrl (panelNEW, NEW_NEW_FIX_TARGET_NUM); 	 } break;
				
				case NEW_NEW_FIX_TARGET_NUM:
					if(foundFlag) {	GetCtrlVal (panelNEW,NEW_NEW_FIX_TARGET_NUM,&ProductInProcess.targetVal);	
					SetActiveCtrl (panelNEW, NEW_NEW_FIX_OPERATION_NUM); } break;
			
				case NEW_NEW_FIX_OPERATION_NUM:
					if(foundFlag) { GetCtrlVal (panelNEW,NEW_NEW_FIX_OPERATION_NUM,&ProductInProcess.operationWeight); 
					SetActiveCtrl (panelNEW, NEW_NEW_FIX_SAVE); } break;
				
				case NEW_NEW_FIX_SAVE:
					SetCtrlAttribute (panelHMI, HMI_OFF_ON, ATTR_DIMMED, 0);

					ProductInProcess.NewTotalError=0;

					SetCtrlVal (panelHMI, HMI_TRANSLATOR, ProductInProcess.productName);
					SetCtrlVal (panelFEEDING, FEEDING_TRANSLATOR,ProductInProcess.productName);		
					
					SetCtrlVal (panelHMI, HMI_RAW,ProductInProcess.productRaw);
					SetCtrlVal (panelFEEDING, FEEDING_RAW, ProductInProcess.productRaw);
 					
					ProductInProcess.targetVal = ProductInProcess.targetVal;
					SetCtrlVal (panelHMI, HMI_MTV,ProductInProcess.targetVal);
					SetCtrlVal (panelFEEDING, FEEDING_MTV, ProductInProcess.targetVal);
								
					SetCtrlVal (panelHMI, HMI_MOCT,ProductInProcess.operationWeight);
					SetCtrlVal (panelFEEDING, FEEDING_MOCT, ProductInProcess.operationWeight);
					UpdateRingProducts();
					DefaultPanel(panelNEW);
					HidePanel(panelNEW);
					DisplayPanel(panelHMI);
				break;
				case NEW_NEW_FIX_CANCEL :
				  DefaultCtrl (panelNEW, NEW_NEW_FIX_NAME_STR);
				  DefaultCtrl (panelNEW, NEW_NEW_FIX_RAW_NUM);
				  DefaultCtrl (panelNEW, NEW_NEW_FIX_TARGET_NUM);
				  DefaultCtrl (panelNEW, NEW_NEW_FIX_OPERATION_NUM);
				break;
			  }	
		}	
			break;
	}
	return 0;
}

int CVICALLBACK CalBackErrorPanel (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelERROR, ERROR_PICTURERING, &ErrorFlag);
			DefaultCtrl (panelERROR, ERROR_PICTURERING);
			HidePanel (panelERROR);
			switch (ErrorFlag)
			{
				case 4:	//Product Exist >>>	Dicard NEW_PANEL >>> Copy Details To work Space >>> Show this part
			
				DisplayPanel (panelNEW);
				break;
					
				case 7:
				DefaultCtrl (panelERROR, ERROR_NAME);
				SetCtrlAttribute (panelERROR, ERROR_NAME, ATTR_VISIBLE, 0);
				DefaultCtrl (panelERROR, ERROR_PASS);
				SetCtrlAttribute (panelERROR, ERROR_PASS, ATTR_VISIBLE, 0);
				DisplayPanel (panelHMI);	
					break;
					
				case 8:
				definePanelFunc(2);
				ThinkingFunc();	
				panelDidplayMachine(MAINPANEL);
				break;
			
			}
			break;
	}
	return 0;
}


void LEDStateFunc()
{
	if(offonFlag)
	{
		SetCtrlVal(panelFEEDING,FEEDING_W_SCALE,1);
		SetCtrlVal(panelFEEDING,FEEDING_W_VIBRATOR,1);
	
		if (Per<=Q && state==3)
			{
			SetCtrlVal(panelFEEDING,FEEDING_W_MSM,1);
			SetCtrlVal(panelFEEDING,FEEDING_W_SSM,1);
			}
			else if(Per>Q && state==3)
			{
			SetCtrlVal(panelFEEDING,FEEDING_W_MSM,0);
			SetCtrlVal(panelFEEDING,FEEDING_W_SSM,1);
			}
			else
			{
			SetCtrlVal(panelFEEDING,FEEDING_W_MSM,0);
			SetCtrlVal(panelFEEDING,FEEDING_W_SSM,0);
			}
	if (state==4)
		{SetCtrlVal(panelFEEDING,FEEDING_W_CONV,1);}
		else{SetCtrlVal(panelFEEDING,FEEDING_W_CONV,0);}

	}
	else {	SetCtrlVal(panelFEEDING,FEEDING_W_SCALE,0);	SetCtrlVal(panelFEEDING,FEEDING_W_VIBRATOR,0);}
}


int CVICALLBACK TimerFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			LEDStateFunc();
			GetTimeFunc(NOW);
			SetCtrlVal (panelFEEDING, FEEDING_HOUR,FEED.NOW_Hour);
			SetCtrlVal (panelFEEDING, FEEDING_MIN,FEED.NOW_Minute);
			SetCtrlVal (panelFEEDING, FEEDING_SEC,(int)FEED.NOW_Second);
			SetCtrlVal (panelHandle, MAINPANEL_STATE, state);
			
			SetCtrlVal (panelHandle, MAINPANEL_PER, Per);
			SetCtrlVal (panelHandle, MAINPANEL_PREV_PER, prev_per);
			
			/*Conveyor Arrow blinking*/
			if(arrowFlag)
			{
			ans++;
			ans=ans%2;
			SetCtrlAttribute (panelFEEDING, FEEDING_ARROW, ATTR_VISIBLE,ans);
			}
			break;
	}
	return 0;
}

void printFunc()
{
char TheoreticDataFileName[50], StringToTheoretic[1000], HeadingArr[1000];
sprintf(HeadingArr,"MSM_Mass_Flow_Pitch[kg/Cyc],MSM_Mass_Flow_Deg[kg/Deg],MSM_Mass_Flow_Step[kg/Step],SSM_Mass_Flow_Pitch[kg/Cyc],SSM_Mass_Flow_Deg[kg/Deg],SSM_Mass_Flow_Step[kg/Step],PRACTICAL_VOL_RATIO_MSM_SSM,REQ_Vol_Total[kg/Cyc],Dynamic_Q,REQ_BOTH_VOL[kg/m^3],REQ_SECOND_VOL[kg/m^3],BOTH_VOL_FOR_PITCH[kg/m^3],REQ_STEP_BOTH[step],REQ_STEP_SECOND[step],REQ_MSM_STEPS[step],REQ_SSM_STEPS[step],M_Flow,V_Flow,TRUE_VOL_SSM_CYC[m^3/Cyc],TRUE_SSM_FACTOR_FILL<0-1>,TRUE_VOL_MSM_CYC[m^3/Cyc],TRUE_MSM_FACTOR_FILL<0-1>,MSM_Factor_FILL,SSM_Factor_FILL,Practicl_MSM_VOL_PITCH[m^3/Cyc],Practicl_SSM_VOL_PITCH[m^3/Cyc]");
/*
MSM_Mass_Flow_Pitch[kg/Cyc],MSM_Mass_Flow_Deg[kg/Deg],MSM_Mass_Flow_Step[kg/Step],
SSM_Mass_Flow_Pitch[kg/Cyc],SSM_Mass_Flow_Deg[kg/Deg],SSM_Mass_Flow_Step[kg/Step],
PRACTICAL_VOL_RATIO_MSM_SSM,REQ_Vol_Total[kg/Cyc],Dynamic_Q
REQ_BOTH_VOL[kg/m^3],REQ_SECOND_VOL[kg/m^3],BOTH_VOL_FOR_PITCH[kg/m^3],
REQ_STEP_BOTH[step],REQ_STEP_SECOND[step],REQ_MSM_STEPS[step],REQ_SSM_STEPS[step],
M_Flow,V_Flow,
TRUE_VOL_SSM_CYC[m^3/Cyc],TRUE_SSM_FACTOR_FILL<0-1>,
TRUE_VOL_MSM_CYC[m^3/Cyc],TRUE_MSM_FACTOR_FILL<0-1>,
MSM_Factor_FILL,SSM_Factor_FILL,
Practicl_MSM_VOL_PITCH[m^3/Cyc],Practicl_SSM_VOL_PITCH[m^3/Cyc],
*/

	sprintf(StringToTheoretic,"%s\n %.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf,%.6lf\n",
			HeadingArr,
			MSM_Mass_Flow_Pitch*pow(10.0,-6.0),	MSM_Mass_Flow_Deg*pow(10.0,-6.0),	MSM_Mass_Flow_Step*pow(10.0,-6.0),
			SSM_Mass_Flow_Pitch*pow(10.0,-6.0),	SSM_Mass_Flow_Deg*pow(10.0,-6.0),	SSM_Mass_Flow_Step*pow(10.0,-6.0),
			PRACTICAL_VOL_RATIO_MSM_SSM,	REQ_Vol_Total*pow(10.0,-6.0),	Dynamic_Q,
			REQ_BOTH_VOL*pow(10.0,-6.0),	REQ_SECOND_VOL*pow(10.0,-6.0),	BOTH_VOL_FOR_PITCH*pow(10.0,-6.0),
			REQ_STEP_BOTH,	REQ_STEP_SECOND,REQ_MSM_STEPS,REQ_SSM_STEPS,
			OperationInProccess.M_Flow[ProductInProcess.ServingsCounter],
			OperationInProccess.VOL_Flow[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_VOL_SSM_CYC[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_SSM_FACTOR_FILL[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_VOL_MSM_CYC[ProductInProcess.ServingsCounter],
			OperationInProccess.TRUE_MSM_FACTOR_FILL[ProductInProcess.ServingsCounter],
			MSM_Factor_FILL,	SSM_Factor_FILL,
			Practicl_MSM_VOL_PITCH,	Practicl_SSM_VOL_PITCH);
			

	sprintf(TheoreticDataFileName,"data\\TheoreticData.csv");
	fp = fopen (TheoreticDataFileName,"a");
	fprintf (fp,"%s\n",StringToTheoretic);
fclose (fp);
}

int CVICALLBACK checkErrorFunc (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		GetCtrlVal (panelHandle, MAINPANEL_ERROR, &checkError);
		if		(checkError==WAIT)		{ThinkingFunc();}
		else if (checkError==ENTERANCE)	{EnteranceFunc();}
		else if (checkError==100)		{printFunc();}
		else{MessagePanelFunc(checkError);}
		DefaultCtrl (panelHandle,MAINPANEL_ERROR);
	break;
	}
return 0;
}

int CVICALLBACK EnterFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control)
			{
				case ERROR_NAME:
					SetActiveCtrl (panelERROR, ERROR_PASS);
					break;
				case ERROR_PASS:
					SetActiveCtrl (panelERROR, ERROR_FIX_SAVE);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK function (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		GetNumListItems (panelHMI, HMI_RING,&RingCounter);
	
	for (int i=1;i<RingCounter;i++)
	{
	GetLabelFromIndex (panelHMI, HMI_RING,i ,try[i]);
	}
		char FileName[50];
			int i=0,k=0;
			{		
					   	sprintf(FileName,"data\\main_productdataFunction.csv");
						fp = fopen (FileName,"w");
						for(k=0;k<1;k++)
						{
						fprintf(fp,"Date, No.Product, Product, Raw, OperationWeight[gram], TargetWeight[geam], Amount[=E/F],	MaxReqFeedTime[msec][=FLOOR.Math(E/14400)],	LastTotalError\n");
						}
						for (i=1;i<=AmountOfLines;i++)
						{
						 fprintf (fp,"%s,%d,%s,%d,%d,%d,%d,%d,%d\n",
						 product[i].date,				product[i].ProductNo,
						 try[i],		product[i].productRaw,	
						 product[i].operationWeight, 	product[i].targetVal,
						 product[i].amount,				product[i].MaxReqfeedTime,
						 product[i].LastTotalError);
						
						 }
				fclose (fp);
				}		

			break;
	}
	return 0;
}
