#include "StdAfx.h"

void DrawWuLine( CDC *pDC, int X0, int Y0, int X1, int Y1, COLORREF clrLine ) 
{ 
    /* Make sure the line runs top to bottom */ 
    if (Y0 > Y1) 
    { 
        int Temp = Y0; Y0 = Y1; Y1 = Temp; 
        Temp = X0; X0 = X1; X1 = Temp; 
    } 
     
    /* Draw the initial pixel, which is always exactly intersected by 
    the line and so needs no weighting */ 
    pDC->SetPixel( X0, Y0, clrLine ); 
     
    int XDir, DeltaX = X1 - X0; 
    if( DeltaX >= 0 ) 
    { 
        XDir = 1; 
    } 
    else 
    { 
        XDir   = -1; 
        DeltaX = 0 - DeltaX; /* make DeltaX positive */ 
    } 
     
    /* Special-case horizontal, vertical, and diagonal lines, which 
    require no weighting because they go right through the center of 
    every pixel */ 
    int DeltaY = Y1 - Y0; 
    if (DeltaY == 0) 
    { 
        /* Horizontal line */ 
        while (DeltaX-- != 0) 
        { 
            X0 += XDir; 
            pDC->SetPixel( X0, Y0, clrLine ); 
        } 
        return; 
    } 
    if (DeltaX == 0) 
    { 
        /* Vertical line */ 
        do 
        { 
            Y0++; 
            pDC->SetPixel( X0, Y0, clrLine ); 
        } while (--DeltaY != 0); 
        return; 
    } 
     
    if (DeltaX == DeltaY) 
    { 
        /* Diagonal line */ 
        do 
        { 
            X0 += XDir; 
            Y0++; 
            pDC->SetPixel( X0, Y0, clrLine ); 
        } while (--DeltaY != 0); 
        return; 
    } 
     
    unsigned short ErrorAdj; 
    unsigned short ErrorAccTemp, Weighting; 
     
    /* Line is not horizontal, diagonal, or vertical */ 
    unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */ 
     
    BYTE rl = GetRValue( clrLine ); 
    BYTE gl = GetGValue( clrLine ); 
    BYTE bl = GetBValue( clrLine ); 
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114; 
     
    /* Is this an X-major or Y-major line? */ 
    if (DeltaY > DeltaX) 
    { 
    /* Y-major line; calculate 16-bit fixed-point fractional part of a 
    pixel that X advances each time Y advances 1 pixel, truncating the 
        result so that we won't overrun the endpoint along the X axis */ 
        ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY; 
        /* Draw all pixels other than the first and last */ 
        while (--DeltaY) { 
            ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */ 
            ErrorAcc += ErrorAdj;      /* calculate error for next pixel */ 
            if (ErrorAcc <= ErrorAccTemp) { 
                /* The error accumulator turned over, so advance the X coord */ 
                X0 += XDir; 
            } 
            Y0++; /* Y-major, so always advance Y */ 
                  /* The IntensityBits most significant bits of ErrorAcc give us the 
                  intensity weighting for this pixel, and the complement of the 
            weighting for the paired pixel */ 
            Weighting = ErrorAcc >> 8; 
            ASSERT( Weighting < 256 ); 
            ASSERT( ( Weighting ^ 255 ) < 256 ); 
             
            COLORREF clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 ); 
            BYTE rb = GetRValue( clrBackGround ); 
            BYTE gb = GetGValue( clrBackGround ); 
            BYTE bb = GetBValue( clrBackGround ); 
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114; 
             
            BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
      (Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : 
      ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) 
      / 255.0 * ( rl - rb ) + rb ) ) ); 
            BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
      (Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : 
      ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) 
      / 255.0 * ( gl - gb ) + gb ) ) ); 
            BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
      (Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : 
      ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) 
      / 255.0 * ( bl - bb ) + bb ) ) ); 
            pDC->SetPixel( X0, Y0, RGB( rr, gr, br ) ); 
             
            clrBackGround = ::GetPixel( pDC->m_hDC, X0 + XDir, Y0 ); 
            rb = GetRValue( clrBackGround ); 
            gb = GetGValue( clrBackGround ); 
            bb = GetBValue( clrBackGround ); 
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114; 
             
            rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : 
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) 
/ 255.0 * ( rl - rb ) + rb ) ) ); 
            gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : 
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) 
/ 255.0 * ( gl - gb ) + gb ) ) ); 
            br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : 
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) 
/ 255.0 * ( bl - bb ) + bb ) ) ); 
            pDC->SetPixel( X0 + XDir, Y0, RGB( rr, gr, br ) ); 
        } 
        /* Draw the final pixel, which is always exactly intersected by the line 
        and so needs no weighting */ 
        pDC->SetPixel( X1, Y1, clrLine ); 
        return; 
    } 
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a 
    pixel that Y advances each time X advances 1 pixel, truncating the 
    result to avoid overrunning the endpoint along the X axis */ 
    ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX; 
    /* Draw all pixels other than the first and last */ 
    while (--DeltaX) { 
        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */ 
        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */ 
        if (ErrorAcc <= ErrorAccTemp) { 
            /* The error accumulator turned over, so advance the Y coord */ 
            Y0++; 
        } 
        X0 += XDir; /* X-major, so always advance X */ 
                    /* The IntensityBits most significant bits of ErrorAcc give us the 
                    intensity weighting for this pixel, and the complement of the 
        weighting for the paired pixel */ 
        Weighting = ErrorAcc >> 8; 
        ASSERT( Weighting < 256 ); 
        ASSERT( ( Weighting ^ 255 ) < 256 ); 
         
        COLORREF clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 ); 
        BYTE rb = GetRValue( clrBackGround ); 
        BYTE gb = GetGValue( clrBackGround ); 
        BYTE bb = GetBValue( clrBackGround ); 
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114; 
         
        BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )  
/ 255.0 * ( rl - rb ) + rb ) ) ); 
        BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )  
/ 255.0 * ( gl - gb ) + gb ) ) ); 
        BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting: 
(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )  
/ 255.0 * ( bl - bb ) + bb ) ) ); 
         
        pDC->SetPixel( X0, Y0, RGB( rr, gr, br ) ); 
         
        clrBackGround = ::GetPixel( pDC->m_hDC, X0, Y0 + 1 ); 
        rb = GetRValue( clrBackGround ); 
        gb = GetGValue( clrBackGround ); 
        bb = GetBValue( clrBackGround ); 
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114; 
         
        rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )  
/ 255.0 * ( rl - rb ) + rb ) ) ); 
        gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )  
/ 255.0 * ( gl - gb ) + gb ) ) ); 
        br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255): 
Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) :  
( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )  
/ 255.0 * ( bl - bb ) + bb ) ) ); 
         
        pDC->SetPixel( X0, Y0 + 1, RGB( rr, gr, br ) ); 
    } 
     
    /* Draw the final pixel, which is always exactly intersected by the line 
    and so needs no weighting */ 
    pDC->SetPixel( X1, Y1, clrLine ); 
}