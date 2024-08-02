import java.io.File;
        import java.io.IOException;
        import java.util.Iterator;
        import javax.imageio.ImageIO;
        import javax.imageio.ImageReader;
        import javax.imageio.metadata.IIOMetadata;
        import javax.imageio.stream.ImageInputStream;
        import org.w3c.dom.Element;

public class ExifDataExample {

    public static void main(String[] args) {
        // File path of the image file
        String filePath = "/path/to/image.jpg";
        try {
            // Get the image file
            File imageFile = new File(filePath);

            // Create an ImageInputStream from the image file
            ImageInputStream iis = ImageIO.createImageInputStream(imageFile);

            // Get an ImageReader that can read the input ImageInputStream
            Iterator<ImageReader> readers = ImageIO.getImageReaders(iis);
            if (!readers.hasNext()) {
                System.out.println("No ImageReader found for the file");
                return;
            }
            ImageReader reader = readers.next();

            // Set the ImageReader's input source to the ImageInputStream
            reader.setInput(iis);

            // Get the image's metadata
            IIOMetadata metadata = reader.getImageMetadata(0);

            // Get the metadata format name
            String formatName = metadata.getNativeMetadataFormatName();

            // Get the root element of the metadata
            Element root = (Element) metadata.getAsTree(formatName);

            // Print out the EXIF metadata
            System.out.println("EXIF metadata:");
            printExifData(root, "tiff:Make");
            printExifData(root, "tiff:Model");
            printExifData(root, "tiff:DateTime");
            printExifData(root, "exif:ExposureTime");
            printExifData(root, "exif:FNumber");
            printExifData(root, "exif:ISOSpeedRatings");
            printExifData(root, "exif:FocalLength");
            printExifData(root, "exif:Flash");
            printExifData(root, "exif:GPSLatitude");
            printExifData(root, "exif:GPSLongitude");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Method to print a specific EXIF metadata
    private static void printExifData(Element root, String tagName) {
        Element elem = (Element) root.getElementsByTagName(tagName).item(0);
        String value = elem.getTextContent();
        System.out.println(tagName + ": " + value);
    }
}
