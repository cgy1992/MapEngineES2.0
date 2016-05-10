/**
 * MapEngineES地图组件的移动状态
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StateMove
 */
vtron.comp.map.MapEngineES.StateMove = Polymer({
  is: "v-map-engine-state-move",
  behaviors: [vtron.comp.map.SubBehavior],
  properties: {
    stateName: String,
  },
});

vtron.comp.map.MapEngineES.StateMove.prototype.change = function() {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;
  
  //鼠标手势修改
  _parent.changeCursor();
  //修改操作方式
  var cmd = {"CmdName":"OPERATORCHANGE","OPERATOR": "PAN"};
  _mapCommon.SendMessage(JSON.stringify(cmd));
}




/**
 * MapEngineES地图组件的标注状态
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StateMark
 */
vtron.comp.map.MapEngineES.StateMark = Polymer({
  is: "v-map-engine-state-mark",
  behaviors: [vtron.comp.map.SubBehavior],
  properties: {
    stateName: String,
    styleAs: String,
    width: Number,
    height: Number,
  },
});

vtron.comp.map.MapEngineES.StateMark.prototype.change = function() {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;

  _parent.waitStyle(_this.styleAs, function(style){
    //鼠标手势修改
    _parent.changeCursor("point");
    //修改操作方式
    var cmd = {"CmdName":"OPERATORCHANGE", "OPERATOR":"AddMark", "StyleID": style.styleId,"Width": _this.width ,"Height": _this.height};
    console.log(JSON.stringify(cmd))
    _mapCommon.SendMessage(JSON.stringify(cmd));
  })

}



/**
 * MapEngineES地图组件的标绘状态
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StateDraw
 */
vtron.comp.map.MapEngineES.StateDraw = Polymer({
  is: "v-map-engine-state-draw",
  behaviors: [vtron.comp.map.SubBehavior],
  properties: {
    stateName: String,
    type: String,
    width: Number,
    r: Number,
    g: Number,
    b: Number,
    a: Number,
  },
});

vtron.comp.map.MapEngineES.StateDraw.prototype.change = function() {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;
  
  //鼠标手势修改
  _parent.changeCursor("draw");
  //修改操作方式
  if(_this.type == "clear"){
    var cmd = {"CmdName":"OPERATORCHANGE","OPERATOR": "CLEAR"};
    _mapCommon.SendMessage(JSON.stringify(cmd));
  } else {
    var cmd = {"CmdName":"OPERATORCHANGE", "OPERATOR":"DRAW", "Width": _this.width, "ColorR": _this.r,"ColorG": _this.g,"ColorB": _this.b,"ColorA": _this.a};
    if(_this.type == "line") cmd["ISPOINT"] = 1; //线绘制
    _mapCommon.SendMessage(JSON.stringify(cmd));
  }
}



/**
 * MapEngineES地图组件的选择状态
 * @module vtron.comp.map
 * @class vtron.comp.map.MapEngineES.StateChoose
 */
vtron.comp.map.MapEngineES.StateChoose = Polymer({
  is: "v-map-engine-state-choose",
  behaviors: [vtron.comp.map.SubBehavior],
  properties: {
    stateName: String,
    type: String,
    width: Number,
    r: Number,
    g: Number,
    b: Number,
    a: Number,
    keepRegion: Boolean,
  },
});

vtron.comp.map.MapEngineES.StateChoose.prototype.change = function() {
  this.assertParent();
  var _this = this;
  var _parent = _this.parentNode;
  var _mapCommon = _parent.mapCommon;
  
  //鼠标手势修改
  _parent.changeCursor("draw");
  //修改操作方式
  var cmd = null;
  if(_this.type == "rectangle")cmd = {"CmdName":"OPERATORCHANGE","OPERATOR": "CHOOSERECT"};
  if(_this.type == "cycle")cmd = {"CmdName":"OPERATORCHANGE","OPERATOR": "CIRCLECHOOSE", "ColorR": _this.r,"ColorG": _this.g,"ColorB": _this.b,"ColorA": _this.a};
  if(_this.type == "polygon")cmd = {"CmdName":"OPERATORCHANGE","OPERATOR": "POLYGONCHOOSE","LINEWIDTH": _this.width};
  if(_this.type == "line")cmd = {"CmdName":"OPERATORCHANGE","OPERATOR":"LINECHOOSE","LINEWIDTH": _this.width};
  if(cmd == null)return;

  if(_this.keepRegion)cmd["Status"] = 1; //框选完之后选区是否消失
  _mapCommon.SendMessage(JSON.stringify(cmd));
}