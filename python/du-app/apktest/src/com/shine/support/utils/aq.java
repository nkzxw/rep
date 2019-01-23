package com.shine.support.utils;

import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

// GET
// https://m.poizon.com/search/list?size=[]&title=&typeId=0&catId=0&unionId=0&sortType=0&sortMode=1&page=5&limit=20&sign=5471f32197aead84ef43fc16f8416776
// HTTP/1.1

// size=[]&title=&typeId=0&catId=0&unionId=0&sortType=0&sortMode=1&page=5&limit=20
// sign=5471f32197aead84ef43fc16f8416776

// duuuid: 83543a9eba78150
// duimei: 862490030196133
// duplatform: android
// duchannel:
// duv: 3.5.1
// duloginToken: c134860a|31644780|d3dc471a7d1e195b
// dudeviceTrait: 1605-A01
// User-Agent: duapp/3.5.1(android;6.0.1)
// Host: m.poizon.com
// Connection: Keep-Alive
// Accept-Encoding: gzip
// Cookie: duToken=a2777e5f%7C31644780%7C1545323500%7Cdde22323364ba71e
//
public class aq {

	// private static final String a = "aq";
	public static String sign(Map<String, String> paramMap) {
		// if (paramMap == null) {
		// return "";
		// }
		paramMap.put("uuid", "83543a9eba78150");
		paramMap.put("platform", "android");
		paramMap.put("v", "3.5.1");
		List<Map.Entry<String, String>> localArrayList = new ArrayList<Map.Entry<String, String>>(
				paramMap.entrySet());
		Collections.sort(localArrayList,
				new Comparator<Map.Entry<String, String>>() {
					public int compare(Map.Entry<String, String> o1,
							Map.Entry<String, String> o2) {
						return (o1.getKey()).compareTo(o2.getKey());
					}
				});
		StringBuilder localStringBuilder1 = new StringBuilder();
		for (int i = 0; i < localArrayList.size(); i++) {
			Entry<String, String> param = (Entry<String, String>) localArrayList
					.get(i);
			// String str = a;
			// StringBuilder localStringBuilder2 = new StringBuilder();
			// localStringBuilder2.append((String) param.getKey());
			// localStringBuilder2.append(" : ");
			// localStringBuilder2.append((String) param.getValue());
			// localStringBuilder2.append("\n");
			// System.out.println(localStringBuilder2.toString());
			// af.e(str, localStringBuilder2.toString());
			StringBuilder localStringBuilder2 = new StringBuilder();
			localStringBuilder2.append((String) param.getKey());
			localStringBuilder2.append((String) param.getValue());
			localStringBuilder1.append(localStringBuilder2.toString());
		}
		// System.out.println(localStringBuilder1.toString());
		localStringBuilder1.append("3542e676b4c80983f6131cdfe577ac9b");
		return ae.a(localStringBuilder1.toString());
	}

	public String search(String Token, int page, int limit) {
		Map<String, String> map = new HashMap<String, String>();
		map.put("size", "[]");
		map.put("title", "");
		map.put("typeId", "0");
		map.put("catId", "0");
		map.put("unionId", "0");
		map.put("sortType", "0");
		map.put("sortMode", "1");
		map.put("page", "" + page);
		map.put("limit", "" + limit);
		map.put("loginToken", Token);
		return aq.sign(map);
	}

	public String detail(String Token, int productId) {
		Map<String, String> map = new HashMap<String, String>();
		map.put("productId", "" + productId);
		map.put("isChest", "1");
		map.put("loginToken", Token);
		return aq.sign(map);
	}

	// userName=13161267107&password=0bd64a3ecfe4047ac5f326f33e9023e0&type=pwd&sourcePage=&countryCode=86&
	// sign=e57d8bff09bea5a4e23cb004cc077afa
	public String Login(String paramString1, String paramString2) {
		Map<String, String> map = new HashMap<String, String>();
		// String paramString2 = password(p2);
		map.put("userName", paramString1);
		map.put("password", paramString2);
		map.put("type", "pwd");
		map.put("countryCode", String.valueOf(86));
		map.put("sourcePage", "");
		map.put("loginToken", "");
		return aq.sign(map);
	}

	static char[] magic = new char[16];
	static {
		magic[0] = 48;
		magic[1] = 49;
		magic[2] = 50;
		magic[3] = 51;
		magic[4] = 52;
		magic[5] = 53;
		magic[6] = 54;
		magic[7] = 55;
		magic[8] = 56;
		magic[9] = 57;
		magic[10] = 97;
		magic[11] = 98;
		magic[12] = 99;
		magic[13] = 100;
		magic[14] = 101;
		magic[15] = 102;
	}

	public String password(String param) {
		try {
			param += "du";
			byte[] md5 = param.getBytes();
			MessageDigest dig = MessageDigest.getInstance("MD5");
			dig.update(md5);
			byte[] md5byte = dig.digest();
			char[] paramString0 = new char[32];
			int i = 0;
			int j = 0;
			while (i < 16) {
				int k = md5byte[i];
				int m = j + 1;
				paramString0[j] = (char) (magic[(k >>> 4 & 0xF)]);
				j = m + 1;
				paramString0[m] = (char) (magic[(k & 0xF)]);
				i++;
			}
			return new String(paramString0).toLowerCase();
		} catch (Exception paramString) {
		}
		return null;
	}

	public static void main(String[] args) {
		aq maq = new aq();
		String token = "c134860a|31644780|d3dc471a7d1e195b";
		System.out.println(maq.password("qqbwstor"));
		// size=[]&title=&typeId=0&catId=0&unionId=0&sortType=0&sortMode=1&page=5&limit=20
		// sign=5471f32197aead84ef43fc16f8416776
		// GET
		System.out.println(maq.search(token, 5, 20));
		// /product/detail?productId=9754&isChest=1&sign=fea873c81d4650a2e68a0babd2ccfb72
		// productId=9754&isChest=1
		// sign=fea873c81d4650a2e68a0babd2ccfb72
		System.out.println(maq.detail(token, 9754));

		// GET
		// https://m.poizon.com/product/detail?productId=9670&isChest=1
		//&sign=573440b9ca20dfb84e653f40f1ecb46a
		// HTTP/1.1
		System.out.println(maq.detail(token, 9670));
		// userName=13161267107&password=0bd64a3ecfe4047ac5f326f33e9023e0&type=pwd&sourcePage=&countryCode=86
		// sign=e57d8bff09bea5a4e23cb004cc077afa
		System.out.println(maq.Login("13161267107", maq.password("qqbwstor")));

	}
	//
	// public static String b(Map<String, String> paramMap) {
	// if (paramMap == null) {
	// return "";
	// }
	// // paramMap.put("uuid", r.a(DuApplication.b()).a(null));
	// paramMap.put("platform", "android");
	// // paramMap.put("v", r.c(DuApplication.b()));
	// paramMap.put("loginToken", h.a().b());
	// paramMap = new ArrayList(paramMap.entrySet());
	// Collections.sort(paramMap, new Comparator() {
	// public int a(Map.Entry<String, String> paramAnonymousEntry1,
	// Map.Entry<String, String> paramAnonymousEntry2) {
	// return ((String) paramAnonymousEntry1.getKey()).toString()
	// .compareTo((String) paramAnonymousEntry2.getKey());
	// }
	// });
	// StringBuilder localStringBuilder1 = new StringBuilder();
	// for (int i = 0; i < paramMap.size(); i++) {
	// Map.Entry localEntry = (Map.Entry) paramMap.get(i);
	// Object localObject = a;
	// StringBuilder localStringBuilder2 = new StringBuilder();
	// localStringBuilder2.append((String) localEntry.getKey());
	// localStringBuilder2.append(" : ");
	// localStringBuilder2.append((String) localEntry.getValue());
	// localStringBuilder2.append("\n");
	// af.e((String) localObject, localStringBuilder2.toString());
	// localObject = new StringBuilder();
	// ((StringBuilder) localObject).append((String) localEntry.getKey());
	// ((StringBuilder) localObject)
	// .append((String) localEntry.getValue());
	// localStringBuilder1
	// .append(((StringBuilder) localObject).toString());
	// }
	// return ae.a(localStringBuilder1.toString());
	// }

}
