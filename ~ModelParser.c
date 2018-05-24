#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODEL_PARSER_MAX_TAG_NAME 40
#define MODEL_PARSER_MAX_POINT_BUFFER 80

#define MODEL_PARSER_READ_BUFFER 512

static char *readLineFromFile( FILE *fp );

static int parseFile( const char *cpFile );

static int parseFileHeader( FILE *fp );
static int parseFileLine( FILE *fp, int iModelPosition );

static int parseModel( char *cpText );

static int findTagContent( char *cpText, const char *cpTagName, int iStartAt, int *ipStart, int *ipEnd );

static int findSubstring( char *cpText, const char *cpSubstring, int iStartAt, int *ipStart, int *ipEnd );


static char *readLineFromFile( FILE *fp ) {
    int nAllocated, iAllocatedPtr;
    char cRead;
    char *cpAllocated;

    cpAllocated = (char*)malloc( MODEL_PARSER_READ_BUFFER );
    if( cpAllocated == NULL ) {
        return NULL;
    }
    nAllocated = MODEL_PARSER_READ_BUFFER;
    iAllocatedPtr = 0;

    for( ; ; ) {
        cRead = fgetc( fp );
        if( feof(fp) ) {
            break;
        }
        if( iAllocatedPtr >= nAllocated-1 ) {
            if( nAllocated == 0 ) {
                cpAllocated = (char*)malloc( MODEL_PARSER_READ_BUFFER );
            } else {
                cpAllocated = (char*)realloc( cpAllocated, nAllocated + MODEL_PARSER_READ_BUFFER );
            }
            if( cpAllocated == NULL ) {
                return NULL;
            }
            nAllocated += MODEL_PARSER_READ_BUFFER;
        }
        cpAllocated[iAllocatedPtr] = cRead;
        iAllocatedPtr++;
        if( cRead == '\n' ) {
            break;
        }
    }
    cpAllocated[iAllocatedPtr] = '\x0';
    return cpAllocated;
}


static int findTagContent( char *cpText, const char *cpTagName, int iStartAt, int *ipStart, int *ipEnd ) {
    char caOpenTag[MODEL_PARSER_MAX_TAG_NAME+3];
    char caCloseTag[MODEL_PARSER_MAX_TAG_NAME+4];
    int iFound, iOpenTagStart, iOpenTagEnd, iCloseTagStart, iCloseTagEnd;

    if( strlen(cpTagName) > MODEL_PARSER_MAX_TAG_NAME ) {
        return 0;
    }

    sprintf( caOpenTag, "<%s>", cpTagName );
    sprintf( caCloseTag, "</%s>", cpTagName );

    iFound = findSubstring( cpText, caOpenTag, iStartAt, &iOpenTagStart, &iOpenTagEnd );
    if( iFound == 0 ) {
        return 0;
    }
    iFound = findSubstring( cpText, caCloseTag, iOpenTagEnd, &iCloseTagStart, &iCloseTagEnd );
    if( iFound == 0 ) {
        return 0;
    }

    if( ipStart != NULL ) {
        *ipStart = iOpenTagEnd+1;
    }
    if( ipEnd != NULL ) {
        *ipEnd = iCloseTagStart-1;
    }
    return 1;
}

static int findSubstring( char *cpText, const char *cpSubstring, int iStartAt, int *ipStart, int *ipEnd ) {
    int iFound = 0;
    int i, iTextLen, iSubstringLen, iSubstringIndex;

    iTextLen = strlen( cpText );
    iSubstringLen = strlen( cpSubstring );
    iSubstringIndex = 0;
    for( i = iStartAt ; i < iTextLen ; i++ ) {
        if( tolower(cpSubstring[iSubstringIndex]) == tolower(cpText[i]) ) {
            iSubstringIndex++;
            if( iSubstringIndex == iSubstringLen ) {
                if( ipStart != NULL ) {
                    *ipStart = i-iSubstringLen+1;
                }
                if( ipEnd != NULL ) {
                    *ipEnd = i;
                }
                iFound = 1;
                break;
            }
        } else {
            if( iSubstringIndex > 0 ) {
                iSubstringIndex = 0;
            }
        }
    }
    return iFound;
}


static int parseModel( char *cpText ) {

    int iStatus, iObjectStart, iObjectEnd, iFacetStart, iFacetEnd, iPointStart, iPointEnd;

    int iPointIndex, nPointsBuffered;
    char caPointBuffer[MODEL_PARSER_MAX_POINT_BUFFER];
    float fX, fY, fZ;

    iObjectEnd = 0;
    while(1) {
        iStatus = findTagContent( cpText, "object", iObjectEnd, &iObjectStart, &iObjectEnd );
        if( iStatus == 0 ) {
            break;
        }
        puts("OBJECT FOUND!");

        iFacetEnd = iObjectStart;
        while(1) {
            iStatus = findTagContent( cpText, "facet", iFacetEnd, &iFacetStart, &iFacetEnd );
            if( iStatus == 0 ) {
                break;
            }
            puts("FACET FOUND!");

            iPointEnd = iFacetStart;
            while(1) {
                iStatus = findTagContent( cpText, "point", iPointEnd, &iPointStart, &iPointEnd );
                if( iStatus == 0 ) {
                    break;
                }

                for( iPointIndex = iPointStart, nPointsBuffered = 0 ; iPointIndex <= iPointEnd && nPointsBuffered < MODEL_PARSER_MAX_POINT_BUFFER ; ) {
                    if( cpText[iPointIndex] == ',' ) {
                        caPointBuffer[ nPointsBuffered ] = ' ';
                    } else {
                        caPointBuffer[ nPointsBuffered ] = cpText[iPointIndex];
                    }
                    iPointIndex++;
                    nPointsBuffered++;
                }
                caPointBuffer[ nPointsBuffered ] = '\x0';
                iStatus = sscanf( caPointBuffer, " %f %f %f", &fX, &fY, &fZ );
                if( iStatus == 3 ) {
                    printf("Point: %f, %f, %f\n", fX, fY, fZ );
                }
            }
        }
    }
    return 0;
}


static int parseFileHeader( FILE *fp ) {
    int i, iStatus, iStart, iModelPosition=-1;
    char *cpHeader;

    cpHeader = readLineFromFile(fp);
    if( cpHeader == NULL ) {
        return -1;
    }

    iStatus = findSubstring( cpHeader, "model", 0, &iStart, NULL );
    if( iStatus == 1 ) {
        iModelPosition = 0;
        for( i = 0 ; i < iStart ; i++ ) {
            if( cpHeader[i] == ';' ) {
                iModelPosition++;
            }
        }
    }

    free(cpHeader);
    return iModelPosition;
}


static int parseFileLine( FILE *fp, int iModelPosition ) {
    int i, iLineLen, iPosition, iModelStart;
    char *cpLine;

    cpLine = readLineFromFile(fp);
    if( cpLine == NULL ) {
        return -1;
    }
    iLineLen = strlen( cpLine );

    iPosition = 0;
    for( i = 0 ; i < iLineLen ; i++ ) {
        if( iPosition == iModelPosition ) {
            break;
        }
        if( cpLine[i] == ';' ) {
            iPosition++;
        }
    }
    if( i == iLineLen ) {
        return -1;
    }
    iModelStart = i;

    for( ; i < iLineLen ; i++ ) {
        if( cpLine[i] == ';' ) {
            cpLine[i] = '\x0';
            break;
        }
    }
    printf( "%s", &cpLine[iModelStart] );

    parseModel( &cpLine[iModelStart] );

    free(cpLine);
    return 0;
}


static int parseFile( const char *cpFile ) {

    //char caText[] = "<object><facet><point> 1, 2, 3</point><point> 4, 5, 6</point><point>7, 8, 9</point><point>10, 11.234234, 12 </point></facet></object>";

    FILE *fp;
    int iStatus, iModelPosition;

    fp = fopen( cpFile, "rb" );
    if( fp != NULL ) {
        iModelPosition = parseFileHeader( fp );
        if( iModelPosition >= 0 ) {

            while(1) {
                iStatus = parseFileLine( fp, iModelPosition );
                if( iStatus == -1 ) {
                    break;
                }
            }
        }
        fclose(fp);
    }

    return 0;
}

int main() {

    //char caSrc[] = "<object><facet><point> 1, 2, 3</point><point> 4, 5, 6</point><point>7, 8, 9</point><point>10, 11.234234, 12 </point></facet></object>";

    parseFile( "from_spider.txt" );
    return 0;
}

