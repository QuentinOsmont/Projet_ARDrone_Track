import java.awt.Color;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.UnknownHostException;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import javax.swing.JToolBar;
import javax.swing.text.ChangedCharSetException;

import com.codeminders.ardrone.ARDrone;
import com.codeminders.ardrone.ARDrone.Animation;
import com.codeminders.ardrone.ARDrone.VideoChannel;
import com.codeminders.ardrone.DroneVideoListener;
import com.codeminders.ardrone.NavData;
import com.codeminders.ardrone.NavDataListener;
import com.codeminders.ardrone.util.BufferedImageVideoListener;
import com.codeminders.ardrone.video.BufferedVideoImage;
import com.google.zxing.BinaryBitmap;
import com.google.zxing.LuminanceSource;
import com.google.zxing.ReaderException;
import com.google.zxing.Result;
import com.google.zxing.client.j2se.BufferedImageLuminanceSource;
import com.google.zxing.common.HybridBinarizer;
import com.google.zxing.qrcode.QRCodeReader;
import com.sun.speech.freetts.Voice;
import com.sun.speech.freetts.VoiceManager;


public class KontrolniStolp1 {
	
	ARDrone letalnik = null;
	
	JLabel video = null;
	
	boolean isTakenOff = true;
	
	String flightMode = "hover";
	
	int posX = 0;
	int posY = 0;
	
	JRadioButton lebdenje = new JRadioButton("Décollage");
	JRadioButton tipkovnica = new JRadioButton("Clavier");
	JRadioButton visual = new JRadioButton("Visuel");
	JRadioButton glasovno = new JRadioButton("Voix");
	
	NavData letalnikData = null;
	
	float roll = 0;
	
	int batt = 0;
	
	BufferedImage slika = null;
	
	//PORT: 12345
	
	public KontrolniStolp1() {
	
		initComponentsForGUI();
		
		VociferatorSocket socket = new VociferatorSocket(this);
		
		
		try {
		
		
			letalnik = new ARDrone();
			letalnik.connect();
			letalnik.enableAutomaticVideoBitrate();
			letalnik.trim();
			
			final RedCVTracker cot = new RedCVTracker(this);
			
			letalnik.addImageListener(new BufferedImageVideoListener() {
				
				@Override
				public void imageReceived(BufferedImage image) {
					video.setIcon(new ImageIcon(ImageModificator.scaleImage(image, 960, 720, Color.black, (int)((float)posX*1.5f), (int)((float)posY*1.5f), roll, batt)));
					slika = ImageModificator.scaleImage3(image, 640, 480, Color.black);
					cot.im = ImageModificator.scaleImage2(image, 640, 480, Color.black);
					try {
						letalnik.sendAllNavigationData();
						//letalnik.selectVideoChannel(VideoChannel.HORIZONTAL_ONLY);
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			});
			
			letalnik.addNavDataListener(new NavDataListener() {
				
				@Override
				public void navDataReceived(NavData data) {
					roll = data.getRoll();
					batt = data.getBattery();
					//System.out.println(roll);
				}
			});
			
			Thread th = new Thread(cot);
	        th.start();
			
	        
	        
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		changeMode("hover");
		
		socket.init(12345);
		
		try {
			Desktop.getDesktop().open(new File("C:/vociferator/Vociferator.exe"));
		} catch (IOException e) {
			e.printStackTrace();
		}
		
	}
	
	//hover, visual, audio, keyboard, land, toff
	public void changeMode(String flightMode){
		try {
		this.flightMode = flightMode;
		if(flightMode.equals("hover")){	
				letalnik.hover();
				lebdenje.setSelected(true);
		}else if(flightMode.equals("toff")){
			letalnik.takeOff();
			isTakenOff = true;
		}else if(flightMode.equals("land")){
			letalnik.land();
			isTakenOff = false;
		}else if(flightMode.equals("visual")){
			visual.setSelected(true);
		}else if(flightMode.equals("audio")){
			glasovno.setSelected(true);
		}else if(flightMode.equals("keyboard")){
			tipkovnica.setSelected(true);
		}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void changeMode2(String flightMode){
		try {
		this.flightMode = flightMode;
		if(flightMode.equals("hover")){	
				letalnik.hover();
		}else if(flightMode.equals("toff")){
			letalnik.takeOff();
			isTakenOff = true;
		}else if(flightMode.equals("land")){
			letalnik.land();
			isTakenOff = false;
		}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void initComponentsForGUI(){
		JFrame okno = new JFrame("iBird Kontrolni Tavr");
		okno.setSize(960, 770);
		okno.setResizable(false);
		
		
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		okno.setLocation((dim.width/2-okno.getSize().width/2 ), dim.height/2-okno.getSize().height/2 - 20);
		
		//okno.setExtendedState(JFrame.MAXIMIZED_VERT);
		
		video = new JLabel();
		
		okno.add("Center",video);
		
		JToolBar bar = new JToolBar();
		
		JButton takeOff = new JButton("Décollage");
		takeOff.setFocusable(false);
		takeOff.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					letalnik.takeOff();
					//Thread.sleep(5000);
					isTakenOff = true;
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		
		bar.add(takeOff);
		
		JButton takeOff22 = new JButton("Atterissage");
		takeOff22.setFocusable(false);
		takeOff22.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					letalnik.land();
					isTakenOff = false;
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		
		bar.add(takeOff22);
		
		JButton takeOff222 = new JButton("Balayer QR");
		takeOff222.setFocusable(false);
		takeOff222.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					scan();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		
		bar.add(takeOff222);
		
		JButton takeOff2222 = new JButton("Animer");
		takeOff2222.setFocusable(false);
		takeOff2222.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					letalnik.playAnimation(Animation.PHI_THETA_MIXED , 3000);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		
		bar.add(takeOff2222);
		
		
		bar.add(lebdenje);
		bar.add(visual);
		bar.add(glasovno);
		bar.add(tipkovnica);
		
		ButtonGroup bg = new ButtonGroup();
		bg.add(lebdenje);
		bg.add(visual);
		bg.add(glasovno);
		bg.add(tipkovnica);
		
		lebdenje.setFocusable(false);
		visual.setFocusable(false);
		glasovno.setFocusable(false);
		tipkovnica.setFocusable(false);
		
		
		lebdenje.setSelected(true);
		
		lebdenje.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if(lebdenje.isSelected()){
					changeMode2("hover");
				}
			}
		});
		
		visual.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if(visual.isSelected()){
					changeMode2("visual");
				}
			}
		});

		glasovno.addActionListener(new ActionListener() {
	
	@Override
	public void actionPerformed(ActionEvent arg0) {
		if(glasovno.isSelected()){
			changeMode2("visual");
		}
	}
	});

		tipkovnica.addActionListener(new ActionListener() {
	
	@Override
	public void actionPerformed(ActionEvent arg0) {
		if(tipkovnica.isSelected()){
			changeMode2("keyboard");
		}
	}
	});
		
		
		
		okno.add("North", bar);
		
		JTextArea ar = new JTextArea();
		
		ar.setPreferredSize(new Dimension(40, 40));
		
		okno.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//okno.add("East", ar);
		
		okno.setFocusable(true);
		okno.setFocusableWindowState(true);
		
		okno.addKeyListener(new KeyListener() {
			
			@Override
			public void keyTyped(KeyEvent ke) {
				
				
			}
			
			@Override
			public void keyReleased(KeyEvent arg0) {
				System.out.println("s");
				try {
					letalnik.hover();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			
			@Override
			public void keyPressed(KeyEvent ke) {
				
				//if(flightMode.equals("keyboard")){
					System.out.println("sg");
					if(ke.getKeyCode() ==KeyEvent.VK_LEFT){
						try {
							letalnik.move(-0.35f, 0, 0, 0);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else if(ke.getKeyCode() ==KeyEvent.VK_RIGHT){
						try {
							letalnik.move(0.35f, 0, 0, 0);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else if(ke.getKeyCode() ==KeyEvent.VK_UP){
						try {
							letalnik.move(0, -0.35f, 0, 0);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else if(ke.getKeyCode() ==KeyEvent.VK_DOWN){
						try {
							letalnik.move(0, 0.35f, 0, 0);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else if(ke.getKeyCode() ==KeyEvent.VK_A){
						try {
							letalnik.move(0, 0, 0, -0.7f);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else if(ke.getKeyCode() ==KeyEvent.VK_D){
						try {
							letalnik.move(0, 0, 0, 0.7f);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
					
					//}//fmode if
			}
		});
		
		video.setFocusable(true);
		
		okno.setVisible(true);
	}
	
	public void scan(){
		
		BufferedImage image = slika;

		//convert the image to a binary bitmap source
		LuminanceSource source = new BufferedImageLuminanceSource(image);
		BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));

		//decode the barcode
		QRCodeReader reader = new QRCodeReader();

		try {
			Result result = reader.decode(bitmap);
			say(result.getText());
		 //System.out.println(result.getText());
		} catch (ReaderException e) {
			say("Sorry. Please try again");
		 e.printStackTrace();
		}

		//byte[] b = result.getRawBytes();
		
		
		
	}
	
	public void say(final String beseda){
		Thread t = new Thread(){
			
			public void run(){
				String voiceName = "kevin";
			     VoiceManager voiceManager = null;
			     Voice voice = null;
			 
			     voiceManager = VoiceManager.getInstance();
			     voice = voiceManager.getVoice(voiceName);
			 
			     //voice.setPitch((float) 6.00);
			     //voice.setPitchShift((float) .005);
			     //voice.setPitchRange((float) 0.01);
			     voice.setVolume(1.0f);
			     //voice.setRate(100);
			     // "business", "casual", "robotic", "breathy"
			     voice.setStyle("business");
			            
			     //allocate the resources for the voice
			     voice.allocate();
			     
			     voice.speak(beseda);
			 
			     voice.deallocate();
			}
			
		};
		t.start();
		 
	}
	
	public static void main(String[] args) {
		new KontrolniStolp1();
	}

}
