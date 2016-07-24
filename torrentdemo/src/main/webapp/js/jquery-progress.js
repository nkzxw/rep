/*!
 * jquery-progress.js
 * @author 云淡然
 * @version 1.0
 */

/**
 * v1.0 2013年12月24日14:20:45
 */


/**
 * v1.0 初始构造
 */


/**
 * 1、初始化
 * $().progress({...});
 *
 * 2、设置进度
 * 会暂停运动
 * $().progress(.5);
 *
 * 3、操作控制
 * 暂停
 * $().progress("pause");
 * 继续
 * $().progress("continue");
 * 运行
 * $().progress("run",0,1,6000);
 */

(function (win, undefined) {
	var prefix = 'jquery-progress____',
		isSupportCanvas = !! win.document.createElement("canvas").getContext,
		// 传入参数可以多次初始化
		defaults = {
			// 是否逆时针递增
			// flase：顺时针递增，逆时针递减
			// true：逆时针递增，顺时针递减
			anticlockwiseAsc: false,
			// 开始角度，以水平向右方向为0度，顺时针+，逆时针-
			// 默认-90度为垂直向上方向
			beginAngle: -90,
			// 进度步进，为小于1的数
			step: .01,
			// 初始的进度
			progress: 0,
			// 运动进度的开始进度
			begin: 0,
			// 运动进度的结束进度
			// 如果开始进度大于结束进度，则进度递减，否则递增
			end: 1,
			// 运动进度的进度时间
			duration: 6000,
			// 背景类型：arc（圆弧），sector（扇形）
			bgType: "arc",
			// 前景类型：arc（圆弧），sector（扇形）
			fgType: "arc",
			// 背景样式
			bgStyle: {
				// 线头：butt，round 和 square
				lineCap: "butt",
				// 线宽
				lineWidth: 1,
				// 轮廓颜色
				strokeStyle: "#ccc",
				// 填充颜色
				fillStyle: "#ccc"
			},
			// 前景样式
			fgStyle: {
				// 线头：butt，round 和 square
				lineCap: "butt",
				// 线宽
				lineWidth: 1,
				// 轮廓颜色
				strokeStyle: "#0595FC",
				// 填充颜色
				fillStyle: "#0595FC"
			},
			// 进度开始回调
			onstart: function () {},
			// 进度暂停回调
			onpause: function () {},
			// 进度继续回调
			oncontinue: function () {},
			// 进度结束回调
			onstop: function () {},
			// 进度回调
			onprogress: function () {}
		};

	$.support.canvas = isSupportCanvas;
	$.fn.progress = function () {
		if (!isSupportCanvas) return;
		var args = arguments;
		this.each(function () {
			var $progress = $(this),
				options = _data($progress, "options") || defaults,
				width = $progress.width(),
				height = $progress.height();

			// 设置进度值
			if ($.isNumeric(args[0])) {
				_pause();
				_data($progress, "isRunning", 0);
				_progress(args[0], 1);
			}
			// 暂停
			else if (args[0] == "pause") {
				_pause();
			}
			// 继续
			else if (args[0] == "continue") {
				_continue();
			}
			// 运动
			else if (args[0] == "run") {
				_run(args[1] === undefined ? options.begin : args[1], args[2] === undefined ? options.end : args[2], args[3] === undefined ? options.duration : args[3]);
			} else {
				_init();
			}



			function _init() {
				var options = $.extend(true, {}, defaults, args[0]),
					toFixed = ((options.step + "").match(/\.(\d+)/) || ["", ""])[1].length - 2;
				if (toFixed < 0) toFixed = 0;
				if (toFixed > 20) toFixed = 20;
				_data($progress, "options", options);
				_data($progress, "toFixed", toFixed);
				_data($progress, "timer", 0);
				// 是否正在进度
				_data($progress, "isRunning", 0);
				// 当前进度值，用于继续进度
				_data($progress, "beginPercent", 0);
				// 结束进度值，用于继续进度
				_data($progress, "endPercent", 1);
				// 当前进度时间，用于继续进度
				_data($progress, "duration", 0);
				_progress(options.progress, 1);
			}



			function _run(beginPercent, endPercent, duration) {
				beginPercent = beginPercent * 1;
				endPercent = endPercent * 1;
				duration = duration * 1;
				var timer = _data($progress, "timer"),
					options = _data($progress, "options"),
					toFixed = _data($progress, "toFixed"),
					intervalTime = duration / (Math.abs(endPercent - beginPercent) / options.step),
					isDesc = endPercent - beginPercent < 0;

				// 开始回调
				options.onstart(beginPercent);
				// 暂停
				_pause(1);
				// 设置初始位置
				_progress(beginPercent);
				_data($progress, "isRunning", 1);
				_data($progress, "endPercent", endPercent);
				timer = setInterval(function () {
					var currentPercent;
					duration -= intervalTime;
					beginPercent = isDesc ? beginPercent - options.step : beginPercent + options.step;
					// 当前进度
					currentPercent = ((beginPercent * 100).toFixed(toFixed) / 100).toFixed(toFixed + 2);
					if (isDesc && currentPercent <= endPercent || !isDesc && currentPercent >= endPercent) {
						_pause();
						_progress(endPercent);
						_data($progress, "beginPercent", endPercent);
						_data($progress, "isRunning", 0);
						// 结束回调
						options.onstop(endPercent);
					} else {
						_data($progress, "beginPercent", currentPercent);
						_progress(currentPercent);
					}
					_data($progress, "duration", duration);
				}, intervalTime);
				_data($progress, "timer", timer);
			}




			// 暂停

			function _pause(isNoCallback) {
				var beginPercent = _data($progress, "beginPercent"),
					options = _data($progress, "options"),
					timer = _data($progress, "timer");
				if (timer) {
					clearInterval(timer);
					_data($progress, "timer", 0);
					!isNoCallback && options.onpause(beginPercent);
				}
			}



			// 继续

			function _continue() {
				var isRunning = _data($progress, "isRunning"),
					beginPercent = _data($progress, "beginPercent"),
					endPercent = _data($progress, "endPercent"),
					duration = _data($progress, "duration"),
					timer = _data($progress, "timer");
				if (isRunning && !timer) {
					_run(beginPercent, endPercent, duration);
					options.oncontinue(beginPercent);
				}
			}



			function _progress(percent) {
				var options = _data($progress, "options"),
					toFixed = _data($progress, "toFixed"),
					style = options.style,
					angle0 = options.beginAngle,
					angle1 = options.anticlockwiseAsc ? angle0 - percent * 360 : angle0 + percent * 360,
					x = Math.min(width / 2, height / 2),
					y = x,
					bgR = Math.min(width / 2 - options.bgStyle.lineWidth, height / 2 - options.bgStyle.lineWidth),
					fgR = Math.min(width / 2 - options.fgStyle.lineWidth, height / 2 - options.fgStyle.lineWidth);

				// 逆时针大于开始度数 || 顺时针小于开始度数
				if (options.anticlockwiseAsc && angle1 > angle0 || !options.anticlockwiseAsc && angle1 < angle0) angle1 = angle0;
				// 逆时针小于结束度数
				if (options.anticlockwiseAsc && angle1 < angle0 - 360) angle1 = angle0 - 360;
				// 顺时针大于结束度数
				if (!options.anticlockwiseAsc && angle1 > angle0 + 360) angle1 = angle0 + 360;

				// 清除画布
				_clear(width, height);

				// 背景
				if (options.bgType == "arc") {
					_arc(x, y, bgR, 0, 360, 1);
				} else {
					_sector(x, y, x, 0, 360, 1);
				}

				// 前景
				if (options.fgType == "arc") {
					_arc(x, y, fgR, angle0, angle1);
				} else {
					_sector(x, y, x, angle0, angle1);
				}
				options.progress = percent;
				// 运动回调
				options.onprogress(percent);

				_data($progress, "options", options);
			}



			function _clear(x, y) {
				var oContext = $progress[0].getContext("2d");
				oContext.clearRect(0, 0, x, y);
			}




			/**
			 * 创建圆圈
			 * @param  {Number}  圆心x
			 * @param  {Number}  圆心y
			 * @param  {Number}  半径
			 * @param  {Number}  开始角度
			 * @param  {Number}  结束角度
			 * @param  {Boolean} 是否绘制背景
			 * @return {Undefined}
			 * @version 1.0
			 * 2013年12月4日11:11:39
			 */

			function _arc(x, y, r, begin, end, isDrawBg) {
				var options = _data($progress, "options"),
					oContext = $progress[0].getContext("2d"),
					style = isDrawBg ? options.bgStyle : options.fgStyle;

				oContext.beginPath();
				oContext.arc(x, y, r, _angle(begin), _angle(end), options.anticlockwiseAsc);
				oContext.lineCap = style.lineCap;
				oContext.lineWidth = style.lineWidth;
				oContext.strokeStyle = style.strokeStyle;
				oContext.stroke();
				oContext.closePath();
			}


			/**
			 * 画扇形
			 * @param  {Number}  圆心x
			 * @param  {Number}  圆心y
			 * @param  {Number}  半径
			 * @param  {Number}  开始角度
			 * @param  {Number}  结束角度
			 * @param  {Boolean} 是否绘制背景
			 * @return {Undefined}
			 * @version 1.0
			 * 2013年12月4日12:01:17
			 */

			function _sector(x, y, r, begin, end, isDrawBg) {
				var oContext = $progress[0].getContext("2d"),
					angle0 = _angle(begin),
					angle1 = _angle(end),
					x0 = r * Math.cos(angle0) + x,
					y0 = r * Math.sin(angle0) + y,
					style = isDrawBg ? options.bgStyle : options.fgStyle;



				oContext.beginPath();
				// oContext.lineWidth = 1;
				// 画圆弧
				oContext.arc(x, y, r, angle0, angle1, options.anticlockwiseAsc);
				// 判断开始角度和结束角度是否重叠
				if ((angle1 - angle0) % Math.PI) {
					// 移动到圆心
					oContext.lineTo(x, y);
					// 移动到圆弧起点
					oContext.lineTo(x0, y0);
				}
				oContext.fillStyle = style.fillStyle;
				oContext.fill();
				oContext.closePath();
			}

		});
	}
	$.fn.progress.defaults = defaults;








	// 读取和存储数据

	function _data($progress, dataKey, dataVal) {
		if (dataVal === undefined) {
			return $progress.data(prefix + dataKey);
		} else {
			return $progress.data(prefix + dataKey, dataVal);
		}
	}

	/**
	 * 转换角度为弧度
	 * @param  {Number} 角度
	 * @return {Number} 弧度
	 * @version 1.0
	 * 2013年12月4日11:12:57
	 */

	function _angle(deg) {
		return deg * Math.PI / 180;
	}


	function _toFixed(num, toFixed) {
		if (toFixed < 0) {

		} else if (toFixed > 20) {

		} else {
			return num.toFixed(toFixed);
		}
	}

})(this);
