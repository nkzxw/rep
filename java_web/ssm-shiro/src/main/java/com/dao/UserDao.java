package com.dao;

import com.entity.User;
public interface UserDao {
	
	public User getUserByName(String username);
	public User getUserById(int id);
	public void addUser(User user);
}
