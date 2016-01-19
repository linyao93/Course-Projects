
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.RandomAccessFile;
import java.net.ConnectException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.CountDownLatch;


public class client {
	
	private static int id;
	private static int sPort;
	private static int globalpeernum;
	private static int filenum;
	private static boolean []localfiletable;
	private static int UploadPeerId;
	private static int DownloadPeerId,DownloadPeerPort;
	private static Queue<Integer> reqfile = new LinkedList<Integer>();
	private static String   fname,
							pf="Partial\\",
							fp="Partial\\",
							of="Merged\\";
	
	
	public client(){}
	
	public void DownloadFromServer(){
		Socket requestSocket = null;           //socket connect to the server
		ObjectOutputStream out = null;         //stream write to the socket
		ObjectInputStream in = null;          //stream read from the socket
		String message;                //message send to the server
		String MESSAGE;                //message read from the server
		String incontent[];			//parsed MESSAGE
		String prefix = "I";
		
		int status=-1;
		
		try{
			//create a socket to connect to the server
			requestSocket = new Socket("localhost", 9000);
			Uts.printinfo(prefix, "Connected to localhost in port 9000");
			
			//initialize inputStream and outputStream
			out = new ObjectOutputStream(requestSocket.getOutputStream());
			out.flush();
			in = new ObjectInputStream(requestSocket.getInputStream());
			
			//communicate with server
			while(true){
				//initialize settings
				if (status==-1){
					message = "i ";
					Uts.sendMessage(out,message);
					Uts.printinfo(prefix, "request network setting");
					MESSAGE = (String)in.readObject();
					incontent = MESSAGE.split(" ", 2);
					if (incontent[0].startsWith("i")){
						Uts.settings(incontent[1]);
					}
					status = 0;
				}
				
				//requesting file list
				else if (status==0){
					//send "request list" message
					message = "t";
					Uts.sendMessage(out, message);
					Uts.printinfo(prefix, "request file list");
					
					status++;
					
					//receive file list
					MESSAGE = (String)in.readObject();
					incontent = MESSAGE.split(" ", 2);
					if (incontent[0].startsWith("r")){
						//parse file list
						Uts.printinfo(prefix, "Server offers file list:"+incontent[1]);
						String cot[]=incontent[1].split(",");
						for (int i=0;i<cot.length;i++){
							int id = Integer.valueOf(cot[i]);
							//if file does not exists and haven't been put in queue
							if (localfiletable[id]==false && reqfile.contains(id)==false){
								reqfile.offer(id);
							}
						}
					}
				}
				//requesting file entities
				else{
					while (reqfile.isEmpty()==false){
						//send "file" request
						message="a "+Integer.toString(reqfile.poll());
						Uts.sendMessage(out,message);
						Uts.printinfo(prefix, "request file "+message.split(" ",2)[1]);
						
						//receive "file delivery" signal
						MESSAGE = (String)in.readObject();
						incontent = MESSAGE.split(" ", 2);
						if (incontent[0].startsWith("d")){
							Uts.printinfo(prefix, "ready for file: "+incontent[1]);
							Uts.recvFile(in, fp);
							Uts.printinfo(prefix, "file: "+incontent[1]+" received");
							localfiletable[Integer.valueOf(incontent[1])] = true;
						}
					}
					break;
				}	
			}
			//send "finish" signal
			Uts.sendMessage(out, "f ");
		}
		catch (ConnectException e) {
			System.err.println("Connection refused. You need to initiate a server first.");
		} 
		catch ( ClassNotFoundException e ) {
        	System.err.println("Class not found");
    	} 
		catch(UnknownHostException unknownHost){
			System.err.println("You are trying to connect to an unknown host!");
		}
		catch(IOException ioException){
			ioException.printStackTrace();
		}
		finally{
			//Close connections
			try{
				in.close();
				out.close();
				requestSocket.close();
			}
			catch(IOException ioException){
				ioException.printStackTrace();
			}
		}
	}
	
	private static class Uts {
		public static void settings(String content){
			String buf[]=content.split(" ",8);
			String prefix = "I";
			
			id=Integer.valueOf(buf[0]);
			sPort = Integer.valueOf(buf[1]);
			globalpeernum = Integer.valueOf(buf[2]);
			UploadPeerId = Integer.valueOf(buf[3]);
			DownloadPeerId = Integer.valueOf(buf[4]);
			DownloadPeerPort = Integer.valueOf(buf[5]);
			filenum=Integer.valueOf(buf[6]);
			fname=buf[7];
			
			localfiletable=new boolean[filenum];
			for (int i=0;i<filenum;i++){
				localfiletable[i]=false;
			}
			
			pf+=fname;
			of+=fname;
			
			printinfo(prefix, "Settings:");
			printinfo(prefix, "Set ID = "+id);
			printinfo(prefix, "Set Port = "+sPort);
			printinfo(prefix, "Set Peernum = "+globalpeernum);
			printinfo(prefix, "Set UploadPeer = "+UploadPeerId);
			printinfo(prefix, "Set DownloadPeer = "+DownloadPeerId+"(Port = "+DownloadPeerPort+")");
			printinfo(prefix, "Set filenum = "+filenum);
			printinfo(prefix, "Set filename = "+fname);
			
		}
		
		
		public static void recvFile(ObjectInputStream in, String path){

			FileOutputStream fos = null;
			try{
		        //get filename and length
				String filename = (String) in.readObject();
				long filelen = in.readLong();
				
				fos = new FileOutputStream(new File(path + filename));
				byte[] sendBytes = new byte[1024];
				long Prog = 0;
				
				System.out.println(">>>begin Receiving File: " + filename + ", File Size: " + filelen);
				while(true){
					int read = 0;
					read = in.read(sendBytes);
					Prog += read; 
					fos.write(sendBytes, 0, read);
					fos.flush();
//					System.out.print(".");
					if(Prog==filelen)
						break;
				}
//				System.out.println(".");
				System.out.println(">>>File Received: " + filename );
			}
			catch(IOException ioException){
				ioException.printStackTrace();
			} catch (ClassNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}finally{
				if(fos != null)
					try {
						fos.close();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
			}
		}
		
		
		public static void sendFile(ObjectOutputStream out, String filename){
			FileInputStream fis = null;
			long prog = 0,filelen=0;
			try{
				//initialize
				File file = new File(filename);
	            fis = new FileInputStream(file);
	            
	            //send filename and length
	            out.writeObject(file.getName());
	            out.flush();
	            filelen = file.length();
	            out.writeLong(filelen);
	            out.flush();
	            
	            //send file
	            byte[] sendBytes = new byte[1024];
	            int length = 0;
	            
	            System.out.println("<<<begin sending file: "+file.getName()+", size: "+ filelen);
				while(prog<filelen){
	            	length = fis.read(sendBytes, 0, sendBytes.length);
	            	prog+=length;
	                out.write(sendBytes, 0, length);
//	                System.out.println(Integer.toString(length));
//	                System.out.print(">");
	                out.flush();
	            }
//				System.out.println(".");
	            System.out.println("<<<finish sending file: "+file.getName());
			}
			catch(IOException ioException){
				ioException.printStackTrace();
			}finally{
				if(fis != null)
					try {
						fis.close();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
			}
		}
		
		
		public static void sendMessage(ObjectOutputStream out, String msg)
		{
			try{
				//stream write the message
				out.writeObject(msg);
				out.flush();
			}
			catch(IOException ioException){
			ioException.printStackTrace();
			}
		}
		
		
	    public static void merge(String file,String tempFiles)
	    {
	        RandomAccessFile ok;
			try {
				ok = new RandomAccessFile(new File(file),"rw");
				
				int i = 0;
		        File tempfile = new File(tempFiles+"_"+i+".tmp");
		        RandomAccessFile read = new RandomAccessFile(tempfile,"r");
		        while (read  != null)
		        {
		            byte[] b = new byte[1024];
		            int n=0;
		            while((n=read.read(b))!= -1)
		            {
		                ok.write(b, 0, n);
		            }
		            read.close();
		            
		            i++;
		            tempfile = new File(tempFiles+"_"+i+".tmp");
		            if (tempfile.canRead())
		            	read = new RandomAccessFile(tempfile,"r");
		            else
		            	read = null;
		        }
		        ok.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	    }
	
	    public static boolean finishcheck(boolean[] filetable){
	    	boolean flag=true;
	    	for (int i = 0;i<10;i++){
	    		if (!filetable[i]){
	    			flag=false;
	    		}
	    	}
	    	return flag;
	    }

	    public static void printinfo(String prefix, String info){
	    	System.out.println("["+prefix+"]: "+info);
	    }
	}
	
	private static class DownloadHandler extends Thread {
		private Socket requestSocket = null;           //socket connect to the server
		private ObjectOutputStream out = null;         //stream write to the socket
		private ObjectInputStream in = null;          //stream read from the socket
		private String message;                //message send to the server
		private String MESSAGE;                //capitalized message read from the server
		private int port;					//peer port
		private String filepath;			//path of partial files
		private String prefix = "D";
		private CountDownLatch countDownLatch;
		
		public DownloadHandler(CountDownLatch countDownLatch, int port, String filepath) {
			this.port = port;
			this.filepath = filepath;
			this.countDownLatch = countDownLatch;
		}
		public void run()
		{
			String incontent[];
			int status = 0;
			try{
				//waiting for server
				while (requestSocket==null || !requestSocket.isConnected()){
					try {
						Uts.printinfo(prefix, "Waiting for Peer Server...");
						Thread.sleep(2000);
						requestSocket = new Socket("localhost", port);
						
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (ConnectException e) {
						Uts.printinfo(prefix, "Waiting for Peer Server...");
					} 
				}
				Uts.printinfo(prefix, "Connected to localhost in port "+Integer.toString(port));
				
				//initialize inputStream and outputStream
				out = new ObjectOutputStream(requestSocket.getOutputStream());
				out.flush();
				in = new ObjectInputStream(requestSocket.getInputStream());

				while(status!=2)
				{
					//request file list from peer
					if (status==0){
						//send "file list" request
						message = "t";
						Uts.sendMessage(out, message);
						Uts.printinfo(prefix, "request file list");
						
						
						//receive file list from peer
						MESSAGE = (String)in.readObject();
						incontent = MESSAGE.split(" ", 2);
						if (incontent[0].startsWith("r")){
							Uts.printinfo(prefix, "Peer offers file list:"+incontent[1]);
							String cot[]=incontent[1].split(",");
							for (int i=0;i<cot.length;i++){
								int id = Integer.valueOf(cot[i]);
								if (localfiletable[id]==false&&reqfile.contains(id)==false){
									reqfile.offer(id);
									status = 1;
								}
							}
						}
						if (status==0){
							try {
								Thread.sleep(1000);
							} catch (InterruptedException e) {
							// TODO Auto-generated catch block
								e.printStackTrace();
							}	
						}
					}
					else if (status ==1){
						while (reqfile.isEmpty()==false){
							//send "file" request
							message="a "+reqfile.poll().toString();
							Uts.sendMessage(out,message);
							Uts.printinfo(prefix, "request file: "+message.split(" ", 2)[1]);
							
							//receive "file delivery" signal
							MESSAGE = (String)in.readObject();
							incontent = MESSAGE.split(" ", 2);
							if (incontent[0].startsWith("d")){
								Uts.printinfo(prefix, "ready for file:"+incontent[1]);
								Uts.recvFile(in, filepath);
								localfiletable[Integer.valueOf(incontent[1])] = true;
							}
							if (Uts.finishcheck(localfiletable)){
								status=2;
								Uts.printinfo(prefix, "finish");
							}
							else
								status=0;
						}	
					}
				}
				//send "finish" signal
				Uts.sendMessage(out, "f ");
			}
			catch ( ClassNotFoundException e ) {
	        	System.err.println("Class not found");
	    	} 
			catch(UnknownHostException unknownHost){
				System.err.println("You are trying to connect to an unknown host!");
			}
			catch(IOException ioException){
				ioException.printStackTrace();
			}
			finally{
				//Close connections
				try{
					if (in!=null)
						in.close();
					if (out!=null)
						out.close();
					if (requestSocket!=null)
						requestSocket.close();
				}
				catch(IOException ioException){
					ioException.printStackTrace();
				}
			}
			countDownLatch.countDown();  
		}
	}

	private static class UploadHandler extends Thread {
		private String message;    //message received from the client
		private String MESSAGE;    //uppercase message send to the client
		private Socket connection;
		private ObjectInputStream in;	//stream read from the socket
		private ObjectOutputStream out;    //stream write to the socket
		private int no;		//The index number of the client
		private Boolean finish = false;   //finish signal
		private String filename;		//path of partial files
		private String prefix="U";		
		
		
		public UploadHandler(Socket connection, int no, String filename) {
			this.connection = connection;
			this.no = no;
			this.filename = filename;
		}

		public void run() {
			String incontent[];
			Uts.printinfo(prefix, "Client "+ UploadPeerId +" is connected!");
			try{
				//initialize Input and Output streams
				out = new ObjectOutputStream(connection.getOutputStream());
				out.flush();
				in = new ObjectInputStream(connection.getInputStream());
				
				try{
					while(finish==false)
					{
						//receive the message sent from the client
						message = (String)in.readObject();
						incontent = message.split(" ", 2);
						
						switch (incontent[0].charAt(0)){
						//Client ask for file list
						case 't':
							Uts.printinfo(prefix, "Client ask for file table");
							//send file list
							MESSAGE = "r "+ FileTabletoString();
							Uts.sendMessage(out,MESSAGE);
							Uts.printinfo(prefix, "send file list: "+MESSAGE.split(" ",2)[1]);
							break;
							
						//Client ask for file
						case 'a':
							int fileId = Integer.valueOf(incontent[1]);
							Uts.printinfo(prefix, "client ask for file-"+fileId);
							//send "file delivery" signal
							MESSAGE =  "d "+Integer.toString(fileId);
							Uts.sendMessage(out,MESSAGE);
							Uts.printinfo(prefix, "ready to send file "+MESSAGE.split(" ",2)[1]);
							Uts.sendFile(out,filename+"_"+fileId+".tmp");
							break;
							
						//receive "finish" signal
						case 'f':
							finish=true;
							break;
							
						default:
							break;
						}
					}
				}
				catch(ClassNotFoundException classnot){
					System.err.println(prefix+"Data received in unknown format");
				}
			}
			catch(IOException ioException){
				System.out.println(prefix+"Disconnect with Client " + no);
			}
			finally{
				//Close connections
				try{
					in.close();
					out.close();
					connection.close();
				}
				catch(IOException ioException){
					System.out.println(prefix+"Disconnect with Client " + no);
				}
			}
		}
		
		
		public String FileTabletoString(){
			String filetable = new String();
			for (int i = 0; i < filenum; i++){
				if (localfiletable[i]){
					filetable += String.valueOf(i);
					if (i != filenum - 1)
						filetable +=',';
				}
			}
			return filetable;
		}
	}
	
	public static void main(String args[]) throws IOException {
		client client = new client();
		CountDownLatch countDownLatch = new CountDownLatch(1);
/*		for(int i =0;i<filenum;i++){
			
			if (i%globalpeernum == id-1){
				localfiletable[i]=true;
			}
			else{
				localfiletable[i]=false;
			}
			
			localfiletable[i]=false;
		}
		*/
		//download initial files from Server
		client.DownloadFromServer();
		ServerSocket listener = new ServerSocket(sPort);
		try {
			//Thread: download files from other peer
			new DownloadHandler(countDownLatch,DownloadPeerPort, fp).start();
			//Thread: upload files to other peer
			new UploadHandler(listener.accept(), UploadPeerId, pf).start();
            
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
        } finally {
           listener.close();
        	//wait for threads to end
            try  
           {  
        	   //wait for threads to end
               countDownLatch.await();  
           }  
           catch (InterruptedException e)  
           {  
               e.printStackTrace();  
           }  
        } 
		try {
			//merge partial files
			Uts.merge(of, pf);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
}
