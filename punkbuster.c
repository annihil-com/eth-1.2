#define _GNU_SOURCE
#include "eth.h"

void eth_glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	eth.pbss++;
	char *picture = getenv("ETH_PBSS");
	
	if( !picture ){
		orig_glReadPixels( x, y, width, height, format, type, pixels );
		return;
	}
		
	//640x480 color PPM image for simplicity
	FILE *fp = fopen(picture,"rb"); 
    if( !fp ){
		orig_glReadPixels( x, y, width, height, format, type, pixels );
		return;
	}

    int pos,r,w,c;
    int pW = 640;
    int pH = 480;
    unsigned char *pic = (unsigned char *) malloc(pH*pW*3);
    for( r=0; r<pH; r++){
        pos = pW*3*r;
        fseek(fp, pos-pW*pH*3, SEEK_END );
        fread(pic+pW*(pH-1)*3-pos,1,pW*3,fp);
    }
    fclose(fp);

    float X,Y,dx,dy,q11,q12,q21,q22;
    int x1,y1;
    float sx = (float) pW / (float) width;
    float sy = (float) pH / (float) height;
    unsigned char *line = (unsigned char *)malloc(width*3);
    for( r=0; r<height-1; r++ ){
        for( w=0;w<width-1;w++) {
            x1 =(int) w*sx;
            y1 =(int) r*sy;
            X = w*sx;
            Y = r*sy;
            dx = X-x1;
            dy = Y-y1;

            for( c=0; c<3; c++ ){
                q11 = (float) pic[y1*pW*3 + x1*3 + c];
                q12 = (float) pic[(y1+1)*pW*3 + x1*3 +c];
                q21 = (float) pic[y1*pW*3 + (x1+1)*3 + c];
                q22 = (float) pic[(y1+1)*pW*3 + (x1+1)*3+c];
                line[w*3+c] = (unsigned char) ((1-dx)*(1-dy)*q11+(1-dx)*dy*q12+dx*(1-dy)*q21+dx*dy*q22);
            }
        }
        memcpy( pixels+r*width*3, line, width*3 );
    }
    free(line);
    free(pic);
}

