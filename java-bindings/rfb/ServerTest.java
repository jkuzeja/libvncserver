package rfb;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.net.ServerSocket;
import java.net.Socket;

public class ServerTest {
	String remoteHost;
	int remotePort;

	int serverHandle;

	ServerSocket listen;

	public ServerTest(String host, int port, int listenPort) {
		remoteHost = host;
		remotePort = port;

		serverHandle = rfb.connectToServer(host, port);
		if (serverHandle < 0)
			throw new RuntimeException("Could not connect to "
				+ host + ":" + port);

		try {
			listen = new ServerSocket(listenPort);
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

	public void run() {
		try {
			new ServerThread().start();
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}

		for (;;) {
			try {
				Socket socket = listen.accept();
				new ClientThread(socket).start();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	class ServerThread extends Thread {
		public void run() {
			for (;;)
				rfb.processServerEvents(serverHandle);
		}
	}

	class ClientThread extends Thread {
		InputStream input;
		OutputStream output;
		RFBClientMessage messageParser;
		Socket serverSocket;

		int client;

		ClientThread(Socket socket) throws IOException {
			input = socket.getInputStream();
			output = socket.getOutputStream();
			messageParser = new RFBClientMessage(input);
			serverSocket = new Socket(remoteHost, remotePort);
		}

		public void run() {
			client = rfb.addClient(serverHandle);
			byte[] buffer = new byte[32768];
			try {
				while (processInitialEvent(buffer))
					; /* do nothing */
				rfb.closeClient(client);
				client = -1;
				while (passThrough(buffer))
					; /* do nothing */
			} catch (Exception e) {
				e.printStackTrace();
			}
		}	

		boolean processInitialEvent(byte[] buffer) throws IOException {
			byte[] message = messageParser.getOne();
			if (messageParser.isInitialMessage(message[0])) {
				rfb.processClientMessage(client,
					message, message.length);
				serverSocket.getOutputStream()
					.write(message, 0, message.length);
			}
			else if (messageParser.isFrameBufferRequest(message[0])) {
				rfb.processClientMessage(client,
					message, message.length);
				for (;;) {
					int len = rfb.getServerResponse(
						client, buffer,
						buffer.length);
					if (len == 0)
						break;
					output.write(buffer, 0, len);
				}
				return false;
			}
			else
				serverSocket.getOutputStream()
					.write(message, 0, message.length);
			return true;
		}

		boolean passThrough(byte[] buffer) throws IOException {
			/* pass through */
			int count = input.read();
			if (count < 0)
				return false;
			output.write(buffer, 0, count);
			return true;
		}
	}

	public static void main(String[] args) {
		System.loadLibrary("rfb_java");
		ServerTest test = new ServerTest("localhost", 5900, 5907);
		test.run();
	}
}
