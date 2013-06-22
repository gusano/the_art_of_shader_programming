#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class shader00noInputApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
    void keyDown(KeyEvent event);
    void loadShader();
    
    gl::GlslProgRef         mShader;
    std::time_t             mTimeVert;
    std::time_t             mTimeFrag;
    fs::path                mPathVert;
    fs::path                mPathFrag;
    bool                    mHide;          //show/hide error and fps
    std::string             mError;
    int                     mMode;
};

void shader00noInputApp::setup() {
    mHide= false;   //also keydown 'i'
    mError= "";
    mMode= 0;
    mPathVert= getPathDirectory(app::getAppPath().string())+"shaders/default_vert.glsl";
    mPathFrag= getPathDirectory(app::getAppPath().string())+"shaders/default_frag.glsl";
    loadShader();
}

void shader00noInputApp::keyDown(ci::app::KeyEvent event) {
	if(event.getChar()=='i') {
        mHide= !mHide;
    } else if(event.getCode()==KeyEvent::KEY_ESCAPE) {
        setFullScreen(!isFullScreen());
    } else if(event.getChar()=='v') {
        fs::path path= getOpenFilePath(mPathVert);
		if(!path.empty()) {
			mPathVert= path;
            loadShader();
		}
    } else if(event.getChar()=='f') {
        fs::path path= getOpenFilePath(mPathFrag);
        if(!path.empty()) {
			mPathFrag= path;
            loadShader();
		}
    } else if(event.getChar()=='m') {
        mMode= (mMode+1)%4;
    }
}
void shader00noInputApp::loadShader() {
    mError= "";
    try {
        mTimeVert= fs::last_write_time(mPathVert);
        mTimeFrag= fs::last_write_time(mPathFrag);
        mShader= gl::GlslProg::create(loadFile(mPathVert), loadFile(mPathFrag));
    }
    catch(gl::GlslProgCompileExc &exc) {
        mError= exc.what();
    }
    catch(...) {
        mError= "Unable to load shader";
    }
}

void shader00noInputApp::update() {
    if((fs::last_write_time(mPathVert)>mTimeVert)||(fs::last_write_time(mPathFrag)>mTimeFrag)) {
        loadShader();
    }
}

void shader00noInputApp::draw() {
	gl::clear(Color(0, 0, 0));
    
    mShader->bind();
    mShader->uniform("iResolution", (Vec2f)getWindowSize());
    mShader->uniform("iGlobalTime", (float)getElapsedSeconds());
    gl::color(1.0f, 1.0f, 1.0f);
    switch(mMode) {
        case 0:
            gl::drawSolidRect(Rectf(getWindowBounds()).scaledCentered(0.8f));
            break;
        case 1:
            gl::drawSolidTriangle(
                                  Vec2f(getWindowCenter()*Vec2f(1.0f, 0.25f)),
                                  Vec2f(getWindowCenter()*Vec2f(0.25f, 1.75f)),
                                  Vec2f(getWindowCenter()*Vec2f(1.75f, 1.75f)));
            break;
        case 2:
            gl::drawSolidCircle(getWindowCenter(), getWindowHeight()*0.4f, 12);
            break;
        case 3:
            gl::drawSphere((Vec3f)getWindowCenter(), getWindowHeight()*0.4f, 100);
            break;
        default:
            break;
    }
    mShader->unbind();
    
    if(!mHide) {
        gl::drawString("mode (m): "+toString(mMode), Vec2f(30.0f, getWindowHeight()-100.0f), Color(1, 1, 1), Font("Verdana", 12));
        gl::drawString("vert (v): "+toString(mPathVert.filename()), Vec2f(30.0f, getWindowHeight()-80.0f), Color(1, 1, 1), Font("Verdana", 12));
        gl::drawString("frag (f): "+toString(mPathFrag.filename()), Vec2f(30.0f, getWindowHeight()-60.0f), Color(1, 1, 1), Font("Verdana", 12));
        gl::drawString("error: "+mError, Vec2f(30.0f, getWindowHeight()-40.0f), Color(1, 1, 1), Font("Verdana", 12));
        gl::drawString("fps: "+toString(getAverageFps()), Vec2f(30.0f, getWindowHeight()-20.0f), Color(1, 1, 1), Font("Verdana", 12));
    }
}

CINDER_APP_NATIVE( shader00noInputApp, RendererGl )
