package com.service.impl;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.dao.UserDao;
import com.entity.User;
import com.service.UserService;

@Service("userService")
public class UserServiceImpl implements UserService {

	@Autowired
	private UserDao dao;

	@Override
	public User getUserByName(String name) {
		return this.dao.getUserByName(name);
	}

	@Override
	public User getUserById(int id) {
		return this.dao.getUserById(id);
	}

	@Override
	public void addUser(User user) {
		dao.addUser(user);
	}

}
