/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BACKENDS_GRAPHICS_OPENGL_FRAMEBUFFER_H
#define BACKENDS_GRAPHICS_OPENGL_FRAMEBUFFER_H

#include "backends/graphics/opengl/opengl-sys.h"

namespace OpenGL {

/**
 * Object describing a framebuffer OpenGL can render to.
 */
class Framebuffer {
public:
	Framebuffer();
	virtual ~Framebuffer() {};

	/**
	 * Activate framebuffer.
	 *
	 * This is supposed to set all state associated with the framebuffer.
	 */
	virtual void activate() = 0;

	/**
	 * Deactivate framebuffer.
	 *
	 * This is supposed to make any cleanup required when unbinding the
	 * framebuffer.
	 */
	virtual void deactivate();

	/**
	 * Set the clear color of the framebuffer.
	 */
	void setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

	/**
	 * Enable/disable GL_BLEND.
	 */
	void enableBlend(bool enable);

	/**
	 * Enable/disable GL_SCISSOR_TEST.
	 */
	void enableScissorTest(bool enable);

	/**
	 * Set scissor box dimensions.
	 */
	void setScissorBox(GLint x, GLint y, GLsizei w, GLsizei h);

	/**
	 * Obtain projection matrix of the framebuffer.
	 */
	const GLfloat *getProjectionMatrix() const { return _projectionMatrix; }
protected:
	bool isActive() const { return _isActive; }

	GLint _viewport[4];
	void applyViewport();

	GLfloat _projectionMatrix[4*4];
private:
	bool _isActive;

	GLfloat _clearColor[4];
	void applyClearColor();

	bool _blendState;
	void applyBlendState();

	bool _scissorTestState;
	void applyScissorTestState();

	GLint _scissorBox[4];
	void applyScissorBox();
};

/**
 * Default back buffer implementation.
 */
class Backbuffer : public Framebuffer {
public:
	virtual void activate();

	/**
	 * Set the dimensions (a.k.a. size) of the back buffer.
	 */
	void setDimensions(uint width, uint height);
};

#if !USE_FORCED_GLES
class GLTexture;

/**
 * Render to texture framebuffer implementation.
 *
 * This target allows to render to a texture, which can then be used for
 * further rendering.
 */
class TextureTarget : public Framebuffer {
public:
	TextureTarget();
	virtual ~TextureTarget();

	virtual void activate();

	/**
	 * Notify that the GL context is about to be destroyed.
	 */
	void destroy();

	/**
	 * Notify that the GL context has been created.
	 */
	void create();

	/**
	 * Set size of the texture target.
	 */
	void setSize(uint width, uint height);

	/**
	 * Query pointer to underlying GL texture.
	 */
	GLTexture *getTexture() const { return _texture; }
private:
	GLTexture *_texture;
	GLuint _glFBO;
	bool _needUpdate;
};
#endif

} // End of namespace OpenGL

#endif
