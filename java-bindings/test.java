public class test {
	public static void main(String[] args) {
		System.loadLibrary("nacro");
		int handle = nacro.initvnc("localhost", 5900, 5910);
		System.err.println("handle: " + handle);
		int result = nacro.waitforinput(handle, 600);
		System.err.println("result: " + result);
	}
}
