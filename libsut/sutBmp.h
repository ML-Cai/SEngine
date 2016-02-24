#ifndef SUT_BMP_H
#define SUT_BMP_H
//-----------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------------
#define GRAY_RESOLUCTION	256
#define	BT_BMP_FREE		1
#define BT_BMP_READY	2
#define BT_BMP_BUSY		4
//-----------------------------------------------------------------------------------
#pragma pack(push,1) // not aligmnet !
struct BMP_file_header
{
	unsigned short type ;
	unsigned int size ;
	unsigned short reserved_1 ;
	unsigned short reserved_2 ;
	unsigned int offset ;
};
#pragma pack(pop)
#pragma pack(push,1) 
//-----------------------------------------------------------------------------------
struct BMP_info_hedaer
{
	unsigned int size ;
	unsigned int width ;
	unsigned int height ;
	unsigned short planes ;
	unsigned short bits;
	unsigned int compression ;
	unsigned int image_size ;
	int x_resoluction ;
	int y_resoluction ;
	unsigned int ncolours ;
	unsigned int important_colors ;
};
#pragma pack(pop)
//-----------------------------------------------------------------------------------
class BMP_file
{
	public :
		BMP_file();
        ~BMP_file();
        void read(const char *fPath, const char *fName);
        void release();
        int getHeight();
        int getWidth();

        unsigned char * get_color_ptr();
        BMP_info_hedaer * get_info_header();
        BMP_file_header * get_file_header();
	private :
		BMP_file_header file_header ;
		BMP_info_hedaer info_header ;
        FILE *fBMP ;
        unsigned char *color ;
};
//-----------------------------------------------------------------------------------
#endif
