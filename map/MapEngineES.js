/**
 * @namespace vtron.comp.map
 */
$.namespace("vtron.comp.map");



vtron.comp.map.SubBehavior = {
  assertParent: function(){
    var _parent = this.parentNode;
    if(!_parent || _parent.tagName.toLowerCase() != "v-map-engine"){
      throw new Error("parentNode is not <v-map-engine>.");
      return;
    }

    var mapCommon = _parent.mapCommon;
    if(!mapCommon){
      throw new Error("mapCommon not found.");
    }
  }
}

/**
 * MapEngineES地图组件
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES
 */
vtron.comp.map.MapEngineES = Polymer({
  is: "v-map-engine",
  behaviors: [vtron.comp.behavior.PositionBehavior, vtron.comp.behavior.SizeBehavior],
  properties: {
    autoload: Boolean,
    mapName: String,
    mapType: String,
    url: String,
  	format: String,
  	level: Number,
  	minLevel: Number,
    maxLevel: Number,
    cenX: Number,
    cenY: Number,
    iconSize: Number,
    currentState: String, //当前状态
    currentCursor: String, //当前鼠标样式
  },
  attached: function() {
    if(!this.mapName)this.mapName = vtron.util.createUUID();
    var _this = this;
    
    //初始地图范围和比例
    this.initRegion = {};
    
    //当前地图范围和比例
    this.region = {};

    this.messageHandlers = {};
    
    if(this.autoload){
      this.init();
    }
  },
});

/**
 * 初始化地图对象
 */
vtron.comp.map.MapEngineES.prototype.init = function(){
  var _this = this;
  
  var dom = this.$$(".map");
  var mapCommon = new Common(this.mapName + "_naclModule", dom, "application/x-nacl-vtronmapengine");
  mapCommon.doHandleMessage = function(message_event){_this.doHandleMessage(message_event);};
  
  this.mapCommon = mapCommon;

  //当前图层编号
  mapCommon.currentLayerid = -1;

  //打开地图事件
  _this.messageHandlers["LoadComplete"] = function(data){
    _this.fire("load", null, {bubbles: false});
  }

  //打开地图事件
  _this.messageHandlers["OPEN"] = function(data){
    if(!data.MapExtern)return;

    // =====地图打开=====
    var extern = data.MapExtern;
    var level = (data.MapLevel == 0 ? 0 : data.MapLevel || data.mapLevel);
    if(level || level == 0)_this.region.level = level;
  
    // 地图的左上角x,y 右下角x,y
    var info = {left:extern[0], top: extern[1], right: extern[2], bottom: extern[3], x: (extern[0]+extern[2])/2, y: (extern[1]+extern[3])/2, level: level};
    _this.fire("open", info);
  }

  //移动地图事件
  _this.messageHandlers["MOVEVIEW"] = function(data){
    if(!data.MapExtern)return;
    
    //=====地图移动=====
    var extern = data.MapExtern;
    var level = (data.MapLevel == 0 ? 0 : data.MapLevel || data.mapLevel);
    if(level || level == 0)_this.region.level = level;
    
    //地图的左上角x,y 右下角x,y
    var info = {left:extern[0], top: extern[1], right: extern[2], bottom: extern[3], x: (extern[0]+extern[2])/2, y: (extern[1]+extern[3])/2, level: level};
    _this.fire("move", info);
  }

  //获取地图上的一个坐标点
  _this.messageHandlers["GETPOSTION"] = function(data){
    _this.fire("position", { x: data.Location[0], y: data.Location[1]});
  }
  
  //点击一个热点
  _this.messageHandlers["SINGLECHOOSE"] = _this.messageHandlers["GPSINFO"] = function(data){
    if(data.NAME != "vMarkChoose"){
      //=====点击一个热点物件=====
      var location = data.Location;
      var layerName = data.NAME;
      var info = data.Information;
      _this.waitLayer(layerName, function(layer){
        layer.fire("click-point", {x: data.Location[0], y: data.Location[1], info: info});
      })
    } else if(data.NAME == "vMarkChoose"){
      //=====点击一个标注物件=====
      var location = data.Location;
      var labelId = data.LabelID;
      _this.fire("click-mark", {labelId: labelId, x: data.Location[0], y: data.Location[1]});
    }
  }
  
  //添加一个标注
  _this.messageHandlers["AddMark"] = function(data){
    _this.mousedownPosGeo = _this.mousedownPosGeo || {};
    _this.mousedownPosPix = _this.mousedownPosGeo || {};
    var info = {lableId: data.LabelID, geoX: _this.mousedownPosGeo.x, geoY: _this.mousedownPosGeo.y, pixX: _this.mousedownPosPix.x, pixY: _this.mousedownPosPix.y};
    _this.fire("add-mark", info);
  }
  
  //自由绘制
  _this.messageHandlers["DRAWPOINT"] = function(data){
    var info = {};
    info.rgb = {
      r : data.ColorR,
      g : data.ColorG,
      b : data.ColorB,
      a : data.ColorA
    };
    info.width = data.Width;
    info.points = [];
    for (var i = 0; data.POINTS && i < data.POINTS.length; i += 2) {
      var p = {
        x : data.POINTS[i],
        y : data.POINTS[i + 1]
      };
      info.points.push(p);
    }
    _this.fire("drawPoint", info);
  }

  _this.messageHandlers["REGMARKERSTYLE"] = function(data){
    _this.fire("style-reg", data);
  }

  _this.messageHandlers["ADDWFSPOINTLAYER"] = _this.messageHandlers["ADDWFSLINELAYER"] = _this.messageHandlers["ADDWFSPOLYLAYER"] = function(data){
    _this.fire("layer-reg", data);
  }
  
  _this.messageHandlers["UPDATEWFSPOINTLAYER"] = _this.messageHandlers["UPDATEWFSLINELAYER"] = _this.messageHandlers["UPDATEWFSPOLYLAYER"] = function(data){
    _this.fire("layer-reload", data);
  }
  
  _this.messageHandlers["MOUSEUP"] = function(data){
    _this.mousedownPosGeo = {x: data.PosGeo[0], y: data.PosGeo[1]};
    _this.mousedownPosPix = {x: data.PosPix[0], y: data.PosPix[1]};
    _this.fire("mouse-up", data);
  }
  
  _this.messageHandlers["MOUSEDOWN"] = function(data){
    _this.mousedownPosGeo = null;
    _this.mousedownPosPix = null;
    _this.fire("mouse-down", data);
  }

  $(_this).on("load", function(event){
    _this.open();
  })
  
  
  //地图当前的区域范围，当move事件触发时改变
  $(_this).on("open", function(event){
    var detail = event.originalEvent.detail;
    //初始地图范围和比例
    _this.initRegion = _this.initRegion || {};
    _this.initRegion.xbegin = detail.left;
    _this.initRegion.ybegin = detail.top;
    _this.initRegion.xend = detail.right;
    _this.initRegion.yend = detail.bottom;
    if(detail.level)_this.initRegion.level = detail.level;
    
    //当前地图范围和比例
    _this.region = _this.region || {};
    _this.region.xbegin = detail.left;
    _this.region.ybegin = detail.top;
    _this.region.xend = detail.right;
    _this.region.yend = detail.bottom;
    if(detail.level)_this.region.level = detail.level;
    
  });

  $(_this).on("move", function(event){
    var detail = event.originalEvent.detail;
    if(!detail)return;
    //当前地图范围和比例
    _this.region = _this.region || {};
    _this.region.xbegin = detail.left;
    _this.region.ybegin = detail.top;
    _this.region.xend = detail.right;
    _this.region.yend = detail.bottom;
    if(detail.level)_this.region.level = detail.level;
  });

};



/**
 * 地图对象处理消息，执行事件
 */
vtron.comp.map.MapEngineES.prototype.doHandleMessage = function(message_event) {
  var data = null;
  try {
    data = JSON.parse(message_event.data);
  } catch (_error) {
    console.log(_error);
  }
  if(data == null) return;

  var handler = this.messageHandlers[data.CmdName];
  if(typeof(handler) == "function")handler(data);
};


/**
 * 打开地图
 */
vtron.comp.map.MapEngineES.prototype.open = function(config){
  config = config || {};
  var msg = {
      "CmdName":"OPEN",
      "Cmdid":1,
      "MapName": config.mapName || this.mapName,
      "URL": config.url || this.url,
      "MapType": config.mapType || this.mapType,
      "Format": config.format || this.format,
      "CenX": config.cenX || this.cenX,
      "CenY": config.cenY || this.cenY,
      "Level": config.level || this.level,
      "MinLevel": config.minLevel || this.minLevel,
      "MaxLevel": config.maxLevel || this.maxLevel,
    };

  if(msg.MapType == "GeoBeansMap"){
    msg = $.extend(msg, {"MinLon":-180,"MaxLon":180,"MinLat":-270,"MaxLat":90,"Resolution0":1.406250078533376});
  }
  this.mapCommon.SendMessage(JSON.stringify(msg));
};

vtron.comp.map.MapEngineES.prototype.refresh = function(){
  this.mapCommon.SendMessage('{"CmdName":"CRTLREFRESH"}');
};

/**
 * 获取样式
 * @param name 样式名称
 * @returns 样式节点
 */
vtron.comp.map.MapEngineES.prototype.waitStyle = function(name, callback) {
  var style = $(this).find("[style-name='" + name + "']")[0];
  if(!style){
    console.error("style not found: " + name);
  } else {
    style.waitStyleId(callback);
  }
};

/**
 * 获取图层
 * @param name 图层名称
 * @returns 图层节点
 */
vtron.comp.map.MapEngineES.prototype.waitLayer = function(name, callback) {
  var layer = $(this).find("[layer-name='" + name + "']")[0];
  if(!layer){
    console.error("layer not found: " + name);
  } else {
    layer.waitLayerId(callback);
  }
};

vtron.comp.map.MapEngineES.prototype.getLevel = function(){
  if(this.region == null) return null;
  return this.region.level;
};

vtron.comp.map.MapEngineES.prototype.getRegion = function(){
  return this.region;
};

vtron.comp.map.MapEngineES.prototype.getCenter = function(){
  if(this.region == null) return null;
  var c = {};
  c.x = (this.region.xend + this.region.xbegin) / 2;
  c.y = (this.region.ybegin + this.region.yend) / 2;
  return c;
};

vtron.comp.map.MapEngineES.prototype.getShiftGeo = function(){
  var level = this.getLevel();
  var r = 0;
  if(level == 0){
    r = 2.3794610058302802E-4;
  } else if(level == 1){
    r = 1.1897305029151401E-4;
  } else if(level == 2){
    r = 5.9486525145757005E-5;
  } else if(level == 3){
    r = 2.3794610058302804E-5;
  } else if(level == 4){
    r = 1.1897305029151402E-5;
  } else if(level == 5){
    r = 4.75892201166056E-6;
  } else if(level == 6){
    r = 2.37946100583028E-6;
  }
  return {x: this.shiftX * r, y: this.shiftY * r};
};


vtron.comp.map.MapEngineES.prototype.changeCursor = function(name){
  if(!name)name = "default";
  var cursor = $(this).find("v-map-engine-cursor[cursor-name='" + name + "']")[0];
  if(!cursor){
    console.error("cursor not found: " + name);
    return;
  }
  cursor.change();
};

vtron.comp.map.MapEngineES.prototype.changeState = function(name){
  if(!name)name = "default";
  var state = $(this).find("[state-name='" + name + "']")[0];
  if(!state){
    console.error("state not found: " + name);
    return;
  }
  state.change();
};

vtron.comp.map.MapEngineES.prototype.moveOffset = function(offsetX, offsetY, level){
  if(this.region == null) return null;
  if(level || level == 0){
    this.region.level = level;
  } else {
    level = this.region.level;
  }
  var str = '{"CmdName":"MOVEVIEW","Coortype":"PIX","OffsetX":'+offsetX+',"OffsetY":'+offsetY+',"Level":'+ level +',"Cmdid":1}';
  this.mapCommon.SendMessage(str);
};
vtron.comp.map.MapEngineES.prototype.moveCenter = function(x, y, level){
  if(this.region == null) return null;
  if(level || level == 0){
    this.region.level = level;
  } else {
    level = this.region.level;
  }
  var shift = this.getShiftGeo();
  if(shift && shift.x)x+=shift.x;
  if(shift && shift.y)y-=shift.y;

  var str = '{"CmdName":"MOVEVIEW","Coortype":"GEO","CenX":'+x+',"CenY":'+y+',"Level":'+level+',"Cmdid":1}';
  this.mapCommon.SendMessage(str);
};
vtron.comp.map.MapEngineES.prototype.moveReset = function(){
  var x = (this.initRegion.xend + this.initRegion.xbegin) / 2;
  var y = (this.initRegion.ybegin + this.initRegion.yend) / 2;
  var level = this.initRegion.level;
  this.moveCenter(x, y, level);
};
vtron.comp.map.MapEngineES.prototype.zoom = function(level){
  this.moveOffset(0, 0, level);
};
vtron.comp.map.MapEngineES.prototype.zoomIn = function(){
  var level = this.getLevel() + 1;
  this.moveOffset(0, 0, level);
};
vtron.comp.map.MapEngineES.prototype.zoomOut = function(){
  var level = this.getLevel() - 1;
  this.moveOffset(0, 0, level);
};