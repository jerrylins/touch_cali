/*
 * =====================================================================================
 *
 *       Filename:  tp_calibration.c
 *
 *    Description:  calibration interface
 *
 *        Version:  1.0
 *        Created:  Friday, May 03, 2013 04:50:13 HKT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangyunxi 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "tp_calibration.h"
#define FLAG_SCREEN_DATA   0x01
#define FLAG_UNCALI_DATA   0x02
static int CALI_FLAG=0;

int screen_x[5] = { 0 };
int screen_y[5] = { 0 };
int uncali_x_default[5] = { 0 };
int uncali_y_default[5] = { 0 };
int uncali_x[5] = { 0 };
int uncali_y[5] = { 0 };

UHPOINT screen_cali[5]; 
UHPOINT uncali_cali[5]; 
UHPOINT screen_dis[5]; 
MATRIX  matrix; 

/*	5 点 校 准
	
	A ------------------- B
	|		   |		  |
	|		   |		  |
	| -------- E -------- |
	|		   |		  |
	|		   |		  | 				 
	C ------------------- D 		
*/
#define LCD_W 800
#define LCD_H 480
static UHPOINT gTouchADCaliPoint[] = { 
		{945,321},		    // 左上校准点 -- A
		{290,321},			// 右上校准点 -- B
		{945,925},			// 左下校准点 -- C
		{290,925},			// 右下校准点 -- D
		{615,622}			// 中心校准点 -- E
	};

static UHPOINT gTouchLogicCaliPoint[] = {
		{20,20},						// 左上校准点 -- A
		{(LCD_W-20),20},				// 右上校准点 -- B
		{20,(LCD_H-20)},				// 左下校准点 -- C
		{(LCD_W-20),(LCD_H-20)},		// 右下校准点 -- D
		{LCD_W>>1,LCD_H>>1} 			// 中心校准点 -- E
	};
static int abs(int m)
{
	if(m<0)
		m=-m;
	return m;
}

/**************************************************************************
* 函数描述: 触摸屏校准
* 入口参数: pLogicPoint -- 5点校准的实际像素坐标值
*			pADPoint -- 5点校准的AD采样数值
* 出口参数: pCaliPtr -- 校准系数
* 返回值:	校准的状态显示
* Log	:
***************************************************************************/
int Touch_Calibrate( UHPOINT * pLogicPoint, UHPOINT * pADPoint, CALI_COEF * pCaliPtr)
{

 UHPOINT TouchADCaliPointE;
	
 //2 判断获取的A/B/C/D			四个采样点是否超出范围
  if ( abs(pADPoint[TopLPos_A].x - pADPoint[BotLPos_C].x) > CALI_X_DELTA  || 
	   abs(pADPoint[TopRPos_B].x - pADPoint[BotRPos_D].x) > CALI_X_DELTA	)
	  return CALI_X_ERR;
  
  if ( abs(pADPoint[TopLPos_A].y - pADPoint[TopRPos_B].y) > CALI_Y_DELTA  || 
	   abs(pADPoint[BotLPos_C].y - pADPoint[BotRPos_D].y) > CALI_Y_DELTA	)
	  return CALI_Y_ERR;
  
 //2 根据符合要求的A/B/C/D 点计算其AD 采样值与像素坐标的转换系数
  // *100是为了将小数点后面的精度体现到int中，反算回去时要/100
  
  pCaliPtr->Kx = ((pADPoint[TopRPos_B].x - pADPoint[TopLPos_A].x) *100/(pLogicPoint[TopRPos_B].x - pLogicPoint[TopLPos_A].x)
				 + (pADPoint[BotRPos_D].x - pADPoint[BotLPos_C].x) *100/(pLogicPoint[BotRPos_D].x - pLogicPoint[BotLPos_C].x))/2;	 

  pCaliPtr->Ky = ((pADPoint[BotLPos_C].y - pADPoint[TopLPos_A].y) *100/(pLogicPoint[BotLPos_C].y - pLogicPoint[TopLPos_A].y)
				 + (pADPoint[BotRPos_D].y - pADPoint[TopRPos_B].y) *100/(pLogicPoint[BotRPos_D].y - pLogicPoint[TopRPos_B].y))/2; 
  
  //2 根据上面得出的转换系数计算出中心校准点的AD 采样值
  TouchADCaliPointE.x = pADPoint[TopLPos_A].x + ( pLogicPoint[CentPos_E].x - pLogicPoint[TopLPos_A].x ) * pCaliPtr->Kx / 100 ;
  TouchADCaliPointE.y = pADPoint[TopLPos_A].y + ( pLogicPoint[CentPos_E].y - pLogicPoint[TopLPos_A].y ) * pCaliPtr->Ky / 100;

  //2 将计算出来的中心校准点AD 采样值与实际该点采样值作比较,是否在允许范围内
  if ( abs(TouchADCaliPointE.x - pADPoint[CentPos_E].x) > CALI_X_DELTA	|| 
	   abs(TouchADCaliPointE.y - pADPoint[CentPos_E].y) > CALI_Y_DELTA	)
	  return CALI_CENT_ERR;

  gTouchADCaliPoint[TopLPos_A].x = (pADPoint[TopLPos_A].x+pADPoint[BotLPos_C].x)>>1;
  gTouchADCaliPoint[TopLPos_A].y = (pADPoint[TopLPos_A].y+pADPoint[TopRPos_B].y)>>1;
  return CALI_ALL_OK;
}

int setCalibrationMatrix( UHPOINTLYF * displayPtr,
                          UHPOINTLYF * screenPtr,
                          MATRIX * matrixPtr)
{

    int  retValue = FALSE ;

#if 1	
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    if( matrixPtr->Divider == 0 )
    {
        retValue = TRUE ;
    }
    else
    {
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    
        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
	 matrixPtr->Mask_Cali=1;
    }
 #endif
    return( retValue ) ;

} /* end of setCalibrationMatrix() */


