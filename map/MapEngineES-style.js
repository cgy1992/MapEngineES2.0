/**
 * MapEngineES地图组件的通用样式行为
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StyleBehavior
 */
vtron.comp.map.StyleBehavior = {


  waitStyleId: function(callback) {
    var func = function(style){
      if(typeof(style.styleId) == "undefined"){
        setTimeout(function(){func(style)}, 100); //隔小段时间再尝试
      } else if(typeof(callback) == "function"){
        callback(style);
      }
    }
    func(this);
  }
}



/**
 * MapEngineES地图组件的图片样式
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StyleImg
 */
vtron.comp.map.MapEngineES.StyleImg = Polymer({
  is: "v-map-engine-style-img",
  behaviors: [vtron.comp.map.SubBehavior, vtron.comp.map.StyleBehavior],
  properties: {
    src: String,
    styleName: String,
    styleId: Number,
  },
  attached: function() {
    var _this = this;
    var _parent = _this.parentNode;

    var url = _this.src;
    if(url.indexOf("http://") != 0){
      var host = "http://" + vtron.util.getHost();
      url = host + url;
    }
    
    //地图初始化完成时，注册样式
    $(_parent).on("load", function(event){
      _this.assertParent();
      var _mapCommon = _parent.mapCommon;
    
      var cmdID = _mapCommon.CmdID || 0;
      var cmd = {"CmdName":"REGMARKERSTYLE", "PicURL": url,"Cmdid": cmdID};
      _mapCommon.SendMessage(JSON.stringify(cmd));
      _mapCommon.CmdID++;
      _this.cmdID = cmdID;
    });
    
    //地图注册样式完成时，设置样式ID
    $(_parent).on("style-reg", function(event){
      _this.assertParent();
      
      var detail = event.originalEvent.detail;
      if(_this.cmdID == detail.Cmdid){
        _this.setAttribute("style-id", detail.Styleid);
        _this.fire("load", null, {bubbles: false});
      }
    });
  },
  detached: function() {
  }
});