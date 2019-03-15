import java.io.*;
import java.util.Map.Entry;
import java.util.*;

//排序
class EntryComparator implements Comparator<Entry<String, Integer>> {
	public int compare(Entry<String, Integer> o1, Entry<String, Integer> o2) {
		if(o2.getValue() == o1.getValue()) {
			return o1.getKey().compareTo(o2.getKey());
		}
		else return (o2.getValue() - o1.getValue());
	}
}

public class Main {
	
	private static ArrayList<String> wordStrings =new ArrayList<String>();
	private static int count = 0;
	private static int lines = 0;
	private static int words = 0;
	
	//获取字符数
	private static void getCharacter(String filename)
	{
		int ls=0;
		String chhString="";
        try {
        	BufferedInputStream bis = new BufferedInputStream(new FileInputStream(new File(filename)));
        	BufferedReader in = new BufferedReader(new InputStreamReader(bis, "utf-8"), 20* 1024* 1024  );
        	while (in.ready()) {
        		chhString = in.readLine();
        		ls++; 	        
        		if(chhString.indexOf("Abstract:") == 0||chhString.indexOf("Title:") == 0) {
            		chhString = chhString.replaceAll("[^\\u0000-\\u007f]", "");
            		count +=chhString.length();
            		lines++;           		
				}       		
        	}
        	ls = ls/5*2;
        	count += ls;
        	in.close();
        } catch (IOException ex) {
        	ex.printStackTrace();
        }
        count -= lines/2*17;
    }
	
	private static boolean isWord(String s) {
		char[] temp = s.toCharArray();
		if(temp.length>3) 
			if(temp[0]>=97 && temp[0]<=122 && temp[1]>=97 && temp[1]<=122 &&temp[2]>=97 && temp[2]<=122 && temp[3]>=97 && temp[3]<=122)
				return true;
		    else return false;
		else return false;
	}
	
	//获取单词数
	private static void getWords(String filename )throws IOException {
		try {
			FileReader fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while((line = br.readLine()) != null) {
				line = line.replace("[^\\u0000-\\u007f]", "");
				line = line.toLowerCase();
				String[] strings = line.split("[^a-z0-9]");
				for(int i=1;i<strings.length;i++) {
					if(isWord(strings[i])) words++;		
				}		
			}
			br.close();
	        fr.close();	
		}catch (Exception e) {
			e.printStackTrace();
		}
	}

	//输出前n的单词及个数
	private static void getMostWord(String filename,boolean w,int times)throws IOException {
		int t=1;
		if(w) t=10;
		int a=t;
		ArrayList<String> text = new ArrayList<String>();
		try {
			FileReader fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while((line = br.readLine()) != null) {
				line = line.toLowerCase();
				line = line.replace("[^\\u0000-\\u007f]", "");
				if(line.indexOf("title:")==0) {
					String[] strings = line.split("[^a-z0-9]");
				    for(int nu=1;nu<strings.length;nu++) {
				    	if(isWord(strings[nu])) {
				    		while((t)>0) {
			        			t--;
			        			text.add(strings[nu]);
			        		}
				    		t = a;
				    	}
				    }
				}
				if(line.indexOf("abstract:")==0) {
					String[] strings = line.split("[^a-z0-9]");
				    for(int nu=1;nu<strings.length;nu++) {
				    	if(isWord(strings[nu])) text.add(strings[nu]);
				    }
				}        
			}
			br.close();
	        fr.close();
		}catch (Exception e) {
			e.printStackTrace();
		}
		
		
        Map<String, Integer> map = new HashMap<String, Integer>();
        for(String st : text) {
        	if(map.containsKey(st)) {
        		map.put(st, map.get(st)+1);
        	}else {
        		map.put(st, 1);
        	}
        }
        
        List<Map.Entry<String, Integer>> list = new ArrayList<Map.Entry<String,Integer>>();
        for(Entry<String, Integer> entry : map.entrySet()) {
        	list.add(entry);
        }
        Collections.sort(list,new EntryComparator());
        int ii = 0;
        String ssString;
        for(Entry<String, Integer> obj : list) {
     	   if(ii>(times-1)) break;
     	   ssString="<"+obj.getKey()+">: " + obj.getValue()+"\r\n";
     	   wordStrings.add(ssString);
     	   ++ii;
        }
	}
	
	private static void writers(String c,String w,String l,ArrayList<String>ws,String path) {
		try {
			File file1 =new File(path);
     		Writer out =new FileWriter(file1);
		    out.write(c);
	    	out.write(w);
	    	out.write(l);
	    	for(int i=0;i<ws.size();i++)out.write(ws.get(i));
	    	out.close();
		}catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) throws IOException  {
		
		
		//long start = System.currentTimeMillis();//要测试的程序或方法
		String ifile = "";
		String ofile = "";
		String w;
		boolean b = false;
		int times = 10;
		
		for(int ar=0;ar<args.length;ar=ar+2) {
			if("-i".equals(args[ar])) ifile = args[ar+1];
			if("-o".equals(args[ar])) ofile = args[ar+1];
			if("-w".equals(args[ar])) {
				w = args[ar+1];
				if(w.equals("1")) {
					b = true;
				}
				else if(w.equals("0")) {
					b = false;
				}
			}
			if("-n".equals(args[ar])) {

				times = Integer.valueOf(args[ar+1]).intValue();
			}
			/*if("-m".equals(args[ar])) {
				nl = Integer.valueOf(args[ar+1]).intValue();
			}*/
		}
		
		getCharacter(ifile);
		getWords(ifile);
		getMostWord(ifile, b, times);
		String c,ws,l;
		c = "characters: "+count+"\r\n";
		ws = "words: "+words+"\r\n";
		l = "lines: "+lines+"\r\n";
		writers(c, ws, l, wordStrings, ofile);
		//long end = System.currentTimeMillis();
		//System.out.println("程序运行时间："+(end-start)+"ms");
		
			
	}

}
