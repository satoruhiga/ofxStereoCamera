#pragma once

#include "ofMain.h"

struct ofxStereoCameraViewMode
{
	enum Mode
	{
		PARALLEL,
		TOE_IN
	};
};

struct ofxStereoCameraOutputMode
{
	enum Mode
	{
		SIDE_BY_SIDE,
        CROSS_EYED,
		LINE_BY_LINE,
		SUTTER,
		ANAGLYPH
	};
};

template <class OF_CAMERA_CLASS = ofEasyCam>
class ofxStereoCamera : public OF_CAMERA_CLASS
{
public:

	ofxStereoCamera() : physical_eye_seperation_cm(6.5), physical_focus_distance_cm(200), focus_distance(200), view(ofxStereoCameraViewMode::PARALLEL), output(ofxStereoCameraOutputMode::ANAGLYPH), OF_CAMERA_CLASS() {}

	void setup(int w, int h)
	{
		width = w;
		height = h;

		left_fbo.allocate(width, height, GL_RGB);
		right_fbo.allocate(width, height, GL_RGB);

		setupShader();
	}

	void update(ofRectangle viewport = ofGetCurrentViewport())
	{
		OF_CAMERA_CLASS::begin();
		OF_CAMERA_CLASS::end();

		eye = physical_eye_seperation_cm * 0.5 / physical_focus_distance_cm;

		aspect = viewport.width / viewport.height;
		fovy_tan = tanf(PI * this->getFov() / 360.);
		fovx_tan = fovy_tan * aspect;

		zNear = this->getNearClip();
		zFar = this->getFarClip();
	}

	void draw(int x = 0, int y = 0)
	{
		draw(x, y, width, height);
	}

	void draw(int x, int y, int w, int h)
	{
		ofPushStyle();
		glPushMatrix();
		
		ofSetColor(255, 255);

		glTranslatef(x, y, 0);

		if (output == ofxStereoCameraOutputMode::SIDE_BY_SIDE)
		{
			right_fbo.draw(0, 0, w / 2, h);
			left_fbo.draw(0 + w / 2, 0, w / 2, h);
		}
		else if (output == ofxStereoCameraOutputMode::CROSS_EYED)
		{
			right_fbo.draw(0 + w / 4, h/4, w / 4, h / 4);
			left_fbo.draw(0 + w / 2, h/4, w / 4, h / 4);
		}
		else if (output == ofxStereoCameraOutputMode::LINE_BY_LINE)
		{
			shader.begin();
			shader.setUniformTexture("left", left_fbo.getTextureReference(), 1);
			shader.setUniformTexture("right", right_fbo.getTextureReference(), 2);

			glBegin(GL_TRIANGLE_STRIP);

			glTexCoord2f(0, 0);
			glVertex2f(0, 0);

			glTexCoord2f(width, 0);
			glVertex2f(w, 0);

			glTexCoord2f(0, height);
			glVertex2f(0, h);

			glTexCoord2f(width, height);
			glVertex2f(w, h);

			glEnd();

			shader.end();
		}
		else if (output == ofxStereoCameraOutputMode::ANAGLYPH)
		{
			ofPushStyle();

			ofEnableBlendMode(OF_BLENDMODE_ADD);

			ofSetColor(0, 255, 255);
			left_fbo.draw(0, 0, w, h);

			ofSetColor(255, 0, 0);
			right_fbo.draw(0, 0, w, h);

			ofPopStyle();
		}
		else if (output == ofxStereoCameraOutputMode::SUTTER)
		{
			frame_count++;
			frame_count = frame_count % 2;
			
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			
			ofSetColor(255);
			if (frame_count > 0)
			{
				left_fbo.draw(0, 0, w, h);
			}
			else
			{
				right_fbo.draw(0, 0, w, h);
			}
		}
		

		glPopMatrix();
		ofPopStyle();
	}

	//

	void setPhysicalEyeSeparation(float cm) { physical_eye_seperation_cm = cm; }
	void setPhysicalFocusDistance(float cm) { physical_focus_distance_cm = cm; }

	void setFocusDistance(float v) { focus_distance = v; }

	void setViewMode(ofxStereoCameraViewMode::Mode m) { view = m; }
	void setOutputMode(ofxStereoCameraOutputMode::Mode m) { output = m; }

	//

	void beginLeft()
	{
		left_fbo.begin();

		ofFloatColor c = ofGetCurrentRenderer()->getBgColor();
		ofClear(c);

		ofPushView();
		glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if (view == ofxStereoCameraViewMode::PARALLEL)
		{
			glFrustum(-zNear * (fovx_tan + eye), zNear * (fovx_tan - eye), -zNear * fovy_tan, zNear * fovy_tan, zNear, zFar);
			glTranslated(-eye * focus_distance, 0, 0);
		}
		else if (view == ofxStereoCameraViewMode::TOE_IN)
		{
			glMultMatrixf(this->getProjectionMatrix().getPtr());
			glRotatef(-rot, 0, 1, 0);
			glTranslated(-eye * focus_distance, 0, 0);
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMultMatrixf(this->getModelViewMatrix().getPtr());
	}

	void endLeft()
	{
		glPopMatrix();
		ofPopView();

		left_fbo.end();
	}

	void beginRight()
	{
		right_fbo.begin();

		ofFloatColor c = ofGetCurrentRenderer()->getBgColor();
		ofClear(c);

		ofPushView();
		glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if (view == ofxStereoCameraViewMode::PARALLEL)
		{
			glFrustum(-zNear * (fovx_tan - eye), zNear * (fovx_tan + eye), -zNear * fovy_tan, zNear * fovy_tan, zNear, zFar);
			glTranslated(eye * focus_distance, 0, 0);
		}
		else if (view == ofxStereoCameraViewMode::TOE_IN)
		{
			glMultMatrixf(this->getProjectionMatrix().getPtr());
			glRotatef(rot, 0, 1, 0);
			glTranslated(eye * focus_distance, 0, 0);
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMultMatrixf(this->getModelViewMatrix().getPtr());
	}

	void endRight()
	{
		glPopMatrix();
		ofPopView();

		right_fbo.end();
	}
	
	ofTexture& getLeftTexture()
	{
		return left_fbo.getTextureReference();
	}
	
	ofTexture& getRightTexture()
	{
		return right_fbo.getTextureReference();
	}

	ofFbo * getLeftFbo()
	{
		return &left_fbo;
	}
	
	ofFbo * getRightFbo()
	{
		return &right_fbo;
	}
    
protected:

	ofxStereoCameraViewMode::Mode view;
	ofxStereoCameraOutputMode::Mode output;

	//

	int width, height;

	float eye, focus_distance;

	float physical_eye_seperation_cm;
	float physical_focus_distance_cm;

	ofFbo left_fbo, right_fbo;

	// toe-in

	float rot;

	// parallel

	float zNear, zFar;
	float fovx_tan, fovy_tan;
	float aspect;
	
	// sutter
	int frame_count;

private:

	void begin() {}
	void end() {}

	ofShader shader;

	void setupShader()
	{
#define _S(code) # code
		const char *vs = _S
		(
			varying vec2 pos;

			void main()
			{
				gl_FrontColor = gl_Color;
				gl_TexCoord[0] = gl_MultiTexCoord0;
				gl_Position = ftransform();

				pos = gl_Vertex.xy;
			}
		);

		const char *fs = _S
		(
			uniform sampler2DRect left;
			uniform sampler2DRect right;

			varying vec2 pos;

			void main()
			{
				vec2 coord = gl_TexCoord[0].xy;
				bool odd = int(mod(pos.y, 2.0)) == 0;

				if (odd)
				{
					gl_FragColor = gl_Color * texture2DRect(left, coord);
				}
				else
				{
					gl_FragColor = gl_Color * texture2DRect(right, coord);
				}
			}
		);
#undef _S

		shader.setupShaderFromSource(GL_VERTEX_SHADER, vs);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fs);
		shader.linkProgram();
	}
};