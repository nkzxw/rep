package com.realm;

import org.apache.shiro.crypto.hash.SimpleHash;

public class PasswordEncoder {
	private String algorithmName;// = "MD5"; // 加密方式==MD5==算法
	private int hashIterations;// = 2; // 加密迭代2次

	public String setEncoder(String username, String password) {
		// 生成加密---
		SimpleHash hash = new SimpleHash(getAlgorithmName(), password,
				username, getHashIterations());

		return hash.toHex();
	}

	public String getAlgorithmName() {
		return algorithmName;
	}

	public void setAlgorithmName(String algorithmName) {
		this.algorithmName = algorithmName;
	}

	public int getHashIterations() {
		return hashIterations;
	}

	public void setHashIterations(int hashIterations) {
		this.hashIterations = hashIterations;
	}
}
