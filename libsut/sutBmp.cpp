#include <string.h>
#include "sutBmp.h"
//===================================================================================
BMP_file::BMP_file()
{
    fBMP = NULL;
    color = NULL;
}

//-----------------------------------------------------------------------------------
BMP_file::~BMP_file()
{
    if(fBMP != NULL) fclose(fBMP);
    if(color != NULL) free(color);
}
//-----------------------------------------------------------------------------------
void BMP_file::read(const char *fPath, const char *fName)
{
    char *FullName = (char *)malloc(strlen(fPath) + strlen(fName) +2);

    color =NULL;
    if(FullName != NULL) {
        strcpy(FullName, fPath);
        strcat(FullName, "/");
        strcat(FullName, fName);
        fBMP = fopen(FullName ,"rb");
        if(fBMP) {
            printf("\tOpen : %s ... ", FullName);
            fread(&file_header,sizeof(unsigned char),sizeof(BMP_file_header) ,fBMP);
            fread(&info_header,sizeof(unsigned char),sizeof(BMP_info_hedaer) ,fBMP);

            printf("\t  Success !\n");
            printf("\t  File Size :%d\n",file_header.size );
            printf("\t  Width : %d  , Height : %d\n",info_header.width , info_header.height );
            printf("\t  Bits :%d\n",info_header.bits);
            printf("\t  Compression :%d\n",info_header.compression  );
            color = (unsigned char*)malloc(sizeof(unsigned char)*info_header.width*info_header.height *3 ); //RGBA
            fread(color,sizeof(unsigned char),sizeof(unsigned char)*info_header.width*info_header.height *3 ,fBMP);
            fclose(fBMP);
            fBMP = NULL;
        }
        else {
            printf("\tOpen : %s ... Failed !\n", FullName);
        }
    }
}
//-----------------------------------------------------------------------------------
unsigned char *BMP_file::get_color_ptr()
{
    return color;
}
//-----------------------------------------------------------------------------------
BMP_info_hedaer *BMP_file::get_info_header()
{
    return &info_header;
}
//-----------------------------------------------------------------------------------
BMP_file_header *BMP_file::get_file_header()
{
    return &file_header;
}
