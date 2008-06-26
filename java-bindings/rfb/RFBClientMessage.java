package rfb;

import java.io.IOException;
import java.io.InputStream;

public class RFBClientMessage {
	InputStream input;

	public RFBClientMessage(InputStream input) {
		this.input = input;
	}

	public static boolean isInitialMessage(byte type) {
		return type < 3;
	}

	public static boolean isFrameBufferRequest(byte type) {
		return type == 3;
	}

	public byte[] getOne() throws IOException {
		byte type = (byte)input.read();
		switch (type) {
		case 0: return setPixelFormat(type);
		case 2: return setEncodings(type);
		case 3: return frameBufferUpdateRequest(type);
		case 4: return keyEvent(type);
		case 5: return pointerEvent(type);
		case 6: return clientCutText(type);
		default: throw new RuntimeException("Unknown message type: "
			+ type);
		}
	}

	public byte[] fixedSize(byte type, int size) throws IOException {
		byte[] buffer = new byte[size];
		buffer[0] = type;
		input.read(buffer, 1, size - 1);
		return buffer;
	}

	public byte[] addFixedSize(byte[] buffer, int size) throws IOException {
		byte[] b = new byte[size];
		System.arraycopy(buffer, 0, b, 0, buffer.length);
		input.read(buffer, buffer.length, size - buffer.length);
		return b;
	}

	public byte[] setPixelFormat(byte type) throws IOException {
		return fixedSize(type, 20);
	}

	public byte[] setEncodings(byte type) throws IOException {
		byte[] buffer = fixedSize(type, 4);
		int count = (buffer[2] & 0xff) << 8 | (buffer[3] & 0xff);
		return addFixedSize(buffer, count * 4);
	}

	public byte[] frameBufferUpdateRequest(byte type) throws IOException {
		return fixedSize(type, 10);
	}

	public byte[] keyEvent(byte type) throws IOException {
		return fixedSize(type, 8);
	}

	public byte[] pointerEvent(byte type) throws IOException {
		return fixedSize(type, 6);
	}

	public byte[] clientCutText(byte type) throws IOException {
		byte[] buffer = fixedSize(type, 8);
		int count = (buffer[4] & 0xff) << 24 | (buffer[5] & 0xff) << 16
			| (buffer[6] & 0xff) << 8 | (buffer[7] & 0xff);
		return addFixedSize(buffer, count);
	}
}
