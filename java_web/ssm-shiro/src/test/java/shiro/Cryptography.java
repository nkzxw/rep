package shiro;

import org.apache.shiro.crypto.hash.SimpleHash;
import org.junit.Test;

public class Cryptography {
	@Test
	public void Test(){
//		String pwd="password";
//		String salt="ss";
//		String base64Encoded=Base64.encodeToString(pwd.getBytes());
//		System.out.println(base64Encoded);
//		String hexEncoded=Hex.encodeToString(pwd.getBytes());
//		System.out.println(hexEncoded);
//		String md5=new Md5Hash(pwd,salt).toString();
//		System.out.println(md5);
//		String d=new Md5Hash(md5).toString();
//		System.out.println(d);
		//5f4dcc3b5aa765d61d8327deb882cf99
		
  
		String algorithmName = "md5";  
//		SimpleHash a=new SimpleHash(algorithmName, "123");
//		System.out.println(a);
//		a=new SimpleHash(algorithmName, "123", "1",2);
//		System.out.println(a);
//		String password = "123";  
//		String salt = new SecureRandomNumberGenerator().nextBytes().toHex();  //随机值
		int hashIterations = 2;  
		  
		SimpleHash hash = new SimpleHash(algorithmName, "123","aa", hashIterations);  
		String encodedPassword = hash.toHex();   
		System.out.println(encodedPassword);
	}
}
