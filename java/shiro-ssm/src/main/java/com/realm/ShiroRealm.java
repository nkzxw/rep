package com.realm;

import org.apache.shiro.SecurityUtils;
import org.apache.shiro.authc.AuthenticationException;
import org.apache.shiro.authc.AuthenticationInfo;
import org.apache.shiro.authc.AuthenticationToken;
import org.apache.shiro.authc.SimpleAuthenticationInfo;
import org.apache.shiro.authc.UnknownAccountException;
import org.apache.shiro.authz.AuthorizationInfo;
import org.apache.shiro.authz.SimpleAuthorizationInfo;
import org.apache.shiro.realm.AuthorizingRealm;
import org.apache.shiro.session.Session;
import org.apache.shiro.subject.PrincipalCollection;
import org.apache.shiro.util.ByteSource;
//import org.slf4j.Logger;
//import org.slf4j.LoggerFactory;
import org.apache.log4j.Logger;

import com.entity.Role;
import com.entity.User;
import com.service.RoleService;
import com.service.UserService;

public class ShiroRealm extends AuthorizingRealm {
	private static final Logger logger = Logger.getLogger(ShiroRealm.class);
	private UserService userService;
	private RoleService roleService;

	@Override
	protected AuthorizationInfo doGetAuthorizationInfo(
			PrincipalCollection principals) {
		// 授权
		User user = (User) principals.fromRealm(getName()).iterator().next();
		if (user != null) {
			SimpleAuthorizationInfo info = new SimpleAuthorizationInfo();
			Role role = roleService.getRoleById(user.getRoleid());
			// info.addStringPermission(role.getRolename());
			// Collection<Role> per=(Collection<Role>)
			// roleService.getRoleById(user.getRoleid());
			// Iterator<Role> it=per.iterator();
			// while (it.hasNext()) {
			// info.addRole(it.next().getRolename());
			// info.addStringPermission(it.next().getRolename());
			// }
			info.addRole(role.getRolename());
			logger.info("" + info.getRoles() + ","
					+ info.getStringPermissions());
			logger.info("ShiroRealmClass---用户名" + user.getUsername()
					+ "***权限****" + role.getRolename());
			return info;
		}
		return null;
	}

	@Override
	protected AuthenticationInfo doGetAuthenticationInfo(
			AuthenticationToken token) throws AuthenticationException {
		User user = userService.getUserByName(token.getPrincipal().toString());
		if (user == null) {
			// 匿名登陆
			logger.info("ShiroRealmClass---匿名登陆");
			throw new UnknownAccountException();
		} else {
			// 身份验证--用户名放到session里
			Session session = SecurityUtils.getSubject().getSession();
			session.setAttribute("username", user.getUsername());
			logger.debug("ShiroRealmClass---用户登录" + user.getUsername());
			// String salt="bb8de00c5c5d2381a4020fba8e37bb22";
			SimpleAuthenticationInfo sai = new SimpleAuthenticationInfo(user,
					user.getPassword(), getName());
			sai.setCredentialsSalt(ByteSource.Util.bytes(user.getUsername()));
			// ai.setCredentialsSalt(ByteSource.Util.bytes(username+salt2));
			// 盐是用户名
			return sai;
		}
	}

	public UserService getUserService() {
		return userService;
	}

	public void setUserService(UserService userService) {
		this.userService = userService;
	}

	public RoleService getRoleService() {
		return roleService;
	}

	public void setRoleService(RoleService roleService) {
		this.roleService = roleService;
	}

}
