#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Spdr3dModel.hpp"

#define SPDRMV_MAX_TAG_NAME 40
#define SPDRMV_MAX_POINT_BUFFER 80

#define SPDRMV_READ_BUFFER 512

static char *readLineFromFile( FILE *fp );

static int parseFile( const char *cpFile );

static int parseFileHeader( FILE *fp );
static int parseFileLine( Spdr3dModel& model, FILE *fp, int iOperationPosition );

static int parseOperation( Spdr3dOperation& operation, char *cpText );

static int findTagContent( char *cpText, const char *cpTagName, int iStartAt, int *ipStart, int *ipEnd );

static int findSubstring( char *cpText, const char *cpSubstring, int iStartAt, int *ipStart, int *ipEnd );


static char *readLineFromFile( FILE *fp ) {
    int nAllocated, iAllocatedPtr;
    char cRead;
    char *cpAllocated;

    cpAllocated = (char*)malloc( SPDRMV_READ_BUFFER );
    if( cpAllocated == NULL ) {
        return NULL;
    }
    nAllocated = SPDRMV_READ_BUFFER;
    iAllocatedPtr = 0;

    for( ; ; ) {
        cRead = fgetc( fp );
        if( feof(fp) ) {
            break;
        }
        if( iAllocatedPtr >= nAllocated-1 ) {
            if( nAllocated == 0 ) {
                cpAllocated = (char*)malloc( SPDRMV_READ_BUFFER );
            } else {
                cpAllocated = (char*)realloc( cpAllocated, nAllocated + SPDRMV_READ_BUFFER );
            }
            if( cpAllocated == NULL ) {
                return NULL;
            }
            nAllocated += SPDRMV_READ_BUFFER;
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
    char caOpenTag[SPDRMV_MAX_TAG_NAME+3];
    char caCloseTag[SPDRMV_MAX_TAG_NAME+4];
    int iFound, iOpenTagStart, iOpenTagEnd, iCloseTagStart, iCloseTagEnd;

    if( strlen(cpTagName) > SPDRMV_MAX_TAG_NAME ) {
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


static int parseFileHeader( FILE *fp ) {
    int i, iStatus, iStart, iOperationPosition=-1;
    char *cpHeader;

    cpHeader = readLineFromFile(fp);
    if( cpHeader == NULL ) {
        return -1;
    }

    iStatus = findSubstring( cpHeader, "model", 0, &iStart, NULL );
    if( iStatus == 1 ) {
        iOperationPosition = 0;
        for( i = 0 ; i < iStart ; i++ ) {
            if( cpHeader[i] == ';' ) {
                iOperationPosition++;
            }
        }
    }

    free(cpHeader);
    return iOperationPosition;
}


static int parseFileLine( Spdr3dModel& model, FILE *fp, int iOperationModelPosition ) {
    int i, iLineLen, iPosition, iOperationModelStart;
    char *cpLine;

    cpLine = readLineFromFile(fp);
    if( cpLine == NULL ) {
        return -1;
    }
    iLineLen = strlen( cpLine );

    iPosition = 0;
    for( i = 0 ; i < iLineLen ; i++ ) {
        if( iPosition == iOperationModelPosition ) {
            break;
        }
        if( cpLine[i] == ';' ) {
            iPosition++;
        }
    }
    if( i == iLineLen ) {
        return -1;
    }
    iOperationModelStart = i;

    for( ; i < iLineLen ; i++ ) {
        if( cpLine[i] == ';' ) {
            cpLine[i] = '\x0';
            break;
        }
    }
    printf( "Operation model found: %s", &cpLine[iOperationModelStart] );

    Spdr3dOperation operation;
    parseOperation( operation, &cpLine[iOperationModelStart] );
    model.add( operation );

    free(cpLine);
    return 0;
}


static int parseFile( Spdr3dModel& model, const char *cpFile ) {

    //char caText[] = "<object><facet><point> 1, 2, 3</point><point> 4, 5, 6</point><point>7, 8, 9</point><point>10, 11.234234, 12 </point></facet></object>";

    FILE *fp;
    int iStatus, iOperationModelPosition;

    fp = fopen( cpFile, "rb" );
    if( fp != NULL ) {
        iOperationModelPosition = parseFileHeader( fp );
        if( iOperationModelPosition >= 0 ) {

            while(1) {
                iStatus = parseFileLine( model, fp, iOperationModelPosition );
                if( iStatus == -1 ) {
                    break;
                }
            }
        }
        fclose(fp);
    }

    return 0;
}


static int parseOperation( Spdr3dOperation& operation, char *cpText ) {

    int iStatus, iObjectStart, iObjectEnd, iFacetStart, iFacetEnd, iPointStart, iPointEnd;

    int iPointIndex, nPointsBuffered;
    char caPointBuffer[SPDRMV_MAX_POINT_BUFFER];
    float fX, fY, fZ;

    iObjectEnd = 0;
    while(1) {
        iStatus = findTagContent( cpText, "object", iObjectEnd, &iObjectStart, &iObjectEnd );
        if( iStatus == 0 ) {
            break;
        }
        puts("OBJECT FOUND!");
        Spdr3dObject object;

        iFacetEnd = iObjectStart;
        while(1) {
            iStatus = findTagContent( cpText, "facet", iFacetEnd, &iFacetStart, &iFacetEnd );
            if( iStatus == 0 ) {
                break;
            }
            puts("FACET FOUND!");
            Spdr3dFacet facet;

            iPointEnd = iFacetStart;
            while(1) {
                iStatus = findTagContent( cpText, "point", iPointEnd, &iPointStart, &iPointEnd );
                if( iStatus == 0 ) {
                    break;
                }

                for( iPointIndex = iPointStart, nPointsBuffered = 0 ; iPointIndex <= iPointEnd && nPointsBuffered < SPDRMV_MAX_POINT_BUFFER ; ) {
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
                    Spdr3dVertex vertex( fX, fY, fZ );
                    facet.add(vertex);
                    printf("Added vertex: %f, %f, %f\n", fX, fY, fZ );
                }
            }
            object.add(facet);
        }
        operation.add(object);
    }
    return 0;
}


int main( int argc, char* argv[] ) {

    //char caSrc[] = "<object><facet><point> 1, 2, 3</point><point> 4, 5, 6</point><point>7, 8, 9</point><point>10, 11.234234, 12 </point></facet></object>";

    Spdr3dModel model;

    parseFile( model, "src.txt" );

    Spdr3dModel::display( model, argc, argv );
    
    return 0;
}

