var $file1 = $("#file1");
var $precent = $("#precent");
var $ret = $("#ret");
$file1.removeClass('hide');
// $canvas.hide();

function UploadData() {
	$ret.empty();
	var type = "image";
	$.ajaxFileUpload({
		url : window.location + 'uploadFile',
		secureuri : false,
		fileElementId : 'file1',
		dataType : 'json',
		complete : function() {
			$button.prop("disabled", 0)
		},
		success : function(data, status) {
			var ps = '文件上传成功。';
			if (data) {
				var ps = '文件上传处理成功。';
				$.each(data,
						function(i, item) {
							ps += ("<br><a href=\"" + item.torLink + "\">"
									+ item.torId + "</a>")
						})
			}
			$ret.append(ps)
		},
		error : function(data, status, e) {
			var ps = '<p class="text-danger">' + '上传出错' + '</p>';
			$ret.append(ps)
		}
	})
}

// 这个问题并不是由change事件失效造成的，而是ajaxFileUpload插件造成的，它会把原来的file元素替换成新的file元素，所以之前绑定的change事件就失效了，需要重新绑定一下

document.getElementById('left').onchange = function () {
	// alert('Selected file: ' + this.value);
	$ret.html("可以点击上传文件了");
}