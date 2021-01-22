package afg_authentification;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import java.sql.*;
import java.security.SecureRandom;
import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;

/**
 * Servlet implementation class afg_authentification
 */
@WebServlet("/afg_authentification")
public class afg_authentification extends HttpServlet {
	static final String JDBC_DRIVER="com.mysql.jdbc.Driver";//"com.mysql.cj.jdbc.Driver";
	static final String DB_URL="jdbc:mysql://localhost:3306/afg_mg?useSSL=false&allowPublicKeyRetrieval=true&serverTimezone=UTC";//"jdbc:mysql://localhost:3306/afg_mg";
	static final String user="root";
	static final String pas="null";//"viter314159";
		
	static final String key="autofuture&afgER";
	static final String iv="123456qwaszx0000";
	
	public static String bytesToHexString(byte[] bArray) {
	    if (bArray != null) {
	      StringBuilder sb = new StringBuilder(bArray.length);
	      String sTemp;
	      for (byte b : bArray) {
	        sTemp = Integer.toHexString(0xFF & b);
	        if (sTemp.length() < 2)
	          sb.append(0);
	        sb.append(sTemp.toLowerCase());
	        
	      }
	      return sb.toString();
	    }
	    return "";
	  }
	/**
	   * 把16进制字符串转换成字节数组
	   * @param hex : hexString
	   * @return : byte[]
	   */
	  public static byte[] hexStringToBytes(String hex) {
	    int len = (hex.length() / 2);
	    byte[] result = new byte[len];
	    char[] achar = hex.toCharArray();
	    for (int i = 0; i < len; i++) {
	      int pos = i * 2;
	      result[i] = (byte) (getByte(achar[pos]) << 4 | getByte(achar[pos + 1]));
	    }
	    return result;
	  }

	  private static byte getByte(char c) {
	    return (byte) "0123456789abcdef".indexOf(c);
	  }
	
	private static final long serialVersionUID = 1L;

    /**
     * Default constructor. 
     */
    public afg_authentification() {
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see Servlet#init(ServletConfig)
	 */
	public void init(ServletConfig config) throws ServletException {
		// TODO Auto-generated method stub
	}

	/**
	 * @see Servlet#destroy()
	 */
	public void destroy() {
		// TODO Auto-generated method stub
	}

    protected long get_valid_time_from_license(String license)
    {
    	IvParameterSpec ivs = new IvParameterSpec(iv.getBytes());
        SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes(), "AES");
        try
        {
        	 Cipher cipher_d = Cipher.getInstance("AES/CBC/Nopadding");
             cipher_d.init(Cipher.DECRYPT_MODE, skeySpec, ivs);
             byte [] byte_encrypt=cipher_d.doFinal(hexStringToBytes(license));
             if(byte_encrypt.length!=32)
             {
            	 return 0;
             }
             int write_pos=byte_encrypt[4];
             if( byte_encrypt[ 5 + write_pos ] != 'a' || byte_encrypt[ 6 + write_pos ] != 'f' || byte_encrypt[ 7 + write_pos ] != 'g' )
             {
                  return 0;
             }
             write_pos+=8;
             
             long valid_time=(byte_encrypt[write_pos+3]<<24|byte_encrypt[write_pos+2]<<16|byte_encrypt[write_pos+1]<<8|byte_encrypt[write_pos])&0xffffffffl;
             return valid_time;
        }
        catch(Exception ex)
        {
        	ex.printStackTrace();
        }
		return 0;	
    }
    protected String get_encrp_validtime_mac(long time_point,long valid_time,String mac)
    {
    	IvParameterSpec ivs = new IvParameterSpec(iv.getBytes());
        SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes(), "AES");
        try
        {
        	 Cipher cipher = Cipher.getInstance("AES/CBC/Nopadding"); 
             cipher.init(Cipher.ENCRYPT_MODE, skeySpec, ivs);
             byte[] bt_plain=new byte[32];
             bt_plain[0]=(byte) (time_point&0xff);
             bt_plain[1]=(byte) (time_point>>8&0xff);   
             bt_plain[2]=(byte) (time_point>>16&0xff);
             bt_plain[3]=(byte) (time_point>>24&0xff); 
             bt_plain[4]=(byte) (valid_time&0xff);
             bt_plain[5]=(byte) (valid_time>>8&0xff);   
             bt_plain[6]=(byte) (valid_time>>16&0xff);
             bt_plain[7]=(byte) (valid_time>>24&0xff); 
             byte[] bt_mac=hexStringToBytes(mac);//应该返回8个字节
             bt_plain[8]=bt_mac[0];
             bt_plain[9]=bt_mac[1];
             bt_plain[10]=bt_mac[2];
             bt_plain[11]=bt_mac[3];
             bt_plain[12]=bt_mac[4];
             bt_plain[13]=bt_mac[5];
             bt_plain[14]=bt_mac[6];
             bt_plain[15]=bt_mac[7];
             byte [] byte_encrypt=cipher.doFinal(bt_plain);
             String encrp=bytesToHexString(byte_encrypt);
             return encrp;
        }
        catch(Exception ex)
        {
        	ex.printStackTrace();
        }
    	return "";
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		BufferedReader streamReader = new BufferedReader( new InputStreamReader(request.getInputStream(), "UTF-8"));
        StringBuilder responseStrBuilder = new StringBuilder();
		PrintWriter out=response.getWriter();
        String inputStr;
        while ((inputStr = streamReader.readLine()) != null)
            responseStrBuilder.append(inputStr);
        
        /*if(true)
        {
        	out.print("invalid license\n");
        	
			out.flush();
			out.close();
			return;
        }*/
        
		Gson gson = new Gson();
		AFG_AU_DATA afg_d;
		try {
			afg_d= gson.fromJson(responseStrBuilder.toString(), AFG_AU_DATA.class);
		}
		catch(JsonSyntaxException e)
		{
			out.print(e.toString());
			out.flush();
			out.close();
			return;
		}

		if(afg_d.license.isEmpty())
		{
			out.print("invalid license\n");
			out.flush();
			out.close();
			return;
		}
		long valid_time=0;//get_valid_time_from_license(afg_d.license);
		IvParameterSpec ivs = new IvParameterSpec(iv.getBytes());
        SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes(), "AES");
        try
        {
        	 Cipher cipher_d = Cipher.getInstance("AES/CBC/Nopadding");
             cipher_d.init(Cipher.DECRYPT_MODE, skeySpec, ivs);
             byte [] byte_encrypt=cipher_d.doFinal(hexStringToBytes(afg_d.license));
             if(byte_encrypt.length!=32)
             {
            	 valid_time=0;
            	 out.println("en_crp_length:"+byte_encrypt.length);
            	 throw new Exception();
             }
             int write_pos=byte_encrypt[4];
             out.println("write_pos:"+write_pos);
             if( byte_encrypt[ 5 + write_pos ] != 'a' || byte_encrypt[ 6 + write_pos ] != 'f' || byte_encrypt[ 7 + write_pos ] != 'g' )
             {
            	 valid_time=0;
            	 out.println("invalid tag");
            	 throw new Exception();
             }
             write_pos+=8;
             out.println("bytes3~0:");
             out.println(byte_encrypt[write_pos+3] & 0xff);
             out.println(byte_encrypt[write_pos+2] & 0xff); 
             out.println(byte_encrypt[write_pos+1] & 0xff);
             out.println(byte_encrypt[write_pos] & 0xff); 
             valid_time=((byte_encrypt[write_pos+3]& 0xff)<<24 |(byte_encrypt[write_pos+2]&0xff)<<16|(byte_encrypt[write_pos+1]&0xff)<<8|(byte_encrypt[write_pos]&0xff))&0xffffffffl;
             
 
        }
        catch(Exception ex)
        {
        	ex.printStackTrace();
        }
		long cur_time_point=System.currentTimeMillis();
		Connection cnn=null;
		Statement stmt=null;
		
		try {
			 Class.forName(JDBC_DRIVER);
			 cnn=DriverManager.getConnection(DB_URL, user, pas);
			 stmt=cnn.createStatement();
			 String qstr="SELECT * FROM afg_user_info WHERE user_license=\'"+afg_d.license+"\'";
			 ResultSet rs=stmt.executeQuery(qstr);
			 String mac_address=null;
			 while(rs.next())
			 {
				 mac_address=rs.getString("mac_address");
			 }
			 if(mac_address!=null)//该license 已经存在
			 { 
				 if(afg_d.mac_adrress==mac_address.toString())//表明该机器已经登录过，只不过相应注册信息抹去
				 {
					 var eclapse_time=cur_time_point-rs.getLong("time_point");
					 valid_time-=eclapse_time;
					 String update_str="UPDATE afg_user_info SET time_point="+cur_time_point+",valid_time="+valid_time;
					 stmt.executeUpdate (update_str);
				 }
				 else
				 if(""==mac_address.toString())//表明该license已经被某机器注销，可接收新机器注册
				 {
					 valid_time=rs.getLong("valid_time");
					 String update_str="UPDATE afg_user_info SET mac_address=\'"+afg_d.mac_adrress+"\',time_point="+cur_time_point;
					 stmt.executeUpdate (update_str);
				 }
				 else//表明试图从另外一台机器登录已经注册的license，必须上一台机器注销了该license才能获取
				 {
					 valid_time=0;
					 afg_d.mac_adrress=rs.getString("mac_address");
				 }
			 }
			 else//首次接收该license
			 {
				 
				 
				 String istr="INSERT INTO afg_user_info ( user_license,mac_address,time_point, valid_time) VALUES(\'"+afg_d.license+"\',\'"+afg_d.mac_adrress+"\',"+cur_time_point+","+valid_time+")";
				 stmt.executeUpdate (istr);
			 }
			 
			 rs.close();
			 stmt.close();
			 cnn.close();
		}catch(SQLException se){
            // 处理 JDBC 错误
            se.printStackTrace();
        }catch(Exception e){
            // 处理 Class.forName 错误
            e.printStackTrace();
        }finally{
            // 关闭资源
            try{
                if(stmt!=null) stmt.close();
            }catch(SQLException se2){
            }// 什么都不做
            try{
                if(cnn!=null) cnn.close();
            }catch(SQLException se){
                se.printStackTrace();
            }
        }
//		MM mm = gson.fromJson(json, MM.class);
		response.setContentType("text/html");
		out.print("before encrp\n");
		String str_rp="";//=get_encrp_validtime_mac(cur_time_point,valid_time,afg_d.mac_adrress);
		IvParameterSpec ivs_en = new IvParameterSpec(iv.getBytes());
        SecretKeySpec skeySpec_en = new SecretKeySpec(key.getBytes(), "AES");
        try
        {
        	 Cipher cipher = Cipher.getInstance("AES/CBC/Nopadding"); 
             cipher.init(Cipher.ENCRYPT_MODE, skeySpec_en, ivs_en);
             byte[] bt_plain=new byte[32];
             bt_plain[0]=(byte) (cur_time_point&0xff);
             bt_plain[1]=(byte) (cur_time_point>>8&0xff);   
             bt_plain[2]=(byte) (cur_time_point>>16&0xff);
             bt_plain[3]=(byte) (cur_time_point>>24&0xff);
             out.println("cur_time"+cur_time_point);
             bt_plain[4]=(byte) (valid_time&0xff);
             bt_plain[5]=(byte) (valid_time>>8&0xff);   
             bt_plain[6]=(byte) (valid_time>>16&0xff);
             bt_plain[7]=(byte) (valid_time>>24&0xff); 
             out.println("valid_time"+valid_time);
             byte[] bt_mac=hexStringToBytes(afg_d.mac_adrress);//应该返回8个字节
             bt_plain[8]=bt_mac[0];
             bt_plain[9]=bt_mac[1];
             bt_plain[10]=bt_mac[2];
             bt_plain[11]=bt_mac[3];
             bt_plain[12]=bt_mac[4];
             bt_plain[13]=bt_mac[5];
             byte [] byte_encrypt=cipher.doFinal(bt_plain);
             str_rp =bytesToHexString(byte_encrypt);
        }
        catch(Exception ex)
        {
        	ex.printStackTrace();
        }
		
		out.print(str_rp);
		//out.println("<h1>hello world! this is afg authentication.</h1>");
		//out.append("Served at: ").append(request.getContextPath());
		out.flush();
		out.close();
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
		doGet(request, response);
	}

}
