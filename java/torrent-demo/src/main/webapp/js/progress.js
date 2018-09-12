var $file1 = $("#file1");
var $button1 = $("#button1");
var $precent = $("#precent");
var $ret = $("#ret");
$file1.removeClass('hide');
// $canvas.hide();
$("#button1").on(
		"click",
		function() {
			$ret.empty();
			var type = "image";
			$button1.prop("disabled", 1)
			$.ajaxFileUpload({
				url : window.location + 'uploadFile',
				secureuri : false,
				fileElementId : 'file1',
				dataType : 'json',
				complete : function() {
					$button1.prop("disabled", 0);
					$("#content").empty();
				},
				success : function(data, status) {
					if (data) {
						var ps = '';
						$.each(data, function(i, item) {
							ps += ("<br><a href=\"" + item.torLink + "\">"
									+ item.torId + "</a>")
						})
						$ret.append(ps)
					}
				},
				error : function(data, status, e) {
					var ps = '<p class="text-danger">' + '处理出错' + '</p>';
					$ret.append(ps)
				}
			})
		});
// 这个问题并不是由change事件失效造成的，而是ajaxFileUpload插件造成的，它会把原来的file元素替换成新的file元素，所以之前绑定的change事件就失效了，需要重新绑定一下
var inputElement = document.getElementById("left");
inputElement.addEventListener("change", handleFiles, false);
function handleFiles() {
	$button1.prop("disabled", 0);
	$ret.empty();
	var fileList = document.getElementById("file1").files;
	var dd = document.getElementById('content');
	dd.innerHTML = "";
	for ( var i = 0; i < fileList.length; i++) {
		//console.log(fileList[i]);
		dd.innerHTML += fileList[i].name;
		if (fileList[i].size > 2 * 1024 * 1024) {
			$button1.prop("disabled", 1);
			dd.innerHTML += ',<p class="text-danger">文件过大</p>';
		}
		dd.innerHTML += "<br>";
	}
}
