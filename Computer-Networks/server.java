
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.RandomAccessFile;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;





public class server {
	private static int sPort;
	private static int clientnum;
	private static int filenum;
	private static String fname,
						  sourcefile = "Original\\",
						  partialfile = "Partial\\";
	private static ArrayList clients;
	
	public static class Client{
		public int id;
		public int port;
		public int uppeer, downpeer;
		
		public Client(int id, int port, int uppeer, int downpeer){
			this.id = id;
			this.port = port;
			this.uppeer = uppeer;
			this.downpeer = downpeer;
		}
		public int getid(){
			return id;
		}
		public int getport(){
			return port;
		}
		public int getup(){
			return uppeer;
		}
		public int getdown(){
			return downpeer;
		}
	};
		
	public server(){}
	
	private static class Uts{
		public static String FileTabletoString(int filenum, int clientid){
			
			int peerfilenum = (int) Math.ceil((float)(filenum-clientid+1)/clientnum);
			int[] fileId;
			String filetable = new String();
			fileId = new int[peerfilenum];
			for (int i = 0; i < peerfilenum; i++){
				fileId[i] = i*clientnum+ clientid-1;
				filetable += Integer.toString(fileId[i]);
				if (i != peerfilenum - 1)
					filetable +=',';
			}
			return filetable;
		}
		
		public static String settingstoString(int id){
			String ret = new String();
			Client c = (Client)clients.get(id-1);
			ret = c.getid()+" "+c.getport()+" "+
				  clientnum+" "+c.getup()+" "+
				  c.getdown()+" "+((Client)clients.get(c.getdown()-1)).getport()+" "+
				  filenum+" "+fname;
			return ret;
		}
		
		public static void sendFile(ObjectOutputStream out, String filename){
			FileInputStream fis = null;
			long prog=0,orlength;
			try{
				File file = new File(filename);
	            fis = new FileInputStream(file);
	            //send filename and length
	            out.writeObject(file.getName());
	            out.flush();
	            orlength = file.length();
	            out.writeLong(orlength);
	            out.flush();
	            //send file
	            byte[] sendBytes = new byte[1024];
	            int length = 0;
	            System.out.println("<<<begin sending file: "+file.getName());
	            while(prog<orlength){
	            	length = fis.read(sendBytes, 0, sendBytes.length);
	            	prog+=length;
	                out.write(sendBytes, 0, length);
//	                System.out.print(">");
	                out.flush();
	            }
//	            System.out.println(".");
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

		//send a message to the output stream
		public static void sendMessage(ObjectOutputStream out,String msg)
		{
			try{
				out.writeObject(msg);
				out.flush();
			}
			catch(IOException ioException){
				ioException.printStackTrace();
			}
		}
		
		
	    /**
	     * file split
	     * @param file: original file
	     * @param tempfile: path & filename for partial files
	     * @param size: maximum size of partial file(in KB) (100KB default)
	     */
	    public static int split(String file ,String tempfile,long size) throws Exception
	    {       
	        size*=1024;
	        RandomAccessFile access = new RandomAccessFile(new File(file),"r");
	        long length = access.length();
	        int count =  (int) (length / size); 
	        access.close();
	        long offset = 0L;
	        for(int i=0;i< count;i++) 
	        {
	            long fbegin = offset;
	            long fend = (i+1) * size;
	            offset =filewrite(file,tempfile,i,fbegin,fend);
	        }
	 
	        if(length- offset>0) 
	            filewrite(file, tempfile,count,offset,length);
			
			return count+1;
	    }
	    
	    /**
	     * create & write partial file according to assigned offset
	     * @param infile Original file
	     * @param outfile partial file
	     * @param index id of partial file (sequence number)
	     * @param begin pointer for begin of content
	     * @param end pointer for end of content 
	     * @return
	     * @throws Exception
	     */
	    private static long filewrite(String infile, String outfile,int index,long begin,long end) throws Exception
	    {
	        RandomAccessFile input = new RandomAccessFile(new File(infile),"r");
	        RandomAccessFile output = new RandomAccessFile(new File(outfile+"_"+index+".tmp"),"rw");
	        byte[] b = new byte[1024];
	        int n=0;
	        input.seek(begin);
	 
	        while(input.getFilePointer() < end && (n= input.read(b))!=-1)
	        {
	            output.write(b, 0, n);
	        }
	        long endPointer =input.getFilePointer();
	        input.close();
	        output.close();
	        return endPointer;
	    }
	    
	    private static void printinfo(String prefix, String info){
	    	System.out.println("["+prefix+"]: "+info);
	    }
		
		@SuppressWarnings("unchecked")
		private static void readconfig(String filename){
			File file = new File(filename);
			BufferedReader reader = null;
			String buf[];
			int para[] = new int[4];
			Client newclient;
		
			try {
				reader = new BufferedReader(new FileReader(file));
				String content = null;
				int line = 1;
				content = reader.readLine();
				buf=content.split(" ");
				sPort = Integer.valueOf(buf[1]);
				clientnum = Integer.valueOf(buf[2]);
				clients=new ArrayList<Client>(clientnum);
				while ((content = reader.readLine()) != null) {
					buf=content.split(" ");
					for (int i =0;i<4;i++)
						para[i]=Integer.valueOf(buf[i]);
					newclient = new Client(para[0],para[1],para[2],para[3]);
					clients.add(newclient);
				}
				reader.close();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if (reader != null) {
					try {
						reader.close();
					} catch (IOException e1) {
					}
				}
			}
		}

	}
	
	
	/**
 	* A handler thread class.  Handlers are spawned from the listening
 	* loop and are responsible for dealing with a single client's requests.
 	*/
	private static class Handler extends Thread {
		private String message;    //message received from the client
		private String MESSAGE;    //uppercase message send to the client
		private Socket connection;
		private ObjectInputStream in;	//stream read from the socket
		private ObjectOutputStream out;    //stream write to the socket
		private int no;		//The index number of the client
		private String prefix;

		

		public Handler(Socket connection, int no) {
			this.connection = connection;
			this.no = no;
			this.prefix = "Client-"+no;
		}

		public void run() {
			Boolean finish = false;
			int count=0;
			String incontent[];
			try{
				Uts.printinfo(prefix," Connected!");
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
						//Client ask for settings
						case 'i':
							Uts.printinfo(prefix, "ask for Settings");
							
							MESSAGE = "i "+ Uts.settingstoString(no);
							Uts.sendMessage(out,MESSAGE);
							Uts.printinfo(prefix, "Settings: "+MESSAGE.split(" " , 2)[1]);
							break;	
						//Client ask for file list
						case 't':
							Uts.printinfo(prefix, "ask for file list");
							
							MESSAGE = "r "+ Uts.FileTabletoString(filenum, this.no);
							Uts.sendMessage(out,MESSAGE);
							Uts.printinfo(prefix, "file list: "+MESSAGE.split(" " , 2)[1]);
							break;
						//Client ask for file
						case 'a':
							int fileId = Integer.valueOf(incontent[1]);
							Uts.printinfo(prefix, "ask for file-"+fileId);
							MESSAGE =  "d "+Integer.toString(fileId);
							Uts.sendMessage(out,MESSAGE);
							Uts.printinfo(prefix, "ready to send file-"+fileId);
							Uts.sendFile(out,partialfile+"_"+fileId+".tmp");
							break;
						//receive "finish" signal from client
						case 'f':
							finish=true;
							break;
							
						default:
							break;
						}
					}
				}
				catch(ClassNotFoundException classnot){
					System.err.println("Data received in unknown format");
				}
			}
			catch(IOException ioException){
				System.out.println("Disconnect with Client " + no);
			}
			finally{
				//Close connections
				try{
					in.close();
					out.close();
					connection.close();
				}
				catch(IOException ioException){
					System.out.println("Disconnect with Client " + no);
				}
			}
		}		
	}

	
	public static void main(String args[]) throws Exception {
		if (args.length!=0){
			fname = args[0];
		}
		else{
			fname="P2Ptutorial.pdf";
		}
		System.out.println("Distribute file: "+fname);
		System.out.println("The server is running."); 
		sourcefile+=fname;
		partialfile+=fname;
		filenum = Uts.split(sourcefile,partialfile,100);
		Uts.readconfig("config.txt");
        ServerSocket listener = new ServerSocket(sPort);
		int clientNum;
		
        try {
            for (clientNum = 1; clientNum<=clientnum; clientNum++) {
                new Handler(listener.accept(),clientNum).start();
                
            }
        } finally {
            listener.close();
        } 
    }
}
