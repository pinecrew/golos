/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>

#include "shadow_map_fbo.hpp"

ShadowMapFBO::ShadowMapFBO() : fbo( 0 ), shadowMap( 0 ){};

ShadowMapFBO::~ShadowMapFBO() {
    if ( fbo != 0 ) {
        glDeleteFramebuffers( 1, &fbo );
    }

    if ( shadowMap != 0 ) {
        glDeleteTextures( 1, &shadowMap );
    }
}

bool ShadowMapFBO::init( unsigned int windowWidth, unsigned int windowHeight ) {
    // Create the FBO
    glGenFramebuffers( 1, &fbo );

    // Create the depth buffer
    glGenTextures( 1, &shadowMap );
    glBindTexture( GL_TEXTURE_2D, shadowMap );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth,
                  windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_TEXTURE_2D, shadowMap, 0 );

    // Disable writes to the color buffer
    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );

    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE ) {
        printf( "FB error, status: 0x%x\n", status );
        return false;
    }

    return true;
}

void ShadowMapFBO::bindForWriting() {
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );
}

void ShadowMapFBO::bindForReading( GLenum textureUnit ) {
    glActiveTexture( textureUnit );
    glBindTexture( GL_TEXTURE_2D, shadowMap );
}
