#include "ofApp.h"

////////////////////////////////////////////////////
///////////////////////VORO/////////////////////////
ofColor colorFondo = 0;
const int N = 20;				//numero de sonidos
ofSoundPlayer sound[ N ];       //Array de sonidos
float vol[N];					//Array con volumenes de los sonidos
vector<bool> play;

////////////////////////////////////////////////////
////////////////////////////////////////////////////


////////////////////////////////////////////////////
///////////////////////BOX2D////////////////////////

// when the ball hits we play this sound
ofSoundPlayer  soundVidrio[N_SOUNDS];

static bool shouldRemove(ofPtr<ofxBox2dBaseShape>shape) {
    
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape.get()->getPosition());
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////

//--------------------------------------------------------------
vector <ofPoint> testApp::loadPoints(string file) {
    vector <ofPoint> pts;
    vector <string>  ptsStr = ofSplitString(ofBufferFromFile(file).getText(), ",");
    
    for (int i=0; i<ptsStr.size(); i+=2) {
        float x = ofToFloat(ptsStr[i]);
        float y = ofToFloat(ptsStr[i+1]);
        pts.push_back(ofPoint(x, y));
    }
	return pts;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////

//--------------------------------------------------------------
void testApp::setup(){
    
    
    imagenGestos.loadImage("gestos.png");
    imagenGestos.setAnchorPercent(0.5, 0.5);
    
    
    ////////////////////////////////////////////////////
    ///////////////////////FLUID////////////////////////
    ofSetCircleResolution(100);
    
    width = 1366;
    height = 768;

    // Initial Allocation
    fluidHand1.allocate(width, height, 0.5);
    // Seting the gravity set up & injecting the background image
    fluidHand1.dissipation = 0.99;
    fluidHand1.velocityDissipation = 0.89;
    fluidHand1.setGravity(ofVec2f(0.0,0.0));
    
    /*
    // Initial Allocation
    fluidHand2.allocate(width, height, 0.5);
    // Seting the gravity set up & injecting the background image
    fluidHand2.dissipation = 0.99;
    fluidHand2.velocityDissipation = 0.89;
    fluidHand2.setGravity(ofVec2f(0.0,0.0));
     */
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////
    ///////////////////////BOX2D////////////////////////
	ofBackgroundHex(0xfdefc2);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetVerticalSync(true);
	
	// Box2d
    
    breakupIntoTriangles = true;
    
    box2d.init();
    box2d.enableEvents();   // <-- turn on the event listener
	box2d.setGravity(0, 50);
	box2d.createGround();
	box2d.setFPS(30.0);
	box2d.registerGrabbing();
	
	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);
	// load the 8 sfx soundfile
	for (int i=0; i<N_SOUNDS; i++) {
		soundVidrio[i].loadSound("ESCENA1/"+ofToString(i,4)+".wav");
		soundVidrio[i].setMultiPlay(true);
		soundVidrio[i].setLoop(false);
	}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    ///////////////////PARTICULAS///////////////////////
    
    
    // ofxVboParticles([max particle number], [particle size]);
    vboPartciles1 = new ofxVboParticles(3000);
    
    // set friction (0.0 - 1.0);
    vboPartciles1->friction = 0.00001;
    
    // ofxVboParticles([max particle number], [particle size]);
    vboPartciles2 = new ofxVboParticles(5000);
    
    // set friction (0.0 - 1.0);
    vboPartciles2->friction = 0.00001;
    
    particlePosition1 = ofVec3f(0, 0, 0);
    particlePosition2 = ofVec3f(0, 0, 0);
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    /////////////////////VORO///////////////////////////
    light.setPosition(100,500, 100);
    
    makeTissue(50, ofGetHeight()*0.8, ofGetHeight()*0.8, 1);
    
    colores.resize(N);
    play.resize(N);
    
    for(int i= 0; i< colores.size();i++)
    {
        colores[i] = ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255));
        
    }
    
    //Cargamos y seteamos los ejemplos de audio
	for ( int i=0; i<N; i++)
    {
		//Vamos armando los nombres segun el indice i
        sound[i].loadSound("ESCENA3/" + ofToString( i, 4 ) + ".wav" );
        //Activamos el loop para cada uno
		//sound[i].setLoop( true );
        
		//Seteamos un panoramico para cada uno, relacionandolo con el ’ndice
		sound[i].setPan( ofMap( i, 0, N-1, -0.5, 0.5 ) );
        
        //Ponemos el volumen de cada archivo en 0
		//sound[i].setVolume( 0 );
        play[i] = 1;
        
	}
    
	//Seteamos el volumen general
	ofSoundSetVolume( 0.5 );
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    /////////////////////OSC////////////////////////////
    // open an outgoing connection to HOST:PORT
	//sender.setup(HOST, PORT);
    /*
    ofxOscMessage m;
	m.setAddress("/escena");
	m.addIntArg(escena);
	sender.sendMessage(m);
    */
    receiver.setup(PORT);
    
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    
    ofSetWindowShape(width, height);
    
    MOUSEHand1x = -100;
    MOUSEHand1y = -100;
    
    MOUSEHand2x = -100;
    MOUSEHand2y = -100;
    
    ofHideCursor();
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e) {
	if(e.a != NULL && e.b != NULL) {
		
		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		//if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
        
        SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
        SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
        
        if(aData) {
            aData->bHit = true;
            
            
            if(usuario==1)
            {
            soundVidrio[aData->soundID].setPan(ofRandom(0,1));
            soundVidrio[aData->soundID].play();
            }
        }
        
        if(bData) {
            
            if(usuario==1)
            {
            bData->bHit = true;
            soundVidrio[bData->soundID].setPan(ofRandom(0,1));
            soundVidrio[bData->soundID].play();
            }
        }
		//}
	}
    
    
    
 
}

//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e) {
	if(e.a != NULL && e.b != NULL) {
		
		SoundData * aData = (SoundData*)e.a->GetBody()->GetUserData();
		SoundData * bData = (SoundData*)e.b->GetBody()->GetUserData();
		
		if(aData) {
			aData->bHit = false;
		}
		
		if(bData) {
			bData->bHit = false;
		}
	}
}

////////////////////////////////////////////////////
/////////////////////VORO///////////////////////////
/////////////////////////////////////////////////////////////////////////////
void testApp::makeTissue(int _nCells, int _width, int _height, int _deep){
    
    //  Fresh begining
    //
    cellMeshes.clear();
    cellCentroids.clear();
    cellRadius.clear();
    
    //  Define a container
    //
    voro::container con(-_width*0.8,_width*0.8,
                        -_height*0.8,_height*0.8,
                        -_deep*0.8,_deep*0.8,
                        1,1,1,
                        true,true,true,
                        8);
    
    
    //  Add walls (un comment one pair if you like to shape the container)
    voro::wall_cylinder cyl(0,0,0,0,0,20, min(_width*0.8, _height*0.8));
    con.add_wall(cyl);

    //  Add the cell seed to the container
    //
    for(int i = 0; i < N;i++){
        
        ofPoint newCell = ofPoint(ofRandom(-_width*0.8,_width*0.8),
                                  ofRandom(-_height*0.8,_height*0.8),
                                  ofRandom(-_deep*0.25,_deep*0.25));
        
        addCellSeed(con, newCell, i, true);
        
    }
    
    cellMeshes = getCellsFromContainer(con,0.0);
    cellMeshWireframes = getCellsFromContainer(con,0.0,true);
    cellRadius = getCellsRadius(con);
    cellCentroids = getCellsCentroids(con);
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

//--------------------------------------------------------------
void testApp::update(){
    
    ////////////////////////////////////////////////////
    ///////////////////////FLUID////////////////////////
    
    float time = ofGetElapsedTimef();
    // Adding temporal Force
    ofPoint m1 = ofPoint(MOUSEHand1x,MOUSEHand1y);
    ofPoint d1 = (m1 - oldM1)*50;
    oldM1 = m1;
    ofPoint c1 = ofPoint(800*0.5, 600*0.5) - m1;
    c1.normalize();
    fluidHand1.addTemporalForce(m1, d1, ofFloatColor(1,ofMap(MOUSEHand2x, 0, ofGetWidth(),0,1),ofMap(MOUSEHand2y, 0, ofGetHeight(),0,1)),10);
    /*
     *sin(ofGetElapsedTimef()
    // Adding temporal Force
    ofPoint m2 = ofPoint(MOUSEHand2x,MOUSEHand2y);
    ofPoint d2 = (m2 - oldM2)*50;
    oldM2 = m2;
    ofPoint c2 = ofPoint(800*0.5, 600*0.5) - m2;
    c2.normalize();
    fluidHand2.addTemporalForce(m2, d2, ofFloatColor(0.5-c2.x,c2.y,0.5)*sin(ofGetElapsedTimef()),10);
    */
    //  Update
    fluidHand1.update();
    /*
    fluidHand2.update();
     */
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    ///////////////////////BOX2D////////////////////////
    box2d.update();
    
    ofRemove(polyShapes, shouldRemove);
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    ///////////////////PARTICULAS///////////////////////
    vboPartciles1->update();
    vboPartciles2->update();
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    /////////////////////VORO///////////////////////////
    
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////

    
    //OSC Receive
    // check for waiting messages
	while(receiver.hasWaitingMessages())
    {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        
        if(m.getAddress() == "/escena")
        {
            escena = m.getArgAsInt32(0);
		}
    
        
		if(m.getAddress() == "/hand1x")
        {
            MOUSEHand1x = ofMap(m.getArgAsFloat(0), 0, 1, 0, ofGetWidth());
		}
        
        if(m.getAddress() == "/hand1y")
        {
            MOUSEHand1y = ofMap(m.getArgAsFloat(0), 0,1, 0, ofGetHeight());
		}
        
        if(m.getAddress() == "/hand2x")
        {
            MOUSEHand2x = ofMap(m.getArgAsFloat(0), 0, 1, 0, ofGetWidth());
		}
        
        if(m.getAddress() == "/hand2y")
        {
            MOUSEHand2y = ofMap(m.getArgAsFloat(0), 0,1, 0, ofGetHeight());
		}
        
        if(m.getAddress() == "/gesto")
        {
            GESTOS = m.getArgAsInt32(0);
		}
        
        if(m.getAddress() == "/audio")
        {
            audio = m.getArgAsInt32(0);
		}
        
        if(m.getAddress() == "/usuario")
        {
            usuario = m.getArgAsInt32(0);
		}
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    
if(usuario == 1)
{
    
    
////////////////////////////////////////////////////
/////////////////////ESCENA0////////////////////////
    if(escena == 0)
    {
    if(black_escena0>0)
    {
        
        if(escena_anterior == 0)
        {
            black_escena0 = 0;
        }
        
        
        //LimpiarEscena1
        if(escena_anterior==1)
        {
            ////////////////////////////////////////////////////
            ///////////////////////BOX2D////////////////////////
            ofFill();
            ofSetColor(255*black_escena0);
            shape.draw();
            
            ofEllipse(MOUSEHand1x,MOUSEHand1y, 10,10);
            
            
            for (int i=0; i<polyShapes.size(); i++) {
                
                ofSetColor(0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena0);
                
                
                
                
                polyShapes[i].get()->draw();
                
            }
            ofNoFill();
        
            black_escena0 -= 0.05;
        
        }
        //LimpiarEscena2
        if(escena_anterior==2)
        {
            ////////////////////////////////////////////////////
            ///////////////////PARTICULAS///////////////////////
            //ofEnableDepthTest();
            
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            time_escena2 = ofGetElapsedTimef();
            
            ofPushMatrix();
            
            ofRotate(sin(ofGetElapsedTimef()) * ofRandom(ofMap(MOUSEHand1x, 0, ofGetWidth(), -10,10),ofMap(MOUSEHand1y, 0, ofGetHeight(), -10,10)), 0, 1, 1);
            
            //vboPartciles->draw();
            
            for(int i = 2; i < vboPartciles1->numParticles-1;i++)
            {
                ofSetColor(sin(time_escena2+10)*255/2 + 255, sin(time_escena2+20)*255/2 + 255, sin(time_escena2+50)*255/2 + 255, black_escena0*50);
                
                ofLine(vboPartciles1->positions[i], vboPartciles1->positions[i+1]);
                ofCircle(vboPartciles1->positions[i], 2);
            }
            
            ofPopMatrix();
            ofDisableBlendMode();
            
            ////////////////////////////////////////////////////
            ////////////////////////////////////////////////////
            
            
            black_escena0 -=0.09;
        }
        
        //LimpiarEscena3
        if(escena_anterior==3)
        {
            ////////////////////////////////////////////////////
            /////////////////////VORO///////////////////////////
            float time =  ofGetElapsedTimef();
            float angulo = time*10;
            ofPushMatrix();
            //cam.begin();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            
            ofRotate(angulo, 0, 0, 1);
            
            
            light.enable();
            ofEnableLighting();
            glEnable(GL_DEPTH_TEST);
            
            for (int i = 0; i < cellCentroids.size(); i++){
                ofSetColor(255);
                
                
            }
            
            for(int i = 0; i < cellMeshes.size(); i++){
                
                
                float radio = sqrt(pow(cellCentroids[i].x, 2) + pow(cellCentroids[i].y, 2));
                
                float anguloRad = ofDegToRad(angulo);
                
                float coordX = ofMap(MOUSEHand1x, 0, ofGetWidth(), -ofGetWidth()/2, ofGetWidth()/2);
                float coordY = ofMap(MOUSEHand1y, 0, ofGetHeight(), -ofGetHeight()/2, ofGetHeight()/2);
                
                
                if((coordX > (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) - cellRadius[i]*2 &&
                    coordX < (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) + cellRadius[i]*2 &&
                    coordY > (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) - cellRadius[i]*2 &&
                    coordY < (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) + cellRadius[i]*2))
                {
                    
                    ofSetColor(255);
                    //Reproducimos los archivos
                    
                    int sonido = ofRandom(0,cellMeshes.size());
                    
                    if(sound[sonido].getIsPlaying())
                    {
                        if(audio ==1) play[sonido]=0;
                    }else
                    {
                        if(audio ==1) play[sonido]=1;
                    }
                    
                    if(play[sonido] == 1)
                    {
                        if(audio ==1) play[sonido]=0;
                        if(audio ==1) sound[sonido].play();
                    }
                }else
                {
                    
                    ofSetColor(colores[i].r*black_escena0, colores[i].g*black_escena0, colores[i].b*black_escena0);
                    //sound[i].setVolume( 0 );
                }
                
                
                cellMeshes[i].drawFaces();
                ofPushStyle();
                ofSetLineWidth(3);
                ofSetColor(0);
                cellMeshWireframes[i].draw();
                
                ofSetColor(0);
                ofCircle(cellCentroids[i].x, cellCentroids[i].y, 40, 10);
                
            }
            
            glDisable(GL_DEPTH_TEST);
            ofDisableLighting();
            light.disable();
            //cam.end();
            ofPopMatrix();
            
            ofPushStyle();
            ofFill();
            ofCircle(MOUSEHand1x, MOUSEHand1y, 50, 10);
            ofPopStyle();
            
            black_escena0 -=0.1;
        }
    }
    else
    {
        black_escena1 = 1;
        black_escena2 = 1;
        black_escena3 = 1;
        
        escena_anterior = 0;
        //Renovar Escena 1
        polyShapes.clear();
        //Renovar Escena 2
        //vboPartciles1->numParticles = 0;
        //vboPartciles1 = new ofxVboParticles(5000, 2000);
        
        //vboPartciles2->numParticles = 0;
        //vboPartciles2 = new ofxVboParticles(5000, 2000);
        //Renovar Escena 3
        makeTissue(50, ofGetHeight()*0.8, ofGetHeight()*0.8, 1);
        
        // Seting the gravity set up & injecting the background image
        fluidHand1.dissipation = 0.99;
        fluidHand1.velocityDissipation = 0.89;
        
        //fluidHand2.dissipation = 0.99;
        //fluidHand2.velocityDissipation = 0.89;
        
        
        ofBackground(0);
        ofEnableAlphaBlending();
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        fluidHand1.draw();
       // fluidHand2.draw();
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
        ofSetColor(255,ofMap(MOUSEHand2x, 0, ofGetWidth(),0,255),ofMap(MOUSEHand2y, 0, ofGetHeight(),0,255));
        ofRect(MOUSEHand2x, MOUSEHand2y, 30,30);
    }
}
    
////////////////////////////////////////////////////
/////////////////////ESCENA1////////////////////////
    if(escena == 1)
    {
        
        
    if(black_escena1>0)
        {
            //LimpiarEscena0
            if(escena_anterior==0)
            {
                fluidHand1.dissipation = black_escena1;
                fluidHand1.velocityDissipation = black_escena1;
                fluidHand1.draw();
                /*
                fluidHand2.dissipation = black_escena1;
                fluidHand2.velocityDissipation = black_escena1;
                fluidHand2.draw();*/
        
                black_escena1 -=0.1;
            }
            
            //LimpiarEscena2
            if(escena_anterior==2)
            {
                ////////////////////////////////////////////////////
                ///////////////////PARTICULAS///////////////////////
                //ofEnableDepthTest();
                ofEnableBlendMode(OF_BLENDMODE_ADD);
                time_escena2 = ofGetElapsedTimef();
                
                ofPushMatrix();
                
                ofRotate(sin(ofGetElapsedTimef()) * ofRandom(ofMap(MOUSEHand1x, 0, ofGetWidth(), -10,10),ofMap(MOUSEHand1y, 0, ofGetHeight(), -10,10)), 0, 1, 1);
                
                //vboPartciles->draw();
                
                for(int i = 2; i < vboPartciles1->numParticles-1;i++)
                {
                    ofSetColor(sin(time_escena2+10)*255/2 + 255, sin(time_escena2+20)*255/2 + 255, sin(time_escena2+50)*255/2 + 255, black_escena1*50);
                    
                    ofLine(vboPartciles1->positions[i], vboPartciles1->positions[i+1]);
                    ofCircle(vboPartciles1->positions[i], 2);
                }
                
                ofPopMatrix();
                ofDisableBlendMode();
                
                ////////////////////////////////////////////////////
                ////////////////////////////////////////////////////


                black_escena1 -=0.09;
            }
            
            //LimpiarEscena3
            if(escena_anterior==3)
            {
                ////////////////////////////////////////////////////
                /////////////////////VORO///////////////////////////
                float time =  ofGetElapsedTimef();
                float angulo = time*10;
                ofPushMatrix();
                //cam.begin();
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                
                ofRotate(angulo, 0, 0, 1);
                
                
                light.enable();
                ofEnableLighting();
                glEnable(GL_DEPTH_TEST);
                
                for (int i = 0; i < cellCentroids.size(); i++){
                    ofSetColor(255);
                    
                    
                }
                
                for(int i = 0; i < cellMeshes.size(); i++){
                    
                    
                    float radio = sqrt(pow(cellCentroids[i].x, 2) + pow(cellCentroids[i].y, 2));
                    
                    float anguloRad = ofDegToRad(angulo);
                    
                    float coordX = ofMap(MOUSEHand1x, 0, ofGetWidth(), -ofGetWidth()/2, ofGetWidth()/2);
                    float coordY = ofMap(MOUSEHand1y, 0, ofGetHeight(), -ofGetHeight()/2, ofGetHeight()/2);
                    
                    
                    if((coordX > (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                        coordX < (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2 &&
                        coordY > (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                        coordY < (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2))
                    {
                        
                        ofSetColor(255);
                        //Reproducimos los archivos
                        
                        if(sound[i].getIsPlaying())
                        {
                            if(audio ==1)play[i]=0;
                        }else
                        {
                            if(audio ==1)play[i]=1;
                        }
                        
                        if(play[i] == 1)
                        {
                            if(audio ==1)play[i]=0;
                            sound[i].play();
                        }
                    }else
                    {
                        
                        ofSetColor(colores[i].r*black_escena1, colores[i].g*black_escena1, colores[i].b*black_escena1);
                        //sound[i].setVolume( 0 );
                    }
                    
                    
                    cellMeshes[i].drawFaces();
                    ofPushStyle();
                    ofSetLineWidth(3);
                    ofSetColor(0);
                    cellMeshWireframes[i].draw();
                    
                    ofSetColor(0);
                    ofCircle(cellCentroids[i].x, cellCentroids[i].y, 40, 10);
                    
                }
                
                glDisable(GL_DEPTH_TEST);
                ofDisableLighting();
                light.disable();
                //cam.end();
                ofPopMatrix();
                
                ofPushStyle();
                ofFill();
                ofCircle(MOUSEHand1x, MOUSEHand1y, 50, 10);
                ofPopStyle();
                
                black_escena1 -=0.1;
            }
        }
    else
    {
        
        //ofBackground(MOUSEHand2x*255,MOUSEHand2y*255);
        
        black_escena0 = 1;
        black_escena2 = 1;
        black_escena3 = 1;
        
        escena_anterior = 1;
        //Renovar Escena 0
        fluidHand1.clear();
        //fluidHand2.clear();
        //Renovar Escena 2
        //vboPartciles1->numParticles = 0;
        //vboPartciles1 = new ofxVboParticles(5000, 2000);
        
        //vboPartciles2->numParticles = 0;
        //vboPartciles2 = new ofxVboParticles(5000, 2000);
        //Renovar Escena 3
        makeTissue(50, ofGetHeight()*0.8, ofGetHeight()*0.8, 1);

        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        
        Generacion(MOUSEHand1x, MOUSEHand1y);
        
        ofFill();
        shape.draw();
        ofSetColor(255);
        ofEllipse(MOUSEHand1x,MOUSEHand1y, 10,10);
        
        ofSetColor(ofRandom(0,255), ofMap(MOUSEHand2x, 0, ofGetWidth(), 0, 255), ofMap(MOUSEHand2y, 0, ofGetWidth(), 0, 255), ofRandom(0,255));
        ofRect(MOUSEHand2x,MOUSEHand2y, 30,30);
        
        for (int i=0; i<polyShapes.size(); i++) {
            ofSetColor(ofRandom(0,255), ofMap(MOUSEHand2x, 0, ofGetWidth(), 0, 255), ofMap(MOUSEHand2y, 0, ofGetWidth(), 0, 255), ofRandom(0,255));
            
            SoundData * data = (SoundData*)polyShapes[i].get()->getData();
            
            polyShapes[i].get()->draw();
            
        }
        ofNoFill();
    }
}

////////////////////////////////////////////////////
/////////////////////ESCENA2////////////////////////
    if(escena == 2)
    {
        
        if(black_escena2>0)
        {
            //LimpiarEscena0
            if(escena_anterior==0)
            {
                fluidHand1.dissipation = black_escena2;
                fluidHand1.velocityDissipation = black_escena2;
                fluidHand1.draw();
                
                /*
                fluidHand2.dissipation = black_escena2;
                fluidHand2.velocityDissipation = black_escena2;
                fluidHand2.draw();
                */
                black_escena2 -=0.1;
            }
            
            //LimpiarEscena1
            if(escena_anterior==1)
            {
                ////////////////////////////////////////////////////
                ///////////////////////BOX2D////////////////////////
                ofFill();
                ofSetColor(255*black_escena2);
                shape.draw();
                
                ofEllipse(MOUSEHand1x,MOUSEHand1y, 10,10);
                
                
                for (int i=0; i<polyShapes.size(); i++) {
                    
                    ofSetColor(0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena2);
                    polyShapes[i].get()->draw();
                    
                }
                ofNoFill();
                
                black_escena2 -= 0.05;
                
            }

            
            //LimpiarEscena3
            if(escena_anterior==3)
            {
                ////////////////////////////////////////////////////
                /////////////////////VORO///////////////////////////
                float time =  ofGetElapsedTimef();
                float angulo = time*10;
                ofPushMatrix();
                //cam.begin();
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                
                ofRotate(angulo, 0, 0, 1);
                
                
                light.enable();
                ofEnableLighting();
                glEnable(GL_DEPTH_TEST);
                
                for (int i = 0; i < cellCentroids.size(); i++){
                    ofSetColor(255);
                    
                    
                }
                
                for(int i = 0; i < cellMeshes.size(); i++){
                    
                    
                    float radio = sqrt(pow(cellCentroids[i].x, 2) + pow(cellCentroids[i].y, 2));
                    
                    float anguloRad = ofDegToRad(angulo);
                    
                    float coordX = ofMap(MOUSEHand1x, 0, ofGetWidth(), -ofGetWidth()/2, ofGetWidth()/2);
                    float coordY = ofMap(MOUSEHand1y, 0, ofGetHeight(), -ofGetHeight()/2, ofGetHeight()/2);
                    
                    
                    if((coordX > (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                        coordX < (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2 &&
                        coordY > (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                        coordY < (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2))
                    {
                        
                        ofSetColor(255);
                        //Reproducimos los archivos
                        
                        if(sound[i].getIsPlaying())
                        {
                           if(audio ==1) play[i]=0;
                        }else
                        {
                            if(audio ==1)play[i]=1;
                        }
                        
                        if(play[i] == 1)
                        {
                           if(audio ==1) play[i]=0;
                           if(audio ==1) sound[i].play();
                        }
                    }else
                    {
                        
                        ofSetColor(colores[i].r*black_escena2, colores[i].g*black_escena2, colores[i].b*black_escena2);
                        //sound[i].setVolume( 0 );
                    }
                    
                    
                    cellMeshes[i].drawFaces();
                    ofPushStyle();
                    ofSetLineWidth(3);
                    ofSetColor(0);
                    cellMeshWireframes[i].draw();
                    
                    ofSetColor(0);
                    ofCircle(cellCentroids[i].x, cellCentroids[i].y, 40, 10);
                    
                }
                
                glDisable(GL_DEPTH_TEST);
                ofDisableLighting();
                light.disable();
                //cam.end();
                ofPopMatrix();
                
                ofPushStyle();
                ofFill();
                ofCircle(MOUSEHand1x, MOUSEHand1y, 50, 10);
                ofPopStyle();
                
                black_escena2 -=0.1;
            }
        }
        else
        {
        black_escena0 = 1;
        black_escena1 = 1;
        black_escena3 = 1;
        escena_anterior = 2;
        ofBackground(0);
            
            
        //Renovar Escena 0
        fluidHand1.clear();
        //fluidHand2.clear();
        //Renovar Escena 1
        polyShapes.clear();
        //Renovar Escena 3
        makeTissue(50, ofGetHeight()*0.8, ofGetHeight()*0.8, 1);
            
            
        GeneracionParticulas(MOUSEHand1x, MOUSEHand1y, particlePosition1, vboPartciles1);
        GeneracionParticulas(MOUSEHand2x, MOUSEHand2y, particlePosition2, vboPartciles2);
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        //ofEnableDepthTest();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        time_escena2 = ofGetElapsedTimef();
        
        ofPushMatrix();
        
        ofRotate(sin(ofGetElapsedTimef()) * ofRandom(ofMap(MOUSEHand1x, 0, ofGetWidth(), -10,10),ofMap(MOUSEHand1y, 0, ofGetHeight(), -10,10)), 0, 1, 1);
        
        //vboPartciles->draw();
        
for(int k =0; k<2; k++)
{
                
            
            
           if(k == 0)
           {
               for(int i = 2; i < vboPartciles1->numParticles-1;i++)
               {
                   ofSetColor(sin(time_escena2+ofMap(MOUSEHand1x, 0, ofGetWidth(),0,1))*255/2 + 255, sin(time_escena2+20)*255/2 + 255, sin(time_escena2+50)*255/2 + 255, 50);
            
                   ofLine(vboPartciles1->positions[i], vboPartciles1->positions[i+1]);
                   ofCircle(vboPartciles1->positions[i], 2);
               }
           }
    
        if(k == 1)
        {
            for(int i = 2; i < vboPartciles2->numParticles-1;i++)
            {
            ofSetColor(sin(time_escena2+(ofMap(MOUSEHand2x, 0, ofGetWidth(),0,1))*10)*255/2 + 255, sin(time_escena2+20)*255/2 + 255, sin(time_escena2+50)*255/2 + 255, 50);
            
            ofLine(vboPartciles2->positions[i], vboPartciles2->positions[i+1]);
            ofCircle(vboPartciles2->positions[i], 2);
            }
        }
    
}
        
        ofPopMatrix();
        ofDisableBlendMode();
    
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
            }
    }
    
////////////////////////////////////////////////////
/////////////////////ESCENA3////////////////////////
    if(escena == 3)
    {
        ////////////////////////////////////////////////////
        //////////////LIMPIAR ESCENAS ANTERIORES////////////
        
        if(black_escena3>0)
        {
            
            //LimpiarEscena0
            if(escena_anterior==0)
            {
                fluidHand1.dissipation = black_escena3;
                fluidHand1.velocityDissipation = black_escena3;
                fluidHand1.draw();
                /*
                fluidHand2.dissipation = black_escena3;
                fluidHand2.velocityDissipation = black_escena3;
                fluidHand2.draw();
                */
                black_escena3 -=0.1;
            }
            
            //LimpiarEscena1
            if(escena_anterior==1)
            {
                ////////////////////////////////////////////////////
                ///////////////////////BOX2D////////////////////////
                ofFill();
                ofSetColor(255*black_escena3);
                shape.draw();
                
                ofEllipse(MOUSEHand1x,MOUSEHand1y, 10,10);
                
                
                for (int i=0; i<polyShapes.size(); i++) {
                    
                    ofSetColor(0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena0, ofRandom(0,255)*black_escena3);
                    polyShapes[i].get()->draw();
                    
                }
                ofNoFill();
                
                black_escena3 -= 0.05;
                
            }
            
            //LimpiarEscena2
            if(escena_anterior==2)
            {
                ////////////////////////////////////////////////////
                ///////////////////PARTICULAS///////////////////////
                //ofEnableDepthTest();
                ofEnableBlendMode(OF_BLENDMODE_ADD);
                time_escena2 = ofGetElapsedTimef();
                
                ofPushMatrix();
                
                ofRotate(sin(ofGetElapsedTimef()) * ofRandom(ofMap(MOUSEHand1x, 0, ofGetWidth(), -10,10),ofMap(MOUSEHand1y, 0, ofGetHeight(), -10,10)), 0, 1, 1);
                
                //vboPartciles->draw();
                
                for(int i = 2; i < vboPartciles1->numParticles-1;i++)
                {
                    ofSetColor(sin(time_escena2+10)*255/2 + 255, sin(time_escena2+20)*255/2 + 255, sin(time_escena2+50)*255/2 + 255, black_escena3*50);
                    
                    ofLine(vboPartciles1->positions[i], vboPartciles1->positions[i+1]);
                    ofCircle(vboPartciles1->positions[i], 2);
                }
                
                ofPopMatrix();
                ofDisableBlendMode();
                
                ////////////////////////////////////////////////////
                ////////////////////////////////////////////////////
                
                
                black_escena3 -=0.09;
            }
           
        }
        else
        {
        
        black_escena0 = 1;
        black_escena1 = 1;
        black_escena2 = 1;
        
        //Renovar Escena 0
        fluidHand1.clear();
       // fluidHand2.clear();
        //Renovar Escena 1
        polyShapes.clear();
        //Renovar Escena 2
        //vboPartciles1->numParticles = 0;
        //vboPartciles1 = new ofxVboParticles(5000, 2000);
         
        //vboPartciles2->numParticles = 0;
       //vboPartciles2 = new ofxVboParticles(5000, 2000);
            
        escena_anterior = 3;
        
            ofBackground(0);
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        float time =  ofGetElapsedTimef();
        float angulo = time*10;
        ofPushMatrix();
        //cam.begin();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        
        ofRotate(angulo, 0, 0, 1);
        
            ofPushStyle();
        light.enable();
        ofEnableLighting();
        glEnable(GL_DEPTH_TEST);
        
        for (int i = 0; i < cellCentroids.size(); i++){
            ofSetColor(255);
            
            
        }
            
            
for(int k=0; k<2; k++)
{
    
        for(int i = 0; i < cellMeshes.size(); i++){
            
            
            float radio = sqrt(pow(cellCentroids[i].x, 2) + pow(cellCentroids[i].y, 2));
            
            float anguloRad = ofDegToRad(angulo);
            
            float coordX;
            float coordY;
            
            
            if(k==0)
            {
            coordX = ofMap(MOUSEHand1x, 0, ofGetWidth(), -ofGetWidth()/2, ofGetWidth()/2);
            coordY = ofMap(MOUSEHand1y, 0, ofGetHeight(), -ofGetHeight()/2, ofGetHeight()/2);
                
                if((coordX > (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                    coordX < (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2 &&
                    coordY > (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                    coordY < (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2))
                {
                    
                    ofSetColor(255);
                    cellMeshes[i].drawFaces();
                    //Reproducimos los archivos
                    
                    if(sound[i].getIsPlaying())
                    {
                        if(audio ==1)play[i]=0;
                    }else
                    {
                        if(audio ==1)play[i]=1;
                    }
                    
                    if(play[i] == 1)
                    {
                        if(audio ==1)play[i]=0;
                        if(audio ==1)sound[i].play();
                    }
                }else
                {
                    
                    //ofSetColor(colores[i]);
                    //cellMeshes[i].drawFaces();
                    //sound[i].setVolume( 0 );
                }
            }
            
            if(k==1)
            {
            coordX = ofMap(MOUSEHand2x, 0, ofGetWidth(), -ofGetWidth()/2, ofGetWidth()/2);
            coordY = ofMap(MOUSEHand2y, 0, ofGetHeight(), -ofGetHeight()/2, ofGetHeight()/2);
                
                if((coordX > (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                    coordX < (cos(anguloRad)*cellCentroids[i].x - sin(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2 &&
                    coordY > (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) - cellRadius[i]/2 &&
                    coordY < (sin(anguloRad)*cellCentroids[i].x + cos(anguloRad)*cellCentroids[i].y) + cellRadius[i]/2))
                {
                    
                    ofSetColor(255);
                    cellMeshes[i].drawFaces();
                    //Reproducimos los archivos
                    
                    if(sound[i].getIsPlaying())
                    {
                        if(audio ==1)play[i]=0;
                    }else
                    {
                        if(audio ==1)play[i]=1;
                    }
                    
                    if(play[i] == 1)
                    {
                        if(audio ==1)play[i]=0;
                        if(audio ==1)sound[i].play();
                    }
                }else
                {
                    
                    ofSetColor(colores[i]);
                    cellMeshes[i].drawFaces();
                    //sound[i].setVolume( 0 );
                }

            }
            
            ofPushStyle();
            ofSetLineWidth(5);
            ofSetColor(0);
            cellMeshWireframes[i].draw();
            ofFill();
            ofCircle(cellCentroids[i].x, cellCentroids[i].y, 40, 10);
            ofPopStyle();
        }
}
        glDisable(GL_DEPTH_TEST);
        ofDisableLighting();
        light.disable();
        //cam.end();
        ofPopMatrix();
        
        ofPushStyle();
        ofFill();
        ofSetColor(255);
        ofCircle(MOUSEHand1x, MOUSEHand1y, 50, 10);
        ofCircle(MOUSEHand2x, MOUSEHand2y, 50, 10);
        ofPopStyle();
            ofPopStyle();
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
        }
    }

    
}
    
    if(GESTOS==1 && usuario==1)
    {
    
        imagenGestos.draw(ofGetWidth()/2, ofGetHeight()- imagenGestos.height/2);
    
    }
    
    if(usuario == 0)
    {
        ofSetColor(255,0,0);
        ofDrawBitmapString("ESPERANDO USUARIO", ofPoint(ofGetWidth()/2-80, 0+20));
    
    }else
    {
        ofSetColor(0,255,0);
        ofDrawBitmapString("USUARIO ACTIVO", ofPoint(ofGetWidth()/2-80, 0+20));
    
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    if(key == '0')
    {
        escena = 0;
    }
    
    if(key == '1')
    {
        escena = 1;
    }
    
    if(key == '2')
    {
        escena = 2;
    }
    if(key == '3')
    {
        escena = 3;
    }
    
    if(key == 'i')
    {
        mano1 = 1;
    }
    
    if(key == 'd')
    {
        mano2 = 1;
    }

    if(key == 'g')
    {
      GESTOS = !GESTOS;
    }
    
    if(key == 'u')
    {
        usuario = !usuario;
    }
    
    
    
   }

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
    /*
    if(key == 'i')
    {
        mano1 = 0;
    }
    
    if(key == 'd')
    {
        mano2 = 0;
    }

    
    ofxOscMessage i;
	i.setAddress("/mano1");
	i.addIntArg(mano1);
	sender.sendMessage(i);
    
    ofxOscMessage d;
	d.setAddress("/mano2");
	d.addIntArg(mano2);
	sender.sendMessage(d);
*/

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
   
    if(mano1)
    {
    MOUSEHand1x = x;
    MOUSEHand1y = y;
        
    MOUSEHand2x = x -100;
    MOUSEHand2y = y -100;
    }
    /*
    ofxOscMessage mx;
	mx.setAddress("/Xmano1");
	mx.addFloatArg(ofMap(MOUSEHand1x, 0, ofGetWidth(), 0, 1));
	sender.sendMessage(mx);
    
    ofxOscMessage my;
	my.setAddress("/Ymano1");
	my.addFloatArg(ofMap(MOUSEHand1y, 0, ofGetHeight(), 0, 1));
	sender.sendMessage(my);
    */
    /*
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        if(mano1)
        {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        int time = ofGetElapsedTimef();
       
        
        if(time%2==0)
        {
            shape.addVertex(MOUSEHand1x, MOUSEHand1y);
            
            //shape.addVertex(x, y);
        }else
        {
            if(breakupIntoTriangles) {
                
                // This is the manual way to triangulate the shape
                // you can then add many little triangles
                
                // first simplify the shape
                //shape.simplify();
                
                // save the outline of the shape
                ofPolyline outline = shape;
                
                // resample shape
                ofPolyline resampled = shape;
                
                // trangleate the shape, return am array of traingles
                vector <TriangleShape> tris = triangulatePolygonWithOutline(resampled, outline);
                
                
                
                // add some random points inside
                addRandomPointsInside(shape, 300);
                
                // now loop through all the trainles and make a box2d triangle
                for (int i=0; i<tris.size(); i++) {
                    
                    ofPtr<ofxBox2dPolygon> triangle = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    triangle.get()->addTriangle(tris[i].a, tris[i].b, tris[i].c);
                    triangle.get()->setPhysics(ofRandom(0,10), ofRandom(0,2), ofRandom(0,2));
                    
                    //triangle.get()->setPhysics(ofRandom(-0.2,10), ofRandom(-0.2,10), ofRandom(-0.2,10));
                    triangle.get()->create(box2d.getWorld());
                
                    
                    triangle.get()->setData(new SoundData());
                    SoundData * sd = (SoundData*)triangle.get()->getData();
                    sd->soundID = ofRandom(0, N_SOUNDS);
                    sd->bHit	= false;
                    
                    
                    polyShapes.push_back(triangle);
                    
                   
                    
                }
               
                
            }
            else {
                
                // create a poly shape with the max verts allowed
                // and the get just the convex hull from the shape
                shape = shape.getResampledByCount(b2_maxPolygonVertices);
                shape = getConvexHull(shape);
                
                ofPtr<ofxBox2dPolygon> poly = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                poly.get()->addVertices(shape.getVertices());
                poly.get()->setPhysics(1.0, 0.3, 0.3);
                poly.get()->create(box2d.getWorld());
                polyShapes.push_back(poly);
                
            }
            
            // done with shape clear it now
            shape.clear();
            }
        }

        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        particlePosition.x  = MOUSEHand1x;
        particlePosition.y  = MOUSEHand1y;
        
        for (int i = 0; i < 50; i++) {
            ofVec3f position = particlePosition;
            ofVec3f velocity = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
            ofColor color;
            color = ofColor(ofRandom(0,255), 0,  ofRandom(0,255), ofRandom(0,255));
            
            // add a particle
            vboPartciles->pointSize = 100;
            
            vboPartciles->addParticle(position, velocity, color);
        
        }
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
*/
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    if(escena == 0)
    {
        ////////////////////////////////////////////////////
        ///////////////////////FLUID////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 2)
    {
        ////////////////////////////////////////////////////
        ///////////////////PARTICULAS///////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }
    else if(escena == 3)
    {
        ////////////////////////////////////////////////////
        /////////////////////VORO///////////////////////////
        
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
    }

}



//--------------------------------------------------------------
void testApp::Generacion(float x, float y){

 
    if(mano1)
    {
        ////////////////////////////////////////////////////
        ///////////////////////BOX2D////////////////////////
        int time = ofGetElapsedTimef();
        
        
        if(time%2==0)
        {
            shape.addVertex(x, y);
    
        }else
        {
            
            if(breakupIntoTriangles) {
                
                // This is the manual way to triangulate the shape
                // you can then add many little triangles
                
                // first simplify the shape
                shape.simplify();
                
                // save the outline of the shape
                ofPolyline outline = shape;
                
                // resample shape
                ofPolyline resampled = shape;
                
                
                // trangleate the shape, return am array of traingles
                vector <TriangleShape> tris = triangulatePolygonWithOutline(resampled, outline);
                
                
                
                // add some random points inside
                addRandomPointsInside(shape, 300);
                
                // now loop through all the trainles and make a box2d triangle
                for (int i=0; i<tris.size(); i++) {
                    
                    ofPtr<ofxBox2dPolygon> triangle = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
                    triangle.get()->addTriangle(tris[i].a, tris[i].b, tris[i].c);
                    triangle.get()->setPhysics(ofRandom(0,10), ofRandom(0,2), ofRandom(0,2));
                    
                    //triangle.get()->setPhysics(ofRandom(-0.2,10), ofRandom(-0.2,10), ofRandom(-0.2,10));
                    triangle.get()->create(box2d.getWorld());
                    
                    
                    triangle.get()->setData(new SoundData());
                    SoundData * sd = (SoundData*)triangle.get()->getData();
                    sd->soundID = ofRandom(0, N_SOUNDS);
                    sd->bHit	= false;
                    
                    polyShapes.push_back(triangle);
                    
                }
                
            }
            
            // done with shape clear it now
            shape.clear();
            
        }
    }

        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
}


void testApp::GeneracionParticulas(float x, float y, ofVec3f partPos, ofxVboParticles *vboPart)
{
    ////////////////////////////////////////////////////
    ///////////////////PARTICULAS///////////////////////
    partPos.x  = x;
    partPos.y  = y;
    
    for (int i = 0; i < 50; i++) {
        ofVec3f position = partPos;
        ofVec3f velocity = ofVec3f(ofRandom(-2, 2), ofRandom(-2, 2), ofRandom(-2, 2));
        ofColor color;
        
        color = ofColor(ofMap(x, 0, ofGetWidth(), 0, 255), 0,  ofMap(y, 0, ofGetHeight(), 0,255), ofRandom(0,255));
        
        // add a particle
        vboPart->pointSize = 100;
        
        vboPart->addParticle(position, velocity, color);
        
    }
}

