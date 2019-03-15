package ≈¿≥Ê1;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

import org.jsoup.Connection;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

public class Main {
	public static void main(String []args)
    {
   	 
   	    String url1="http://openaccess.thecvf.com/CVPR2018.py";
      	Document document1 = null,document2 = null;   	
		try 
		{
			File file1 =new File("result.txt");
    		Writer out =new FileWriter(file1);
    		Connection connection = Jsoup.connect(url1);
    		connection.maxBodySize(0);
			document1 = connection.get();
			Elements x = document1.getElementsByClass("ptitle");
			//System.out.print(x.size());
			for(int i=0;i<x.size();i++)
			{
				//System.out.print(i+1+" ");
				//System.out.print("Title: "+x.get(i).text()+" ");
				String n = i+"\r\n";
				String t="Title: "+x.get(i).text()+"\r\n";
				Elements links = document1.select("dt a");
				String url2=links.get(i).attr("href");
				url2="http://openaccess.thecvf.com/"+url2;
				document2 = Jsoup.connect(url2).get();
				Element y= document2.getElementById("abstract");
				//System.out.println("Abstract:"+y.text()+"\n\n");
				String a="Abstract: "+y.text()+"\r\n\r\n\r\n";
				
	     		out.write(n);
			    out.write(t);
		    	out.write(a);		    	
			}
			out.close();
		} 
		catch (IOException e) 
		{
			System.out.println("≈¿»° ß∞‹");
		}  
    }
}
