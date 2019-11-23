#ifndef INOUT_H_
#define INOUT_H_

#include "../mylib/typedefine.h"
#include "../mylib/sys.h"

#define M16D(adr) (*((volatile unsigned short *)(adr)))

#define OutputEnable() PFout(4) = 1  //输出使能
#define OutputDisable() PFout(4) = 0 //输出禁止

#define OUTPUTADDR 0x6C000000

#define I000 !PEin(15)
#define I001 !PDin(8)
#define I002 !PDin(9)
#define I003 !PDin(10)
#define I004 !PDin(4)
#define I005 !PDin(5)
#define I006 !PDin(7)
#define I007 !PFin(1)

#define I010 !PFin(2)
#define I011 !PFin(3)
#define I012 !PBin(8)
#define I013 !PBin(9)
#define I014 !PDin(3)
#define I015 !PBin(4)
#define I016 !PBin(5)
#define I017 !PGin(10)

#define I020 !PGin(12)
#define I021 !PGin(15)
#define I022 !PBin(3)
#define I023 !PEin(0)
#define I024 !PEin(1)
#define I025 !PEin(2)
#define I026 !PEin(3)
#define I027 !PCin(14)

#define I030 !PCin(0)
#define I031 !PCin(1)
#define I032 0
#define I033 0
#define I034 0
#define I035 0
#define I036 0
#define I037 0

#define I040 !PCin(15)// ZALARM
#define I041 !PCin(2)// UALARM
#define I042 !PAin(2)// XALARM
#define I043 !PAin(3)// YALARM
#define I044 PEin(4)// ZZORIGN
#define I045 PFin(0)// UZORIGN
#define I046 PFin(10)// XZORIGN
#define I047 0

#define I050 0 //PBin(4)
#define I051 0 //PBin(5)
#define I052 0 //PCin(6)
#define I053 0 //PCin(7)
#define I054 0
#define I055 0
#define I056 0
#define I057 0

extern WORD_BITS cRealInputVal[4];
#define XREAL00 cRealInputVal[0].bits.b0
#define XREAL01 cRealInputVal[0].bits.b1
#define XREAL02 cRealInputVal[0].bits.b2
#define XREAL03 cRealInputVal[0].bits.b3
#define XREAL04 cRealInputVal[0].bits.b4
#define XREAL05 cRealInputVal[0].bits.b5
#define XREAL06 cRealInputVal[0].bits.b6
#define XREAL07 cRealInputVal[0].bits.b7
#define XREAL10 cRealInputVal[0].bits.b8
#define XREAL11 cRealInputVal[0].bits.b9
#define XREAL12 cRealInputVal[0].bits.b10
#define XREAL13 cRealInputVal[0].bits.b11
#define XREAL14 cRealInputVal[0].bits.b12
#define XREAL15 cRealInputVal[0].bits.b13
#define XREAL16 cRealInputVal[0].bits.b14
#define XREAL17 cRealInputVal[0].bits.b15

#define XREAL20 cRealInputVal[1].bits.b0
#define XREAL21 cRealInputVal[1].bits.b1
#define XREAL22 cRealInputVal[1].bits.b2
#define XREAL23 cRealInputVal[1].bits.b3
#define XREAL24 cRealInputVal[1].bits.b4
#define XREAL25 cRealInputVal[1].bits.b5
#define XREAL26 cRealInputVal[1].bits.b6
#define XREAL27 cRealInputVal[1].bits.b7
#define XREAL30 cRealInputVal[1].bits.b8
#define XREAL31 cRealInputVal[1].bits.b9
#define XREAL32 cRealInputVal[1].bits.b10
#define XREAL33 cRealInputVal[1].bits.b11
#define XREAL34 cRealInputVal[1].bits.b12
#define XREAL35 cRealInputVal[1].bits.b13
#define XREAL36 cRealInputVal[1].bits.b14
#define XREAL37 cRealInputVal[1].bits.b15

#define XREAL40 cRealInputVal[2].bits.b0
#define XREAL41 cRealInputVal[2].bits.b1
#define XREAL42 cRealInputVal[2].bits.b2
#define XREAL43 cRealInputVal[2].bits.b3
#define XREAL44 cRealInputVal[2].bits.b4
#define XREAL45 cRealInputVal[2].bits.b5
#define XREAL46 cRealInputVal[2].bits.b6
#define XREAL47 cRealInputVal[2].bits.b7
#define XREAL50 cRealInputVal[2].bits.b8
#define XREAL51 cRealInputVal[2].bits.b9
#define XREAL52 cRealInputVal[2].bits.b10
#define XREAL53 cRealInputVal[2].bits.b11
#define XREAL54 cRealInputVal[2].bits.b12
#define XREAL55 cRealInputVal[2].bits.b13
#define XREAL56 cRealInputVal[2].bits.b14
#define XREAL57 cRealInputVal[2].bits.b15

#define XREAL60 cRealInputVal[3].bits.b0
#define XREAL61 cRealInputVal[3].bits.b1
#define XREAL62 cRealInputVal[3].bits.b2
#define XREAL63 cRealInputVal[3].bits.b3
#define XREAL64 cRealInputVal[3].bits.b4
#define XREAL65 cRealInputVal[3].bits.b5
#define XREAL66 cRealInputVal[3].bits.b6
#define XREAL67 cRealInputVal[3].bits.b7
#define XREAL70 cRealInputVal[3].bits.b8
#define XREAL71 cRealInputVal[3].bits.b9
#define XREAL72 cRealInputVal[3].bits.b10
#define XREAL73 cRealInputVal[3].bits.b11
#define XREAL74 cRealInputVal[3].bits.b12
#define XREAL75 cRealInputVal[3].bits.b13
#define XREAL76 cRealInputVal[3].bits.b14
#define XREAL77 cRealInputVal[3].bits.b15

extern WORD_BITS cInputVal[4];
#define cInputX00_X17 cInputVal[0].Val
#define X00 cInputVal[0].bits.b0
#define X01 cInputVal[0].bits.b1
#define X02 cInputVal[0].bits.b2
#define X03 cInputVal[0].bits.b3
#define X04 cInputVal[0].bits.b4
#define X05 cInputVal[0].bits.b5
#define X06 cInputVal[0].bits.b6
#define X07 cInputVal[0].bits.b7
#define X10 cInputVal[0].bits.b8
#define X11 cInputVal[0].bits.b9
#define X12 cInputVal[0].bits.b10
#define X13 cInputVal[0].bits.b11
#define X14 cInputVal[0].bits.b12
#define X15 cInputVal[0].bits.b13
#define X16 cInputVal[0].bits.b14
#define X17 cInputVal[0].bits.b15

#define cInputX20_X37 cInputVal[1].Val
#define X20 cInputVal[1].bits.b0
#define X21 cInputVal[1].bits.b1
#define X22 cInputVal[1].bits.b2
#define X23 cInputVal[1].bits.b3
#define X24 cInputVal[1].bits.b4
#define X25 cInputVal[1].bits.b5
#define X26 cInputVal[1].bits.b6
#define X27 cInputVal[1].bits.b7
#define X30 cInputVal[1].bits.b8
#define X31 cInputVal[1].bits.b9
#define X32 cInputVal[1].bits.b10
#define X33 cInputVal[1].bits.b11
#define X34 cInputVal[1].bits.b12
#define X35 cInputVal[1].bits.b13
#define X36 cInputVal[1].bits.b14
#define X37 cInputVal[1].bits.b15

#define cInputX40_X57 cInputVal[2].Val
#define X40 cInputVal[2].bits.b0
#define X41 cInputVal[2].bits.b1
#define X42 cInputVal[2].bits.b2
#define X43 cInputVal[2].bits.b3
#define X44 cInputVal[2].bits.b4
#define X45 cInputVal[2].bits.b5
#define X46 cInputVal[2].bits.b6
#define X47 cInputVal[2].bits.b7
#define X50 cInputVal[2].bits.b8
#define X51 cInputVal[2].bits.b9
#define X52 cInputVal[2].bits.b10
#define X53 cInputVal[2].bits.b11
#define X54 cInputVal[2].bits.b12
#define X55 cInputVal[2].bits.b13
#define X56 cInputVal[2].bits.b14
#define X57 cInputVal[2].bits.b15

#define cInputX60_X77 cInputVal[3].Val
#define X60 cInputVal[3].bits.b0
#define X61 cInputVal[3].bits.b1
#define X62 cInputVal[3].bits.b2
#define X63 cInputVal[3].bits.b3
#define X64 cInputVal[3].bits.b4
#define X65 cInputVal[3].bits.b5
#define X66 cInputVal[3].bits.b6
#define X67 cInputVal[3].bits.b7
#define X70 cInputVal[3].bits.b8
#define X71 cInputVal[3].bits.b9
#define X72 cInputVal[3].bits.b10
#define X73 cInputVal[3].bits.b11
#define X74 cInputVal[3].bits.b12
#define X75 cInputVal[3].bits.b13
#define X76 cInputVal[3].bits.b14
#define X77 cInputVal[3].bits.b15

extern WORD_BITS cOldInputVal[4];
#define cOldInputX00_X17 cOldInputVal[0].Val
#define bOldX00 cOldInputVal[0].bits.b0
#define bOldX01 cOldInputVal[0].bits.b1
#define bOldX02 cOldInputVal[0].bits.b2
#define bOldX03 cOldInputVal[0].bits.b3
#define bOldX04 cOldInputVal[0].bits.b4
#define bOldX05 cOldInputVal[0].bits.b5
#define bOldX06 cOldInputVal[0].bits.b6
#define bOldX07 cOldInputVal[0].bits.b7
#define bOldX10 cOldInputVal[0].bits.b8
#define bOldX11 cOldInputVal[0].bits.b9
#define bOldX12 cOldInputVal[0].bits.b10
#define bOldX13 cOldInputVal[0].bits.b11
#define bOldX14 cOldInputVal[0].bits.b12
#define bOldX15 cOldInputVal[0].bits.b13
#define bOldX16 cOldInputVal[0].bits.b14
#define bOldX17 cOldInputVal[0].bits.b15

#define cOldInputX20_X37 cOldInputVal[1].Val
#define bOldX20 cOldInputVal[1].bits.b0
#define bOldX21 cOldInputVal[1].bits.b1
#define bOldX22 cOldInputVal[1].bits.b2
#define bOldX23 cOldInputVal[1].bits.b3
#define bOldX24 cOldInputVal[1].bits.b4
#define bOldX25 cOldInputVal[1].bits.b5
#define bOldX26 cOldInputVal[1].bits.b6
#define bOldX27 cOldInputVal[1].bits.b7
#define bOldX30 cOldInputVal[1].bits.b8
#define bOldX31 cOldInputVal[1].bits.b9
#define bOldX32 cOldInputVal[1].bits.b10
#define bOldX33 cOldInputVal[1].bits.b11
#define bOldX34 cOldInputVal[1].bits.b12
#define bOldX35 cOldInputVal[1].bits.b13
#define bOldX36 cOldInputVal[1].bits.b14
#define bOldX37 cOldInputVal[1].bits.b15

#define cOldInputX40_X57 cOldInputVal[2].Val
#define bOldX40 cOldInputVal[2].bits.b0
#define bOldX41 cOldInputVal[2].bits.b1
#define bOldX42 cOldInputVal[2].bits.b2
#define bOldX43 cOldInputVal[2].bits.b3
#define bOldX44 cOldInputVal[2].bits.b4
#define bOldX45 cOldInputVal[2].bits.b5
#define bOldX46 cOldInputVal[2].bits.b6
#define bOldX47 cOldInputVal[2].bits.b7
#define bOldX50 cOldInputVal[2].bits.b8
#define bOldX51 cOldInputVal[2].bits.b9
#define bOldX52 cOldInputVal[2].bits.b10
#define bOldX53 cOldInputVal[2].bits.b11
#define bOldX54 cOldInputVal[2].bits.b12
#define bOldX55 cOldInputVal[2].bits.b13
#define bOldX56 cOldInputVal[2].bits.b14
#define bOldX57 cOldInputVal[2].bits.b15

#define cOldInputX60_X77 cOldInputVal[3].Val
#define bOldX60 cOldInputVal[3].bits.b0
#define bOldX61 cOldInputVal[3].bits.b1
#define bOldX62 cOldInputVal[3].bits.b2
#define bOldX63 cOldInputVal[3].bits.b3
#define bOldX64 cOldInputVal[3].bits.b4
#define bOldX65 cOldInputVal[3].bits.b5
#define bOldX66 cOldInputVal[3].bits.b6
#define bOldX67 cOldInputVal[3].bits.b7
#define bOldX70 cOldInputVal[3].bits.b8
#define bOldX71 cOldInputVal[3].bits.b9
#define bOldX72 cOldInputVal[3].bits.b10
#define bOldX73 cOldInputVal[3].bits.b11
#define bOldX74 cOldInputVal[3].bits.b12
#define bOldX75 cOldInputVal[3].bits.b13
#define bOldX76 cOldInputVal[3].bits.b14
#define bOldX77 cOldInputVal[3].bits.b15

extern WORD_BITS cOutputVal[4];
#define cOutputValY00_Y17 cOutputVal[0].Val
#define Y00 cOutputVal[0].bits.b0
#define Y01 cOutputVal[0].bits.b1
#define Y02 cOutputVal[0].bits.b2
#define Y03 cOutputVal[0].bits.b3
#define Y04 cOutputVal[0].bits.b4
#define Y05 cOutputVal[0].bits.b5
#define Y06 cOutputVal[0].bits.b6
#define Y07 cOutputVal[0].bits.b7
#define Y10 cOutputVal[0].bits.b8
#define Y11 cOutputVal[0].bits.b9
#define Y12 cOutputVal[0].bits.b10
#define Y13 cOutputVal[0].bits.b11
#define Y14 cOutputVal[0].bits.b12
#define Y15 cOutputVal[0].bits.b13
#define Y16 cOutputVal[0].bits.b14
#define Y17 cOutputVal[0].bits.b15

#define cOutputValY20_Y37 cOutputVal[1].Val
#define Y20 cOutputVal[1].bits.b0
#define Y21 cOutputVal[1].bits.b1
#define Y22 cOutputVal[1].bits.b2
#define Y23 cOutputVal[1].bits.b3
#define Y24 cOutputVal[1].bits.b4
#define Y25 cOutputVal[1].bits.b5
#define Y26 cOutputVal[1].bits.b6
#define Y27 cOutputVal[1].bits.b7
#define Y30 cOutputVal[1].bits.b8
#define Y31 cOutputVal[1].bits.b9
#define Y32 cOutputVal[1].bits.b10
#define Y33 cOutputVal[1].bits.b11
#define Y34 cOutputVal[1].bits.b12
#define Y35 cOutputVal[1].bits.b13
#define Y36 cOutputVal[1].bits.b14
#define Y37 cOutputVal[1].bits.b15

#define cOutputValY40_Y57 cOutputVal[2].Val
#define Y40 cOutputVal[2].bits.b0 //40  272
#define Y41 cOutputVal[2].bits.b1 //41  273
#define Y42 cOutputVal[2].bits.b2 //42
#define Y43 cOutputVal[2].bits.b3 //43  275
#define Y44 cOutputVal[2].bits.b4 //44
#define Y45 cOutputVal[2].bits.b5 //45
#define Y46 cOutputVal[2].bits.b6
#define Y47 cOutputVal[2].bits.b7
#define Y50 cOutputVal[2].bits.b8
#define Y51 cOutputVal[2].bits.b9
#define Y52 cOutputVal[2].bits.b10
#define Y53 cOutputVal[2].bits.b11
#define Y54 cOutputVal[2].bits.b12
#define Y55 cOutputVal[2].bits.b13
#define Y56 cOutputVal[2].bits.b14
#define Y57 cOutputVal[2].bits.b15

#define cOutputValY60_Y77 cOutputVal[3].Val
#define Y60 cOutputVal[3].bits.b0 //40  272
#define Y61 cOutputVal[3].bits.b1 //41  273
#define Y62 cOutputVal[3].bits.b2 //42
#define Y63 cOutputVal[3].bits.b3 //43  275
#define Y64 cOutputVal[3].bits.b4 //44
#define Y65 cOutputVal[3].bits.b5 //45
#define Y66 cOutputVal[3].bits.b6
#define Y67 cOutputVal[3].bits.b7
#define Y70 cOutputVal[3].bits.b8
#define Y71 cOutputVal[3].bits.b9
#define Y72 cOutputVal[3].bits.b10
#define Y73 cOutputVal[3].bits.b11
#define Y74 cOutputVal[3].bits.b12
#define Y75 cOutputVal[3].bits.b13
#define Y76 cOutputVal[3].bits.b14
#define Y77 cOutputVal[3].bits.b15

extern WORD_BITS cRealOutputVal[4];
#define YREAL00 cRealOutputVal[0].bits.b0
#define YREAL01 cRealOutputVal[0].bits.b1
#define YREAL02 cRealOutputVal[0].bits.b2
#define YREAL03 cRealOutputVal[0].bits.b3
#define YREAL04 cRealOutputVal[0].bits.b4
#define YREAL05 cRealOutputVal[0].bits.b5
#define YREAL06 cRealOutputVal[0].bits.b6
#define YREAL07 cRealOutputVal[0].bits.b7
#define YREAL10 cRealOutputVal[0].bits.b8
#define YREAL11 cRealOutputVal[0].bits.b9
#define YREAL12 cRealOutputVal[0].bits.b10
#define YREAL13 cRealOutputVal[0].bits.b11
#define YREAL14 cRealOutputVal[0].bits.b12
#define YREAL15 cRealOutputVal[0].bits.b13
#define YREAL16 cRealOutputVal[0].bits.b14
#define YREAL17 cRealOutputVal[0].bits.b15

#define YREAL20 cRealOutputVal[1].bits.b0
#define YREAL21 cRealOutputVal[1].bits.b1
#define YREAL22 cRealOutputVal[1].bits.b2
#define YREAL23 cRealOutputVal[1].bits.b3
#define YREAL24 cRealOutputVal[1].bits.b4
#define YREAL25 cRealOutputVal[1].bits.b5
#define YREAL26 cRealOutputVal[1].bits.b6
#define YREAL27 cRealOutputVal[1].bits.b7
#define YREAL30 cRealOutputVal[1].bits.b8
#define YREAL31 cRealOutputVal[1].bits.b9
#define YREAL32 cRealOutputVal[1].bits.b10
#define YREAL33 cRealOutputVal[1].bits.b11
#define YREAL34 cRealOutputVal[1].bits.b12
#define YREAL35 cRealOutputVal[1].bits.b13
#define YREAL36 cRealOutputVal[1].bits.b14
#define YREAL37 cRealOutputVal[1].bits.b15

#define YREAL40 cRealOutputVal[2].bits.b0
#define YREAL41 cRealOutputVal[2].bits.b1
#define YREAL42 cRealOutputVal[2].bits.b2
#define YREAL43 cRealOutputVal[2].bits.b3
#define YREAL44 cRealOutputVal[2].bits.b4
#define YREAL45 cRealOutputVal[2].bits.b5
#define YREAL46 cRealOutputVal[2].bits.b6
#define YREAL47 cRealOutputVal[2].bits.b7
#define YREAL50 cRealOutputVal[2].bits.b8
#define YREAL51 cRealOutputVal[2].bits.b9
#define YREAL52 cRealOutputVal[2].bits.b10
#define YREAL53 cRealOutputVal[2].bits.b11
#define YREAL54 cRealOutputVal[2].bits.b12
#define YREAL55 cRealOutputVal[2].bits.b13
#define YREAL56 cRealOutputVal[2].bits.b14
#define YREAL57 cRealOutputVal[2].bits.b15

#define YREAL60 cRealOutputVal[3].bits.b0
#define YREAL61 cRealOutputVal[3].bits.b1
#define YREAL62 cRealOutputVal[3].bits.b2
#define YREAL63 cRealOutputVal[3].bits.b3
#define YREAL64 cRealOutputVal[3].bits.b4
#define YREAL65 cRealOutputVal[3].bits.b5
#define YREAL66 cRealOutputVal[3].bits.b6
#define YREAL67 cRealOutputVal[3].bits.b7
#define YREAL70 cRealOutputVal[3].bits.b8
#define YREAL71 cRealOutputVal[3].bits.b9
#define YREAL72 cRealOutputVal[3].bits.b10
#define YREAL73 cRealOutputVal[3].bits.b11
#define YREAL74 cRealOutputVal[3].bits.b12
#define YREAL75 cRealOutputVal[3].bits.b13
#define YREAL76 cRealOutputVal[3].bits.b14
#define YREAL77 cRealOutputVal[3].bits.b15

#define Z_ARST Y40 //40  272
#define Z_SON Y41  //41  273
#define U_ARST Y42 //42
#define U_SON Y43  //43  275
#define X_ARST Y44 //44
#define X_SON Y45  //45
#define Y_ARST Y46 //46
#define Y_SON Y47  //47

extern unsigned char InputFiltering[100]; //�����˲��Ĵ���
#define FILTERTIME_P 8                    //���˲�����
#define FILTERTIME_N 2                    //���˲�����
extern unsigned short RunTestTime;

extern void GPIO_init(void);
extern void RunLedInit(void);

extern void ReadInput(void);
extern void TestOutput(void);
extern void WriteOutput(void);
extern unsigned char Test24c04(void);
extern unsigned char TestMCX314(void);
extern unsigned char TestMCX312(void);
extern unsigned char TestFlash(void);
extern void TestHandWheel(void);
extern void TestSDRAM(void);
#endif /* INOUT_H_ */
