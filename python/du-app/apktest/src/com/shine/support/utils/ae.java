package com.shine.support.utils;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class ae {
	private static MessageDigest a;
	private static StringBuilder b;

	static {
		try {
			a = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		b = new StringBuilder();
	}

	public static String a(String paramString) {
		a.reset();
		a.update(paramString.getBytes());
		byte[] arrayOfByte = a.digest();
		// paramString = b;
		int i = 0;
		b.setLength(0);
		while (i < arrayOfByte.length) {
			int j = arrayOfByte[i] & 0xFF;
			if (j < 16) {
				b.append('0');
			}
			b.append(Integer.toHexString(j));
			i++;
		}
		return b.toString();
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println(ae.a("123"));
	}

}
