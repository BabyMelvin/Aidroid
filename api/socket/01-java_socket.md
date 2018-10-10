# Java网络编Java

java TCP和 java UDP编程

# 1.Java TCP
## 1.1 Java TCP服务器

* `ServerSocket` 类的一个对象表示Java中的TCP服务器套接字。
* `ServerSocket` 对象可以接受来自远程客户端的连接请求。
我们可以使用no-args构造函数创建一个未绑定的服务器套接字，并使用其`bind()`方法将其绑定到本地端口和本地IP地址。

### 例子
以下代码显示如何创建服务器套接字：

```java
import java.net.InetSocketAddress;
import java.net.ServerSocket;

public class Main{
  public static void main(String[] argv) throws Exception{
    //create an unbound server socket
    ServerSocket serverSocket=new ServerSocket();
    
    //create a socket address object
    InetSocketAddress endPoint=new InetSocketAddress("localhost",12900);
    
    //set the wait queue size to 100
    int waitQueueSize = 100;
    
    //Bind the server socket to localhost and at port 12900 with
    //a wait queue size of 100
    serverSocket.bind(endPoint,waitQueueSize);
  }
}
```
您可以通过使用以下任何构造函数在一个步骤中组合create，bind和listen操作。等待队列大小的默认值为50。本地IP地址的缺省值是通配符地址，即服务器计算机的所有IP地址。

```java
ServerSocket(int port)
ServerSocket(int port, int waitQueueSize)
ServerSocket(int port, int waitQueueSize,  InetAddress  bindAddr)
```
您可以将套接字创建和绑定步骤合并为一个语句。

以下代码显示如何在端口12900创建服务器套接字，其中100作为等待队列大小，并在localhost回送地址。

```java
ServerSocket serverSocket  = new ServerSocket(12900, 100, InetAddress.getByName("localhost"));
```
要接受远程连接请求，请调用服务器套接字上的 accept()方法。

accept()方法调用阻塞执行，直到来自远程客户端的请求到达其等待队列。

```java
//The following code calls on ServerSocket will wait  for a  new remote  connection request.
Socket activeSocket = serverSocket.accept();
```
Socket类包含两个方法 `getInputStream()`和 `getOutputStream()`用于读取和写入连接的套接字。

```java
BufferedReader br  = new BufferedReader(new InputStreamReader(activeSocket.getInputStream()));
BufferedWriter bw  = new BufferedWriter(new OutputStreamWriter(activeSocket.getOutputStream()));
String s = br.readLine();
bw.write("hello"); 
bw.flush();
```
最后，使用套接字的close()方法关闭连接。关闭套接字还会关闭其输入和输出流。

```java
activeSocket.close();
```

### 以下代码显示如何创建服务器套接字

```java
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class Main {
  public static void main(String[] args) throws Exception {
    ServerSocket serverSocket = new ServerSocket(12900, 100,
        InetAddress.getByName("localhost"));
    System.out.println("Server started  at:  " + serverSocket);

    while (true) {
      System.out.println("Waiting for a  connection...");

      final Socket activeSocket = serverSocket.accept();

      System.out.println("Received a  connection from  " + activeSocket);
      Runnable runnable = () -> handleClientRequest(activeSocket);
      new Thread(runnable).start(); // start a new thread
    }
  }

  public static void handleClientRequest(Socket socket) {
    try{
      BufferedReader socketReader = null;
      BufferedWriter socketWriter = null;
      socketReader = new BufferedReader(new InputStreamReader(
          socket.getInputStream()));
      socketWriter = new BufferedWriter(new OutputStreamWriter(
          socket.getOutputStream()));

      String inMsg = null;
      while ((inMsg = socketReader.readLine()) != null) {
        System.out.println("Received from  client: " + inMsg);

        String outMsg = inMsg;
        socketWriter.write(outMsg);
        socketWriter.write("\n");
        socketWriter.flush();
      }
      socket.close();
    }catch(Exception e){
      e.printStackTrace();
    }

  }
}
```
## 1.2 java 网络TCP服务端套接字
Socket 类表示一个TCP客户端套接字。以下代码显示如何创建TCP客户端套接字：

```java
//Create Socket for 192.168.1.2 at  port 1234
Socket   socket = new Socket("192.168.1.2", 1234);
```
以下代码显示如何创建未绑定的客户端套接字，绑定它并连接它。

```java
Socket socket = new Socket();
socket.bind(new InetSocketAddress("localhost",  1234));
socket.connect(new InetSocketAddress("localhost",  1234));
````
在连接Socket对象之后，我们可以分别使用getInputStream()和getOutputStream()方法使用其输入和输出流。

```java
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class Main {
  public static void main(String[] args) throws Exception {
    Socket socket = new Socket("localhost", 12900);
    System.out.println("Started client  socket at "
        + socket.getLocalSocketAddress());
    BufferedReader socketReader = new BufferedReader(new InputStreamReader(
        socket.getInputStream()));
    BufferedWriter socketWriter = new BufferedWriter(new OutputStreamWriter(
        socket.getOutputStream()));
    BufferedReader consoleReader = new BufferedReader(
        new InputStreamReader(System.in));

    String promptMsg = "Please enter a  message  (Bye  to quit):";
    String outMsg = null;

    System.out.print(promptMsg);
    while ((outMsg = consoleReader.readLine()) != null) {
      if (outMsg.equalsIgnoreCase("bye")) {
        break;
      }
      // Add a new line to the message to the server,
      // because the server reads one line at a time.
      socketWriter.write(outMsg);
      socketWriter.write("\n");
      socketWriter.flush();

      // Read and display the message from the server
      String inMsg = socketReader.readLine();
      System.out.println("Server: " + inMsg);
      System.out.println(); // Print a blank line
      System.out.print(promptMsg);
    }
    socket.close();
  }
}
```
# 2.Java UDP
## 2.1 Java网络UDP套接字
TCP套接字是面向连接的，基于流。基于UDP的套接字是无连接的，基于数据报。

使用UDP发送的数据块称为数据报或UDP数据包。每个UDP分组具有数据，目的地IP地址和目的地端口号。

无连接套接字在通信之前不建立连接。

UDP是不可靠的协议，因为它不保证到达分组的传递和顺序。

在无连接协议UDP中，不会有服务器套接字。

在UDP连接中，客户端和服务器发送或接收一组数据，而无需事先知道它们之间的通信。

发送到同一目的地的每个数据块独立于先前发送的数据。

当编码UDP连接时，使用以下两个类。

* `DatagramPacket`类表示UDP数据报。
* `DatagramSocket`类表示用于**发送**或**接收**数据报包的UDP套接字。

以下代码显示如何在localhost中创建绑定到端口号12345的UDP套接字。

```java
DatagramSocket udpSocket = new DatagramSocket(12345, "localhost");
```

DatagramSocket 类提供了一个`bind()`方法，它允许您将套接字绑定到本地IP地址和本地端口号。

### 2.1.1 DatagramPacket
DatagramPacket包含三个东西：

* 目的IP地址
* 目的端口号
* 数据

DatagramPacket类的构造函数创建一个数据包来接收数据如下：

```java
DatagramPacket(byte[] buf,  int  length)
DatagramPacket(byte[] buf,  int offset, int length)
```

DatagramPacket类的构造函数创建一个数据包来发送数据如下：

```java
DatagramPacket(byte[] buf,  int  length,  InetAddress address, int port)
DatagramPacket(byte[] buf,  int offset, int length,  InetAddress address, int port)
DatagramPacket(byte[] buf,  int length, SocketAddress address)
DatagramPacket(byte[] buf,  int offset, int length, SocketAddress address)
```

以下代码演示如何创建数据报：

以下代码创建一个数据包以接收1024字节的数据。

```java
byte[] data = new byte[1024];
DatagramPacket packet  = new DatagramPacket(data, data.length);
```

以下代码创建一个包的缓冲区大小为1024，并从偏移量8开始接收数据，它将只接收32字节的数据。

```java
byte[]  data2 = new byte[1024];
DatagramPacket packet2  = new DatagramPacket(data2, 8, 32);
```
数据包中的数据总是指定偏移量和长度。我们需要使用offset和length来读取数据包中的数据。
## 2.2 Java网络UDP服务器
以下代码显示了如何编写UDP回显服务器：基于UDP套接字的Echo服务器

```java
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class Main {
  public static void main(String[] args) throws Exception {
    final int LOCAL_PORT = 12345;
    final String SERVER_NAME = "localhost";
    DatagramSocket udpSocket = new DatagramSocket(LOCAL_PORT,
        InetAddress.getByName(SERVER_NAME));

    System.out.println("Created UDP  server socket at "
        + udpSocket.getLocalSocketAddress() + "...");
    while (true) {
      System.out.println("Waiting for a  UDP  packet...");
      DatagramPacket packet = new DatagramPacket(new byte[1024], 1024);
      udpSocket.receive(packet);
      displayPacketDetails(packet);
      udpSocket.send(packet);
    }
  }
  public static void displayPacketDetails(DatagramPacket packet) {
    byte[] msgBuffer = packet.getData();
    int length = packet.getLength();
    int offset = packet.getOffset();

    int remotePort = packet.getPort();
    InetAddress remoteAddr = packet.getAddress();
    String msg = new String(msgBuffer, offset, length);

    System.out.println("Received a  packet:[IP Address=" + remoteAddr
        + ", port=" + remotePort + ", message=" + msg + "]");
  }
}
```
上面的代码生成以下结果

```
Create UDP server socket at 127.0.0.1:12345...
Waiting for a UDP packet...
```

### Echo 客户端
基于UDP套接字的Echo客户端

```java
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class Main {
  public static void main(String[] args) throws Exception {
    DatagramSocket udpSocket = new DatagramSocket();
    String msg = null;
    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    System.out.print("Please enter a  message  (Bye  to quit):");
    while ((msg = br.readLine()) != null) {
      if (msg.equalsIgnoreCase("bye")) {
        break;
      }
      DatagramPacket packet = Main.getPacket(msg);
      udpSocket.send(packet);
      udpSocket.receive(packet);
      displayPacketDetails(packet);
      System.out.print("Please enter a  message  (Bye  to quit):");
    }
    udpSocket.close();
  }
  public static void displayPacketDetails(DatagramPacket packet) {
    byte[] msgBuffer = packet.getData();
    int length = packet.getLength();
    int offset = packet.getOffset();
    int remotePort = packet.getPort();
    InetAddress remoteAddr = packet.getAddress();
    String msg = new String(msgBuffer, offset, length);
    System.out.println("[Server at IP  Address=" + remoteAddr + ", port="
        + remotePort + "]: " + msg);
  }
  public static DatagramPacket getPacket(String msg)
      throws UnknownHostException {
    int PACKET_MAX_LENGTH = 1024;
    byte[] msgBuffer = msg.getBytes();

    int length = msgBuffer.length;
    if (length > PACKET_MAX_LENGTH) {
      length = PACKET_MAX_LENGTH;
    }
    DatagramPacket packet = new DatagramPacket(msgBuffer, length);
    InetAddress serverIPAddress = InetAddress.getByName("localhost");
    packet.setAddress(serverIPAddress);
    packet.setPort(15900);
    return packet;
  }
}
```
上面的代码生成以下结果

```
Please enter a message(Byte to quit):Byte
```
## 2.3 Java网络UDP多播
Java使用MulticastSocket类来创建UDP多播套接字，以接收发送到多播IP地址的数据包。

组播套接字基于组成员资格。创建并绑定组播套接字后，调用其joinGroup（InetAddress multiCastIPAddress）方法加入组播组，发送到该组的任何数据包数据包将被传递到此套接字。

要离开组，请调用leaveGroup（InetAddress multiCastIPAddress）方法。

在IPv4中，范围224.0.0.0到239.255.255.255中的任何IP地址都可以用作组播地址来发送数据报。

IP地址224.0.0.0保留，您不应在您的应用程序中使用它。

组播IP地址不能用作数据包的源地址。

### 例子

接收UDP组播消息的UDP组播套接字。

```java
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class Main {
  public static void main(String[] args) throws Exception {
    int mcPort = 12345;
    String mcIPStr = "230.1.1.1";
    MulticastSocket mcSocket = null;
    InetAddress mcIPAddress = null;
    mcIPAddress = InetAddress.getByName(mcIPStr);
    mcSocket = new MulticastSocket(mcPort);
    System.out.println("Multicast Receiver running at:"
        + mcSocket.getLocalSocketAddress());
    mcSocket.joinGroup(mcIPAddress);

    DatagramPacket packet = new DatagramPacket(new byte[1024], 1024);

    System.out.println("Waiting for a  multicast message...");
    mcSocket.receive(packet);
    String msg = new String(packet.getData(), packet.getOffset(),
        packet.getLength());
    System.out.println("[Multicast  Receiver] Received:" + msg);

    mcSocket.leaveGroup(mcIPAddress);
    mcSocket.close();
  }
}
```
上面的代码生成以下结果。

```
Multicast Receiver running at:0.0.0.0/0.0.0.0:12345
Waiting for a multicast message...
```

### UDP数据报套接字，多播发送方应用程序

```java
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class Main {
  public static void main(String[] args) throws Exception {
    int mcPort = 12345;
    String mcIPStr = "230.1.1.1";
    DatagramSocket udpSocket = new DatagramSocket();

    InetAddress mcIPAddress = InetAddress.getByName(mcIPStr);
    byte[] msg = "Hello".getBytes();
    DatagramPacket packet = new DatagramPacket(msg, msg.length);
    packet.setAddress(mcIPAddress);
    packet.setPort(mcPort);
    udpSocket.send(packet);

    System.out.println("Sent a  multicast message.");
    System.out.println("Exiting application");
    udpSocket.close();
  }
}
```
上面的代码生成以下结果

```
Send a multicast message
Exiting application
```
