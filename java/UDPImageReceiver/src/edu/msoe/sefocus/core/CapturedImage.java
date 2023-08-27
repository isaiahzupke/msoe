package edu.msoe.sefocus.core;

public class CapturedImage {
    private int startTime;
    private int finishTime;
    int height;
    int width;
    int id;
    // Allocate space to receive the data.
    byte myArray[];
    boolean done;
    boolean valid = false;
    /**
     * This is a counbt of the number of lines received in the captured image.
     */
    int linesReceived = 0;

    public byte[] getMyArray() {
        return myArray;
    }

    public int getId() {
        return id;
    }

    public boolean isDone() {
        return done;
    }

    public CapturedImage(int h, int w, int id, int startTime) {
        height = h;
        width = w;
        this.id = id;
        myArray = new byte[3 * height * width];
        this.startTime=startTime;
    }

    /**
     * This method will return the time from the first transmission to the final transmission in ms.
     * @return The delta of transmitting the image will be returned.
     */
    public int getTransmissionTime() {
        int retVal;
        if (startTime < finishTime) {
            retVal = finishTime - startTime;
        } else {
            retVal = (0x7FFFFFFF - startTime) + finishTime;
        }
        return retVal;
    }

    public void addLine(byte[] line, int cseg) {
        // Now we need to copy this into the main image buffer.
        System.arraycopy(line, 0, myArray, 3 * width * cseg, 3 * width);
        linesReceived++;
        if (linesReceived == height) {
            done = true;
        }
        if (linesReceived > height / 5)
        {
            valid = true;
        }
    }

    public int getLinesReceived()
    {
        return linesReceived;
    }

    public void setFinishTime(int finishTime)
    {
        this.finishTime=finishTime;
    }

    public boolean isValid() {
        return valid;
    }


}