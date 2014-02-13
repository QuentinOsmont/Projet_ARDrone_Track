import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;

import com.codeminders.ardrone.ARDrone;
import com.codeminders.ardrone.NavData;


public class ImageModificator {
	
	public static BufferedImage scaleImage(BufferedImage img, int width, int height, Color background, int a, int b, float angle, int bt) {
	    int imgWidth = img.getWidth();
	    int imgHeight = img.getHeight();
	    if (imgWidth*height < imgHeight*width) {
	        width = imgWidth*height/imgHeight;
	    } else {
	        height = imgHeight*width/imgWidth;
	    }
	    BufferedImage newImage = new BufferedImage(width, height,
	            BufferedImage.TYPE_INT_RGB);
	    Graphics2D g = newImage.createGraphics();
	    try {
	    	g.rotate(0);
	        g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,RenderingHints.VALUE_INTERPOLATION_BICUBIC);
	        g.setBackground(background);
	        g.clearRect(0, 0, width, height);
	        g.drawImage(img, 0, 0, width, height, null);
	        g.setColor(Color.green);
	        g.drawLine(width/2, 0, width/2, height);
	        g.drawLine(0, height/2, width, height/2);
	        g.setColor(Color.white);
	        g.drawOval(Math.abs(a - width)+1, b+1, 30, 30);
	        g.drawOval(Math.abs(a - width)+1, b+1, 31, 31);
	        g.setColor(Color.white);
	        g.setFont(new Font("Arial", Font.PLAIN, 16));
	        g.drawString("Baterija: "+bt + "%", 5, 35);
	        g.rotate(Math.toRadians(angle), width/2, height/2+10);
	        g.drawLine(width/2-150, height/2-5, width/2 + 150, height/2-5);
	        g.drawLine(width/2-150, height/2+5, width/2 + 150, height/2+5);
	    } finally {
	        g.dispose();
	    }
	    return newImage;
	}
	
	public static BufferedImage scaleImage2(BufferedImage img, int width, int height,
	        Color background) {
	    int imgWidth = img.getWidth();
	    int imgHeight = img.getHeight();
	    if (imgWidth*height < imgHeight*width) {
	        width = imgWidth*height/imgHeight;
	    } else {
	        height = imgHeight*width/imgWidth;
	    }
	    BufferedImage newImage = new BufferedImage(width, height,
	            BufferedImage.TYPE_3BYTE_BGR);
	    Graphics2D g = newImage.createGraphics();
	    try {
	        //(g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
	        //        RenderingHints.VALUE_INTERPOLATION_BICUBIC);
	        g.setBackground(background);
	        g.clearRect(0, 0, width, height);
	        g.drawImage(img, 0, 0, width, height, null);
	    } finally {
	        g.dispose();
	    }
	    return newImage;
	}
	
	public static BufferedImage scaleImage3(BufferedImage img, int width, int height,
	        Color background) {
	    int imgWidth = img.getWidth();
	    int imgHeight = img.getHeight();
	    if (imgWidth*height < imgHeight*width) {
	        width = imgWidth*height/imgHeight;
	    } else {
	        height = imgHeight*width/imgWidth;
	    }
	    BufferedImage newImage = new BufferedImage(width, height,
	            BufferedImage.TYPE_BYTE_GRAY);
	    Graphics2D g = newImage.createGraphics();
	    try {
	        //(g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
	        //        RenderingHints.VALUE_INTERPOLATION_BICUBIC);
	        g.setBackground(background);
	        g.clearRect(0, 0, width, height);
	        g.drawImage(img, 0, 0, width, height, null);
	    } finally {
	        g.dispose();
	    }
	    return newImage;
	}
	

}
