//https://eavise.be/aph/
#include <GL/glut.h>
#include <math.h>
#include "bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INNM "../gegev.txt"
#define LLEN 500
#define ESC 27
#define ACT 8
#define LBL 20
#define STEP 20


GLint winWidth = 800, winHeight = 800, prevHeight = 0;
Activity activities[ACT];
bool lines = false;

void processFile(void)
{
    FILE *finp;
    char line [LLEN] ;
    int lineCount = 1, structCount = 0;
    int start, end;
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

                // only persist data between start and end (week) dates?
                // if (colCount >= start && colCount <= end)
                // {
                //     activities[structCount].hours[colCount]= atoi(input);
                //     printf("processed %d\n",activities[structCount].hours[colCount]);
                // }

                // or persist all week data?
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

    // test
    // for (int i = 0; i < 8; i++) 
    // {
    //     printf("\n activity %s has these hours: ", activities[i].name);
    //     for (int j = 0; j < 53; j++) 
    //     {
    //         printf(" %d ", activities[i].hours[j]);
    //     }
    // }
}



void init(void)
{
	glClearColor(1.0,1.0,1.0,1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 200.0, 0.0, 150.0);
    glColor3f(0.0, 0.0, 1.0);
}

// draw x-axis on window
void xAxesDef(void)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.412, 0.412, 0.412);
    glVertex2i(30, 10);
    glVertex2i(195, 10);
    glEnd();
    
}

// draw y-axis on window
void yAxesDef(void)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.412, 0.412, 0.412);
    glVertex2i(30, 10);
    glVertex2i(30, 140);
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

void drawTopLines(int x1, int y1, int x2, int y2)
{
    glBegin(GL_LINES);
    // glPointSize(20.0);
    glColor3f(0.93, 0.05, 0.03);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();

}

void showLines(void)
{
    int startPos = 37.5;
    int endPos = 57.5;
    int labelXpos = 35;
    int labelYpos;
    int prevHeight;
    int currentHeight;
    char int_str[LBL];
    
    for (int i = 0; i < ACT; i++) 
    {
        int activityHours = 0;
        for (int j = 0; j < 53; j++) 
        {
            activityHours += activities[i].hours[j];
        }
        currentHeight = 10+(activityHours/10);
        labelYpos = currentHeight+2;
        sprintf(int_str, "%d", activityHours);
        printLabels(labelXpos, labelYpos, int_str);

        if (i>0)
        {       
            drawTopLines(startPos, prevHeight, endPos, currentHeight);
            startPos += STEP; 
            endPos += STEP; 
        }
        prevHeight = currentHeight;
        labelXpos += STEP;
        
    }
    

}

// draw bars on the chart
void drawBars(int x1, int width, int totalHours)
{
    glColor3f(0.0, 0.0, 1.0);
    int y2 = 10+(totalHours/10);
    int x2 = x1+width;
    glRecti(x1, 10, x2, y2);
}



// draw chart
void drawChart(void)
{
    int startPos = 32.5;
    int labelXpos = 35;
    
    for (int i = 0; i < ACT; i++) 
    {
        // printf("\n activity %s has these hours: ", activities[i].name);
        printLabels(labelXpos, 5, activities[i].name);
        int activityHours = 0;
        for (int j = 0; j < 53; j++) 
        {
            activityHours += activities[i].hours[j];
            // printf(" %d ", activityHours);
        }
        drawBars(startPos, 10, activityHours);
        labelXpos += STEP;
        startPos += STEP; 
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