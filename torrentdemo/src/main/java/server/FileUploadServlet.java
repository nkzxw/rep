package server;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

//import test.User;

import com.alibaba.fastjson.JSON;
//import org.json.JSONStringer;

import BEncoding.BTEncoding;

class torrent {
	private String torLink;
	private String torId;

	public String getTorLink() {
		return torLink;
	}

	public void setTorLink(String torLink) {
		this.torLink = torLink;
	}

	public String getTorId() {
		return torId;
	}

	public torrent(String torLink, String torId) {
		super();
		this.torLink = torLink;
		this.torId = torId;
	}

	public void setTorId(String torId) {
		this.torId = torId;
	}
}

/**
 * A Java servlet that handles file upload from client.
 * 
 * @author www.codejava.net
 */
public class FileUploadServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
	// location to store file uploaded
	private static final String UPLOAD_DIRECTORY = "upload";
	// upload settings
	private static final int MEMORY_THRESHOLD = 1024 * 1024 * 3; // 3MB
	private static final int MAX_FILE_SIZE = 1024 * 1024 * 40; // 40MB
	private static final int MAX_REQUEST_SIZE = 1024 * 1024 * 100; // 50MB

	/**
	 * Upon receiving file upload submission, parses the request to read upload
	 * data and saves the file on disk.
	 */
	protected void doPost(HttpServletRequest request,
			HttpServletResponse response) throws ServletException, IOException {
		// System.out.println("Error: Form must has enctype=multipart/form-data.");
		response.setContentType("text/html; charset=UTF-8");

		// checks if the request actually contains upload file
		if (!ServletFileUpload.isMultipartContent(request)) {
			// if not, we stop here
			PrintWriter writer = response.getWriter();
			writer.println("Error: Form must has enctype=multipart/form-data.");
			writer.flush();
			return;
		}
		// configures upload settings
		DiskFileItemFactory factory = new DiskFileItemFactory();
		// sets memory threshold - beyond which files are stored in disk
		factory.setSizeThreshold(MEMORY_THRESHOLD);
		// sets temporary location to store files
		factory.setRepository(new File(System.getProperty("java.io.tmpdir")));
		ServletFileUpload upload = new ServletFileUpload(factory);
		// sets maximum size of upload file
		upload.setFileSizeMax(MAX_FILE_SIZE);
		// sets maximum size of request (include file + form data)
		upload.setSizeMax(MAX_REQUEST_SIZE);
		upload.setHeaderEncoding("UTF-8");
		// constructs the directory path to store upload file this path is
		// relative to application's directory
		String uploadPath = getServletContext().getRealPath("")
				+ File.separator + UPLOAD_DIRECTORY;
		// System.out.println(uploadPath + "," + request.getContextPath());
		// creates the directory if it does not exist
		File uploadDir = new File(uploadPath);
		if (!uploadDir.exists()) {
			uploadDir.mkdir();
		}
		// String basePath =
		// request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort();

		String basePath = request.getScheme() + "://" + request.getServerName();
		if (request.getServerPort() != 80) {
			basePath += (":" + request.getServerPort());
		}
		basePath += request.getContextPath();
		// JSONStringer stringer = new JSONStringer();
		List<torrent> users21 = new ArrayList<torrent>();
		// users21.add(torrent);
		// users21.add(torrent);

		String torLink;
		try {
			// stringer.array();

			// parses the request's content to extract file data
			List<FileItem> formItems = upload.parseRequest(request);

			if (formItems != null && formItems.size() > 0) {
				// iterates over form's fields
				for (FileItem item : formItems) {
					// processes only fields that are not form fields
					if (!item.isFormField()) {
						String fileName = new File(item.getName()).getName();
						String filePath = uploadPath + File.separator
								+ fileName;
						InputStream sin = item.getInputStream();
						FileOutputStream fos = new FileOutputStream(filePath);
						if (0 == BTEncoding.process(sin, fos)) {
							fos.flush();
							fos.close();
							// item.write(storeFile);
							// session.setAttribute("bbbb", "<a href=\""
							// + basePath + UPLOAD_DIRECTORY + "/"
							// + fileName + "\">" + fileName + "</a>");

							torLink = basePath + "/" + UPLOAD_DIRECTORY + "/"
									+ fileName;
							// stringer.object().key("torId").value(fileName)
							// .key("torLink").value(torLink).endObject();
							users21.add(new torrent(torLink, fileName));
						}
						// request.setAttribute("message","<a href=\""+basePath+UPLOAD_DIRECTORY+"/"+fileName+"\">"+fileName+"</a>");
					}
				}
			}

			// stringer.endArray();

		} catch (Exception ex) {
			System.out.println(ex.getMessage());
			request.setAttribute("message",
					"There was an error: " + ex.getMessage());
		}

		// request.getInputStream().close();
		// redirects client to message page
		// getServletContext().getRequestDispatcher("/upload.jsp").forward(request,
		// response);
		// response.sendRedirect("/");
		//
		// try {
		// stringer.array();
		// for (int i = 0; i < strID.length; i++) {
		// tID = Integer.parseInt(strID[i]);
		// tourName = Dao.getTourName(tID);
		// tourList = Dao.getTourList(tID);
		// tourIdList = Dao.getPlaceIdList(tID);
		// stringer.object().key("tID").value(tID).key("name").value(
		// tourName).key("tourList").value(tourList).key(
		// "tourIDList").value(tourIdList).endObject();
		// }
		// stringer.endArray();
		// } catch (JSONException e) {
		// e.printStackTrace();
		// } catch (Exception e) {
		// e.printStackTrace();
		// }
		response.getWriter().write(JSON.toJSONString(users21));
		// response.getOutputStream().write(stringer.toString().getBytes("UTF-8"));

	}

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		System.out.println(request.getServletPath());

		String filename = URLDecoder.decode(request.getServletPath(), "utf-8");
		System.out.println(filename);
		// 读取要下载的文件
		File f = new File(filename);
		if (f.exists()) {
			FileInputStream fis = new FileInputStream(f);
			filename = URLEncoder.encode(f.getName(), "utf-8");
			// 解决中文文件名下载后乱码的问题
			byte[] b = new byte[fis.available()];
			fis.read(b);
			response.setCharacterEncoding("utf-8");
			response.setHeader("Content-Disposition", "attachment; filename="
					+ filename + "");
			// 获取响应报文输出流对象
			ServletOutputStream out = response.getOutputStream();
			out.write(b);
			out.flush();
			out.close();
		} else {
			getServletContext().getRequestDispatcher("/upload.jsp").forward(
					request, response);
		}
	}
}