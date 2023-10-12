#include <GL/glut.h>
#include <math.h>
#include "bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INNM "../gegev.txt"
#define LLEN 500
#define ESC 27
#define LBL 20
#define STEP 2


GLint winWidth = 800, winHeight = 800, prevHeight = 0, structCount = 0, start, end, xMax, yMax=0, act;
Activity activities[ACTNUM];
// int weekData [ WLEN ][ ACTNUM ];
int weekData [ WLEN ];

bool lines = false;

void processData(void)
{

    for (int i = 0; i < WLEN; i++) 
    {
        int total = 0;
        for (int j = 0; j < structCount; j++) 
        {
            total += activities[j].hours[i];
            weekData[i] = total;
            if (total>yMax) yMax=total;
        }
        
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


void init(void)
{
    processData();
    glClearColor(1.0,1.0,1.0,1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-0.1*xMax*2, 1.1*xMax*2, -0.1*yMax,1.1*yMax);
    glColor3f(0.0, 0.0, 1.0);
}

// draw x-axis on window
void xAxesDef(void)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.412, 0.412, 0.412);
    glVertex2i(0, 0);
    glVertex2i(xMax*2, 0);
    glEnd();
    
}

// draw y-axis on window
void yAxesDef(void)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.412, 0.412, 0.412);
    glVertex2i(0, 0);
    glVertex2i(0, yMax);
    glEnd();
    
}

void printLabels(int x, int y, char *string)
{
    glColor3f(0.412, 0.412, 0.412);
    glRasterPos2f(x, y);
    int len, i;
    len = (int)strlen(string);
    for (i = 0; i < len; i++) 
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
    }
}

// draw lines connecting the top of consecutive bars
void drawTopLines(int x1, int y1, int x2, int y2)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.93, 0.05, 0.03);
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
        sprintf(int_str, "%d", weekData[i]);
        printLabels(startLabelX, weekData[i]+5, int_str);
        
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

// draw bars on the chart
void drawBars(int x1, int width, int totalHours)
{
    glColor3f(0.0, 0.0, 1.0);
    int x2 = x1+width;
    glRecti(x1, 0, x2, totalHours);
}

// draw chart
void drawChart(void)
{
    char int_str[2];
    int i = (start-1);
    int startX = 0;
    int startLabelX=1;
    for ( i; i < end; i++) 
    {
        sprintf(int_str, "%d", i+1);
        printLabels(startLabelX, -25, int_str);
        drawBars(startX, 2, weekData[i]);
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

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
	glViewport (0,0,newWidth,newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
    fprintf(stderr, "window size %dx%d\n", newWidth, newHeight);
}


void displayFcn(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
    drawChart();
    xAxesDef();
    yAxesDef();
    if (lines) showLines();
    glFlush();
    glutSwapBuffers();
}


int main (int argc,char* argv[])
{   
    processFile();

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);  /* top left corner */
	glutInitWindowSize(winWidth,winHeight);
	glutCreateWindow("Bar chart");
	init();
	glutDisplayFunc(displayFcn);	
	glutReshapeFunc(winReshapeFcn);
    glutKeyboardFunc(keyInteraction);
	glutMainLoop();

	return 0;
}