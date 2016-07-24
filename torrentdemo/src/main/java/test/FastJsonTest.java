package test;

import java.util.ArrayList;
import java.util.List;

import com.alibaba.fastjson.JSON;

class User {
	private String name;
	private int age;
	private int age2;

	public User(String name, int age, int age2) {
		super();
		this.name = name;
		this.age = age;
		this.age2 = age2;
	}

	public User() {
		// TODO Auto-generated constructor stub
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getAge() {
		return age;
	}

	public void setAge(int age) {
		this.age = age;
	}

	public int getAge2() {
		return age2;
	}

	public void setAge2(int age) {
		this.age2 = age;
	}

	@Override
	public String toString() {
		return "User [name=" + name + ", age=" + age + "]";
	}
};

class UserGroup {
	private String name;
	private List<User> users = new ArrayList<User>();

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public List<User> getUsers() {
		return users;
	}

	public void setUsers(List<User> users) {
		this.users = users;
	}

	@Override
	public String toString() {
		return "UserGroup [name=" + name + ", users=" + users + "]";
	}
}

class FastJsonTest {
	public static void main(String[] args) {
		// 构建用户geust
		User guestUser = new User();
		guestUser.setName("guest");
		guestUser.setAge(28);
		// 构建用户root
		User rootUser = new User();
		rootUser.setName("root");
		guestUser.setAge(35);
		guestUser.setAge2(37);

		User rootUser2 = new User("ee", 33, 39);

		// 构建用户组对象
		UserGroup group = new UserGroup();
		group.setName("admin");
		group.getUsers().add(guestUser);
		group.getUsers().add(rootUser);
		// 用户组对象转JSON串
		String jsonString = JSON.toJSONString(group);
		System.out.println("jsonString:" + jsonString);
		// JSON串转用户组对象
		UserGroup group2 = JSON.parseObject(jsonString, UserGroup.class);
		System.out.println("group2:" + group2);
		System.out.println("=================");
		// 构建用户对象数组
		User[] users = new User[2];
		users[0] = guestUser;
		users[1] = rootUser;
		// 用户对象数组转JSON串
		String jsonString2 = JSON.toJSONString(users);
		System.out.println("jsonString2:" + jsonString2);
		// JSON串转用户对象列表
		List<User> users2 = JSON.parseArray(jsonString2, User.class);
		System.out.println("users2:" + users2);

		System.out.println("====================");
		List<User> users21 = new ArrayList<User>();
		users21.add(guestUser);
		users21.add(rootUser);
		users21.add(rootUser2);
		jsonString = JSON.toJSONString(users21);
		System.out.println("jsonString:" + jsonString);
	}
}