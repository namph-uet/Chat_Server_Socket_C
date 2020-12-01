// name: Phạm Hoàng Nam
// mssv: 17021164
import java.io.*;
import java.net.*;
import java.util.*;

public class Server {
    private int port;
    private Hashtable<String, UserThread> userThreads = new Hashtable<>();

    public Server(int port) {
        this.port = port;
    }

    public synchronized void execute() {
        try (ServerSocket serverSocket = new ServerSocket(port)) {

            System.out.println("Chat Server is listening on port " + port);

            while (true) {
                Socket socket = serverSocket.accept();
                System.out.println("New user connected");

                InputStream input = socket.getInputStream();
                BufferedReader reader = new BufferedReader(new InputStreamReader(input));
                String userID = reader.readLine();

                UserThread newUser = new UserThread(socket, this, userID);
                userThreads.put(userID, newUser);
                newUser.start();
            }

        } catch (IOException ex) {
            System.out.println("Error in the server: " + ex.getMessage());
            ex.printStackTrace();
        }
    }

    public static void main(String[] args) {
        int port = 9000;

        Server server = new Server(port);
        server.execute();
    }

    /**
     * Delivers a message from one user to others (broadcasting)
     */
    boolean broadcast(String message, String toUser) {
        System.out.println("send to " + toUser + " message: " + message);
        if(toUser != null) {
            if(userThreads.get(toUser) != null) {
                userThreads.get(toUser).sendMessage(message);
                return true;
            }
            else return false;
        }
        else {
            userThreads.entrySet().stream().forEach(element -> {
                element.getValue().sendMessage(message);
            });
            return true;
        }
    }


    /**
     * When a client is disconneted, removes the associated username and UserThread
     */
    void removeUser(String userID) {
        System.out.println("The user " + userID + " quitted");
        userThreads.remove(userID);
    }


    /**
     * Returns true if there are other users connected (not count the currently connected user)
     */
    boolean hasUsers(String userID) {
        return (userThreads.get(userID) != null);
    }

    /**
     * log user online
     */
    public void logUserOnline() {
        StringBuffer userOnline = new StringBuffer();
        userOnline.append("user online:\n");
        System.out.println("user online:");
        userThreads.entrySet().stream().forEach(element -> {
            System.out.println(element.getKey());
            userOnline.append(element.getKey() + "\n");
        });
        broadcast(userOnline.toString(), null);
    }
}


class UserThread extends Thread {
    private Socket socket;
    private Server server;
    private PrintWriter writer;
    private String userID;

    public UserThread(Socket socket, Server server, String userID) {
        this.socket = socket;
        this.server = server;
        this.userID = userID;
    }

    public void run() {
        try {
            InputStream input = socket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));

            OutputStream output = socket.getOutputStream();
            writer = new PrintWriter(output, true);

            printUsers();
            String serverMessage = "New user connected: " + this.userID;
            server.broadcast(serverMessage, null);

            String clientMessage;

            do {
                clientMessage = reader.readLine();
                String toUser = null;
                StringBuffer strBuff = new StringBuffer();
                try {
                    toUser = clientMessage.substring(clientMessage.indexOf('[') + 1, clientMessage.indexOf(']'));
                    strBuff.append(clientMessage);
                    strBuff.delete(clientMessage.indexOf('['), clientMessage.indexOf(']') + 1);
                }
                catch(StringIndexOutOfBoundsException e) {
                    if(!clientMessage.equals("bye")) sendMessage("Wrong syntax!");
                }
                catch(NullPointerException e) {
                    break;
                }

                if(!server.broadcast(strBuff.toString(), toUser)) writer.println("user not online");

            } while (!clientMessage.equals("bye"));

            server.removeUser(userID);
            socket.close();

            serverMessage = userID + " has quitted.";
            server.broadcast(serverMessage, null);
            server.logUserOnline();

        } catch (IOException ex) {
            System.out.println("Error in UserThread: " + ex.getMessage());
            ex.printStackTrace();
        }
    }

    /**
     * Sends a list of online users to the newly connected user.
     */
    void printUsers() {
        if (server.hasUsers(userID)) {
            writer.println("Connected users: " + userID);
            server.logUserOnline();
        } else {
            writer.println("No other users connected");
        }
    }

    /**
     * Sends a message to the client.
     */
    void sendMessage(String message) {
        writer.println(message);
    }
}

