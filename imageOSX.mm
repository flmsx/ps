//
//  imageOSX.m
//  popstar
//
//  Created by ammon feng on 13-8-7.
//  Copyright (c) 2013 Ammon Feng. All rights reserved.
//

#import "imageOSX.h"

#ifndef IOS
CGImageRef MyCreateCGImageFromFile (NSString* path)
{
    // Get the URL for the pathname passed to the function.
    NSURL *url = [NSURL fileURLWithPath:path];
    CGImageRef        myImage = NULL;
    CGImageSourceRef  myImageSource;
    CFDictionaryRef   myOptions = NULL;
    CFStringRef       myKeys[2];
    CFTypeRef         myValues[2];
 
    // Set up options if you want them. The options here are for
    // caching the image in a decoded form and for using floating-point
    // values if the image format supports them.
    myKeys[0] = kCGImageSourceShouldCache;
    myValues[0] = (CFTypeRef)kCFBooleanTrue;
    myKeys[1] = kCGImageSourceShouldAllowFloat;
    myValues[1] = (CFTypeRef)kCFBooleanTrue;
    // Create the dictionary
    myOptions = CFDictionaryCreate(NULL, (const void **) myKeys,
                   (const void **) myValues, 2,
                   &kCFTypeDictionaryKeyCallBacks,
                   & kCFTypeDictionaryValueCallBacks);
    // Create an image source from the URL.
    myImageSource = CGImageSourceCreateWithURL((CFURLRef)url, myOptions);
    CFRelease(myOptions);
    // Make sure the image source exists before continuing
    if (myImageSource == NULL){
        fprintf(stderr, "Image source is NULL.");
        return  NULL;
    }
    // Create an image from the first item in the image source.
    myImage = CGImageSourceCreateImageAtIndex(myImageSource,
                                           0,
                                           NULL);
 
    CFRelease(myImageSource);
    // Make sure the image exists before continuing
    if (myImage == NULL){
         fprintf(stderr, "Image not created from image source.");
         return NULL;
    }
 
    return myImage;
}
#endif

void imageDump(char* file)
{
	NSString* f = [NSString stringWithUTF8String:file];
#ifdef IOS
    UIImage* image = [UIImage imageNamed:f];
    CGImageRef cgimage = image.CGImage;
#else
    CGImageRef cgimage = MyCreateCGImageFromFile(f); 
#endif    
    size_t width  = CGImageGetWidth(cgimage);
    size_t height = CGImageGetHeight(cgimage);
    
    size_t bpr = CGImageGetBytesPerRow(cgimage);
    size_t bpp = CGImageGetBitsPerPixel(cgimage);
    size_t bpc = CGImageGetBitsPerComponent(cgimage);
    size_t bytes_per_pixel = bpp / bpc;
    
    CGBitmapInfo info = CGImageGetBitmapInfo(cgimage);
    
    NSLog(
          @"\n"
          "===== %@ =====\n"
          "CGImageGetHeight: %d\n"
          "CGImageGetWidth:  %d\n"
          "CGImageGetColorSpace: %@\n"
          "CGImageGetBitsPerPixel:     %d\n"
          "CGImageGetBitsPerComponent: %d\n"
          "CGImageGetBytesPerRow:      %d\n"
          "CGImageGetBitmapInfo: 0x%.8X\n"
          "  kCGBitmapAlphaInfoMask     = %s\n"
          "  kCGBitmapFloatComponents   = %s\n"
          "  kCGBitmapByteOrderMask     = %s\n"
          "  kCGBitmapByteOrderDefault  = %s\n"
          "  kCGBitmapByteOrder16Little = %s\n"
          "  kCGBitmapByteOrder32Little = %s\n"
          "  kCGBitmapByteOrder16Big    = %s\n"
          "  kCGBitmapByteOrder32Big    = %s\n",
          f,
          (int)width,
          (int)height,
          CGImageGetColorSpace(cgimage),
          (int)bpp,
          (int)bpc,
          (int)bpr,
          (unsigned)info,
          (info & kCGBitmapAlphaInfoMask)     ? "YES" : "NO",
          (info & kCGBitmapFloatComponents)   ? "YES" : "NO",
          (info & kCGBitmapByteOrderMask)     ? "YES" : "NO",
          (info & kCGBitmapByteOrderDefault)  ? "YES" : "NO",
          (info & kCGBitmapByteOrder16Little) ? "YES" : "NO",
          (info & kCGBitmapByteOrder32Little) ? "YES" : "NO",
          (info & kCGBitmapByteOrder16Big)    ? "YES" : "NO",
          (info & kCGBitmapByteOrder32Big)    ? "YES" : "NO"
          );
    
    CGDataProviderRef provider = CGImageGetDataProvider(cgimage);
    NSData* data = (id)CGDataProviderCopyData(provider);
    [data autorelease];
    const uint8_t* bytes = (uint8_t*)[data bytes];
    
    printf("Pixel Data:\n");
	size_t row, col, x;
    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            const uint8_t* pixel =
            &bytes[row * bpr + col * bytes_per_pixel];
            
            printf("(");
            for(x = 0; x < bytes_per_pixel; x++)
            {
                printf("%.2X", pixel[x]);
                if( x < bytes_per_pixel - 1 )
                    printf(",");
            }
            
            printf(")");
            if( col < width - 1 )
                printf(", ");
        }
        
        printf("\n");
    }
}

#define BLUE_HEX    0x5AA2E7FF
#define PURPLE_HEX  0xFF41EFFF
#define GREEN_HEX   0x00E718FF
#define YELLOW_HEX  0xD6CB00FF
#define RED_HEX     0xFF5D5AFF

void image2Board(const char *file, uint8_t *board)
{
#ifdef IOS
    NSString *f = @"IMG_xxxx.PNG";
    NSString* number = [NSString stringWithUTF8String:file];
    f = [f stringByReplacingOccurrencesOfString:@"xxxx" withString:number];
    UIImage* image = [UIImage imageNamed:f];
    CGImageRef cgimage = image.CGImage;
#else
    NSString* f = [NSString stringWithUTF8String:file];
    CGImageRef cgimage = MyCreateCGImageFromFile(f); 
#endif
    size_t width  = CGImageGetWidth(cgimage);
    size_t height = CGImageGetHeight(cgimage);
    
    size_t bpr = CGImageGetBytesPerRow(cgimage);
    size_t bpp = CGImageGetBitsPerPixel(cgimage);
    size_t bpc = CGImageGetBitsPerComponent(cgimage);
    size_t bytes_per_pixel = bpp / bpc;
    
    CGBitmapInfo info = CGImageGetBitmapInfo(cgimage);
    
    NSLog(
          @"\n"
          "===== %@ =====\n"
          "CGImageGetHeight: %d\n"
          "CGImageGetWidth:  %d\n"
          "CGImageGetColorSpace: %@\n"
          "CGImageGetBitsPerPixel:     %d\n"
          "CGImageGetBitsPerComponent: %d\n"
          "CGImageGetBytesPerRow:      %d\n"
          "CGImageGetBitmapInfo: 0x%.8X\n"
          "  kCGBitmapAlphaInfoMask     = %s\n"
          "  kCGBitmapFloatComponents   = %s\n"
          "  kCGBitmapByteOrderMask     = %s\n"
          "  kCGBitmapByteOrderDefault  = %s\n"
          "  kCGBitmapByteOrder16Little = %s\n"
          "  kCGBitmapByteOrder32Little = %s\n"
          "  kCGBitmapByteOrder16Big    = %s\n"
          "  kCGBitmapByteOrder32Big    = %s\n",
          [NSString stringWithUTF8String:file],
          (int)width,
          (int)height,
          CGImageGetColorSpace(cgimage),
          (int)bpp,
          (int)bpc,
          (int)bpr,
          (unsigned)info,
          (info & kCGBitmapAlphaInfoMask)     ? "YES" : "NO",
          (info & kCGBitmapFloatComponents)   ? "YES" : "NO",
          (info & kCGBitmapByteOrderMask)     ? "YES" : "NO",
          (info & kCGBitmapByteOrderDefault)  ? "YES" : "NO",
          (info & kCGBitmapByteOrder16Little) ? "YES" : "NO",
          (info & kCGBitmapByteOrder32Little) ? "YES" : "NO",
          (info & kCGBitmapByteOrder16Big)    ? "YES" : "NO",
          (info & kCGBitmapByteOrder32Big)    ? "YES" : "NO"
          );
    
    CGDataProviderRef provider = CGImageGetDataProvider(cgimage);
    NSData* data = (id)CGDataProviderCopyData(provider);
    const uint8_t* bytes = (uint8_t*)[data bytes];
    
	size_t row, col;
	for (row = 0; row < 10; row++) {
        for (col = 0; col < 10; col++) {
            uint32_t *color = (uint32_t *)&bytes[(col*64+31)*bytes_per_pixel + (409+row*64+31)*bpr];
           	*color = htonl(*color); 
            if (*color == BLUE_HEX)
                board[(row+1)*12+col+1] = BLUE;
            else if (*color == RED_HEX)
                board[(row+1)*12+col+1] = RED; 
            else if (*color == GREEN_HEX)
                board[(row+1)*12+col+1] = GREEN;
            else if (*color == YELLOW_HEX)
                board[(row+1)*12+col+1] = YELLOW;
            else if (*color == PURPLE_HEX)
                board[(row+1)*12+col+1] = PURPLE;
            else {
                printf("Something wrong when convert image to board!\n");
                exit(-1);
            }
        }
    }
}

#define STAR '*'

void printStar(int color, int style, char shape)
{
    const char *color_str;
    char format_str[50] = {0,};
    const char *close_style_str = "\e[0m";
    const char *blink_style_str = "\e[5m";
    const char *bright_style_str = "\e[1m";
    const char *reverse_style_str = "\e[7m";
    switch (color) {
    case RED:
        color_str = "\e[31m";
        break;
    case GREEN:
        color_str = "\e[32m";
        break;
    case YELLOW:
        color_str = "\e[33m";
        break;
    case BLUE:
        color_str = "\e[34m";
        break;
    case PURPLE:
        color_str = "\e[35m";
        break;
    case POPED:
        color_str = "\e[30m";
        break;
    default:
        color_str = "\e[30m";
        //fprintf(stderr, "Unkonw color! Black displayed. \n");
    }
    if (style & P_Bright)
        strcat(format_str, bright_style_str);
    if (style & P_Blink)
        strcat(format_str, blink_style_str);
    if (style & P_Reverse)
        strcat(format_str, reverse_style_str);
    strcat(format_str, color_str);
    strcat(format_str, "%c ");
    strcat(format_str, close_style_str);
#ifdef TEST
	printf("%s :", format_str);
#endif
    printf(format_str, shape);
}

static void print_board(uint8_t *board)
{
	size_t i, j;
	for (i = 1; i < 11; i++) {
		for (j = 1; j < 11; j++) {
        	printStar(board[i*12+j], P_Bright, '*');
		}
		printf("\n");
	}
}

#ifdef TEST
int main(int argc, char* argv[])
{
	uint8_t* board[12][12];
	memset(board, 0, sizeof(board));
	image2Board(argv[1], (uint8_t*)board);
	print_board((uint8_t*)board);
	return 0;
}
#endif
