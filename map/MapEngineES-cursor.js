/**
 * MapEngineES地图组件的鼠标样式
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.Cursor
 */
vtron.comp.map.MapEngineES.Cursor = Polymer({
  is: "v-map-engine-cursor",
  behaviors: [vtron.comp.map.SubBehavior],
  properties: {
    src: String,
    cursorName: String,
  },
});

vtron.comp.map.MapEngineES.Cursor.prototype.change = function() {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;
  
  var url = _this.src;
  if(url.indexOf("http://") != 0){
    var host = "http://" + vtron.util.getHost();
    url = host + url;
  }

  var cmd = {"CmdName":"CHANGECURSORSTYLE", "TYPE": _this.cursorName, "URL": url};
  _mapCommon.SendMessage(JSON.stringify(cmd));
}