package com.controller;

import javax.servlet.http.HttpServletRequest;

import org.apache.shiro.SecurityUtils;
import org.apache.shiro.authc.UsernamePasswordToken;
import org.apache.shiro.subject.Subject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;

import com.entity.User;
import com.realm.PasswordEncoder;
import com.service.UserService;

/**
 * 用户Controller层
 * 
 * @author Administrator
 * 
 */
@Controller
@RequestMapping("/user")
public class UserController {
	@Autowired
	private UserService userService;
	@Autowired
	private PasswordEncoder encoder;
	private static Logger logger = LoggerFactory
			.getLogger(UserController.class);

	/**
	 * 用户登录
	 * 
	 * @param user
	 * @param request
	 * @return
	 */
	@RequestMapping("/login")
	public String login(User user, HttpServletRequest request) {

		Subject subject = SecurityUtils.getSubject();
		logger.info("get user =" + user.getUsername() + ","
				+ user.getPassword());
		if (user.getUsername() == null) {
			return "redirect:/pages/login.jsp";
		}
		UsernamePasswordToken token = new UsernamePasswordToken(
				user.getUsername(), user.getPassword());
		try {
			subject.login(token);
			request.getSession().setAttribute("user", user);

			boolean isadmin = subject.hasRole("admin");
			logger.info("--------------------------------------------");
			logger.info("管理员" + isadmin);
			logger.info("--------------------------------------------");
			// String userId=(String)subject.getPrincipal();
			// User users=userService.getUserById(userId);
			if (isadmin) {
				return "redirect:/pages/admin.jsp";
			} else {
				return "redirect:/pages/default.jsp";
			}

		} catch (Exception e) {
			e.printStackTrace();
			request.getSession().setAttribute("user", user);
			request.setAttribute("error", "用户名或密码错误！");
			return "redirect:/pages/login.jsp";
		}
	}

	@RequestMapping("/logout")
	public String logout(HttpServletRequest request) {
		request.getSession().invalidate();
		return "redirect:/index.jsp";
	}

	@RequestMapping("/admin")
	public String admin(HttpServletRequest request) {
		return "redirect:/index.jsp";
	}

	@RequestMapping("/student")
	public String student(HttpServletRequest request) {
		return "redirect:/index.jsp";
	}

	@RequestMapping("/teacher")
	public String teacher(HttpServletRequest request) {
		return "redirect:/index.jsp";
	}

	@RequestMapping("/register")
	public String register(User user, HttpServletRequest request) {
		// 加密--插入数据库
		try {
			if (user.getUsername() != null) {
				logger.info("get user =" + user.getUsername() + ","
						+ user.getPassword() + "," + user.getRoleid() + encoder);
				user.setPassword(encoder.setEncoder(user.getUsername(),
						user.getPassword()));
				logger.info("UserServiceImpl =" + user.getUsername() + ","
						+ user.getPassword() + "," + user.getRoleid()
						+ userService);
				userService.addUser(user);
			} else {
				return "redirect:/pages/login.jsp";
			}
		} catch (Exception e) {
			e.printStackTrace();
			logger.info("注册出现异常");
			return "redirect:/pages/login.jsp";
		}
		return "redirect:/pages/default.jsp";
	}

}
