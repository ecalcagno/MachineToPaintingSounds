#pragma once

#include "ofMain.h"

///////////////////////FLUID////////////////////////
#include "ofxFluid.h"
////////////////////////////////////////////////////


///////////////////////BOX2D////////////////////////
#include "ofxBox2d.h"
////////////////////////////////////////////////////


///////////////////////PARTICULAS///////////////////
#include "ofxVboParticles.h"
////////////////////////////////////////////////////


///////////////////////VORO/////////////////////////
#include "ofxVoro.h"
////////////////////////////////////////////////////

///////////////////////OSC//////////////////////////
#include "ofxosc.h"

#define HOST "localhost"
#define PORT 9001
////////////////////////////////////////////////////


#define N_SOUNDS 5

class SoundData {
public:
	int	 soundID;
	bool bHit;
};


class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
		
    void Generacion(float x, float y);
    void GeneracionParticulas(float x, float y, ofVec3f partPos, ofxVboParticles *vboPart);
    
    int escena = 0;
    int escena_anterior=0;
    
    //Varibale que controla las transiciones entre escenas
    float black_escena0 = 1;
    float black_escena1 = 1;
    float black_escena2 = 1;
    float black_escena3 = 1;
    
    
    //Variable que controla las coordenadas del mouse
    int MOUSEHand1x;
    int MOUSEHand1y;
 
    int MOUSEHand2x;
    int MOUSEHand2y;

    int time_escena2;
    
    bool mano1 = 1;
    bool mano2 = 1;
    
    ofImage imagenGestos;
    int GESTOS = 0;
    
    int audio = 1;
    int usuario = 0;
    ////////////////////////////////////////////////////
    ///////////////////////FLUID////////////////////////
    ofxFluid fluidHand1;
    ofxFluid fluidHand2;
    
    ofVec2f oldM1;
    ofVec2f oldM2;
    int     width,height;
    bool    bPaint, bObstacle, bBounding, bClear;
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////
    ///////////////////////BOX2D////////////////////////
    // little helper function to load
	// point that we saved
    
	vector <ofPoint> loadPoints(string file);
	
	bool                                breakupIntoTriangles;
	ofPolyline                          shape;
	ofxBox2d                            box2d;
	vector <ofPtr<ofxBox2dCircle> >     circles;
	vector <ofPtr<ofxBox2dPolygon> >	polyShapes;
    
    // this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);
    
    
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    ///////////////////PARTICULAS///////////////////////
    ofxVboParticles *vboPartciles1;
    ofVec3f particlePosition1;
    
    ofxVboParticles *vboPartciles2;
    ofVec3f particlePosition2;
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    /////////////////////VORO///////////////////////////
    void makeTissue(int _nCells, int _width, int _height, int _deep);
    
    ofEasyCam   cam;
    ofLight     light;
    
    vector<ofPoint> cellCentroids;
    vector<float>   cellRadius;
    vector<ofVboMesh>  cellMeshes;
    vector<ofVboMesh>  cellMeshWireframes;
    
    vector<ofColor>  colores;
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    /////////////////////OSC////////////////////////////
    ofxOscSender sender;
    ofxOscReceiver receiver;
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
};
