package BEncoding;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Map.Entry;

//todo：java引用，java泛型，文件与数据流

public class BTEncoding {

	/**
	 * @param args
	 * @throws IOException
	 */
	public static int process(InputStream is, OutputStream os) {
		// Properties props = System.getProperties();
		// System.out.println(new Date().toString() + "\nfile.encodeing= "
		// + props.get("file.encoding"));
		// System.out.println("Charset.defaultCharset= "
		// + Charset.defaultCharset());

		BTparser parser = new BTparser(is);
		Type root;
		try {
			root = parser.parse();
			if (root == null)
				return 1;
			new BTReplacer().replaceAType(root);
			root.Write(os);
			return 0;
		} catch (IOException e) {
			e.printStackTrace();
			return -1;
		}
	}

	/**
	 * @param args
	 * @throws IOException
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		// ByteArrayInputStream is = new ByteArrayInputStream(
		// "ld5:Helloi5e5:World7:Testing4:Fivei4eeee".getBytes());
		// long startMili = System.currentTimeMillis();// 当前时间对应的毫秒数
		long startMili = System.nanoTime();

		// 执行一段代码,求一百万次随机值
		// for(int i=0;i<1000000;i++){
		// Math.random();
		// }
		// Properties props = System.getProperties();
		// System.out.println("file encodeing= "+props.get("file.encoding"));
		// System.out.println(Charset.defaultCharset());
		File from = new File("77.torrent");
		File to = new File("ddXRiOzWMc22.torrent");
		FileInputStream fis;
		FileOutputStream os;
		try {
			fis = new FileInputStream(from);
			os = new FileOutputStream(to);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return;
		}
		process(fis, os);
		long endMili = System.nanoTime();

		System.out.println("总耗时为：" + (endMili - startMili) / 1000 + " us");
	}

}

abstract class Replacer {
	public abstract byte[] replaceAValue(byte[] value);
}

interface Type {
	public int Parse(InputStream is, byte firstbyte) throws IOException;

	public int Write(OutputStream os) throws IOException;
}

class BTparser {
	private InputStream is;

	public BTparser(InputStream is) {
		this.is = is;
	}

	// 解析一个完整的段
	public Type parse() throws IOException {
		byte b = -1;
		b = (byte) is.read();
		if (b == -1)
			return null;
		Type res;
		if (b == ' ' || b == '\n' || b == '\r' || b == '\t') {
			return parse(); // loop state
		}
		if (b == 'd') {
			res = new BTDic();
		} else if (b == 'l') {
			res = new BtList();
		} else if (b == 'i') {
			res = new BTInt();
		} else if (b >= '0' && b <= '9') {
			res = new BTString();
		} else {
			return null;
		}
		if (res.Parse(is, b) != -1) {
			return res;
		} else {
			return null;
		}
	}
}

class BTReplacer extends Replacer {
	public static final String charset = "abcdefghijklmnopqrstuvwxyz0123456789";// ABCDEFGHIJKLMNOPQRSTUVWXYZ

	// only replace dic
	public void replaceAType(Type type) {
		if (type instanceof BTDic) {
			BTDic dict = (BTDic) type;
			for (Entry<BTString, Type> mapEntry : dict.map.entrySet()) {
				Type pairValue = mapEntry.getValue();
				if (replaceRequired(mapEntry.getKey().value)) {
					if (pairValue instanceof BtList) {
						for (Type listEntry : ((BtList) pairValue).list) {
							((BTString) listEntry).fixDirtyWord(this);
						}
					} else {
						((BTString) pairValue).fixDirtyWord(this);
					}
				} else {
					replaceAType(pairValue);
				}
			}
		} else if (type instanceof BtList) {
			for (Type entry : ((BtList) type).list) {
				replaceAType(entry);
			}
		}
	}

	private String getRandomString(int length) {
		Random random = new Random();
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < length; i++) {
			int number = random.nextInt(BTReplacer.charset.length());
			sb.append(BTReplacer.charset.charAt(number));
		}
		return sb.toString();
	}

	@Override
	public byte[] replaceAValue(byte[] value) {
		String str;
		try {
			str = new String(value, "utf-8");
			// System.out.println("replace:" + str);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return null;
		}
		// System.out.println(str);
		String newString = "xi-" + this.getRandomString(4);
		byte[] val = newString.getBytes();
		int indexOfDot = str.lastIndexOf('.');
		if (indexOfDot > 0) {
			// System.out.println("substring = " + str );
			str = str.substring(indexOfDot);
			boolean isWord = str.matches("[a-zA-Z4.]{4,5}");//.mkv,.avi,.rmvb,.mp4
			if (isWord) {
				val = (newString + str).getBytes();
			}
		}
		return val;
	}

	private boolean replaceRequired(byte[] key) {
		String str;
		try {
			str = new String(key, "utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return false;
		}
		// System.out.println(str);
		if ((str.compareToIgnoreCase("comment") == 0)
				|| (str.compareToIgnoreCase("path") == 0)
				|| (str.compareToIgnoreCase("created by") == 0)
				|| (str.compareToIgnoreCase("publisher") == 0)
				|| (str.compareToIgnoreCase("publisher-url") == 0)
				|| (str.compareToIgnoreCase("name") == 0)

				|| (str.compareToIgnoreCase("comment.utf-8") == 0)
				|| (str.compareToIgnoreCase("path.utf-8") == 0)
				|| (str.compareToIgnoreCase("created by.utf-8") == 0)
				|| (str.compareToIgnoreCase("publisher.utf-8") == 0)
				|| (str.compareToIgnoreCase("publisher-url.utf-8") == 0)
				|| (str.compareToIgnoreCase("name.utf-8") == 0)) {
			return true;
		}
		return false;
	}
}

// d<<key><value><key><value>...<key><value>>e
//
class BTDic implements Type {
	SortedMap<BTString, Type> map;

	@Override
	public int Write(OutputStream os) throws IOException {
		os.write('d');
		for (Entry<BTString, Type> entry : map.entrySet()) {
			entry.getKey().Write(os);
			// System.out.println(new String(entry.getKey().value) + ","
			// + entry.getValue().getClass().getSimpleName());
			entry.getValue().Write(os);
		}
		os.write('e');
		return 0;
	}

	// 除去开头的'd'剩余字段
	@Override
	public int Parse(InputStream is, byte Skiped) throws IOException {
		// TODO Auto-generated method stub
		this.map = new TreeMap<BTString, Type>(); // 通过子类实例化接口对象
		byte b;
		while ((b = (byte) is.read()) != 'e') {
			// System.out.println("dic Skiped=" + (char) Skiped + " b=" + (char)
			// b);
			if (b == ' ' || b == '\n' || b == '\r' || b == '\t') {
				continue;
			}
			if (b == -1) {
				return -1;
			}
			BTString key = new BTString();
			if (key.Parse(is, b) == -1) {
				return -1;
			}
			Type value;
			if ((value = new BTparser(is).parse()) == null) {
				return -1;
			}
			this.map.put(key, value);
		}
		return 0;
	}
}

// l<子元素>e
// 子元素可以是字符串，整数，列表和字典，或者是它们的组合体
// l4:spam4:eggse 表示 [ "spam", "eggs" ] l3:logi32ee 表示 [ "log", 32 ]
class BtList implements Type {
	public List<Type> list;

	@Override
	public int Write(OutputStream os) throws IOException {
		os.write('l');
		for (Type entry : list) {
			entry.Write(os);
		}
		os.write('e');
		return 0;
	}

	@Override
	public int Parse(InputStream is, byte Skiped) throws IOException {
		// TODO Auto-generated method stub
		this.list = new ArrayList<Type>();
		Type res = null;
		byte b;
		while ((b = (byte) is.read()) != 'e') {
			// System.out.println("list Skiped=" + (char) Skiped);
			// System.out.println("b=" + (char) b);
			if (b == ' ' || b == '\n' || b == '\r' || b == '\t') {
				continue;
			}
			if (b == -1) {
				return -1;
			}
			if (b == 'd') {
				res = new BTDic();
			} else if (b == 'l') {
				res = new BtList();
			} else if (b == 'i') {
				res = new BTInt();
			} else if (b >= '0' && b <= '9') {
				res = new BTString();
			} else {
				return -1;
			}
			if (res.Parse(is, b) != -1) {
				this.list.add(res);
			} else {
				return -1;
			}
		}
		return 0;
	}
}

// i<整数>e, i3e 表示整数3, i1024e 表示整数1024
class BTInt implements Type {
	// private int length;
	private long value;

	@Override
	public int Write(OutputStream os) throws IOException {
		// TODO Auto-generated method stub
		os.write('i');
		os.write(Long.toString(value).getBytes("US-ASCII"));
		os.write('e');
		return 0;
	}

	@Override
	public int Parse(InputStream is, byte Skiped) throws IOException {
		// TODO Auto-generated method stub
		int b;
		boolean negative = false;
		value = 0;
		while ((b = is.read()) >= 0) {
			// System.out.println("int Skiped=" + (char) Skiped);
			// System.out.println("b=" + (char) b);
			int digit = b - '0';
			if (b == '-') {
				negative = true;
			} else if (b == 'e') {
				break;
			} else if ((b >= '0') && (b <= '9')) {
				value = (value * 10) + digit;
			} else {
				return -1;
			}
		}
		if (negative) {
			value *= -1;
		}
		// System.out.println("int=" + value);
		return 0;
	}
}

// <字符串的长度>:<字符串的内容>
// 4:spam 表示spam, 2:ab 表示ab
class BTString implements Type, Comparable<BTString> {
	private long length;
	public byte[] value;

	@Override
	public int Write(OutputStream os) throws IOException {
		os.write(Long.toString(length).getBytes("US-ASCII"));
		os.write(':');
		// System.out.println("string write" + new String(value));
		os.write(value);
		return 0;
	}

	public void fixDirtyWord(Replacer r) {
		int len = 0;
		byte[] val = r.replaceAValue(this.value);
		this.length = len = val.length;
		this.value = new byte[len];
		System.arraycopy(val, 0, this.value, 0, len);
	}

	@Override
	public int Parse(InputStream is, byte Skiped) throws IOException {
		long len = Skiped - '0';
		byte b;
		while ((b = (byte) is.read()) >= 0) {
			if (b == ':') {
				this.length = len;
				value = new byte[(int) len];
				int read = is.read(value, 0, (int) len);
				if (read != len) {
					return -1;
				}
				break;
			} else if (b >= '0' && b <= '9') {
				len = len * 10 + b - '0';
			} else {
				return -1;
			}
		}
		return 0;
	}

	// the value 0 if the argument string is equal to this string; a value less
	// than 0 if this string is lexicographically less than the string argument;
	// and a value greater than 0 if this string is lexicographically greater
	// than the string argument.
	@Override
	public int compareTo(BTString o) {
		// TODO Auto-generated method stub
		// if (o.value[0] == value[0]) {
		// return 0;
		// }
		return o.value[0] > value[0] ? -1 : 1;
	}

}