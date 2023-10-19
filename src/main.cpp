#include <GL/glut.h>
#include <math.h>
#include "bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colour.h"

#define INNM "../gegev.txt"
#define LLEN 500
#define ESC 27
#define LBL 20
#define STEP 2


GLint winWidth = 800, winHeight = 800, prevHeight = 0, structCount = 0, start, end, xMax, yMax=0, act, clickCount=0;
Activity activities[ACTNUM];
GLint clicked[WLEN];

// colours to use for acitivity share
// todo: add more colours for [ACTNUM]...
Colour colours[ACTNUM] = {
        {0.5f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.000f, 0.502f, 0.000f},
        {1.000f, 0.000f, 1.000f},
        {1.000f, 1.000f, 0.000f},
        {0.251f, 0.878f, 0.816f},
        {0.722f, 0.525f, 0.043f},
        {0.184f, 0.310f, 0.310f} 
    };

// int weekData [ WLEN ][ ACTNUM ];
int weekData [ WLEN ];

bool lines = false;
bool labels = false;

void processData(void)
{

    for (int i = 0; i < WLEN; i++) 
    {
        int total = 0;
        for (int j = 0; j < structCount; j++) 
        {
            total += activities[j].hours[i];
            weekData[i] = total;
        }
        if (total>yMax) yMax=total;
    }
   
}


void processFile(void)
{
    FILE *finp;
    char line [LLEN] ;
    int lineCount = 1;
    char activity [NLEN];

    finp = fopen(INNM, "r");
       
    if ( finp == NULL )
    {
        fprintf(stderr,"file %s not found\n", INNM ) ;
    }

    while (fgets(line, LLEN, finp) !=NULL)
    {
        if (lineCount==1)
        {       
            sscanf(line, "%d", &start);
            printf("start: %d\n", start);
        }
        else if(lineCount==2)
        {
            sscanf(line, "%d", &end);
            printf("end: %d\n", end);
            xMax = (end-start+1);
        }
        else
        {
            char input[NLEN];
            char* parsableLine = line;
            int offset;
            int colCount = 0;
            int hourCount = 0;

            while (sscanf(parsableLine, "%s%n", input, &offset) == 1) 
            {
                parsableLine += offset;

                // first col for name
                if (colCount == 0)
                {
                    strcpy(activities[structCount].name, input);
                    // printf("processed %s\n",input);
                }
                else
                {
                    activities[structCount].hours[hourCount]= atoi(input);
                    // printf("processed %d\n",activities[structCount].hours[hourCount]);
                    hourCount++;
                }
                colCount++;
            }
            structCount++;
        }
        lineCount++;
    }
    fclose(finp);
}

// menu definition
void handleMenu(int id)
{
    printf("your option %d\n", id);
    switch(id)
    {
        case 1: lines=true; break;
        case 2: labels=true; break;
        case 3: glClearColor(0.0, 0.0, 1.0, 0.0); break;
    }
    glutPostRedisplay();
}


void init(void)
{
    processData();
    glClearColor(1.0,1.0,1.0,1.0);
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // gluOrtho2D(-0.1*xMax*2, 1.1*xMax*2, -0.1*yMax,1.1*yMax);
    glColor3f(0.0, 0.0, 1.0);

    // GLint menu;
    // menu = 
    glutCreateMenu(handleMenu);
    glutAddMenuEntry(" show lines", 1);
    glutAddMenuEntry(" show labels", 2);
    glutAddMenuEntry("change background", 3);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
}



// draw x-axis on window
void xAxesDef(void)
{    
    glColor3f(0.412, 0.412, 0.412);
    glBegin(GL_LINES);
    glVertex2i(0, 0);
    glVertex2i(xMax*2, 0);
    glEnd();
    
    
}

// draw y-axis on window
void yAxesDef(void)
{
    glColor3f(0.412, 0.412, 0.412);
    glBegin(GL_LINES);
    glVertex2i(0, 0);
    glVertex2i(0, yMax);
    glEnd();
    
}

void printLabels(int x, int y, char *string)
{
    glColor3f(0.93, 0.05, 0.03);
    glRasterPos2f(x, y);
    int len, i;
    len = (int)strlen(string);
    for (i = 0; i < len; i++) 
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
    }
}

// draw lines connecting the top of consecutive bars
// use glBegin(GL_LINE_STRIP) as optimization?
void drawTopLines(int x1, int y1, int x2, int y2)
{
    glColor3f(0.93, 0.05, 0.03);
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();

}

// show result labels and lines connecting bars
void showLines(void)
{
    
    char int_str[4];
    int i = (start-1);
    int startX = 1;
    int endX = 3;
    int startLabelX=1;
    int prevHeight;
    for ( i; i < end; i++) 
    {
        if (labels) 
        {
            sprintf(int_str, "%d", weekData[i]);
            printLabels(startLabelX, weekData[i]+5, int_str);
        }
        
        
        if (i>=start)
        {       
            drawTopLines(startX, prevHeight, endX, weekData[i]);
            startX += STEP; 
            endX += STEP; 
        }

        startLabelX+=STEP;
        prevHeight = weekData[i];
    }
}

// draw regular bars on the chart
void drawRegularBar(int x1, int width, int totalHours)
{
    glColor3f(0.0, 0.0, 1.0);
    int x2 = x1+width;
    glRecti(x1, 0, x2, totalHours);
}

// draw coloured bars on the chart
void drawColourBar(int x1, int width, int week)
{
    int y1 = 0;
    int y2 = 0;
    int x2 = x1+width;
    for (int i = 0; i < ACTNUM; i++) 
    {
        glColor3f(colours[i].r, colours[i].g, colours[i].b);
        y2 += activities[i].hours[week];
        glRecti(x1, y1, x2, y2);
        y1=y2;
    }
    
}

// draw chart
void drawChart(void)
{
    char int_str[2];
    int i = (start-1);
    int startX = 0;
    int startLabelX=1;
    bool printColour; 
    for ( i; i < end; i++) 
    {
        printColour = false;
        sprintf(int_str, "%d", i+1);
        printLabels(startLabelX, (-yMax *0.025), int_str);

        for (int j = 0; j < xMax; j++) 
        {
            if (clicked[j] == (i+1))
            {
                printColour = true;
            }
        }

        if (printColour)
        {
            drawColourBar(startX, 2, i);
        }
        else
        {
            drawRegularBar(startX, 2, weekData[i]);
        }
        
        startX+=STEP;
        startLabelX+=STEP;
    }
}

// key press interaction
void keyInteraction(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 's': printf(" press q"); lines=true; break;    
        case 'q': printf(" press q"); lines=false; break;
        case ESC: exit(0); break;
    }
    glutPostRedisplay();
}

// helper func
int getIndex (int *array, int element, int arrayLength)
{
    int index = 0;
     
    for (int i = 0; i < arrayLength; i++) {
        if (array[i] == element) {
            index = i;
            break;
        }
    }
    return index;

}

// helper func
void removeElement(int *array, int index, int arrayLength)
{
   int i;
   for (i = index; i < arrayLength; i++) 
   {
    array[i] = array[i + 1];
   }
}


int getBar (int x, int y)
{
    int bar = 0;

    // cfr. gluOrtho def
    GLdouble width=(xMax*1.2*2);
    GLdouble xProportion = width/winWidth;
    int xLocation = (int)((xProportion*x) -0.1*xMax*2) ;
    GLdouble height=(1.2*yMax);
    GLdouble yProportion = height/winHeight;
    int yLocation = (int)((yProportion*y) -0.1*yMax) ;

    if (xLocation < 0 || xLocation > (xMax*2))
    {
        printf(" %d not between %d or %d\n", xLocation, 0, xMax);
        return bar;
    }
    else if (yLocation < 0 || yLocation > yMax)
    {
        printf(" %d not between %d or %d\n", yLocation, 0, yMax);
        return bar;
    }
    else 
    {
        int week = (start-1) + (xLocation/2);
        if (weekData[week] == 0) 
        {
            return bar;
        }
        if ((yMax - weekData[week]) < yLocation)
        {
            // printf(" ylocoation? %d for week %d\n",yLocation, week+1);
            return  week+1; 

        }

        return bar;
        
    }
    
}

// mouse click interaction
void mouseInteraction(int button, int state, int x, int y)
{
    int bar = getBar(x,y);
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(bar == 0) break;
            clicked[clickCount]=bar;
            clickCount++; 
            break;    
        case GLUT_RIGHT_BUTTON: 
            if(bar == 0) break;
            else if (clickCount == 0)break;
            else if (clickCount > 0)
            {
                removeElement(clicked, getIndex(clicked, bar, clickCount), clickCount);
                clickCount--;
                break;
            }
            else break;
            
            
    }
    glutPostRedisplay();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.1*xMax*2, 1.1*xMax*2, -0.1*yMax,1.1*yMax);
    glViewport (0,0,newWidth,newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
    fprintf(stderr, "window size %dx%d\n", newWidth, newHeight);
}


void displayFcn(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawChart();
    xAxesDef();
    yAxesDef();
    if (lines) showLines();
    glFlush();
    // glutSwapBuffers(); // only use for animation and double buffering...
}


int main (int argc,char* argv[])
{   
    processFile();

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); // GLUT_DOUBLE for animation...
	glutInitWindowPosition(100,100);  /* top left corner */
	glutInitWindowSize(winWidth,winHeight);
	glutCreateWindow("Bar chart");
	init();
	glutDisplayFunc(displayFcn);	
	glutReshapeFunc(winReshapeFcn);
    glutKeyboardFunc(keyInteraction);
    glutMouseFunc(mouseInteraction);
	glutMainLoop();

	return 0;
}