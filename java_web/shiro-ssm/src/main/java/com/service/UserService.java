package com.service;


import com.entity.User;

public interface UserService {
	public User getUserByName(String name);
	public User getUserById(int id);
	public void addUser(User user);
}
