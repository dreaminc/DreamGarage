//
//  OSXOpenGLView.m
//  DreamOS
//
//  Created by Idan Beck on 12/15/15.
//  Copyright (c) 2015 Dream Garage. All rights reserved.
//

#import "OSXOpenGLView.h"

@implementation OSXOpenGLView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    glEnable(GL_DEPTH_TEST);
    
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float s = .15;
    glBegin(GL_QUADS); {
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
    }
    glEnd();
    
    glFlush();
    
    [self setNeedsDisplay:YES];
}

// Synchronize buffer swaps with vertical refresh rate
- (void)prepareOpenGL {
    GLint swapInt = 1;
    [m_oglctx setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

- (void)reshape {
    NSRect rect = [self bounds];
    
    glViewport(0, 0, rect.size.width, rect.size.height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(50, rect.size.width / rect.size.height, 0.1f, 30);     // TODO: This is deprecated us GLKMatrix4MakePerspective etc
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
}

@end
