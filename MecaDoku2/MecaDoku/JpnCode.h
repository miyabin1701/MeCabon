
#define DEFSIZE	163840		// default wave buffer size 

//#define isSjisdigit(c)	((0x824f<=c)&&(c<=0x8258))
//#define isSjisAlphaS(c)	((0x8281<=c)&&(c<=0x829a))
//#define isSjisAlphaL(c)	((0x8260<=c)&&(c<=0x8279))
//#define isSjisAlpha(c)	(isSjisAlphaL(c)||isSjisAlphaS(c))
///#define isSjisAlpha(c)	(((0x8260<=c)&&(c<=0x8279))||((0x8281<=c)&&(c<=0x829a)))
#define isU8digit(c)	((0xEFBC90/*u8'‚O'*/<=c)&&(c<=0xEFBC99/*u8'‚X'*/))
#define isU8AlphaL(c)	((0xEFBCA1/*u8'A'*/<=c)&&(c<=0xEFBCBA/*u8'Z'*/))
#define isU8AlphaS(c)	((0xEFBD81/*u8'‚'*/<=c)&&(c<=0xEFBD9A/*u8'‚š'*/))
#define isU8Alpha(c)	(isU8AlphaL(c)||isU8AlphaS(c))
#define isU8Hankaku(c)	((0x00<=c)&&(c<=0x7f))
#define isU82Byte(c)	((0xc0<=c)&&(c<=0xdf))
#define isU83Byte(c)	((( unsigned )0xe0<=c)&&(c<=( unsigned )0xef))
#define isU84Byte(c)	((0xf0<=c)&&(c<=0xf7))
#define isU85Byte(c)	((0xf8<=c)&&(c<=0xfb))
#define isU86Byte(c)	((0xfc<=c)&&(c<=0xfd))
#define isU8nonTop(c)	(((c)&0xC0)==0x80)

