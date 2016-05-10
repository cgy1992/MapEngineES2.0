/**
 * MapEngineES地图组件的通用图层行为
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.LayerBehavior
 */
vtron.comp.map.LayerBehavior = {

  waitLayerId: function(callback) {
    var func = function(layer){
      if(typeof(layer.layerId) == "undefined"){
        setTimeout(function(){func(layer)}, 100); //隔小段时间再尝试
      } else if(typeof(callback) == "function"){
        callback(layer);
      }
    }
    func(this);
  },

  show: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;
    var _mapCommon = _parent.mapCommon;

    //前置事件
    var ret = _this.fire("beforeShow");
    if(!ret.returnValue)return;
    
    var cmd = {"CmdName":"SHOWLAYER","Layerid": _this.layerId};
    _mapCommon.SendMessage(JSON.stringify(cmd));
    _this.removeAttribute("hidden");

    //后置事件
    _this.fire("show");
  },

  hide: function() {
    this.assertParent();
    var _this = this;
    var _parent = this.parentNode;
    var _mapCommon = _parent.mapCommon;

    //前置事件
    var ret = _this.fire("beforeHide");
    if(!ret.returnValue)return;

    var cmd = {"CmdName":"HIDELAYER","Layerid": _this.layerId};
    _mapCommon.SendMessage(JSON.stringify(cmd));
    _this.setAttribute("hidden", true);

    //后置事件
    _this.fire("hide");
  }
}

/**
 * MapEngineES地图组件的WFS点图层
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.WFSPoint
 */
vtron.comp.map.MapEngineES.WFSPoint = Polymer({
  is: "v-map-engine-wfs-point",
  behaviors: [vtron.comp.map.SubBehavior, vtron.comp.map.LayerBehavior],
  properties: {
    layerName: String,
    layerId: Number,
    url: String,
    hidden: Boolean,
    styleAs: String,
    width: Number,
    height: Number,
    hidden: Boolean,
  },
  attached: function() {
    var _this = this;
    var _parent = _this.parentNode;

    var url = _this.url;
    if(url.indexOf("http://") != 0){
      var host = "http://" + vtron.util.getHost();
      url = host + url;
    }

    //样式初始化完成时，注册图层
    _parent.waitStyle(_this.styleAs, function(style){
      _this.assertParent();
      
      //URL中获取typeName
      var typeName = vtron.util.getQueryString("typeName", url);
      if(!typeName){
        typeName = _this.layerName;
      } else if(typeName.indexOf(":") >= 0){
        typeName = typeName.split(":")[1];
      }
      
      var _mapCommon = _parent.mapCommon;
      var cmdID = _mapCommon.CmdID || 0;
      var cmd = {
        "CmdName":"ADDWFSPOINTLAYER", "LayerName": typeName/*_this.layerName*/, "PointFileURL": url,
        "Markerid": style.styleId, "Width": _this.width ,"Height": _this.height,
        "Cmdid": cmdID
      };
      _mapCommon.SendMessage(JSON.stringify(cmd));
      _mapCommon.CmdID++;
      _this.cmdID = cmdID;
    });

    //地图组件的图层注册事件
    $(_parent).on("layer-reg", function(event){
      _this.assertParent();
      
      var detail = event.originalEvent.detail;
      if(_this.cmdID == detail.Cmdid){ //先识别指令ID是否一致
        _this.setAttribute("layer-id", detail.Layerid); //回写layerId
        _this.fire("load", null, {bubbles: false});  //触发图层加载事件（不冒泡）
        if(_this.getAttribute("hidden"))_this.hide(); //当设置为hidden时，隐藏图层
      }
    });

  },
  detached: function() {
  },
  reload: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;
    var _mapCommon = _parent.mapCommon;

    var cmd = {"CmdName":"UPDATEWFSPOINTLAYER","LayerId": _this.layerId};
    _mapCommon.SendMessage(JSON.stringify(cmd));

    _this.fire("reload");
  }
});

/**
 * 修改热点地理位置
 */
vtron.comp.map.MapEngineES.WFSPoint.prototype.movePoints = function(points) {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;

  var cmd = {
    "CmdName":"MOVEWFSPOINTLAYER",
    "LayerHandle": _this.layerId,
    "FID":[],
    "X":[],
    "Y": [],
  };
  for(var i = 0; points && i < points.length; i++){
    var item = points[i];
    cmd.FID.push(parseInt(item.fid));
    cmd.X.push(item.x);
    cmd.Y.push(item.y);
  }
  if(cmd.FID.length > 0)_mapCommon.SendMessage(JSON.stringify(cmd));
}


/**
 * MapEngineES地图组件的WFS线图层
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.WFSLine
 */
vtron.comp.map.MapEngineES.WFSLine = Polymer({
  is: "v-map-engine-wfs-line",
  behaviors: [vtron.comp.map.SubBehavior, vtron.comp.map.LayerBehavior],
  properties: {
    layerName: String,
    layerId: Number,
    url: String,
    hidden: Boolean,
    width: Number,
    levelWidth: String,
    r: Number,
    g: Number,
    b: Number,
    a: Number,
  },
  attached: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;

    var url = _this.url;
    if(url.indexOf("http://") != 0){
      var host = "http://" + vtron.util.getHost();
      url = host + url;
    }

    //地图初始化完成时，注册图层
    $(_parent).on("load", function(event){
      setTimeout(function(){
        _this.assertParent();
        var _mapCommon = _parent.mapCommon;
      
        //URL中获取typeName
        var typeName = vtron.util.getQueryString("typeName", url);
        if(!typeName){
          typeName = _this.layerName;
        } else if(typeName.indexOf(":") >= 0){
          typeName = typeName.split(":")[1];
        }
    
        console.log("TODO: 线图层现在延迟1ms，要修改地图插件的异步机制");
        var cmdID = _mapCommon.CmdID || 0;
        var cmd = {
          "CmdName":"ADDWFSLINELAYER", "LayerName": typeName/*_this.layerName*/,"LineFileURL": url,
          "Width": _this.width ,"ColorR": _this.r, "ColorG": _this.g, "ColorB": _this.b, "ColorA": _this.a,
          "Cmdid": cmdID
        };
        if(_this.levelWidth)cmd.levelWidth = eval("[" + _this.levelWidth + "]");
        _mapCommon.SendMessage(JSON.stringify(cmd));
        _mapCommon.CmdID++;
        _this.cmdID = cmdID;
      }, 1);
    });

    //地图组件的图层注册事件
    $(_parent).on("layer-reg", function(event){
      _this.assertParent();
      
      var detail = event.originalEvent.detail;
      if(_this.cmdID == detail.Cmdid){ //先识别指令ID是否一致
        _this.setAttribute("layer-id", detail.Layerid); //回写layerId
        _this.fire("load", null, {bubbles: false});  //触发图层加载事件（不冒泡）
        if(_this.getAttribute("hidden"))_this.hide(); //当设置为hidden时，隐藏图层
      }
    });
  },
  detached: function() {
  },
  reload: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;
    var _mapCommon = _parent.mapCommon;

    var cmd = {"CmdName":"UPDATEWFSLINELAYER","LayerId": _this.layerId};
    _mapCommon.SendMessage(JSON.stringify(cmd));

    _this.fire("reload");
  }
});


/**
 * MapEngineES地图组件的WFS面图层
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.WFSPoly
 */
vtron.comp.map.MapEngineES.WFSPoly = Polymer({
  is: "v-map-engine-wfs-poly",
  behaviors: [vtron.comp.map.SubBehavior, vtron.comp.map.LayerBehavior],
  properties: {
    layerName: String,
    layerId: Number,
    url: String,
    hidden: Boolean,
    width: Number,
    r: Number,
    g: Number,
    b: Number,
    a: Number,
  },
  attached: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;

    var url = _this.url;
    if(url.indexOf("http://") != 0){
      var host = "http://" + vtron.util.getHost();
      url = host + url;
    }
    
    //地图初始化完成时，注册图层
    $(_parent).on("load", function(event){
      _this.assertParent();
      var _mapCommon = _parent.mapCommon;
      
      //URL中获取typeName
      var typeName = vtron.util.getQueryString("typeName", url);
      if(!typeName){
        typeName = _this.layerName;
      } else if(typeName.indexOf(":") >= 0){
        typeName = typeName.split(":")[1];
      }
      
      var cmdID = _mapCommon.CmdID || 0;
      var cmd = {
        "CmdName":"ADDWFSPOLYLAYER", "LayerName": typeName/*_this.layerName*/,"PolyFileURL": url,
        "Width": _this.width ,"ColorR": _this.r, "ColorG": _this.g, "ColorB": _this.b, "ColorA": _this.a,
        "Cmdid": cmdID
      };
      _mapCommon.SendMessage(JSON.stringify(cmd));
      _mapCommon.CmdID++;
      _this.cmdID = cmdID;
    });

    //地图组件的图层注册事件
    $(_parent).on("layer-reg", function(event){
      _this.assertParent();
      
      var detail = event.originalEvent.detail;
      if(_this.cmdID == detail.Cmdid){ //先识别指令ID是否一致
        _this.setAttribute("layer-id", detail.Layerid); //回写layerId
        _this.fire("load", null, {bubbles: false});  //触发图层加载事件（不冒泡）
        if(_this.getAttribute("hidden"))_this.hide(); //当设置为hidden时，隐藏图层
      }
    });

  },
  detached: function() {
  },
  reload: function() {
    this.assertParent();
    var _this = this;
    var _parent = _this.parentNode;
    var _mapCommon = _parent.mapCommon;

    var cmd = {"CmdName":"UPDATEWFSPOLYLAYER","LayerId": _this.layerId};
    _mapCommon.SendMessage(JSON.stringify(cmd));

    _this.fire("reload");
  }
});