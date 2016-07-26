package test;

import java.io.UnsupportedEncodingException;

public class Hex {

	public static void main(String[] args) throws UnsupportedEncodingException {
		System.out.println(hexEncode(new String("呵呵")));
	}

	public static String hexEncode(String str) throws UnsupportedEncodingException {
		// 根据默认编码获取字节数组
		byte[] bytes = str.getBytes("utf-8");
		StringBuilder sb = new StringBuilder(bytes.length * 2);
		// 将字节数组中每个字节拆解成2位16进制整数
		for (int i = 0; i < bytes.length; i++) {
			sb.append(Integer.toHexString((bytes[i] & 0xf0) >> 4));
			sb.append(Integer.toHexString((bytes[i] & 0x0f) >> 0));
			sb.append("-");
		}
		return sb.toString();
	}
}
