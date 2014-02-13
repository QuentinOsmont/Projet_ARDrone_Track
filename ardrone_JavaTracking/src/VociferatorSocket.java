import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import com.codeminders.ardrone.ARDrone.Animation;


public class VociferatorSocket {
	
	KontrolniStolp1 stolp = null;
	
	public VociferatorSocket(KontrolniStolp1 stolp) {
		this.stolp = stolp;
	}
	
	public void init(final int port){
		Thread t = new Thread(){
			
			public void run(){
				try{
					String clientSentence;                  
					ServerSocket welcomeSocket = new ServerSocket(port);
					Socket connectionSocket = welcomeSocket.accept();             
					BufferedReader inFromClient =  new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
					while(( clientSentence = inFromClient.readLine()  )!=null)          
					{                  
						System.out.println("Received: " + clientSentence);
						usePackage(clientSentence);
					}
					inFromClient.close();
					connectionSocket.close();
					welcomeSocket.close();
					//System.out.println("zaprl!");
				}catch(Exception e){}
			}
		};
		
		t.start();
		
	}
	
	public void usePackage(String pkg) throws IOException{
		
		if(pkg.contains("rotiraj levo")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(0, 0, 0, -0.7f);
		}
		else if(pkg.contains("rotiraj desno")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(0, 0, 0, 0.7f);
		}
		else if(pkg.contains("levo")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(-0.35f, 0, 0, 0);
		}
		else if(pkg.contains("desno")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(0.35f, 0, 0, 0);
		}
		else if(pkg.contains("naprej")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(0, -0.4f, 0, 0);
		}
		else if(pkg.contains("nazaj")&&stolp.flightMode.equals("audio")){
			stolp.letalnik.move(0, 0.4f, 0, 0);
		}
		
		else if(pkg.contains("vzleti")){
			stolp.changeMode("toff");
		}
		else if(pkg.contains("pristani")){
			stolp.changeMode("land");
		}
		else if(pkg.contains("lebdi")){
			stolp.changeMode("hover");
		}
		
		else if(pkg.contains("tipkovnica")){
			stolp.changeMode("keyboard");
		}
		else if(pkg.contains("glasovno")){
			stolp.changeMode("audio");
		}
		else if(pkg.contains("vizualno")){
			stolp.changeMode("visual");
		}
		
		else if(pkg.contains("skeniraj")){
			stolp.scan();
		}
		
		else if(pkg.contains("mayday")){
			try {
				stolp.letalnik.playAnimation(Animation.WAVE, 6000);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		else if(pkg.contains("gaming")){
			try {
				stolp.letalnik.playAnimation(Animation.TURNAROUND, 5000);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		else if(pkg.contains("gaming")){
			try {
				stolp.letalnik.playAnimation(Animation.PHI_THETA_MIXED , 3000);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		
		
		
		
		
		
	}

}
