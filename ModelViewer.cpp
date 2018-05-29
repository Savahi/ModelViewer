#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Spdr3dModel.hpp"

#define SPDRMV_MAX_TAG_NAME 40
#define SPDRMV_MAX_POINT_BUFFER 80

#define SPDRMV_READ_BUFFER 512

static char *readLineFromFile( FILE *fp );

static int parseFile( Spdr3dModel& model, const char *cpFile );

static int parseFileHeader( FILE *fp );
static int getPosByColumnName( char *cpBuffer, const char *cpColumn );

static int parseFileLine( Spdr3dModel& model, FILE *fp );
static int parseDatetimeStr( char *cpDatetime, tm& tmDatetime );
static int getValuesByColumnPos( char *cpLine, int *ipOperation, int *ipFactStart, int *ipFactFin, int *ipAsapStart, int *ipAsapFin );

static int parseOperationModel( Spdr3dOperation& operation, char *cpText );

static int findTagContent( char *cpText, const char *cpTagName, int iStartAt, int iStopAt, int *ipStart, int *ipEnd );

static int findSubstring( char *cpText, const char *cpSubstring, int iStartAt, int iStopAt, int *ipStart, int *ipEnd );

static int i_OperationModelPos, i_FactStartPos, i_FactFinPos, i_AsapStartPos, i_AsapFinPos;


static char *readLineFromFile( FILE *fp ) {
    int nAllocated, iAllocatedPtr;
    char cRead;
    char *cpAllocated;

    if( feof(fp) ) {
        return NULL;
    }

    cpAllocated = (char*)malloc( SPDRMV_READ_BUFFER );
    if( cpAllocated == NULL ) {
        return NULL;
    }
    nAllocated = SPDRMV_READ_BUFFER;
    iAllocatedPtr = 0;

    while( ( cRead = fgetc( fp ) ) != EOF ) {
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


static int findTagContent( char *cpText, const char *cpTagName, int iStartAt, int iStopAt, int *ipStart, int *ipEnd ) {
    char caOpenTag[SPDRMV_MAX_TAG_NAME+3];
    char caCloseTag[SPDRMV_MAX_TAG_NAME+4];
    int iFound, iOpenTagStart, iOpenTagEnd, iCloseTagStart, iCloseTagEnd;

    if( strlen(cpTagName) > SPDRMV_MAX_TAG_NAME ) {
        return 0;
    }

    sprintf( caOpenTag, "<%s>", cpTagName );
    sprintf( caCloseTag, "</%s>", cpTagName );

    iFound = findSubstring( cpText, caOpenTag, iStartAt, iStopAt, &iOpenTagStart, &iOpenTagEnd );
    if( iFound == 0 ) {
        return 0;
    }
    iFound = findSubstring( cpText, caCloseTag, iOpenTagEnd, iStopAt, &iCloseTagStart, &iCloseTagEnd );
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

static int findSubstring( char *cpText, const char *cpSubstring, int iStartAt, int iStopAt, int *ipStart, int *ipEnd ) {
    int iFound = 0;
    int i, iTextLen, iSubstringLen, iSubstringIndex;

    iTextLen = strlen( cpText );
    if( iStopAt == -1 ) { 
        iStopAt = iTextLen-1;
    }
    iSubstringLen = strlen( cpSubstring );
    iSubstringIndex = 0;
    for( i = iStartAt ; i <= iStopAt ; i++ ) {
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
    int iReturn=0, i, iStatus, iStart;
    char *cpHeader;

    cpHeader = readLineFromFile(fp);
    if( cpHeader == NULL ) {
        return -1;
    }

    i_OperationModelPos = getPosByColumnName( cpHeader, "model" );
    if( i_OperationModelPos == -1 ) {
        iReturn = -1;
    }
    i_FactStartPos = getPosByColumnName( cpHeader, "factstart" );
    if( i_FactStartPos == -1 ) {
        iReturn = -1;
    }
    i_FactFinPos = getPosByColumnName( cpHeader, "factfin" );
    if( i_FactFinPos == -1 ) {
        iReturn = -1;
    }
    i_AsapStartPos = getPosByColumnName( cpHeader, "asapstart" );
    if( i_AsapStartPos == -1 ) {
        iReturn = -1;
    }
    i_AsapFinPos = getPosByColumnName( cpHeader, "asapfin" );
    if( i_AsapFinPos == -1 ) {
        iReturn = -1;
    }

    free(cpHeader);
    return iReturn;
}

static int getPosByColumnName( char *cpBuffer, const char *cpColumn ) {
    int iPosition=-1, i, iStatus, iStart;

    iStatus = findSubstring( cpBuffer, cpColumn, 0, -1, &iStart, NULL );
    if( iStatus == 1 ) {
        iPosition = 0;
        for( i = 0 ; i < iStart ; i++ ) {
            if( cpBuffer[i] == ';' ) {
                iPosition++;
            }
        }
    }
    return iPosition;
}


static int parseFileLine( Spdr3dModel& model, FILE *fp ) {
    int iStatus, iOperation, iFactStart, iFactFin, iAsapStart, iAsapFin;
    char *cpLine; 

    cpLine = readLineFromFile(fp);
    if( cpLine == NULL ) {
        return -1;
    }

    iStatus = getValuesByColumnPos( cpLine, &iOperation, &iFactStart, &iFactFin, &iAsapStart, &iAsapFin );
    if( iStatus == 0 ) {
        printf( "Operation model found: %s", &cpLine[iOperation] );

        Spdr3dOperation operation;
        operation.parseSpdrDates( &cpLine[iFactStart], &cpLine[iFactFin], &cpLine[iAsapStart], &cpLine[iAsapFin] );
        parseOperationModel( operation, &cpLine[iOperation] );
        model.add( operation );
    }
    free(cpLine);
    return 0;
}


static int getValuesByColumnPos( char *cpLine, int *ipOperation, int *ipFactStart, int *ipFactFin, int *ipAsapStart, int *ipAsapFin ) {
    int i, iPos, iLineLen;
    *ipOperation=-1; 
    *ipFactStart=-1;
    *ipFactFin=-1;
    *ipAsapStart=-1;
    *ipAsapFin=-1;

    iLineLen = strlen( cpLine ); 
    for( i = 0, iPos = 0 ; i < iLineLen ; i++ ) {
        if( iPos == i_OperationModelPos && *ipOperation == -1 ) {
            *ipOperation = i;
        } else if( iPos == i_FactStartPos && *ipFactStart == -1 ) {
            *ipFactStart = i;
        } else if( iPos == i_FactFinPos && *ipFactFin == -1 ) { 
            *ipFactFin = i;
        } else if( iPos == i_AsapStartPos && *ipAsapStart == -1 ) {
            *ipAsapStart = i;
        } else if( iPos == i_AsapFinPos && *ipAsapFin == -1 ) { 
            *ipAsapFin = i;
        }
        if( cpLine[i] == ';' ) {
            iPos++;
            cpLine[i] = '\x0';
        } else if( cpLine[i] == '\r' || cpLine[i] == '\n') {
            cpLine[i] = '\x0';            
        }
    }
    if( *ipOperation == -1 || *ipFactStart == -1 || *ipFactFin == -1 || *ipAsapStart == -1 || *ipAsapFin == -1 ) {
        return -1;
    }
    return 0;
}


static int parseFile( Spdr3dModel& model, const char *cpFile ) {

    //char caText[] = "<object><facet><point> 1, 2, 3</point><point> 4, 5, 6</point><point>7, 8, 9</point><point>10, 11.234234, 12 </point></facet></object>";

    FILE *fp;
    int iStatus, iOperationModelPosition;

    fp = fopen( cpFile, "rb" );
    if( fp != NULL ) {
        iStatus = parseFileHeader( fp );
        if( iStatus != -1 ) {
            while(1) {
                iStatus = parseFileLine( model, fp );
                if( iStatus == -1 ) {
                    break;
                }
            }
        }
        fclose(fp);
    }

    return 0;
}


static int parseOperationModel( Spdr3dOperation& operation, char *cpText ) {

    int iStatus, iObjectStart, iObjectEnd, iFacetStart, iFacetEnd, iPointStart, iPointEnd;

    int iPointIndex, nPointsBuffered;
    char caPointBuffer[SPDRMV_MAX_POINT_BUFFER];
    float fX, fY, fZ;

    iObjectEnd = 0;
    while(1) {
        iStatus = findTagContent( cpText, "object", iObjectEnd, -1, &iObjectStart, &iObjectEnd );
        if( iStatus == 0 ) {
            break;
        }
        puts("OBJECT FOUND!");
        Spdr3dObject object;

        iFacetEnd = iObjectStart;
        while(1) {
            iStatus = findTagContent( cpText, "facet", iFacetEnd, iObjectEnd, &iFacetStart, &iFacetEnd );
            if( iStatus == 0 ) {
                break;
            }
            puts("FACET FOUND!");
            Spdr3dFacet facet;

            iPointEnd = iFacetStart;
            while(1) {
                iStatus = findTagContent( cpText, "point", iPointEnd, iFacetEnd, &iPointStart, &iPointEnd );
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

    if( argc < 2 ) {
        printf( "The required argument is missing!\nUse: %s <source.csv>.\nExiting...", argv[0] );
        return 0;
    }

    Spdr3dModel model;

    parseFile( model, argv[1] );

    model.display( argc, argv );
    
    return 0;
}

