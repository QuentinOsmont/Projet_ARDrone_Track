import java.io.IOException;

import com.sun.speech.freetts.Voice;
import com.sun.speech.freetts.VoiceManager;


public class FlightNavigator {
	
	int prevPosX, prevPosY;
	
	boolean isSearching = false;
	
	boolean isTracking = false;
	
	long timeToSearch;
	
	float yawMove = 0;
	float pitchMove = 0;
	
	boolean isMovingForward = false;
	
	float yawTrackingP = 0.75f;
	
	float pitchTrackingP = 0.42f;
	float pitchTrackingI = 0.0011f;
	
	float pitchE = 0;
	
	public void navigateRotationBasedOnRedDot(KontrolniStolp1 stolp, int posX, int posY, int w, int h){
		
		if(posX>0&&posY>0){
			
			int XposRelToCenter = (w/2)-posX;
			yawMove = (yawTrackingP * ((float)(XposRelToCenter) / 280.0f));
			

			int YposRelToCenter = (h/2)-posY;
			pitchE = pitchE + ((float)(YposRelToCenter) / 200.0f);
			pitchMove = (pitchTrackingP * ((float)(YposRelToCenter) / 200.0f) )  + (pitchTrackingI*pitchE);
			
		}else{
			pitchMove = 0;
			yawMove = 0;
		}
		
		//if(pitchMove!=0&&yawMove!=0){
		//	System.out.println("Random: " +  ((Math.abs(pitchMove*2))*yawMove)*10  );
		//}

		try {
			stolp.letalnik.move(0,pitchMove,0, yawMove);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		//System.out.println("Pitch: " + pitchMove);
		
		prevPosX = posX;
		prevPosY = posY;
		
		
	}

}
