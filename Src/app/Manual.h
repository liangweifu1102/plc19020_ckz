/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: huacong
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-04-20 16:56:45
 */
#ifndef ManualProc_H
#define ManualProc_H
#include "../mylib/typedefine.h"

extern WORD_BITS cAxisManuRunState;
#define X_Dotting cAxisManuRunState.bits.b0
#define Y_Dotting cAxisManuRunState.bits.b1
#define Z_Dotting cAxisManuRunState.bits.b2
#define U_Dotting cAxisManuRunState.bits.b3
#define A_Dotting cAxisManuRunState.bits.b4
#define B_Dotting cAxisManuRunState.bits.b5
#define C_Dotting cAxisManuRunState.bits.b6
#define D_Dotting cAxisManuRunState.bits.b7
#define bXRst cAxisManuRunState.bits.b8
#define bYRst cAxisManuRunState.bits.b9
#define bZRst cAxisManuRunState.bits.b10
#define bURst cAxisManuRunState.bits.b11
#define bARst cAxisManuRunState.bits.b12
#define bBRst cAxisManuRunState.bits.b13
#define bCRst cAxisManuRunState.bits.b14
#define bDRst cAxisManuRunState.bits.b15

extern unsigned short ManuSpeed;
//extern unsigned char cRstOrignStep;
//extern unsigned char cRSoftOriginStep;
//extern unsigned char cDuiDaoStep;
//extern unsigned char cHuanDaoStep;
//extern unsigned char cXResumStep, cYResumStep, cZResumStep, cUResumStep;
//extern unsigned char XManulDir, YManulDir, ZManulDir, UManulDir;
//extern unsigned char XLDec, XRDec;
//extern unsigned char YLDec, YRDec;
//extern unsigned char ZLDec, ZRDec;
//extern unsigned char ULDec, URDec;
//extern unsigned short DuidaoDelay;
//extern unsigned char cMainGas1MStep;
//extern unsigned char cMainGas2MStep;
//extern unsigned char DuiDaoNo;

extern void ManualAction(void);
extern void ResumeLocation(void);
extern void DuiDaoProc(void);
extern void OritinSoftOrigin(void);
extern void RebackOrignInt(void);

extern void Scan_MuBanSelect(void);
extern void Cao_MuBanSelect(void);
extern void Hole_MuBanSelect(void);
#endif
