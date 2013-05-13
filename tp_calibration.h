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
#define     CALI_X_DELTA            50 
#define     CALI_Y_DELTA            50

#define     CALI_ALL_OK             0
#define     CALI_X_ERR              1
#define     CALI_Y_ERR              2
#define     CALI_CENT_ERR           3

#define		FALSE                   0
#define		TRUE					1
enum
{
	CMD_SCREEN_DATA,
	CMD_UNCALI_DATA,
	CMD_CALI_START,

};

typedef struct
{
	int Kx;
	int Ky;

	int Ax;
	int Ay;
} CALI_COEF;

typedef enum
{
	TopLPos_A = 0,      // 左上校准点
	TopRPos_B,          // 右上校准点
	BotLPos_C,          // 左下校准点
	BotRPos_D,          // 右下校准点
	CentPos_E           // 中心校准点
}eTOUCH_CALIPOINT;



typedef struct {
    int   a1;
    int   b1;
    int   c1;
    int   a2;
    int   b2;
    int   c2;
    int   delta;
}   
CALIBRATION_PARAMETER, *PCALIBRATION_PARAMETER;

typedef struct {
    int   Xa;
    int   Xb;
    int   Xc;
    int   Ya;
    int   Yb;
    int   Yc;
    int   xa;
    int   xb;
    int   xc;
    int   ya;
    int   yb;
    int   yc;
}   
Matrix, *pMatrix;

/*******************************************************/
typedef struct _UHPOINT
{
	int		x;
	int		y;
}  UHPOINT , *PUHPOINT;

typedef struct _UHPOINTLYF
{
	int		x;
    int		y;
}  UHPOINTLYF , *PUHPOINTLYF;

/* This arrangement of values facilitates *  calculations within getDisplayPoint() */
typedef struct Matrix {
	int		 An,     /* A = An/Divider */
			 Bn,     /* B = Bn/Divider */
			 Cn,     /* C = Cn/Divider */
			 Dn,     /* D = Dn/Divider */
			 En,     /* E = En/Divider */
			 Fn,     /* F = Fn/Divider */
			 Divider ,
			 Mask_Cali;
} MATRIX ; 

extern int Touch_Calibrate( UHPOINT * pLogicPoint, UHPOINT * pADPoint, CALI_COEF * pCaliPtr);

extern int setCalibrationMatrix( UHPOINTLYF * displayPtr,
                          UHPOINTLYF * screenPtr,
                          MATRIX * matrixPtr);

#define CALI_ATTR(_name) \
static struct kobj_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0666,			\
	},					\
	.show	= _name##_show,			\
	.store	= _name##_store,		\
}


