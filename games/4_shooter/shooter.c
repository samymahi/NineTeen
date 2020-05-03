#include "../../include/communFunctions.h"
#include "../../define/define.h"
#include "../../include/hashage.h"
#include "../../include/libWeb.h"
#include "config.h"
#include <time.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include "../../include/SDL2/SDL2_gfxPrimitives.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Ellipse-circle collision detection
 * http://yehar.com/blog/?p=2926
 * by Olli Niemitalo in 2012-08-06.
 * This work is placed in the public domain.
 */
//hitbox
#define maxIterations 5
double innerPolygonCoef[maxIterations+1];
double outerPolygonCoef[maxIterations+1];

int iterate(double x, double y, double c0x, double c0y, double c2x, double c2y, double rr) {
    for (int t = 1; t <= maxIterations; t++) {
      double c1x = (c0x + c2x)*innerPolygonCoef[t];
      double c1y = (c0y + c2y)*innerPolygonCoef[t];
      double tx = x - c1x;
      double ty = y - c1y;
      if (tx*tx + ty*ty <= rr) {
	return SDL_TRUE;
      }
      double t2x = c2x - c1x;
      double t2y = c2y - c1y;
      if (tx*t2x + ty*t2y >= 0 && tx*t2x + ty*t2y <= t2x*t2x + t2y*t2y &&
	  (ty*t2x - tx*t2y >= 0 || rr*(t2x*t2x + t2y*t2y) >= (ty*t2x - tx*t2y)*(ty*t2x - tx*t2y))) {
	return SDL_TRUE;
      }
      double t0x = c0x - c1x;
      double t0y = c0y - c1y;
      if (tx*t0x + ty*t0y >= 0 && tx*t0x + ty*t0y <= t0x*t0x + t0y*t0y &&
	  (ty*t0x - tx*t0y <= 0 || rr*(t0x*t0x + t0y*t0y) >= (ty*t0x - tx*t0y)*(ty*t0x - tx*t0y))) {
	return SDL_TRUE;
      }
      double c3x = (c0x + c1x)*outerPolygonCoef[t];
      double c3y = (c0y + c1y)*outerPolygonCoef[t];
      if ((c3x-x)*(c3x-x) + (c3y-y)*(c3y-y) < rr) {
	c2x = c1x;
	c2y = c1y;
	continue;
      }
      double c4x = c1x - c3x + c1x;
      double c4y = c1y - c3y + c1y;
      if ((c4x-x)*(c4x-x) + (c4y-y)*(c4y-y) < rr) {
	c0x = c1x;
	c0y = c1y;
	continue;
      }
      double t3x = c3x - c1x;
      double t3y = c3y - c1y;
      if (ty*t3x - tx*t3y <= 0 || rr*(t3x*t3x + t3y*t3y) > (ty*t3x - tx*t3y)*(ty*t3x - tx*t3y)) {
	if (tx*t3x + ty*t3y > 0) {
	  if (fabs(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c3x)*(c0x-c3x) + (y-c3y)*(c0y-c3y) >= 0) {
	    c2x = c1x;
	    c2y = c1y;
	    continue;
	  }
	} else if (-(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c4x)*(c2x-c4x) + (y-c4y)*(c2y-c4y) >= 0) {
	  c0x = c1x;
	  c0y = c1y;
	  continue;
	}
      }
      return SDL_FALSE;
    }
    return SDL_FALSE; // Out of iterations so it is unsure if there was a collision. But have to return something.
}

  // Test for collision between an ellipse of horizontal radius w and vertical radius h at (x0, y0) and
  // a circle of radius r at (x1, y1)
int collide(double x0, double y0, double w, double h, double x1, double y1, double r) {

    double x = fabs(x1 - x0);
    double y = fabs(y1 - y0);

    if (x*x + (h - y)*(h - y) <= r*r || (w - x)*(w - x) + y*y <= r*r || x*h + y*w <= w*h
	|| ((x*h + y*w - w*h)*(x*h + y*w - w*h) <= r*r*(w*w + h*h) && x*w - y*h >= -h*h && x*w - y*h <= w*w)) {
    	return SDL_TRUE;
    }
	else {
		if ((x-w)*(x-w) + (y-h)*(y-h) <= r*r || (x <= w && y - r <= h) || (y <= h && x - r <= w)) {
		  return iterate(x, y, w, 0, 0, h, r*r);
	  	}
      	return SDL_FALSE;
    }
}




extern int updateEnded;
int shooter( SDL_Renderer *renderer ,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures){

// // // // // // //
// MISE EN PLACE   //``
// // // // // // //
	//myInit();
	for (int t = 0; t <= maxIterations; t++) {
		int numNodes = 4 << t;
		innerPolygonCoef[t] = 0.5/cos(4*acos(0.0)/numNodes);
		outerPolygonCoef[t] = 0.5/(cos(2*acos(0.0)/numNodes)*cos(2*acos(0.0)/numNodes));
	}

	float ratioWindowSize = (float)WinWidth/BASE_WINDOW_W;
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
	SDL_RenderClear(renderer);

	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;



	//Fonts

	//audio

	int quit = SDL_FALSE;
	while(!quit){

   		 // // // //
		// Vars   //`
		 // // // //

		int gameOver = SDL_FALSE;

		//score
		ScoreTotal score = {0,0,0};

		//hash setup
		long keys[4];
		initialisationConstantHashage(keys);
		long long score_hash = hashage(score.score, keys);

		//Keyboard
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		//sound

		//Time
		unsigned int lastTime = 0, currentTime;

		//mouse
		SDL_Point mouseCoor;

		//vaiss
		int x,y;
		int form=0;

		 // // // // // // // //
		//   Initialize vars   //
		 // // // // // // // //

	 // // // // // // //
	//   BOUCLE DU JEU  //``
	 // // // // // // //

		while( 1 ){

		// // // //
		// Events //`
		// // // //
			SDL_Event event;
			while( SDL_PollEvent(&event) ){
				switch( event.type ){
					case SDL_QUIT:
						// fermer
						return 0;
						break;
				}
			}


		 // // // // // // // // //
		// Handle Keyboard inputs //`
		 // // // // // // // // //
			SDL_PumpEvents();

			if( keystate[SDL_SCANCODE_ESCAPE] ){
				return 0;
			}

			if( keystate[SDL_SCANCODE_UP] ){
				y-=3;
			}

			if( keystate[SDL_SCANCODE_DOWN] ){
				y+=3;
			}
			if( keystate[SDL_SCANCODE_LEFT] ){
				x-=3;
			}
			if( keystate[SDL_SCANCODE_RIGHT] ){
				x+=3;
			}


		// // // // //
		// Gameplay //`
		// // // // //


		 // // //
		// Draw //`
		 // // //
		 	SDL_Rect circle = {200,200,200,200};
			SDL_SetRenderDrawColor(renderer,255,255,255,255);

			SDL_Rect srcShip = SHIP_SRC;
			srcShip.x += form * srcShip.w;

			SDL_Rect srcFlame = SHIP_SRC;


			SDL_Rect dest = SHIP_SRC;
			dest.x+=x;
			dest.y+=y;

			SDL_Rect hitbox = SHIP_HITBOX[form];
			hitbox.x+=x;
			hitbox.y+=y;

			if(collide( hitbox.x + hitbox.w/2, hitbox.y + hitbox.h/2, hitbox.w/2 ,hitbox.h/2,
						circle.x + circle.w/2, circle.y + circle.h/2,circle.h/2)){
				SDL_SetTextureColorMod(textures[SH_SHIP],255,0,0);
			}
			else{
				SDL_SetTextureColorMod(textures[SH_SHIP],255,255,255);
			}


			for(int i=0; i<NB_MAX_WEAPON; i++){
				SDL_RenderCopy(renderer, textures[SH_FLAME], &srcFlame, &dest);
				srcFlame.x+= srcFlame.w;
			}

			SDL_Rect destWeapon = WEAPON_DEST[0];
			for(int i=0; i<NB_MAX_WEAPON; i++){
				destWeapon.x = dest.x + WEAPON_DEST[i].x;
				destWeapon.y = dest.y + WEAPON_DEST[i].y;
				SDL_RenderCopy(renderer, textures[SH_WEAPON], &WEAPON_SRC, &destWeapon);
			}


			SDL_RenderCopy(renderer, textures[SH_SHIP], &srcShip, &dest);


			ellipseColor (renderer, circle.x + circle.w/2, circle.y + circle.h/2, circle.w/2, circle.h/2, 0xFFFFFFFF);

			SDL_RenderPresent(renderer);



		 // // // // //
		// Next frame //`
		 // // // // //

			//regulateFPS
			currentTime = SDL_GetTicks();
			while( currentTime - lastTime < 1000./FRAMES_PER_SECOND )
				currentTime = SDL_GetTicks();

			lastTime = currentTime;

			// On efface
			SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
			SDL_RenderClear(renderer);

		}

	}
	return 0;
}