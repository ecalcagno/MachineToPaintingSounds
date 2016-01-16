import SimpleOpenNI.*;
import signal.library.*;
import de.voidplus.dollar.*;
import oscP5.*;
import netP5.*;

SimpleOpenNI kinect;
NetAddress kinectOSC;
OscP5 oscP5;
SignalFilter[] filtros;
OneDollar[] gestos;

int circulo[] = new int[] {
  127, 141, 124, 140, 120, 139, 118, 139, 116, 139, 111, 140, 109, 141, 104, 144, 100, 147, 96, 152, 93, 157, 90, 163, 87, 169, 85, 175, 83, 181, 82, 190, 82, 195, 83, 200, 84, 205, 88, 213, 91, 216, 96, 219, 103, 222, 108, 224, 111, 224, 120, 224, 133, 223, 142, 222, 152, 218, 160, 214, 167, 210, 173, 204, 178, 198, 179, 196, 182, 188, 182, 177, 178, 167, 170, 150, 163, 138, 152, 130, 143, 129, 140, 131, 129, 136, 126, 139
};
int triangulo[] = new int[] {
  137, 139, 135, 141, 133, 144, 132, 146, 130, 149, 128, 151, 126, 155, 123, 160, 120, 166, 116, 171, 112, 177, 107, 183, 102, 188, 100, 191, 95, 195, 90, 199, 86, 203, 82, 206, 80, 209, 75, 213, 73, 213, 70, 216, 67, 219, 64, 221, 61, 223, 60, 225, 62, 226, 65, 225, 67, 226, 74, 226, 77, 227, 85, 229, 91, 230, 99, 231, 108, 232, 116, 233, 125, 233, 134, 234, 145, 233, 153, 232, 160, 233, 170, 234, 177, 235, 179, 236, 186, 237, 193, 238, 198, 239, 200, 237, 202, 239, 204, 238, 206, 234, 205, 230, 202, 222, 197, 216, 192, 207, 186, 198, 179, 189, 174, 183, 170, 178, 164, 171, 161, 168, 154, 160, 148, 155, 143, 150, 138, 148, 136, 148
};
int pigtail[] = new int[] {
  81, 219, 84, 218, 86, 220, 88, 220, 90, 220, 92, 219, 95, 220, 97, 219, 99, 220, 102, 218, 105, 217, 107, 216, 110, 216, 113, 214, 116, 212, 118, 210, 121, 208, 124, 205, 126, 202, 129, 199, 132, 196, 136, 191, 139, 187, 142, 182, 144, 179, 146, 174, 148, 170, 149, 168, 151, 162, 152, 160, 152, 157, 152, 155, 152, 151, 152, 149, 152, 146, 149, 142, 148, 139, 145, 137, 141, 135, 139, 135, 134, 136, 130, 140, 128, 142, 126, 145, 122, 150, 119, 158, 117, 163, 115, 170, 114, 175, 117, 184, 120, 190, 125, 199, 129, 203, 133, 208, 138, 213, 145, 215, 155, 218, 164, 219, 166, 219, 177, 219, 182, 218, 192, 216, 196, 213, 199, 212, 201, 211
};
int x[] = new int[] {
  87, 142, 89, 145, 91, 148, 93, 151, 96, 155, 98, 157, 100, 160, 102, 162, 106, 167, 108, 169, 110, 171, 115, 177, 119, 183, 123, 189, 127, 193, 129, 196, 133, 200, 137, 206, 140, 209, 143, 212, 146, 215, 151, 220, 153, 222, 155, 223, 157, 225, 158, 223, 157, 218, 155, 211, 154, 208, 152, 200, 150, 189, 148, 179, 147, 170, 147, 158, 147, 148, 147, 141, 147, 136, 144, 135, 142, 137, 140, 139, 135, 145, 131, 152, 124, 163, 116, 177, 108, 191, 100, 206, 94, 217, 91, 222, 89, 225, 87, 226, 87, 224
};

PVector[] articulaciones;
PVector centro;
float[] ratio;
float umbralPresencia;
float distancia;
float umbralEstado;
int maxUsuarios, estadoActual;
boolean[] estado;
boolean display;
color colorF;


void setup() {

  size(640, 480, P3D);
  frameRate(30);

  kinect = new SimpleOpenNI(this);
  kinect.enableDepth();
  kinect.enableUser();

  maxUsuarios = 1;
  estadoActual = 0;

  oscP5 = new OscP5(this, 12001);
  kinectOSC = new NetAddress("127.0.0.1", 12000);

  filtros = new SignalFilter[2*maxUsuarios];
  gestos = new OneDollar[2*maxUsuarios];

  articulaciones = new PVector[6*maxUsuarios];
  ratio = new float[2*maxUsuarios];
  estado = new boolean[2*maxUsuarios];
  colorF = color(255);

  centro = new PVector(-13.629661, -70.683754, 1000.0);
  umbralEstado = 0.75;
  umbralPresencia = 1000;

  for (int i = 0; i<maxUsuarios*2; i++) {
    gestos[i] = new OneDollar(this);
    gestos[i].learn("1", circulo);
    gestos[i].learn("2", pigtail);
    gestos[i].learn("3", triangulo);
    gestos[i].learn("4", x);
    gestos[i].bind("1 2 3 4", "detected");
    gestos[i].setMinSimilarity(85);
    gestos[i].setMinDistance(100).enableMinDistance();
    gestos[i].setMaxTime(1500).enableMaxTime();

    filtros[i] = new SignalFilter(this, 3);
    filtros[i].setFrequency(30);
    filtros[i].setMinCutoff(0.01);
    filtros[i].setBeta(8);
    filtros[i].setDerivateCutoff(1);
  }
}

void draw() {
  kinect.update();
  OscBundle myBundle = new OscBundle();

  if (display) image(kinect.userImage(), 0, 0);
  else background(colorF);

  int[] usuarios = kinect.getUsers();
  float distComC = umbralPresencia;
  int usuarioActivo = 0;
  boolean hayUsuario = false;


  //modificar para multiUsuario
  for (int i = 0; i < usuarios.length; i++) {
    PVector com = new PVector();
    if (kinect.isTrackingSkeleton(usuarios[i])) {
      kinect.getCoM(usuarios[i], com);
      //calcular a mano
      float distComCActual = com.dist(centro);
      if (com.dist(centro) < umbralPresencia) {
        if (distComCActual < distComC) {
          distComC = distComCActual;
          usuarioActivo = usuarios[i];
          hayUsuario = true;
        }
      }
    }
  }


  OscMessage myMessage = new OscMessage("/usuario");
  myMessage.add(hayUsuario);
  myBundle.add(myMessage);
  myMessage.clear();

  if (hayUsuario) {
    articulaciones(usuarioActivo, ratio);
    for (int i = 0; i<estado.length; i++) {
      if (!estado[i] && ratio[i] > umbralEstado) estado[i] = true;
      else if (estado[i] && ratio[i] < umbralEstado * 0.6) estado[i] = false;

      PVector mano = new PVector();
      kinect.convertRealWorldToProjective(articulaciones[3*i], mano);
      PVector filtrado = filtros[i].filterCoord2D(mano, 640, 480);
      filtrado.x = abs(640-filtrado.x);

      myMessage.setAddrPattern("estado"+str(i));
      myMessage.add(estado[i]);
      myBundle.add(myMessage);
      myMessage.clear();
      myMessage.setAddrPattern("pos"+str(i));
      myMessage.add(new float[] {
        filtrado.x/640, filtrado.y/480
      }
      );
      myBundle.add(myMessage);
      myMessage.clear();

      if (estado[i]) {
        ellipse(filtrado.x, filtrado.y, 20, 20);
      } else {
        gestos[i].track(filtrado.x, filtrado.y);
        gestos[i].draw();
      }
    }
  }

  myMessage.setAddrPattern("gesto");
  myMessage.add(estadoActual);
  myBundle.add(myMessage);
  myMessage.clear();
  oscP5.send(myBundle, kinectOSC);
  
  String textoUsuario;
  if(hayUsuario) textoUsuario = "Usuario Activo";
  else textoUsuario = "Esperando usuario";
  fill(0);
  textSize(17);
  text(textoUsuario,20,20);
}

void articulaciones(int usuario, float[] ratioLargos) {
  for (int i = 0; i< articulaciones.length; i++) {
    articulaciones[i] = new PVector();
  }
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_LEFT_HAND, articulaciones[0]);
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_LEFT_ELBOW, articulaciones[1]);
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_LEFT_SHOULDER, articulaciones[2]);
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_RIGHT_HAND, articulaciones[3]);
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_RIGHT_ELBOW, articulaciones[4]);
  kinect.getJointPositionSkeleton(usuario, SimpleOpenNI.SKEL_RIGHT_SHOULDER, articulaciones[5]);

  //calcular distancias a mano
  for (int i = 0; i<2; i++) {
    float largoAr = articulaciones[0+i*3].dist(articulaciones[1+i*3])+articulaciones[1+i*3].dist(articulaciones[2+i*3]);
    float ratioAr = abs(articulaciones[0+i*3].z-articulaciones[2+i*3].z)/largoAr;
    ratioLargos[i] = ratioAr;
  }
  //usar angulo para control en lugar de projective
}

void onNewUser(SimpleOpenNI curContext, int userId) {
  curContext.startTrackingSkeleton(userId);
  println(millis()/1000);
}

void onLostUser(SimpleOpenNI curContext, int userId) {
  println(millis()/1000);
}

void onVisibleUser(SimpleOpenNI curContext, int userId) {
}

void detected(String gesture, float percent, int startX, int startY, int centroidX, int centroidY, int endX, int endY) {
  
  estadoActual = int(gesture);
  
  switch(int(estadoActual)) {
  case 1: 
    colorF = color(255, 255, 255);
    break;
  case 2: 
    colorF = color(255, 100, 100);
    break;
  case 3: 
    colorF = color(100, 255, 100);
    break;
  case 4: 
    colorF = color(100, 100, 255);
    break;
  }
}

void keyPressed() {
  display = !display;
}

