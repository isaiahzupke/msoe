import java.io.IOException;
import javax.swing.WindowConstants;
import edu.msoe.sefocus.pcgui.STreamedImageFrame;
import edu.msoe.sefocus.pcgui.StreamedImagePanel;

public class Main {
	public static void main(String args[]) throws IOException, InterruptedException {
		if (args.length!=1)
		{
			System.err.println("Usage: <Program Name> <Port>");
		}
		else {
			int port = Integer.parseInt(args[0]);

			STreamedImageFrame frame = new STreamedImageFrame(port, System.out);

			frame.setVisible(true);
			frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);

			frame.startReceivingImages();
			frame.waitToStopReceivingImages();
		}
	}
}
